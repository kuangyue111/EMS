"""Build an offline reading/search interface, using only the Python standard library."""
from pathlib import Path
import json,sqlite3,html,re
BASE=Path(__file__).resolve().parents[1]

def inline(value):
    value=html.escape(value)
    value=re.sub(r'`([^`]+)`',r'<code>\1</code>',value)
    value=re.sub(r'\*\*([^*]+)\*\*',r'<strong>\1</strong>',value)
    value=re.sub(r'\[([^\]]+)\]\(([^)]+)\)',r'<a href="\2">\1</a>',value)
    return value

def markdown(text):
    out=[]; lines=text.splitlines(); n=0
    while n<len(lines):
        line=lines[n]
        if line.startswith('```'):
            lang=line[3:].strip(); buf=[]; n+=1
            while n<len(lines) and not lines[n].startswith('```'): buf.append(lines[n]);n+=1
            if lang=='mermaid':
                out.append('<details><summary>查看关系图定义（Mermaid）</summary><pre>'+html.escape('\n'.join(buf))+'</pre></details>')
            else: out.append('<pre>'+html.escape('\n'.join(buf))+'</pre>')
        elif line.startswith('|'):
            buf=[]
            while n<len(lines) and lines[n].startswith('|'):
                cells=lines[n].strip().strip('|').split('|')
                if not all(re.fullmatch(r'\s*:?-+:?\s*',v) for v in cells): buf.append(cells)
                n+=1
            out.append('<div class="tablewrap"><table>'+''.join('<tr>'+''.join('<'+('th' if i==0 else 'td')+'>'+inline(v.strip())+'</'+('th' if i==0 else 'td')+'>' for v in row)+'</tr>' for i,row in enumerate(buf))+'</table></div>');continue
        elif re.match(r'^#{1,6} ',line):
            level=len(line)-len(line.lstrip('#'));out.append(f'<h{level}>'+inline(line[level+1:])+f'</h{level}>')
        elif re.match(r'^(- |\d+\. )',line):
            out.append('<p class="item">'+inline(line)+'</p>')
        elif line.strip(): out.append('<p>'+inline(line)+'</p>')
        n+=1
    return '\n'.join(out)

notes=[]
for p in sorted((BASE/'docs').glob('*.md')):
    content=p.read_text(encoding='utf-8')
    notes.append(dict(title=content.splitlines()[0].lstrip('# '),path=p.relative_to(BASE).as_posix(),html=markdown(content),content=content))
conn=sqlite3.connect(BASE/'index'/'knowledge.sqlite')
corpus=[dict(source=s,path=p,category=k,content=c) for s,p,k,c in conn.execute("SELECT source,path,category,content FROM documents WHERE category NOT IN ('Build artifact','Backup') ORDER BY source,path")]
conn.close()
payload=dict(notes=notes,corpus=corpus)
(BASE/'data.js').write_text('window.KB = '+json.dumps(payload,ensure_ascii=True)+';',encoding='utf-8')
print(json.dumps({'notes':len(notes),'web_search_files':len(corpus),'web_payload_bytes':(BASE/'data.js').stat().st_size}))
