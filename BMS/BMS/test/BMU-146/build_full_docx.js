const fs = require('fs');
const {
  Document, Packer, Paragraph, TextRun, HeadingLevel,
  AlignmentType, Header, Footer, PageNumber, PageBreak,
  TableOfContents
} = require('docx');

const BASE = '/home/wan_lu/BMS/test/BMU-146/embd_qa';
const SECTION_TITLES = {
  section1: '一、C语言与数据结构基础 (Q1-Q60)',
  section2: '二、MCU架构与外设驱动 (Q61-Q130)',
  section3: '三、RTOS与实时系统 (Q131-Q180)',
  section4: '四、通信协议与物联网 (Q181-Q220)',
  section5: '五、调试、测试与工程实践 (Q221-Q260)',
  section6: '六、算法与手撕代码 (Q261-Q280)',
  section7: '七、场景设计与开放题 (Q281-Q300)',
};

function parseMd(filePath) {
  const text = fs.readFileSync(filePath, 'utf-8');
  const lines = text.split('\n');
  const elements = [];
  let inCodeBlock = false;
  let codeLines = [];
  let codeLang = '';

  for (const line of lines) {
    if (line.startsWith('```')) {
      if (inCodeBlock) {
        if (codeLines.length > 0) {
          elements.push({ type: 'code', lines: codeLines, lang: codeLang });
        }
        codeLines = [];
        codeLang = '';
        inCodeBlock = false;
      } else {
        inCodeBlock = true;
        codeLang = line.slice(3).trim();
      }
      continue;
    }
    if (inCodeBlock) { codeLines.push(line); continue; }

    const h3Match = line.match(/^###\s+(Q\d+)\.?\s*(.*)/);
    if (h3Match) {
      elements.push({ type: 'heading', number: h3Match[1], title: h3Match[2].trim() });
      continue;
    }

    const h2Match = line.match(/^##\s+(.+)/);
    if (h2Match && !h2Match[1].startsWith('Q')) {
      elements.push({ type: 'section', text: h2Match[1].trim() });
      continue;
    }
    if (line.match(/^#\s+/)) continue;
    if (line.match(/^\|[\s\-:|]+\|$/)) continue;
    if (line.match(/^---+$/)) continue;
    if (line.startsWith('###')) continue;

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
      case 'section':
        children.push(new Paragraph({
          heading: HeadingLevel.HEADING_1,
          children: [new TextRun({ text: el.text, bold: true, size: 32, font: 'Microsoft YaHei' })]
        }));
        break;
      case 'heading':
        children.push(new Paragraph({
          heading: HeadingLevel.HEADING_2,
          children: [new TextRun({ text: `${el.number}. ${el.title}`, bold: true, size: 26, font: 'Microsoft YaHei' })]
        }));
        break;
      case 'code':
        for (const cl of el.lines) {
          children.push(new Paragraph({
            indent: { left: 720 },
            spacing: { before: 0, after: 0, line: 240 },
            children: [new TextRun({ text: cl || ' ', font: 'Consolas', size: 17 })]
          }));
        }
        children.push(new Paragraph({ spacing: { after: 80 }, children: [] }));
        break;
      case 'text':
        const parts = parseInline(el.content);
        children.push(new Paragraph({
          spacing: { before: 40, after: 40, line: 340 },
          children: parts.map(p => {
            if (p.code) return new TextRun({ text: p.text, font: 'Consolas', size: 19 });
            if (p.bold) return new TextRun({ text: p.text, bold: true, font: 'Microsoft YaHei', size: 20 });
            return new TextRun({ text: p.text, font: 'Microsoft YaHei', size: 20 });
          })
        }));
        break;
      case 'spacer':
        children.push(new Paragraph({ spacing: { after: 40 }, children: [] }));
        break;
    }
  }
  return children;
}

function parseInline(text) {
  const parts = [];
  const regex = /(`[^`]+`)/g;
  let lastIdx = 0, match;
  while ((match = regex.exec(text)) !== null) {
    if (match.index > lastIdx) parts.push({ code: false, bold: false, text: text.slice(lastIdx, match.index) });
    parts.push({ code: true, bold: false, text: match[1].slice(1, -1) });
    lastIdx = match.index + match[1].length;
  }
  if (lastIdx < text.length) {
    const remaining = text.slice(lastIdx);
    if (remaining.startsWith('**') && remaining.endsWith('**')) {
      parts.push({ code: false, bold: true, text: remaining.slice(2, -2) });
    } else {
      parts.push({ code: false, bold: false, text: remaining });
    }
  }
  return parts.length ? parts : [{ code: false, bold: false, text }];
}

function pageProps() {
  return {
    page: {
      size: { width: 11906, height: 16838 },
      margin: { top: 1440, right: 1440, bottom: 1440, left: 1440 }
    }
  };
}

function defaultHeader(text) {
  return {
    default: new Header({
      children: [new Paragraph({
        alignment: AlignmentType.RIGHT,
        children: [new TextRun({ text, size: 16, font: 'Microsoft YaHei', color: '999999' })]
      })]
    })
  };
}

function defaultFooter() {
  return {
    default: new Footer({
      children: [new Paragraph({
        alignment: AlignmentType.CENTER,
        children: [
          new TextRun({ text: 'Page ', size: 16 }),
          new TextRun({ children: [PageNumber.CURRENT], size: 16 })
        ]
      })]
    })
  };
}

async function buildFullDocx() {
  console.log('Building 300-question full document...');

  const sections = [];
  const headerText = '嵌入式MCU面试300题答案';

  // Title page
  sections.push({
    properties: pageProps(),
    children: [
      new Paragraph({ spacing: { before: 5000 }, children: [] }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        children: [new TextRun({ text: '嵌入式MCU面试300题', bold: true, size: 52, font: 'Microsoft YaHei' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 300 },
        children: [new TextRun({ text: '答案解析手册', size: 36, font: 'Microsoft YaHei', color: '333333' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 400 },
        children: [new TextRun({ text: 'C语言 · MCU架构 · RTOS · 通信协议 · 调试工程 · 算法 · 场景设计', size: 20, font: 'Microsoft YaHei', color: '666666' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 500 },
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
  });

  // Content sections
  const sectionKeys = ['section1', 'section2', 'section3', 'section4', 'section5', 'section6', 'section7'];
  for (const key of sectionKeys) {
    const filePath = `${BASE}/${key}.md`;
    console.log(`  Parsing ${key}.md...`);
    const elements = parseMd(filePath);
    const secProps = {
      properties: pageProps(),
      headers: defaultHeader(headerText),
      footers: defaultFooter(),
      children: [
        new Paragraph({
          heading: HeadingLevel.HEADING_1,
          pageBreakBefore: true,
          children: [new TextRun({ text: SECTION_TITLES[key], bold: true, size: 36, font: 'Microsoft YaHei' })]
        }),
        ...buildChildren(elements),
      ]
    };
    sections.push(secProps);
    console.log(`  ${key}.md parsed: ${elements.length} elements`);
  }

  const doc = new Document({
    styles: {
      default: { document: { run: { font: 'Microsoft YaHei', size: 20 } } },
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
    sections
  });

  console.log('  Packing to buffer...');
  const buffer = await Packer.toBuffer(doc);
  const outPath = '/home/wan_lu/BMS/test/BMU-146/嵌入式MCU面试300题答案.docx';
  fs.writeFileSync(outPath, buffer);
  console.log(`Full doc created: ${outPath} (${buffer.length} bytes)`);
  return outPath;
}

buildFullDocx().catch(console.error);
