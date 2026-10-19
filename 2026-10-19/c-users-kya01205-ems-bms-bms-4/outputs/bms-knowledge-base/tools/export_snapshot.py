"""Export UTF-8 source snapshots and a module inventory from the captured index."""
from pathlib import Path
import sqlite3,json,collections,csv
BASE=Path(__file__).resolve().parents[1]
conn=sqlite3.connect(BASE/'index'/'knowledge.sqlite')
rows=conn.execute('SELECT source,path,category,content,lines FROM documents').fetchall()
modules=collections.defaultdict(lambda:dict(files=0,lines=0))
exported=[]
for source,path,category,content,lines in rows:
    p=Path(path)
    if category=='Build artifact' or category=='Backup': continue
    if p.suffix.lower() in {'.c','.h','.s','.ld'} or path in {'.project','.cproject'}:
        target=BASE/'source-snapshot'/source/path
    elif p.suffix.lower() in {'.md','.txt'}:
        target=BASE/'references'/source/path
    else: continue
    target.parent.mkdir(parents=True,exist_ok=True)
    target.write_text(content,encoding='utf-8',newline='')
    exported.append(dict(source=source,path=path,snapshot=target.relative_to(BASE).as_posix(),lines=lines))
    key=source+'/'+str(p.parent).replace('\\','/')
    modules[key]['files']+=1
    modules[key]['lines']+=lines
(BASE/'index'/'snapshot-map.json').write_text(json.dumps(exported,ensure_ascii=False,indent=2),encoding='utf-8')
(BASE/'index'/'modules.json').write_text(json.dumps(modules,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'snapshot_files':len(exported),'modules':len(modules)},ensure_ascii=False))
