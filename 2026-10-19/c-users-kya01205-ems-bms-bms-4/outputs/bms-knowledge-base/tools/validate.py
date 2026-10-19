"""Validate artifact integrity and representative evidence; this is not a firmware test."""
from pathlib import Path
import sqlite3,json,hashlib,re
BASE=Path(__file__).resolve().parents[1]
conn=sqlite3.connect(BASE/'index'/'knowledge.sqlite')
assert conn.execute('pragma integrity_check').fetchone()[0]=='ok'
rows=conn.execute('select source,path,content,text_sha256,lines from documents').fetchall()
docs={(s,p):text for s,p,text,sha,lines in rows}
assert all(hashlib.sha256(text.encode('utf-8')).hexdigest()==sha for s,p,text,sha,lines in rows)
assert all(len(text.splitlines())==lines for s,p,text,sha,lines in rows)
manifest=json.loads((BASE/'index/files.json').read_text(encoding='utf-8'))
assert len(manifest)==1874 and len(rows)==1575
snapshots=json.loads((BASE/'index/snapshot-map.json').read_text(encoding='utf-8'))
for item in snapshots:
    expected=docs[(item['source'],item['path'])].replace('\r\n','\n')
    actual=(BASE/item['snapshot']).read_text(encoding='utf-8').replace('\r\n','\n')
    assert actual==expected,item['snapshot']

checks=[
 ('BMS','Sources/Funtion/SystemInfo/GroupInfo.c',2002,'1℃'),
 ('EKF','SocEst/ekf_adapter.c',21,'0.1f'),
 ('EKF','SocEst/ekf_adapter.c',61,'GetGCellAvgVoltAPI'),
 ('BMS','Generated_Code/FreeRTOSConfig.h',175,'( 8 )'),
 ('BMS','Sources/OSUser/OSUserHOOK.h',37,'18'),
 ('BMS','Sources/Funtion/SocLib/SocUserAPI.c',73,';'),
 ('BMS','Sources/Funtion/SocLib/SocSoeSync.c',123,'<= 25'),
 ('BMS','Sources/Funtion/SocLib/SocSoeSync.c',132,'<= 30'),
 ('EKF','SocEst/ekf_params.h',26,'#ifndef CELL_EVE_314'),
 ('EKF','SocEst/ekf_bms.c',41,'0.01f'),
 ('EKF','docs/EKF_SOC算法测试验证报告.md',31,'全程为 0'),
]
for source,path,line,needle in checks:
    assert needle in docs[(source,path)].splitlines()[line-1],(source,path,line)
assert not any('EkfAdapterTask' in t for (s,p),t in docs.items() if s=='BMS' and p.startswith('Sources/'))
assert sum(s=='EKF' and p.startswith('SocEst/') for s,p in docs)==7
for p in [BASE/'README.md',*list((BASE/'docs').glob('*.md'))]:
    text=p.read_text(encoding='utf-8')
    for target in re.findall(r'\[[^\]]+\]\(([^)]+)\)',text):
        if not target.startswith(('http:','https:','#')):
            assert (p.parent/target).exists(),(p,target)
result=dict(artifact_integrity='pass',files=len(manifest),indexed_documents=len(rows),snapshot_files=len(snapshots),evidence_checks=len(checks),source_hashes='pass',snapshot_match='pass',markdown_links='pass',firmware_compilation='not run',hardware_tests='not run',experiment_reproduction='not run',browser_checks='Homepage, function search, full source dialog and notes verified interactively')
(BASE/'index/validation.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False))
