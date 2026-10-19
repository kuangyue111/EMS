# BMS 与 EKF SOC 工程知识库

快照日期：2026-10-19。对象：S32K146OFBMU 固件与 EKF_SOC 阶段性成果包。

这是已经建立的本地工程知识库，包含源码与文档检索、架构分析、SOC 数据链、EKF 对接差异和验证路线。打开 `index.html` 可以离线阅读并检索；无需账号、网络或安装数据库服务。

## 阅读入口

1. [工程架构与调度](docs/01-architecture.md)
2. [现有 SOC、SOE 与保护链](docs/02-bms-soc.md)
3. [EKF 模型、参数与适配](docs/03-ekf.md)
4. [关键发现与源码依据](docs/04-findings.md)
5. [接入和验证路线](docs/05-integration.md)
6. [阅读覆盖与证据边界](docs/06-coverage.md)
7. [后续会话交接](docs/07-handoff.md)
8. [全部文件清单](index/files.csv)
9. [全目录模块索引](docs/08-module-catalog.md)

## 当前最重要的认识

- 当前固件主 SOC 链是电流积分、容量管理、校准与显示处理；`SocSoeLibAllTask` 的 500 ms EKF 位置仍为空。
- 独立成果包实现四状态 EKF：SOC、两支 RC 极化电压、慢变电压偏置；以影子方式记录结果。
- 两份代码直接对接存在温度单位问题：固件接口为 1℃，适配层却乘 0.1。25℃会进入算法成为2.5℃。
- 任务优先级定义超出当前 FreeRTOS 的 0～7 范围；此份内核会把超限优先级压到7，需核对实际部署版本。
- 报告中的输出限速、充放电选压策略与交付适配层存在差异；不能把报告精度直接归给这份源码。

## 已完成的建档

共盘点 1,874 个文件：BMS 1,835 个、EKF 39 个。1,575 个文本文件已进入 SQLite 检索库，共496,491行，其中包含大量SDK、生成代码、构建参数和依赖清单。BMS业务源码目录为343个文件、183,468行；独立EKF代码为7个文件、1,757行。源码/配置和参考文档导出快照共591个文件。

**全文索引不等于逐行审计。** 本轮完成全目录建档和核心运行链、SOC链、EKF链的重点学习；未宣称每个SDK寄存器、每条通信字段、每个保护分支都已审计。具体边界见覆盖说明。

## 检索和更新

在本目录执行：

```powershell
$env:PYTHONIOENCODING='utf-8'
python tools/search.py GetGCellAvgTempAPI --source BMS
python tools/search.py --source EKF --path SocEst/ekf_adapter.c --show
python tools/search.py --source BMS --path Sources/OSUser/OSUserHOOK.c --show --start 479 --end 558
```

本机可用的Python：`C:\Users\KYA01205\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe`。

刷新索引：先保存当前知识库版本，再运行 `python tools/build_index.py --rg <rg.exe路径>`、`python tools/export_snapshot.py`、`python tools/build_site.py`。源路径配置在 `index/source-roots.json`，可用 `--roots` 指定新路径。刷新文本后，分析结论仍须重新核对；脚本不会自动证明旧结论有效。导出目录若保留旧文件，以 `index/snapshot-map.json` 为当前快照权威清单。

## 保存方式

知识库独立于原工程，建立本地Git版本；原始两目录已有共同Git根 `C:\Users\KYA01205\EMS`，读取时HEAD为 `d46b2f33`。本轮未修改原固件、未烧录、未推送Git远端、未上传Google Drive。本地知识库包含源码内容，日后共享应按项目自身的访问规则选择收件范围。

后续会话可直接打开本知识库所在目录作为Codex项目，或先提供本README和交接页。文件是持续学习的依据，不能依赖对话模型永久记住所有源码。
