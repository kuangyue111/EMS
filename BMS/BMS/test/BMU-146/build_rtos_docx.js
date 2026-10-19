const fs = require('fs');
const {
  Document, Packer, Paragraph, TextRun, HeadingLevel,
  AlignmentType, Header, Footer, PageNumber, PageBreak,
  TableOfContents, BorderStyle
} = require('docx');

const BASE = '/home/wan_lu/BMS/test/BMU-146/embd_qa';

function parseMd(filePath) {
  const text = fs.readFileSync(filePath, 'utf-8');
  const lines = text.split('\n');
  const elements = [];
  let inCodeBlock = false;
  let codeLines = [];
  let codeLang = '';

  for (const line of lines) {
    // Code block toggle
    if (line.startsWith('```')) {
      if (inCodeBlock) {
        // End code block
        if (codeLines.length > 0) {
          elements.push({ type: 'code', lines: codeLines, lang: codeLang });
        }
        codeLines = [];
        codeLang = '';
        inCodeBlock = false;
      } else {
        // Start code block
        inCodeBlock = true;
        codeLang = line.slice(3).trim();
      }
      continue;
    }

    if (inCodeBlock) {
      codeLines.push(line);
      continue;
    }

    // Heading: ### Q123. title
    const h3Match = line.match(/^###\s+(Q\d+)\.?\s*(.*)/);
    if (h3Match) {
      elements.push({ type: 'heading', number: h3Match[1], title: h3Match[2].trim() });
      continue;
    }

    // Section headers like "## 一、xxx" or "## Section"
    const h2Match = line.match(/^##\s+(.+)/);
    if (h2Match && !h2Match[1].startsWith('Q')) {
      elements.push({ type: 'section', text: h2Match[1].trim() });
      continue;
    }

    // h1
    const h1Match = line.match(/^#\s+(.+)/);
    if (h1Match && !h1Match[1].startsWith('Q')) {
      elements.push({ type: 'h1', text: h1Match[1].trim() });
      continue;
    }

    // Table separator (skip)
    if (line.match(/^\|[\s\-:|]+\|$/)) continue;

    // Horizontal rule (skip)
    if (line.match(/^---+$/)) continue;

    // Skip continuation notes
    if (line.startsWith('###')) continue;

    // Regular text
    const trimmed = line.trim();
    if (trimmed.length > 0) {
      elements.push({ type: 'text', content: trimmed });
    } else if (elements.length > 0 && elements[elements.length - 1].type !== 'spacer') {
      elements.push({ type: 'spacer' });
    }
  }

  return elements;
}

function buildChildren(elements) {
  const children = [];
  
  for (const el of elements) {
    switch (el.type) {
      case 'h1':
        children.push(new Paragraph({
          heading: HeadingLevel.HEADING_1,
          children: [new TextRun({ text: el.text, bold: true, size: 36, font: 'Microsoft YaHei' })]
        }));
        children.push(new Paragraph({ spacing: { after: 200 }, children: [] }));
        break;

      case 'section':
        children.push(new Paragraph({
          heading: HeadingLevel.HEADING_1,
          children: [new TextRun({ text: el.text, bold: true, size: 32, font: 'Microsoft YaHei' })]
        }));
        children.push(new Paragraph({ spacing: { after: 120 }, children: [] }));
        break;

      case 'heading':
        children.push(new Paragraph({
          heading: HeadingLevel.HEADING_2,
          children: [new TextRun({ text: `${el.number}. ${el.title}`, bold: true, size: 26, font: 'Microsoft YaHei' })]
        }));
        children.push(new Paragraph({ spacing: { after: 80 }, children: [] }));
        break;

      case 'code':
        // Add code as indented paragraphs with monospace font
        for (const codeLine of el.lines) {
          children.push(new Paragraph({
            indent: { left: 720 },
            spacing: { before: 0, after: 0, line: 240 },
            children: [new TextRun({ text: codeLine || ' ', font: 'Consolas', size: 18 })]
          }));
        }
        children.push(new Paragraph({ spacing: { after: 120 }, children: [] }));
        break;

      case 'text':
        // Handle inline code (backtick)
        const parts = parseInline(el.content);
        children.push(new Paragraph({
          spacing: { before: 60, after: 60, line: 360 },
          children: parts.map(p => {
            if (p.code) {
              return new TextRun({ text: p.text, font: 'Consolas', size: 20, italics: false });
            }
            // Handle bold markers
            if (p.text.startsWith('**') && p.text.endsWith('**')) {
              return new TextRun({ text: p.text.slice(2, -2), bold: true, font: 'Microsoft YaHei', size: 20 });
            }
            return new TextRun({ text: p.text, font: 'Microsoft YaHei', size: 20 });
          })
        }));
        break;

      case 'spacer':
        children.push(new Paragraph({ spacing: { after: 60 }, children: [] }));
        break;
    }
  }
  
  return children;
}

function parseInline(text) {
  const parts = [];
  const regex = /(`[^`]+`)/g;
  let lastIdx = 0;
  let match;
  
  while ((match = regex.exec(text)) !== null) {
    if (match.index > lastIdx) {
      parts.push({ code: false, text: text.slice(lastIdx, match.index) });
    }
    parts.push({ code: true, text: match[1].slice(1, -1) });
    lastIdx = match.index + match[1].length;
  }
  if (lastIdx < text.length) {
    parts.push({ code: false, text: text.slice(lastIdx) });
  }
  if (parts.length === 0) {
    parts.push({ code: false, text });
  }
  return parts;
}

async function buildRTOSDocx() {
  console.log('Building RTOS专题文档...');
  
  const section3 = parseMd(`${BASE}/section3.md`);
  const rtosDeep = parseMd(`${BASE}/rtos_deep.md`);
  
  const doc = new Document({
    styles: {
      default: {
        document: {
          run: { font: 'Microsoft YaHei', size: 20 }
        }
      },
      paragraphStyles: [
        {
          id: 'Heading1', name: 'Heading 1', basedOn: 'Normal', next: 'Normal', quickFormat: true,
          run: { size: 36, bold: true, font: 'Microsoft YaHei' },
          paragraph: { spacing: { before: 360, after: 200 }, outlineLevel: 0 }
        },
        {
          id: 'Heading2', name: 'Heading 2', basedOn: 'Normal', next: 'Normal', quickFormat: true,
          run: { size: 28, bold: true, font: 'Microsoft YaHei' },
          paragraph: { spacing: { before: 240, after: 120 }, outlineLevel: 1 }
        }
      ]
    },
    sections: [
      // Title page
      {
        properties: {
          page: {
            size: { width: 11906, height: 16838 },
            margin: { top: 1440, right: 1440, bottom: 1440, left: 1440 }
          }
        },
        children: [
          new Paragraph({ spacing: { before: 6000 }, children: [] }),
          new Paragraph({
            alignment: AlignmentType.CENTER,
            children: [new TextRun({ text: 'RTOS面试深度解析', bold: true, size: 56, font: 'Microsoft YaHei' })]
          }),
          new Paragraph({
            alignment: AlignmentType.CENTER,
            spacing: { before: 400 },
            children: [new TextRun({ text: 'FreeRTOS / RT-Thread 内核原理与进阶', size: 28, font: 'Microsoft YaHei', color: '555555' })]
          }),
          new Paragraph({
            alignment: AlignmentType.CENTER,
            spacing: { before: 600 },
            children: [new TextRun({ text: '2026年7月', size: 22, font: 'Microsoft YaHei', color: '888888' })]
          }),
          new Paragraph({ children: [new PageBreak()] }),
          new Paragraph({
            heading: HeadingLevel.HEADING_1,
            children: [new TextRun({ text: '目录', bold: true, size: 36, font: 'Microsoft YaHei' })]
          }),
          new TableOfContents('目录', { hyperlink: true, headingStyleRange: '1-2' }),
          new Paragraph({ children: [new PageBreak()] }),
        ]
      },
      // Section 3: RTOS基础
      {
        properties: {
          page: {
            size: { width: 11906, height: 16838 },
            margin: { top: 1440, right: 1440, bottom: 1440, left: 1440 }
          }
        },
        headers: {
          default: new Header({
            children: [new Paragraph({
              alignment: AlignmentType.RIGHT,
              children: [new TextRun({ text: 'RTOS面试深度解析', size: 16, font: 'Microsoft YaHei', color: '999999' })]
            })]
          })
        },
        footers: {
          default: new Footer({
            children: [new Paragraph({
              alignment: AlignmentType.CENTER,
              children: [
                new TextRun({ text: 'Page ', size: 16, font: 'Microsoft YaHei' }),
                new TextRun({ children: [PageNumber.CURRENT], size: 16, font: 'Microsoft YaHei' })
              ]
            })]
          })
        },
        children: [
          new Paragraph({
            heading: HeadingLevel.HEADING_1,
            children: [new TextRun({ text: '第一部分：RTOS基础与实时系统 (Q131-Q180)', bold: true, size: 36, font: 'Microsoft YaHei' })]
          }),
          ...buildChildren(section3),
        ]
      },
      // RTOS Deep Dive
      {
        properties: {
          page: {
            size: { width: 11906, height: 16838 },
            margin: { top: 1440, right: 1440, bottom: 1440, left: 1440 }
          }
        },
        headers: {
          default: new Header({
            children: [new Paragraph({
              alignment: AlignmentType.RIGHT,
              children: [new TextRun({ text: 'RTOS面试深度解析', size: 16, font: 'Microsoft YaHei', color: '999999' })]
            })]
          })
        },
        footers: {
          default: new Footer({
            children: [new Paragraph({
              alignment: AlignmentType.CENTER,
              children: [
                new TextRun({ text: 'Page ', size: 16, font: 'Microsoft YaHei' }),
                new TextRun({ children: [PageNumber.CURRENT], size: 16, font: 'Microsoft YaHei' })
              ]
            })]
          })
        },
        children: [
          new Paragraph({
            heading: HeadingLevel.HEADING_1,
            pageBreakBefore: true,
            children: [new TextRun({ text: '第二部分：RTOS内核深度剖析', bold: true, size: 36, font: 'Microsoft YaHei' })]
          }),
          ...buildChildren(rtosDeep),
        ]
      }
    ]
  });

  const buffer = await Packer.toBuffer(doc);
  const outPath = '/home/wan_lu/BMS/test/BMU-146/RTOS面试深度解析.docx';
  fs.writeFileSync(outPath, buffer);
  console.log(`RTOS dok created: ${outPath} (${buffer.length} bytes)`);
  return outPath;
}

buildRTOSDocx().catch(console.error);
