"""Search exact text in the local snapshot, with original source line numbers."""
from pathlib import Path
import argparse, sqlite3, sys

BASE=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser()
ap.add_argument('query', nargs='?', default='')
ap.add_argument('--source',choices=['BMS','EKF'])
ap.add_argument('--path',default='',help='Path substring; use with --show to read a whole file')
ap.add_argument('--show',action='store_true')
ap.add_argument('--limit',type=int,default=30)
ap.add_argument('--start',type=int,default=1)
ap.add_argument('--end',type=int,default=10000000)
a=ap.parse_args()
conn=sqlite3.connect(BASE/'index'/'knowledge.sqlite')
rows=conn.execute('SELECT source,path,content FROM documents WHERE (? IS NULL OR source=?) AND instr(lower(path),lower(?))>0 ORDER BY source,path',(a.source,a.source,a.path))
count=0
for source,path,content in rows:
    if not a.show and a.query.lower() not in content.lower() and a.query.lower() not in path.lower(): continue
    for n,line in enumerate(content.splitlines(),1):
        if not (a.start <= n <= a.end): continue
        if a.show or a.query.lower() in line.lower():
            print(f'{source}/{path}:{n}: {line}')
            count+=1
            if not a.show and count>=a.limit: sys.exit(0)
conn.close()
