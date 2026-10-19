"use strict";
const tabs = [...document.querySelectorAll('[data-tab]')];
function setTab(name) {
  tabs.forEach(b => b.classList.toggle('active',b.dataset.tab===name));
  ['overview','notes','search'].forEach(k=>document.getElementById(k).hidden=k!==name);
}
tabs.forEach(b=>b.addEventListener('click',()=>setTab(b.dataset.tab)));
const noteList=document.getElementById('noteList');
function showNote(index) {
  // HTML was generated from our authored reports with escaped text.
  document.getElementById('noteContent').innerHTML=KB.notes[index].html;
  [...noteList.children].forEach((b,i)=>b.classList.toggle('active',i===index));
}
KB.notes.forEach((n,i)=>{const b=document.createElement('button');b.textContent=`${String(i+1).padStart(2,'0')}  ${n.title}`;b.onclick=()=>showNote(i);noteList.append(b);});
showNote(0);
const status=document.getElementById('searchStatus');
status.textContent=`可搜索 ${KB.corpus.length} 个文件；结果保留原文件行号。输入关键词后点击搜索。`;
function openSource(item,line) {
  document.getElementById('sourceTitle').textContent=`${item.source}/${item.path} · 第 ${line} 行`;
  const pre=document.getElementById('sourceText');
  pre.textContent=item.content.split(/\r?\n/).map((s,i)=>`${String(i+1).padStart(5,' ')}  ${s}`).join('\n');
  document.getElementById('sourceDialog').showModal();
  requestAnimationFrame(()=>{pre.scrollTop=Math.max(0,(line-5)*22.1);});
}
document.getElementById('closeDialog').onclick=()=>document.getElementById('sourceDialog').close();
document.getElementById('searchForm').addEventListener('submit',e=>{
  e.preventDefault();const q=document.getElementById('query').value.trim().toLowerCase();
  const source=document.getElementById('sourceFilter').value;const area=document.getElementById('results');area.replaceChildren();
  if(!q){status.textContent='请输入函数名、路径片段或关键词。';return;}
  let total=0,shown=0;
  for(const item of KB.corpus){
    if(source&&source!==item.source)continue;
    if(!item.path.toLowerCase().includes(q)&&!item.content.toLowerCase().includes(q))continue;
    total++;if(shown>=60)continue;shown++;
    const lines=item.content.split(/\r?\n/);let idx=lines.findIndex(s=>s.toLowerCase().includes(q));if(idx<0)idx=0;
    const row=document.createElement('div');row.className='result';
    const badge=document.createElement('span');badge.className='badge';badge.textContent=item.category;row.append(badge);
    const button=document.createElement('button');button.textContent=`${item.source}/${item.path}:${idx+1}`;button.onclick=()=>openSource(item,idx+1);row.append(button);
    const pre=document.createElement('pre');pre.textContent=lines.slice(Math.max(0,idx-1),idx+3).map((s,j)=>`${Math.max(0,idx-1)+j+1}: ${s}`).join('\n');row.append(pre);area.append(row);
  }
  status.textContent=`命中 ${total} 个文件，显示 ${shown} 个。点击路径查看全文及行号。${total>shown?' 请缩小关键词或来源范围。':''}`;
});
