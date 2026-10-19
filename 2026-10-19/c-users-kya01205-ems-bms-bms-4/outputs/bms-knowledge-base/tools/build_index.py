"""Read-only inventory and searchable text snapshot for the two source roots.

Uses ordinary rg reads because native Python/PowerShell file reads can expose a
different wrapped view on this host. Never executes source-project commands.
"""
from pathlib import Path
import argparse, base64, csv, hashlib, json, re, sqlite3, subprocess, datetime

BASE = Path(__file__).resolve().parents[1]
DEFAULT_ROOTS = {
    'BMS': r'C:\Users\KYA01205\EMS\BMS\BMS\BMU-146\S32K146OFBMU',
    'EKF': r'C:\Users\KYA01205\EMS\EKF_SOC阶段性成果总结\EKF_SOC阶段性成果总结',
}
TEXT_EXT = {'.c','.h','.s','.md','.txt','.xml','.ld','.pe','.prefs','.project','.cproject','.mk','.args','.d','.map','.launch','.g_c','.g_x','.local','.bak'}
WRAPPED = bytes.fromhex('887d1c')

def text_candidate(path):
    return path.suffix.lower() in TEXT_EXT or path.name.lower() in {'makefile','sources','objects','.project','.cproject'}

def read_batch(rg, paths):
    """Explicit paths avoid ignore surprises; JSON preserves non-UTF8 bytes."""
    captured={}
    for start in range(0,len(paths),75):
        batch=paths[start:start+75]
        proc=subprocess.run([rg,'--no-config','--json','-a','--encoding','none','--color','never','^',*[str(p) for p in batch]],capture_output=True)
        parts={str(p):[] for p in batch}
        seen=set()
        for line in proc.stdout.splitlines():
            event=json.loads(line)
            if event['type'] not in ('begin','match'): continue
            datum=event['data']
            name=datum['path'].get('text')
            if name is None: name=base64.b64decode(datum['path']['bytes']).decode('utf-8')
            key=str(Path(name)); seen.add(key)
            if event['type']=='match':
                value=datum['lines']
                parts[key].append(value['text'].encode('utf-8') if 'text' in value else base64.b64decode(value['bytes']))
        for p in batch:
            key=str(p)
            captured[key]=(b''.join(parts[key]),0 if key in seen or proc.returncode in (0,1) else 2)
    return captured

def decode(data):
    for enc in ('utf-8-sig', 'gb18030'):
        try:
            return data.decode(enc), enc
        except UnicodeDecodeError:
            pass
    return data.decode('utf-8', errors='replace'), 'utf-8-replacement'

def category(label, rel):
    p = rel.replace('\\','/')
    if label == 'EKF':
        return 'EKF source' if p.startswith('SocEst/') else 'EKF documentation/artifact'
    if p.startswith('Sources/'): return 'Application'
    if p.startswith('SDK/'): return 'Vendor SDK'
    if p.startswith('Generated_Code/'): return 'Generated code'
    if p.startswith(('Debug/','Release/')): return 'Build artifact'
    if '.bak' in p: return 'Backup'
    return 'Project/configuration/documentation'

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--rg', default='rg')
    ap.add_argument('--roots', type=Path)
    a = ap.parse_args()
    roots = json.loads(a.roots.read_text(encoding='utf-8')) if a.roots else DEFAULT_ROOTS
    data_dir = BASE/'index'
    data_dir.mkdir(parents=True, exist_ok=True)
    db_path = data_dir/'knowledge.sqlite'
    conn = sqlite3.connect(db_path)
    conn.executescript('DROP TABLE IF EXISTS documents; DROP TABLE IF EXISTS search; CREATE TABLE documents(id INTEGER PRIMARY KEY, source TEXT, path TEXT, category TEXT, encoding TEXT, lines INTEGER, text_sha256 TEXT, content TEXT); CREATE VIRTUAL TABLE search USING fts5(source, path, content, tokenize="unicode61");')
    manifest=[]
    for label, root in roots.items():
        root = Path(root)
        source_files=[p for p in sorted(root.rglob('*')) if p.is_file() and '.git' not in p.parts]
        captured=read_batch(a.rg,[p for p in source_files if text_candidate(p)])
        for path in source_files:
            rel = path.relative_to(root).as_posix()
            stat = path.stat()
            raw = path.read_bytes()
            wrapped = raw[:3] == WRAPPED and raw[4:8] == bytes.fromhex('7c081301')
            row = dict(source=label, path=rel, category=category(label,rel), bytes=stat.st_size,
                       modified_utc=datetime.datetime.fromtimestamp(stat.st_mtime, datetime.timezone.utc).isoformat(),
                       filesystem_sha256=hashlib.sha256(raw).hexdigest(),
                       filesystem_view='wrapped' if wrapped else 'ordinary',
                       status='binary-inventory-only', encoding='', lines=0, text_sha256='')
            is_text = text_candidate(path)
            if is_text:
                # -a makes the read deterministic; no source commands are executed.
                value,read_result = captured[str(path)]
                if read_result not in (0,1):
                    row['status']='read-error'
                elif value[:3] == WRAPPED or b'\x00' in value[:4096]:
                    row['status']='text-unreadable-wrapped-or-binary'
                else:
                    content, enc = decode(value)
                    row.update(status='text-indexed',encoding=enc,lines=len(content.splitlines()),text_sha256=hashlib.sha256(content.encode('utf-8')).hexdigest())
                    conn.execute('INSERT INTO documents(source,path,category,encoding,lines,text_sha256,content) VALUES(?,?,?,?,?,?,?)',
                                 (label,rel,row['category'],enc,row['lines'],row['text_sha256'],content))
                    conn.execute('INSERT INTO search(source,path,content) VALUES(?,?,?)',(label,rel,content))
            manifest.append(row)
    conn.commit()
    conn.execute('VACUUM')
    conn.close()
    with (data_dir/'files.csv').open('w',encoding='utf-8-sig',newline='') as f:
        writer=csv.DictWriter(f, fieldnames=list(manifest[0])); writer.writeheader();writer.writerows(manifest)
    (data_dir/'files.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),encoding='utf-8')
    (data_dir/'source-roots.json').write_text(json.dumps(roots,ensure_ascii=False,indent=2),encoding='utf-8')
    snapshot_date=datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=8))).date().isoformat()
    summary = dict(snapshot_date=snapshot_date,files=len(manifest),bytes=sum(x['bytes'] for x in manifest),text_indexed=sum(x['status']=='text-indexed' for x in manifest),indexed_lines=sum(x['lines'] for x in manifest),by_source={},by_category={},status={})
    for x in manifest:
        for group,key in [('by_source','source'),('by_category','category'),('status','status')]:
            summary[group][x[key]]=summary[group].get(x[key],0)+1
    (data_dir/'summary.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps(summary,ensure_ascii=False))

if __name__=='__main__': main()
