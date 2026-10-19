# 嵌入式/MCU工程师面试题库 — 第4部分：网络协议与通信 (Q181-Q220)

---

## Q181. TCP三次握手/四次挥手？为什么不是两次/三次？

### 三次握手（连接建立）

```
Client                              Server
  |                                   |
  |------- SYN (seq=x) ------------>  |  (1) SYN_SENT -> SYN_RCVD
  |                                   |
  |<---- SYN+ACK (seq=y, ack=x+1) --|  (2) SYN_RCVD -> ESTABLISHED
  |                                   |
  |------- ACK (ack=y+1) ----------> |  (3) ESTABLISHED
```

**为什么不是两次？** 核心原因是TCP需要双方确认"自己的发送能力和对方的接收能力"都正常。如果只握手两次，服务端无法知道客户端的接收能力是否正常，也无法防止历史重复连接请求到达服务端时导致错误建立连接。RFC 793明确要求三次握手来同步双方的初始序列号（ISN），防止旧数据干扰新连接。

**为什么不是四次？** 握手阶段的SYN和ACK可以合并到一个报文段中（称为"捎带确认"），因为握手时对端还没有数据需要发送，应答和控制信息可以在同一个TCP段中携带。而挥手阶段，由于TCP是全双工的，一方关闭后另一方可能还有数据要发送，所以FIN和ACK必须分开发送。

### 四次挥手（连接关闭）

```
Client                              Server
  |                                   |
  |------- FIN (seq=u) ------------>  |  (1) FIN_WAIT_1 -> CLOSE_WAIT
  |<------ ACK (ack=u+1) ----------|  (2) FIN_WAIT_2
  |                                   |
  |<------ FIN+ACK (seq=v) --------|  (3) TIME_WAIT
  |------- ACK (ack=v+1) ----------> |  (4) CLOSED
```

TCP连接是全双工的，每个方向必须单独关闭。客户端发送FIN仅表示"我没有数据要发了"，服务端ACK后可能还有数据发送（CLOSE_WAIT状态）。服务端发完数据后再发送FIN，客户端进入TIME_WAIT状态等待2MSL（Maximum Segment Lifetime，典型值60-120秒），确保最后一个ACK能被服务端收到、且网络中该连接的旧数据包全部消失。

### MCU实现要点

在lwIP等轻量级TCP/IP协议栈中，状态机由`tcp_process()`函数驱动，MCU开发者需要关注的回调包括：`tcp_connect()` → `tcp_sent_callback` → `tcp_recv_callback` → `tcp_close()`。TIME_WAIT状态在资源受限的MCU上可通过`SO_LINGER`设置为0来避免，但可能导致旧数据干扰下一次同端口连接。

**Wireshark过滤技巧：**
- 跟踪握手：`tcp.flags.syn==1 and tcp.flags.ack==0` (初始SYN)
- 跟踪挥手：`tcp.flags.fin==1`
- 完整流跟踪：`tcp.stream eq N`

---

## Q182. TCP粘包/拆包问题及解决方案？

### 问题本质

TCP是面向流的协议，不保留应用层消息边界。发送方多次`write()`的数据可能在TCP发送缓冲区合并（Nagle算法），接收方一次`read()`可能读到多次`write`的数据（粘包），也可能读到一次`write`的部分数据（拆包）。

```
发送: [MSG1][MSG2][MSG3]
TCP流: ...MSG1MSG2MSG3...
接收可能: [MSG1M] [SG2] [MSG3]  (拆包+粘包混合)
```

### 五种解决方案

**1. 定长包**
所有消息固定N字节，不足补零。适用于指令集固定场景（如Modbus RTU请求帧固定8字节）。缺点：浪费带宽。

```c
#define PKT_LEN 64
uint8_t buf[PKT_LEN];
recv(sock, buf, PKT_LEN, MSG_WAITALL);
```

**2. 分隔符**
以特殊字符（如`\r\n`，类似HTTP头、AT指令）标记消息结束。适用于文本协议，但需转义规则处理数据中出现的分隔符。

```c
// 环形缓冲区 + 扫描\r\n
void parse_line(ringbuf_t *rb) {
    char *ptr = ringbuf_find(rb, "\r\n", 2);
    if (ptr) { /* 提取一条完整消息 */ }
}
```

**3. 长度前缀（Length-Prefixed）**
最通用方案。包头固定N字节表示载荷长度：

```
|<-- 4字节长度 -->|<-- N字节载荷 -->|
|   payload_len   |     data        |
```

```c
typedef struct {
    uint32_t len;    // 网络字节序
    uint8_t  data[];
} pkt_t;

// 接收逻辑：先读4字节定长度，再读len字节
uint32_t pkt_len;
recv_exact(sock, &pkt_len, 4);
pkt_len = ntohl(pkt_len);
uint8_t *payload = malloc(pkt_len);
recv_exact(sock, payload, pkt_len);
```

**4. 固定包头+包体（TLV）**
包头包含类型(Type)、长度(Length)，包体为值(Value)。可扩展性好，适配多种消息类型。

**5. 应用层协议自带边界**
如HTTP的`Content-Length`头、WebSocket的帧头（FIN+opcode+mask+payload length）。

### MCU实现注意事项

- 使用DMA+环形缓冲区接收，避免频繁中断拷贝
- 实现`recv_exact()`函数循环调用`recv()`直到收到指定字节
- 超时机制：设置`SO_RCVTIMEO`或使用定时器
- 防御性编程：校验长度字段上限，防止恶意包导致内存溢出

---

## Q183. UDP与TCP的应用场景选择？

### 核心对比

| 维度 | TCP | UDP |
|------|-----|-----|
| 连接 | 面向连接(三次握手) | 无连接 |
| 可靠性 | 确认重传、流量控制、拥塞控制 | 无保障 |
| 顺序 | 有序交付 | 无序，可能乱序 |
| 边界 | 流式(无边界) | 报文边界保留 |
| 头部开销 | 20-60字节 | 8字节 |
| 实时性 | 延迟波动大(重传) | 低延迟 |
| 支持方式 | 一对一 | 一对一/一对多/多对多 |

### 选用UDP的场景

1. **实时音视频/VoIP**：丢一两个包影响可接受，重传反而导致卡顿。
2. **DNS查询**：单包请求-应答，开销小，超时重试在应用层实现。
3. **物联网传感器数据上报**：数据时效性 > 可靠性，丢包影响小。
4. **广播/组播**：如设备发现(SSDP)、网络授时。
5. **QUIC/HTTP3底层**：在UDP上构建可靠传输。

### 选用TCP的场景

1. **文件传输/固件OTA**：数据完整性至关重要。
2. **Web浏览/HTTP(S)**: 页面渲染必须完整无损。
3. **数据库连接**：交易数据零容忍丢失。
4. **MQTT(QoS 1/2)**：需要可靠消息分发。

### MCU上的决策

- **资源受限MCU (RAM < 64KB)**：优先UDP或CoAP（基于UDP），TCP状态机消耗RAM大（lwIP中TCP_PCB约200+字节每个连接）。
- **电池供电传感器**：UDP免去连接维护的心跳开销。
- **固件升级网关**：必须TCP/TFTP+校验。
- **混合方案**：控制信令走TCP，数据流走UDP（如RTSP+RTP）。

### Socket代码示例

```c
// UDP发送（无需connect，每次sendto指定目标）
int sock = socket(AF_INET, SOCK_DGRAM, 0);
struct sockaddr_in dst = {.sin_family=AF_INET, .sin_port=htons(5683)};
inet_pton(AF_INET, "192.168.1.100", &dst.sin_addr);
sendto(sock, buf, len, 0, (struct sockaddr*)&dst, sizeof(dst));

// TCP发送（需connect建立连接）
int sock = socket(AF_INET, SOCK_STREAM, 0);
connect(sock, (struct sockaddr*)&dst, sizeof(dst));
send(sock, buf, len, 0);
```

**Wireshark技巧**：`udp.port==5683`过滤CoAP；`tcp.analysis.retransmission`查看TCP重传统计，判断网络质量。

---

## Q184. HTTP/HTTPS协议区别？TLS握手流程？

### HTTP vs HTTPS

| 维度 | HTTP | HTTPS |
|------|------|-------|
| 端口 | 80 | 443 |
| 传输层 | TCP(明文) | TLS over TCP(加密) |
| 证书 | 不需要 | CA签发证书 |
| 性能 | 快(无加解密) | 慢(TLS握手+加解密开销) |
| 安全性 | 无 | 保密性+完整性+身份认证 |
| URL前缀 | http:// | https:// |

HTTPS = HTTP over TLS。TLS提供：**机密性**(对称加密)、**完整性**(HMAC/AEAD)、**身份认证**(证书链+非对称签名)。

### TLS 1.3握手流程（简化，1-RTT）

```
Client                                  Server
  |--- ClientHello(key_share, 支持的密码套件) -->|
  |<-- ServerHello(key_share) + {证书} + {Finished} --|
  |--- {Finished} ---------------------->|
  |<===== 加密应用数据 =================>|
```

相比TLS 1.2（2-RTT），TLS 1.3减少一轮往返，且移除不安全算法（RSA密钥交换、CBC模式、SHA-1等），强制前向安全性（ECDHE）。

### 证书验证链

```
根CA证书(预置在OS/浏览器)
  └── 中间CA证书(由根CA签发)
       └── 服务器证书(由中间CA签发)
```

验证步骤：检查证书链 → 检查有效期 → 检查域名匹配(CN/SAN) → 检查吊销状态(CRL/OCSP) → 用上级CA公钥验证签名。

### MCU实现考虑

- **mbedTLS** / **wolfSSL**：常用嵌入式TLS库，ROM占用50-150KB
- TLS握手的椭圆曲线运算（ECDHE）在Cortex-M4上约500ms-2s，需要合理设置超时
- 证书存储：Flash预置或从文件系统加载，注意CA证书过期更新机制
- TLS Session Resumption：使用Session ID或PSK减少握手开销
- **AT固件方案**：ESP8266/ESP32内置TLS，MCU只需发AT指令

---

## Q185. MQTT协议QoS等级(0/1/2)的区别？遗嘱消息作用？

### 三个QoS等级

```
QoS 0: 最多一次 (At most once)
----------------------------------
Publisher --> [PUBLISH] --> Broker --> Subscriber
              (fire & forget, 无确认)

QoS 1: 至少一次 (At least once)
----------------------------------
Publisher --> [PUBLISH(QoS1, pktId)] --> Broker
Publisher <-- [PUBACK(pktId)] <--------- Broker

QoS 2: 恰好一次 (Exactly once)
----------------------------------
Publisher --> [PUBLISH(QoS2, pktId)] --> Broker
Publisher <-- [PUBREC(pktId)] <--------- Broker
Publisher --> [PUBREL(pktId)] ---------> Broker
Publisher <-- [PUBCOMP(pktId)] <-------- Broker
```

- **QoS 0**：适用于高频传感器数据（温度每秒上报），丢几包无影响。无额外开销。
- **QoS 1**：适用于控制指令（开关灯），确保至少送达，但可能重复（PUBACK丢失时发布方重发）。接收方需做幂等处理（pktId去重）。
- **QoS 2**：适用于计费数据、关键告警，精确一次。四步握手开销最大，嵌入式设备慎重使用。

**注意**：QoS是发布方到Broker的保障级别，Broker到订阅方可以不同QoS。发布QoS 1，订阅方可能以QoS 0接收（取决于订阅时的QoS声明）。

### 遗嘱消息（Last Will and Testament, LWT）

遗嘱消息是客户端连接时预设的一条消息，当客户端**异常断开**(未发送DISCONNECT且Keep Alive超时)时，Broker自动将该消息发布到指定Topic。

```
CONNECT 报文:
  Will Topic:   "devices/DEV001/status"
  Will Message: "offline"
  Will QoS:     1
  Will Retain:  true
```

**核心应用场景：**

1. **设备在线/离线状态监控**：设备连接时发布`online`，遗嘱发`offline`。配合Retain标志，新订阅者立即获得最后状态。
2. **故障告警**：遗嘱消息携带故障码，Broker转发到告警Topic。
3. **资源释放**：通知其他设备当前设备掉线，触发备用方案。

### MCU实现要点

```c
// paho.mqtt.embedded-c 示例
MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
data.clientID.cstring = "DEV001";
data.keepAliveInterval = 60;
data.cleansession = 1;
data.will.topicName.cstring = "devices/DEV001/status";
data.will.message.cstring = "offline";
data.will.qos = 1;
data.will.retained = 1;
MQTTSerialize_connect(buf, buflen, &data);
```

- Keep Alive通常设为ping周期的1.5倍
- 遗嘱Retain消息需要Broker支持
- 低功耗设备唤醒后需检测Broker是否仍维持会话

---

## Q186. CoAP协议特点？与MQTT对比？

### CoAP概述

CoAP (Constrained Application Protocol, RFC 7252) 是专为资源受限设备和低功耗有损网络设计的应用层协议。

**核心特点：**
- 基于UDP（DTLS加密），头部仅4字节固定部分
- RESTful模型：GET/PUT/POST/DELETE，类似HTTP but精简
- 内置资源发现：`/.well-known/core`，类似REST API文档
- 请求/响应模型 + 内置观察(Observe)模式(订阅)
- 两种可靠性：Confirmable(CON，需ACK) / Non-Confirmable(NON)
- 支持组播(224.0.1.187)，用于设备发现

```
CoAP报文格式（4字节固定头）:
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Ver| T |  TKL  |      Code     |          Message ID           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Token (if TKL > 0) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Options (if any) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|1 1 1 1 1 1 1 1|    Payload (if any) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

### CoAP vs MQTT 对比

| 维度 | CoAP | MQTT |
|------|------|------|
| 传输层 | UDP (可选DTLS) | TCP (WebSocket可选) |
| 模型 | RESTful请求/响应 | 发布/订阅(Broker中转) |
| 可靠性 | CON消息+重传(默认4次指数退避) | QoS 0/1/2 |
| 标头开销 | 4字节固定 | 2字节固定(可变头更长) |
| 组播 | 原生支持(UDP) | 不支持 |
| 服务发现 | 内置`/.well-known/core` | 无(需外部方案) |
| 观察/推送 | OBSERVE(RFC 7641) | 订阅Topic |
| 适用场景 | 传感器直接上报，设备间直连 | 大规模设备云平台对接 |

### MCU选型建议

- **CoAP适用**：LPWAN(NB-IoT/LoRaWAN需适配)、低功耗传感器直连、小规模局域网组网
- **MQTT适用**：需要Broker解耦、大规模设备管理、与云平台(AWS IoT/Azure IoT)集成
- **混合架构**：边缘网关用CoAP收集传感器数据，网关向上用MQTT对接云平台

---

## Q187. Modbus RTU/TCP帧格式？功能码含义？

### Modbus RTU帧（串行链路，RS-485/RS-232）

```
| 地址(1B) | 功能码(1B) | 数据(N B) | CRC16(2B) |
  1-247        0x01-0x10    变长        小端
```

帧间隔要求：帧间>3.5字符时间静默（9600bps时约3.6ms），帧内字符间隔<1.5字符时间。

### Modbus TCP帧（以太网）

```
| 事务ID(2B) | 协议ID(2B,=0) | 长度(2B) | 单元ID(1B) | 功能码(1B) | 数据(N B) |
   MBAP头(7字节)                         RTU帧(去掉地址+CRC)
```

MBAP头中的长度字段=单元ID + 功能码 + 数据的总字节数。

### 常用功能码

| 功能码 | 名称 | 说明 |
|--------|------|------|
| 0x01 | 读线圈 | 读取DO位状态 |
| 0x02 | 读离散量输入 | 读取DI位状态 |
| 0x03 | 读保持寄存器 | 读取16位寄存器（最常用） |
| 0x04 | 读输入寄存器 | 读取AI数据 |
| 0x05 | 写单线圈 | 控制单路DO |
| 0x06 | 写单寄存器 | 写入1个16位寄存器 |
| 0x0F | 写多线圈 | 批量控制DO |
| 0x10 | 写多寄存器 | 批量写入寄存器 |

### 异常响应

功能码最高位置1表示异常：`0x83`=读寄存器异常。异常码跟在功能码后：0x01(非法功能)、0x02(非法数据地址)、0x03(非法数据值)、0x04(从站故障)。

### MCU实现要点

```c
// 读取保持寄存器 请求帧
uint8_t req[] = {
    0x01,       // 从站地址
    0x03,       // 功能码：读保持寄存器
    0x00, 0x10, // 起始地址 0x0010
    0x00, 0x04, // 寄存器数量 4
    0xXX, 0xXX  // CRC16(计算后填充)
};

// CRC16计算
uint16_t crc16(uint8_t *buf, int len) {
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < len; i++) {
        crc ^= buf[i];
        for (int j = 0; j < 8; j++)
            crc = (crc >> 1) ^ ((crc & 1) ? 0xA001 : 0);
    }
    return crc;
}
```

- 波特率一般9600/115200，数据位8，停止位1，无校验/偶校验
- RS-485需控制DE/RE引脚切换收发方向
- 地址0为广播地址（主站发，所有从站执行但不回复）
- FreeModbus是常用嵌入式Modbus协议栈

---

## Q188. CANopen/DeviceNet协议栈基础？

### CAN总线基础

CAN 2.0帧格式（标准帧11位ID）：

```
| SOF 1b | ID 11b | RTR 1b | IDE 1b | r0 1b | DLC 4b | Data 0-8B | CRC 15b | ACK 2b | EOF 7b |
```

### CANopen（基于CAN的高层协议，CiA 301）

CANopen在CAN数据链路层之上定义了应用层和通信规范。

**核心概念：**

1. **对象字典(OD, Object Dictionary)**：CANopen设备的核心，一个16位索引+8位子索引的查找表，包含设备所有配置和状态参数。索引结构：
   - 0x1000-0x1FFF：通信对象参数
   - 0x2000-0x5FFF：制造商特定
   - 0x6000-0x9FFF：标准化设备子协议参数

2. **四种通信对象：**
   - **NMT(网络管理)**：ID=0x000，主站控制从站状态（初始化/预操作/操作/停止）
   - **SDO(服务数据对象)**：点对点配置，确认传输，访问对象字典
   - **PDO(过程数据对象)**：实时数据广播，高优先级，无协议开销
   - **特殊对象**：SYNC(同步)、EMCY(紧急)、TIME(时间戳)

3. **CAN-ID分配（11位）**：
```
| 功能码 4b | 节点ID 7b |
```
NMT=0x000, SYNC=0x080, EMCY=0x080+NodeID, PDO1(Tx)=0x180+NodeID

4. **心跳/节点守护**：周期性发送心跳报文(0x700+NodeID, 1字节状态)，替代传统Guard机制。

### DeviceNet（基于CAN的工业总线，ODVA标准）

DeviceNet也是CAN物理层上的高层协议，但面向工业I/O和自动化设备。

**与CANopen的主要区别：**
- DeviceNet使用**预定义主从连接集**，通信基于连接ID(CID)
- 支持**UCMM(非连接消息管理器)**和**Group 2 Server**两种模式
- 使用**EDS文件**(电子数据表)描述设备能力，类似CANopen的EDS但格式不同
- 物理层使用DeviceNet专有电缆（5芯：CAN_H, CAN_L, V+, V-, Shield）

| 特性 | CANopen | DeviceNet |
|------|---------|-----------|
| 主导组织 | CiA | ODVA |
| 应用领域 | 运动控制、医疗、汽车 | 工厂自动化、I/O |
| 地址范围 | 127节点 | 64节点 |
| 配置方式 | SDO+对象字典 | 显式消息+连接 |

### MCU实现

CANopen可选用**CANopenNode**(开源C实现)或**MicroCANopen+**。对象字典通常由EDS/DCF文件通过代码生成器生成C代码。CAN控制器外设使用bxCAN(STM32)或MCP2515(SPI外部控制器)。

---

## Q189. BLE GATT服务/特征值/描述符的关系？

### GATT层级结构

```
Profile (规范，标准化设备类型)
  └── Service (服务，功能集合)
       ├── Characteristic (特征值，数据+属性)
       │    ├── Properties (读/写/通知/指示)
       │    ├── Value (实际数据字节)
       │    └── Descriptor (描述符，元数据)
       │         ├── CCCD (客户端特征配置描述符，0x2902)
       │         └── CUD (用户描述，0x2901)
       └── Characteristic ...
```

### 核心角色

- **GATT Server**：存放数据的设备（如传感器），提供Service → Characteristic → Value
- **GATT Client**：访问数据的设备（如手机），发现服务、读写特征值
- 一个设备可以同时是Server和Client

### ATT协议数据交互方式

GATT基于ATT(属性协议)：

1. **Client → Server**：Read Request / Write Request / Write Command(无响应)
2. **Server → Client**：Notification(无需确认) / Indication(需确认)
3. MTU协商：ATT_MTU影响单次传输效率，BLE 4.2起支持数据长度扩展(Data Length Extension, 最高251字节)

### 关键概念

**UUID**：16位(标准服务/特征值，如0x180D心率服务)或128位(自定义)。

**Properties位掩码**：
```
Broadcast(0x01) | Read(0x02) | WriteWithoutResponse(0x04) | Write(0x08)
| Notify(0x10) | Indicate(0x20) | SignedWrite(0x40)
```

**CCCD (0x2902)**：客户端特征配置描述符，控制是否启用Notification/Indication：
- 0x0000：关闭
- 0x0001：启用Notification
- 0x0002：启用Indication

### 实战模式：温度传感器

```
Service: 0x1809 (Health Thermometer)
  Characteristic: 0x2A1C (Temperature Measurement)
    Properties: Indicate
    Value: Flags(1B) + Temperature(IEEE 11073 float, 4B)
    Descriptor: CCCD (0x2902) → 写入0x0002启用Indicate

连接流程:
1. Client扫描→连接→发现服务
2. 找到0x1809→0x2A1C→发现CCCD
3. 写入CCCD=0x0002
4. Server每次测量后发Indication→Client必须回应Confirmation
```

### MCU实现（以nRF52 SDK为例）

```c
// 添加特征值（含CCCD）
ble_add_char_params_t char_params = {0};
char_params.uuid              = TEMP_MEAS_CHAR_UUID;     // 0x2A1C
char_params.uuid_type         = BLE_UUID_TYPE_BLE;
char_params.max_len           = 5;                       // 1+4 bytes
char_params.char_props.indicate = 1;
char_params.cccd_write_access = SEC_OPEN;                // 允许绑定

// 发送Indication
sd_ble_gatts_hvx(conn_handle, &hvx_params);
```

要点：
- Indication需要应用层处理确认(Confirmation事件)，未确认前不能发下一个
- Notification可以连续发送(3-4包/连接间隔)
- 连接间隔影响吞吐量：最小7.5ms → 理论最大约300kbps

---

## Q190. Wi-Fi配网方式(SmartConfig/BLE配网/AP配网)？

### 问题背景

IoT Wi-Fi设备(无屏幕、无键盘)如何让用户输入SSID和密码？三种主流方案：

### 1. SmartConfig（一键配网）

**原理**：手机App将SSID+密码编码到UDP广播包的长度字段中(或目标IP/端口)，设备在监听模式下抓取所有空中包，解码出Wi-Fi凭据。

```
手机App → 编码(UDP长度/地址序列) → 空中广播 → 设备监听模式抓包 → 解码SSID+密码 → 连接路由器
```

| 厂商 | 技术名 | 编码方式 |
|------|--------|----------|
| TI | SmartConfig | UDP包长度编码 |
| Espressif | ESP-TOUCH | 组播IP地址编码 |
| 微信 | AirKiss | UDP包长度 |

**优点**：用户体验好(一键完成)。
**缺点**：兼容性差(某些路由器优化广播包导致失败)、2.4GHz only、编码空间有限(长密码可能截断)。

### 2. BLE配网

**原理**：Wi-Fi设备同时开启BLE广播，手机App通过BLE连接发送SSID+密码。

```
手机App → BLE连接 → GATT Write(SSID+Password) → 设备存NVS → 连接Wi-Fi → 关闭BLE
```

**优点**：可靠性远高于SmartConfig，支持5GHz，密码无长度限制，双向通信可反馈连接状态。
**缺点**：需BLE硬件(ESP32内置，成本增加不大)；用户需打开蓝牙。

### 3. AP配网（热点配网）

**原理**：设备开机自建一个Wi-Fi热点(如`ESP_XXXXXX`)，手机连接该热点后通过HTTP/Custom UDP发送凭据。

```
设备创建AP(无密码) → 手机连接AP → 打开配网页面(或App发送) → 输入SSID+密码 → 设备切换Station模式连接路由器
```

**优点**：100%兼容，不依赖路由器特性，适用于所有Wi-Fi芯片。
**缺点**：用户操作步骤多(切Wi-Fi→配网→切回)，体验最差。

### 方案对比与MCU选型

| 方案 | 可靠性 | 用户体验 | 硬件要求 | 适用芯片 |
|------|--------|----------|----------|----------|
| SmartConfig | 中(70-85%) | 好 | Wi-Fi only | ESP8266/ESP32 |
| BLE配网 | 高(>95%) | 较好 | Wi-Fi+BLE | ESP32/BK7231N |
| AP配网 | 100% | 一般 | Wi-Fi only | 所有 |
| 声波配网 | 中 | 新奇 | 麦克风 | 不常用 |

### 最佳实践

1. **混合方案**：SmartConfig + AP配网双模，SmartConfig优先，失败后引导用户切换到AP配网。
2. **配网状态反馈**：LED闪烁模式指示(快闪=配网中, 常亮=已连接, 慢闪=失败)。
3. **配网超时**：60-120秒无结果自动切换备用方案。
4. **凭证存储**：NVS/Flash存储，重启自动重连，长按按键清除凭据进入配网模式。
5. **安全**：配网期间AP应设置密码(随机生成)，BLE配网数据应加密。

---

## Q191. LoRaWAN网络架构？Class A/B/C区别？

### 网络架构

```
End Device  →  Gateway  →  Network Server  →  Application Server
  (节点)       (网关)       (网络服务器)         (应用服务器)
    ↓            ↓            ↓                    ↓
LoRa RF   <--IP回传-->  去重/解密/下行调度    数据处理/可视化
```

1. **End Device (节点)**：传感器/执行器，Class A/B/C。
2. **Gateway (网关)**：透明转发(RF→IP)，不解析载荷，可接8-16个频道的并发接收。
3. **Network Server (NS)**：去重(多网关收到同一包)、下行调度(MAC命令)、入网管理(OTAA/ABP)、密钥管理。
4. **Application Server (AS)**：应用层数据处理，订阅MQTT接口获取上行数据。
5. **Join Server**：负责入网认证(OTAA模式)。

### 三类设备模式

```
Class A (必选，所有设备支持)
═════════════════════════════
TX(上行) → RX1(延迟1s) → RX2(延迟2s)
        └── 两个短暂接收窗口 ──┘

设备自主上行后，打开两个短暂的接收窗口。功耗最低，适用于电池供电传感器。
下行数据只能在设备上行后的窗口内发送，实时性差。

Class B (可选，增加同步信标)
═════════════════════════════
在Class A基础上，网关周期性广播Beacon(128s周期)，设备按约定时隙打开接收窗口(ping slot)。
适合需要周期性下行但不紧急的场景(远程配置)。

Class C (可选，持续监听)
═════════════════════════════
除自身发射时外，接收窗口始终打开。下行延迟最低(接近实时)，但功耗大(需持续供电)。
适用于市电供电设备(路灯控制器、插座)。
```

| 特性 | Class A | Class B | Class C |
|------|---------|---------|---------|
| 下行延迟 | 最高(等上行触发) | 中等(等Beacon时隙) | 最低(实时) |
| 功耗 | 最低(uA级休眠) | 低(mA级周期性唤醒) | 最高(持续接收) |
| 电池寿命 | 数年 | 1-2年 | 数天(非电池方案) |
| 应用 | 传感器数据采集 | 远程配置/固件升级 | 路灯/门锁实时控制 |

### 入网方式

**OTAA (Over-The-Air Activation)**：设备发起Join-Request，NS下发DevAddr + NwkSKey + AppSKey。更安全(密钥动态协商)。

**ABP (Activation By Personalization)**：设备预烧录DevAddr和密钥，直接收发。简单但固定密钥，安全性较低。

### MCU实现

常用LoRa芯片：SX1276/SX1278(SPI接口) + MCU。LoRaWAN协议栈推荐**LMIC**(IBM开源，占用约10KB RAM)或**Semtech官方LoRaMac-node**。

```c
// LMIC关键配置示例
static const lmic_pinmap lmic_pins = {
    .nss = PA4, .rxtx = LMIC_UNUSED_PIN,
    .rst = PB0, .dio = {PB1, PB2, PB3},
};
os_setCallback(&sendjob, do_send);
LMIC_setTxData2(1, mydata, sizeof(mydata), 0); // 非确认上行
```

LoRaWAN优势：远距离(2-5km城区/15km郊区)，低功耗，免授权频段(CN470/CN779, EU868, US915)。局限：低速率(0.3-50kbps)，占空比限制(如EU868需遵守1% duty cycle)，不适合实时大数据量。

---

## Q192. Zigbee Mesh组网原理？

### Zigbee协议栈

```
应用层:  ZDO(设备对象) + APS(应用支持子层)
网络层:  NWK(路由/组网/安全)
MAC层:   IEEE 802.15.4 (CSMA-CA, 信标/非信标模式)
物理层:  IEEE 802.15.4 (2.4GHz/868MHz/915MHz, O-QPSK)
```

### 三种设备类型

| 设备 | 功能 | 供电 | 典型 |
|------|------|------|------|
| Coordinator | 建网、选择信道/PAN ID、安全管理 | 长供电 | 网关 |
| Router | 转发数据、允许子设备加入、扩展网络 | 长供电(不可休眠) | 中继/灯控 |
| End Device | 终端节点，不路由，可休眠 | 电池 | 传感器 |

### Mesh组网过程

**1. 网络建立(Coordinator)**
- 选择最空闲信道(ED扫描→Active Scan)
- 确定PAN ID(16位)和扩展PAN ID(64位)
- 开始允许设备加入(permit join)

**2. 设备入网(Router/End Device)**
```
End Device → Beacon Request(广播)
Router ← Beacon(含PAN ID, 允许加入标志)
End Device → Association Request
Router ← Association Response(分配短地址16位)
End Device → NWK层建立，Router分配邻居表项
```

**3. 地址分配(分布式树形地址)**
Zigbee使用Cskip算法分布式分配16位短地址，每个Router预留地址段给子设备，无需中心协调。

**4. Mesh路由**

Zigbee Mix Routing = 树路由(层次路由) + AODV按需路由：

- **树路由**：根据地址块判定目的节点是否在自己的子树中，沿树转发。无路由发现延迟。
- **AODV (Ad-hoc On-demand Distance Vector)**：源→广播Route Request(RREQ)→目的→单播Route Reply(RREP)，沿途建立路由表项。

```
AODV路由发现示意:
S --RREQ--> R1 --RREQ--> R2 --RREQ--> D
S <--RREP-- R1 <--RREP-- R2 <--RREP-- D
路由表:S→R1, R1→R2, R2→D
```

**5. 路由修复**：链路断开时发送Route Error(RERR)，触发局部修复或源路由重建。

### 关键特性

- **最大跳数**：通常15-30跳
- **最大节点数**：理论65535，实际200-500
- **Child Table**：每个Router最多32个子设备(16 Router + 16 End Device)
- **邻居表**：存储邻接设备信息(短地址/长地址/链路质量LQI/RSSI)
- **广播事务表(BTT)**：保证广播消息的可靠传递

### MCU选型

主流方案：
- **TI CC2530/CC2538** + Z-Stack(闭源)
- **Silicon Labs EFR32MG** + EmberZNet
- **Nordic nRF52840** + ZBOSS(第三方)
- **开源替代**：Z-Stack兼容的Z-Stack-firmware

Zigbee 3.0统一了应用层规范(ZCL = Zigbee Cluster Library)，标准化设备类型(灯/开关/门锁/传感器)可跨厂商互操作。

---

## Q193. Matter协议统一了什么？

### Matter概述

Matter(原CHIP项目)是CSA联盟(Connectivity Standards Alliance, 原Zigbee联盟)推出的智能家居统一应用层协议，目标："一次开发，多生态运行"。

### Matter统一的核心

**1. 应用层统一（核心价值）**

Matter不定义新的物理层，而是运行在已有传输层之上：

```
       Matter应用层（统一的设备模型 + 交互模型）
     ┌───────────────────────────────────────────┐
     │    Matter | Matter | Matter               │
     │ over Wi-Fi│ o/Thrd │ o/BLE(配网)          │
     │  TCP/IP   │ TCP/IP │ BTP                  │
     │  Wi-Fi    │ 802.15.4│ BLE                  │
     └───────────────────────────────────────────┘
```

开发一个Matter灯泡 → 自动支持Apple HomeKit, Google Home, Amazon Alexa, Samsung SmartThings → 无需为每个生态单独开发适配。

**2. 设备类型标准化**

Matter定义了标准设备类型(Device Type)及其Cluster：
- 灯泡(On/Off Light)：OnOff Cluster, Level Control Cluster, Color Control Cluster
- 门锁(Door Lock)：Door Lock Cluster
- 温控器(Thermostat)：Thermostat Cluster
- 传感器(Temperature/Humidity/Occupancy Sensor)

**3. 交互模型**

```
Node(物理设备, 如智能灯泡)
  └── Endpoint(功能单元, 如Endpoint 0=整灯控制)
       └── Cluster(功能组, 如OnOff Cluster)
            ├── Attribute(属性, 如OnOff=TRUE/FALSE)
            ├── Command(命令, 如On/Off/Toggle)
            └── Event(事件, 如状态变化通知)
```

**4. 安全机制一致**

- 设备认证证书(DAC) + PAKE(S/PAKE)入网认证
- 每条消息使用AES-CCM加密+完整性验证
- 访问控制列表(ACL)精细权限管理
- 强制OTA安全更新支持

**5. 配网标准化**

统一使用BLE + QR码/NFC进行设备配网(Commissioning)，不需手机切Wi-Fi。

### 解决的历史痛点

| 历史问题 | Matter解决方案 |
|----------|---------------|
| 生态壁垒(HomeKit vs Alexa) | 一套代码多生态认证 |
| 网关依赖(Zigbee需专用网关) | Wi-Fi设备直连云，Thread设备需边界路由器(已内置) |
| 本地 vs 云端断开不工作 | 强制支持本地控制(控制器在局域网内) |
| 配网方式各异 | 统一BLE配网+QR码 |
| 安全标准不统一 | 统一PKI体系+DAC证书 |

### MCU实现

Matter SDK(开源，GitHub: project-chip/connectedhomeip)支持：
- ESP32系列(Espressif full-featured BSP)
- Nordic nRF52840/nRF5340(Thread+BLE)
- Silicon Labs EFR32(Thread)
- NXP K32W0x
- 资源需求：Flash > 1MB, RAM > 256KB

---

## Q194. DNS解析过程？mDNS/DNS-SD本地发现？

### DNS解析完整流程

```
应用(浏览器) → 检查浏览器缓存 → 检查hosts文件 → 
    检查本地DNS缓存 → 递归查询本地DNS服务器(ISP/114.114.114.114)

递归解析过程（本地DNS服务器）：
① Root DNS  → 返回 .com 顶级域名服务器
② TLD DNS(.com) → 返回 example.com 权威服务器
③ 权威DNS → 返回 www.example.com 的A记录(1.2.3.4)

                     Root DNS(13组根服务器)
                         ↓ 告诉去问 .com
本地DNS ←→ TLD DNS(.com服务器)
                         ↓ 告诉去问 ns.example.com
本地DNS ←→ 权威DNS(ns.example.com)
                         ↓ 返回 1.2.3.4
本地DNS → 缓存结果 → 返回给客户端
```

| 记录类型 | 含义 | 示例 |
|---------|------|------|
| A | IPv4地址 | `www A 1.2.3.4` |
| AAAA | IPv6地址 | `www AAAA 2001:db8::1` |
| CNAME | 别名 | `www CNAME example.com` |
| MX | 邮件服务器 | `MX 10 mail.example.com` |
| SRV | 服务定位 | `_mqtt._tcp SRV 0 5 1883 broker` |
| PTR | 反向解析 | `4.3.2.1.in-addr.arpa PTR www` |
| TXT | 文本信息 | `TXT "key=value"` |

### mDNS (Multicast DNS, RFC 6762)

无需集中DNS服务器，在局域网内通过组播(224.0.0.251:5353)实现域名解析。设备直接发送组播查询，目标设备单播回复。

```
查询: 谁叫 "printer.local" ?
  → PC1组播发送 → 所有设备收到
  → Printer回复(单播): 我是192.168.1.50

冲突检测(Probing):
  新设备上电 → 先发3次组播查询自己的名字
  → 如果收到回复 → 名字冲突 → 追加数字: printer-2.local
```

### DNS-SD (DNS Service Discovery, RFC 6763)

基于DNS SRV/PTR/TXT记录的服务发现，与mDNS配合使用。

```
服务发现 = PTR查询 "_services._dns-sd._udp.local"
  → 返回服务类型列表: "_http._tcp.local", "_printer._ipp._tcp.local"
  
找具体服务 = PTR查询 "_http._tcp.local"
  → 返回实例名: "MyWebServer._http._tcp.local"
  
解析实例 = SRV查询 "MyWebServer._http._tcp.local"
  → 主机: webserver.local, 端口: 80
  → TXT查询 → {"path": "/api", "version": "1.0"}
```

### 三者在物联网的应用

**Bonjour/ZeroConf = mDNS + DNS-SD**。Apple AirPrint/AirPlay、HomeKit设备发现都依赖此机制。

**MCU实现**：
- 支持mDNS/DNS-SD的轻量库：**mongoose-os mdns**、**LWIP mdns contrib**
- 典型场景：设备上电注册`_http._tcp`服务，App扫描发现设备IP和端口

```c
// 注册mDNS服务示例 (esp-idf)
mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
mdns_service_txt_item_set("_http", "_tcp", "path", "/");
mdns_service_instance_name_set("_http", "_tcp", "My Device");
```

---

## Q195. NTP时间同步原理？

### NTP工作原理

NTP(Network Time Protocol, RFC 5905)使用层级结构(Stratum 0-15)进行时间同步：

- **Stratum 0**：原子钟/GPS时钟源
- **Stratum 1**：直连Stratum 0的NTP服务器
- **Stratum 2-15**：逐级同步

### 单次同步过程（SNTP简化版）

```
Client                           Server
  |-- req(t1=客户端发送时刻) -------->| t2=服务器接收时刻
  |                                   |
  |<-- resp(t2,t3=服务器回复时刻) ---| 
  |
  t4=客户端接收时刻

计算:
  往返延迟: d = (t4 - t1) - (t3 - t2)
  时间偏移: o = [(t2 - t1) + (t3 - t4)] / 2
  
  修正: 当前时间 = 本地时间 + o
```

假设网络对称(上行延迟=下行延迟)，o公式消除路径延迟影响。

### NTP报文格式（简化）

```
NTP Timestamp: 64位
  | 整数秒(32bit, 1900-01-01 00:00:00起) | 小数秒(32bit, 精度 2^-32 ~ 233ps) |
```

NTP协议在UDP 123端口上运行，支持Client/Server模式(最常用)、Symmetric(对称对等)、Broadcast(广播)三种模式。

### 嵌入式设备NTP实现

**1. SNTP客户端(Simple NTP)**

lwIP内置sntp模块，资源占用极小：

```c
// 配置SNTP服务器
sntp_setoperatingmode(SNTP_OPMODE_POLL);
sntp_setservername(0, "ntp.aliyun.com");
sntp_setservername(1, "pool.ntp.org");  // 备用
sntp_init();

// 获取同步后的时间
void sntp_sync_callback(struct timeval *tv) {
    // tv = 1970-01-01起的Unix时间戳
}
```

**2. 同步策略**

- **冷启动**：快速连接(burst模式，每3秒一次，共4-8次)快速收敛
- **稳态**：64-1024秒周期同步一次，避免服务器负载和网络开销
- **退避**：同步失败后指数退避(1min → 2min → 4min... → max 1hour)

**3. 本地时间维护**

大部分MCU无RTC后备电池，NTP同步后需维护：

```c
uint64_t system_time_ms = ntp_timestamp * 1000; // 系统启动时的UTC基值
uint64_t sys_tick_base = xTaskGetTickCount() * portTICK_PERIOD_MS;

uint64_t get_utc_ms(void) {
    return system_time_ms + (xTaskGetTickCount()*portTICK_PERIOD_MS - sys_tick_base);
}
```

### 注意事项

- **闰秒处理**：通过"闰秒指示器"(LI)字段通告，MCU可忽略(非精密场景)
- **NTP时间 = UTC**，需加时区偏移变本地时间（中国UTC+8）
- **NTP vs GPS**：GPS时间与UTC差18秒(闰秒累计)，需换算
- **安全**：使用NTP认证(对称密钥或Autokey)防止时间投毒攻击
- **对MCU**：精度通常1-100ms足够，无需实现完整NTP算法

---

## Q196. SNMP协议在设备管理中的应用？

### SNMP概述

SNMP(Simple Network Management Protocol)是基于UDP 161(查询)/162(Trap)端口的管理协议，由IETF定义(RFC 1157/3411-3418)，是网络设备管理的工业标准。

### 核心组件

```
Manager(NMS, 网管系统) ←→ Agent(被管设备上的SNMP代理)
     ↓                        ↓
  Get/Set/Walk           MIB(管理信息库)
  Trap接收               OID树结构
```

### MIB与OID

MIB(Management Information Base)是SNMP的数据模型，以树形结构组织(OID树)：

```
iso(1).org(3).dod(6).internet(1).mgmt(2).mib-2(1)
  ├── system(1)   → .1.3.6.1.2.1.1
  │   ├── sysDescr(1)    .1.3.6.1.2.1.1.1  设备描述
  │   ├── sysUpTime(3)   .1.3.6.1.2.1.1.3  运行时间
  │   └── sysName(5)     .1.3.6.1.2.1.1.5  设备名称
  ├── interfaces(2) → .1.3.6.1.2.1.2
  └── private(4).enterprises(1) → .1.3.6.1.4.1 厂商私有MIB
```

### 五种操作类型

| 操作 | PDU类型 | 说明 |
|------|---------|------|
| GET | GetRequest | 获取单个OID值 |
| GETNEXT | GetNextRequest | 遍历MIB树(Walk) |
| GETBULK | GetBulkRequest(SNMPv2+) | 批量获取，减少交互次数 |
| SET | SetRequest | 修改OID值(配置设备) |
| TRAP | Trap/Inform | 主动告警(无需轮询) |

### SNMP版本对比

| 版本 | 安全机制 | 特点 |
|------|---------|------|
| SNMPv1 | Community String(明文) | 最简单，不推荐 |
| SNMPv2c | Community String | 增加GETBULK和INFORM，常用 |
| SNMPv3 | USM(用户安全模型) + VACM(视图访问控制) | 认证+加密+访问控制，推荐 |

### 嵌入式设备实现

**开源库**：Agent++/SNMP++(C++), Net-SNMP(C, 太重), lwip-snmp(轻量)。

```c
// 自定义MIB节点(温度传感器)
// OID: .1.3.6.1.4.1.99999.1.0
static s32_t temperature_value;
static struct snmp_scalar_node_def temperature_node = {
    .oid = 1,
    .node_type = SNMP_NODE_SCALAR,
    .access = SNMP_NODE_ACCESS_READONLY,
    .get_value = temperature_get_value,
    .set_value = NULL,
};

// GET 回调
s32_t *temperature_get_value(void) {
    temperature_value = read_ds18b20() * 10; // 定点小数 25.5→255
    return &temperature_value;
}
```

### 实际应用场景

1. **交换机/路由器管理**：端口流量统计、错误包计数、VLAN配置
2. **UPS监控**：电池电量、负载百分比、旁路状态
3. **IoT设备运维**：在线状态、固件版本、信号强度、内存使用
4. **告警管理**：Trap推送过温/掉线/电压异常到NMS
5. **批量配置**：通过SNMP SET统一配置全部设备的NTP服务器地址

### 与IoT方案的对比

| 场景 | SNMP | MQTT/CoAP |
|------|------|-----------|
| 轮询监控 | 适合(GET大量数据) | 不适合(需发布/订阅转换) |
| 实时告警 | Trap适合 | 订阅适合 |
| 低功耗 | 不适合(轮询耗电) | 适合 |
| 安全 | SNMPv3较好 | TLS/DTLS基础好 |
| 工具生态 | MRTG/Zabbix/Cacti丰富 | 相对少 |

---

## Q197. WebSocket全双工通信原理？

### WebSocket协议升级

WebSocket通过HTTP/1.1的Upgrade机制将HTTP连接升级为全双工持久连接(RFC 6455)：

```
Client → Server (HTTP Upgrade请求):
  GET /chat HTTP/1.1
  Host: server.example.com
  Upgrade: websocket
  Connection: Upgrade
  Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
  Sec-WebSocket-Version: 13

Server → Client (101 Switching Protocols):
  HTTP/1.1 101 Switching Protocols
  Upgrade: websocket
  Connection: Upgrade
  Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
  (Accept = base64(sha1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11")))
```

### WebSocket帧格式

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length           |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)                  |
|N|V|V|V|       |S|             | (if payload len==126/127)            |
| |1|2|3|       |K|             |                                      |
+-+-+-+-+-+-+-+-+-------+-+-------------+-------------------------------+
|     Masking-key(4B, if MASK=1)     |   Payload Data                  |
+------------------------------------+----------------------------------+
```

**关键字段：**
- **FIN(1b)**：是否为最后一帧（支持分片）
- **opcode(4b)**：0x1=文本, 0x2=二进制, 0x8=关闭, 0x9=Ping, 0xA=Pong
- **MASK(1b)**：客户端-服务器必须掩码(防缓存投毒)
- **Payload len(7b)**：<126=实际长度, 126=后续2字节, 127=后续8字节

### 与HTTP长轮询对比

| 特性 | WebSocket | HTTP长轮询 | SSE |
|------|-----------|-----------|-----|
| 通信方向 | 全双工 | 半双工(请求驱动) | 服务器→客户端单向 |
| 连接数 | 1个 | 每次轮流新建 | 1个 |
| 头部开销 | 2-14字节/帧 | 每次HTTP头部(数百字节) | 小 |
| 适用场景 | 实时双向(聊天/游戏) | 兼容旧浏览器 | 实时推送(股票) |

### MCU实现

**客户端集成（MCU主动连接云平台）：**

```c
// WebSocket帧构建（简化）
void ws_send_text(int sock, const char *msg) {
    size_t len = strlen(msg);
    uint8_t header[10];
    int hdr_len = 2;
    header[0] = 0x81; // FIN + text opcode
    if (len < 126) {
        header[1] = len | 0x80; // MASK=1
    } else if (len <= 0xFFFF) {
        header[1] = 126 | 0x80;
        header[2] = (len >> 8) & 0xFF;
        header[3] = len & 0xFF;
        hdr_len = 4;
    }
    // 生成4字节随机mask
    uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78};
    memcpy(header + hdr_len, mask, 4);
    send(sock, header, hdr_len + 4, 0);
    
    // 掩码后发送payload
    uint8_t masked[256];
    for (int i = 0; i < len; i++) masked[i] = msg[i] ^ mask[i % 4];
    send(sock, masked, len, 0);
}
```

**嵌入式WebSocket库**：
- **libwebsockets**：C语言，支持客户端/服务端，依赖少
- **mongoose**：轻量，单文件集成
- **wolfSSL websocket**：与TLS深度集成

### 应用场景

- **Web仪表盘**：MCU通过WebSocket向网页实时推送传感器数据
- **远程Shell**：嵌入式设备Web终端(类似阿里云IoT Platform的远程调试)
- **OTA进度推送**：升级过程中实时汇报下载/烧录百分比
- **设备控制**：网页按钮→WebSocket→MCU→GPIO控制

---

## Q198. Protobuf vs JSON序列化效率对比？

### Protobuf (Protocol Buffers)

Google开源的高效二进制序列化格式。基于`.proto`文件定义消息结构，通过编译器生成语言特定代码。

```protobuf
// sensor.proto
syntax = "proto3";
message SensorData {
  fixed32 device_id = 1;
  float temperature = 2;
  int32 humidity = 3;
  uint64 timestamp = 4;
}
```

**编码原理（Varint + Wire Type）**：

```
字段编码: | field_number(5b) | wire_type(3b) |
Varint: 每字节最高位=1表示后续还有字节，=0表示结束
        值 300 → 0xAC 0x02 → 10101100 00000010
                             └──后续──┘ └──最后──┘

固定长度(fixed32/fixed64): 直接4或8字节，小端序
Length-delimited(string/bytes/嵌套消息): wire_type + 长度前缀 + 内容
```

### JSON

文本格式，人类可读，跨语言通用，但无Schema约束，解析成本高。

```json
{"device_id":12345,"temperature":25.6,"humidity":60,"timestamp":1710000000000}
```

### 性能对比（同一数据）

| 维度 | Protobuf | JSON |
|------|----------|------|
| 序列化后大小 | ~14字节 | ~75字节 |
| 序列化速度 | ~200ns (C++) | ~2us (C++) |
| 解析速度 | ~300ns | ~5us |
| RAM使用(解析中) | 低(流式解码) | 高(需存储字符串) |
| 数字精度 | 精确(float/double/varint) | 所有数字均为double |
| 向前/向后兼容 | 原生支持(字段编号不变) | 需手动处理 |
| Schema | 有(强制, .proto) | 无(灵活但易出错) |
| 人类可读 | 否(需工具) | 是 |

### MCU上的序列化模式

**二进制协议（无Protobuf编译器依赖）**：

```c
typedef struct __attribute__((packed)) {
    uint8_t  header;     // 0xAA 帧头
    uint8_t  type;
    uint16_t len;        // 载荷长度
    uint32_t device_id;
    float    temperature;
    int16_t  humidity;   // 0.1C精度, 实际值*10
    uint32_t timestamp;  // Unix时间戳
    uint16_t crc;        // CRC16校验
    uint8_t  tail;       // 0x55 帧尾
} sensor_pkt_t;
```

### 选择指南

**选Protobuf的场景：**
- 设备与云端通信(AWS IoT, Google Cloud IoT Core原生支持)
- 需要严格的Schema定义和数据验证
- 带宽或存储严格受限(如LoRaWAN载荷、Flash存储)
- 协议需要迭代升级(字段标注reserved/deprecated)

**选JSON的场景：**
- RESTful API调试(可读性重要)
- Web前端直接解析
- 灵活的动态结构
- 开发初期快速迭代

**嵌入式Protobuf实现**：
- **nanopb**：针对MCU优化的Protobuf C库，Flash占用<10KB
  ```c
  // 编码: 用回调流式输出
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  pb_encode(&stream, SensorData_fields, &message);
  ```
- **protobuf-c**：通用C实现，较大

**Protobuf在嵌入式中的挑战**：
1. 需要离线生成C/H文件(protoc + nanopb插件)
2. 默认不支持真正的零拷贝(需手动管理buffer)
3. 嵌套消息和repeated字段占用栈空间
4. 与MQTT/CoAP集成时需额外封装

---

## Q199. 自定义二进制协议设计原则（帧头/长度/校验/尾）？

### 协议分层设计

```
|<--- 帧头 1-2B --->|<--- 版本/标志 1B --->|<--- 命令/类型 1B --->|
|<--- 序列号 1-2B --->|<--- 长度 2B --->|<--- 载荷 N bytes --->|
|<--- CRC/校验 2-4B --->|<--- 帧尾 1-2B --->|
```

### 各字段设计原则

**1. 帧头 (Start-of-Frame)**
- 2字节魔数：`0xAA 0x55`(交替0/1，便于示波器观察)、`0xEB 0x90`
- 避免与载荷数据冲突：通过长度字段判断是否真帧头(长度合理)、CRC校验确认
- 可选：帧头+版本组合验证（`0xAA 0x01`=V1, `0xAA 0x02`=V2）

**2. 版本/标志位**
- 前4bit=协议版本，后4bit=标志位(ACK标志、加密标志、分包标志)
- 支持协议升级同时兼容旧设备

**3. 序列号**
- 自动递增(0-255循环)，用于请求-应答匹配和重复包检测
- 2字节可减少循环碰撞概率

**4. 长度字段**
- 通常2字节(最大65535)够用，但不建议太大(MCU RAM限制)
- **长度定义一致性**：是整个帧长度？还是载荷长度？文档必须明确
- **防御检查**：`if(len > MAX_PAYLOAD) { return ERROR; }`

**5. CRC/校验**
- **CRC16/CRC32**：检测随机错误(误码率场景)
- **CRC16-CCITT多项式(0x1021)**，初始值0xFFFF，反射输入输出
- **校验范围**：从帧头到校验字段之前的所有字节，包含长度但不含自身
- **特殊情况**：如果是白名单可靠传输(如USB/UART直连)，可用累加和或XOR校验

**6. 帧尾**
- 提供帧边界二次确认(如`0x0D 0x0A`，模拟回车换行便于串口调试)
- 可选：非必须，CRC足够可靠时省去

### 完整实现示例

```c
#define FRAME_HEADER1  0xAA
#define FRAME_HEADER2  0x55
#define FRAME_TAIL     0x0D
#define MAX_PAYLOAD    256

typedef enum { CMD_HEARTBEAT=0x01, CMD_DATA=0x02, CMD_ACK=0x03, CMD_NACK=0x04 } cmd_t;

typedef struct __attribute__((packed)) {
    uint8_t  head1;
    uint8_t  head2;
    uint8_t  version_flags; // [version:4|flags:4]
    cmd_t    cmd;
    uint8_t  seq;
    uint16_t payload_len;   // 大端
    uint8_t  payload[];
} frame_t;
// 帧尾 + CRC 在 payload 之后

uint16_t calc_crc16(const uint8_t *buf, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)buf[i] << 8;
        for (int j = 0; j < 8; j++)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
    }
    return crc;
}

// 打包发送
int frame_send(const uint8_t *data, uint16_t len, cmd_t cmd, uint8_t seq) {
    uint8_t buf[sizeof(frame_t) + MAX_PAYLOAD + 3]; // +CRC16 +Tail
    frame_t *frame = (frame_t *)buf;
    
    frame->head1 = FRAME_HEADER1;
    frame->head2 = FRAME_HEADER2;
    frame->version_flags = (1 << 4) | 0; // version=1, flags=0
    frame->cmd = cmd;
    frame->seq = seq;
    frame->payload_len = htons(len);
    memcpy(frame->payload, data, len);
    
    size_t crc_pos = sizeof(frame_t) + len;
    uint16_t crc = calc_crc16(buf, crc_pos);
    memcpy(buf + crc_pos, &crc, 2); 
    buf[crc_pos + 2] = FRAME_TAIL;
    
    return uart_send(buf, crc_pos + 3);
}
```

### 状态机解析器

```
States: IDLE → HEAD1_OK → HEAD2_OK → VERSION → CMD → SEQ → LEN_H → LEN_L → PAYLOAD → CRC_H → CRC_L → TAIL_OK

环形缓冲 + 状态机是无OS MCU上最可靠的解析方式。
每个状态读一个字节，不符合预期则回退到IDLE（但需处理帧头在载荷中出现的误触发）。
```

### 设计禁忌

1. 不要使用可变长度字段(除payload外) —— 增加解析复杂度
2. 不要依赖固定间隔(如Modbus RTU的3.5字符静默)在RTOS下不可靠
3. 不要忽略字节序(Endianness) —— 明确规定网络字节序(大端)或小端
4. 不要设计`payload_len=0`却仍有payload的歧义逻辑
5. 始终在文档中画帧格式图，标注每个字段的位宽和字节序

---

## Q200. 串口透传协议的可靠性保障机制？

### 什么是串口透传

串口透传指通过串口(UART/RS-485/RS-232)在两个设备间透明传输数据，常见于DTU(数据透传单元)、蓝牙串口模块、Zigbee透传模块。特点是"无协议处理"，发什么收什么。

### 串口通信的不可靠性来源

- 电磁干扰导致位错误(UART无纠错，只有奇偶校验)
- 缓冲区溢出(接收方处理不过来)
- 波特率偏差(晶振误差导致采样偏移)
- 连接松动、接触不良

### 可靠性保障方案

**方案一：在透传层之上增加确认重传（ACK/NACK协议）**

```
发送方                           接收方
  |---- DATA(seq=n, payload) ---> | 
  |<--- ACK(seq=n) ------------- |
  |  (超时未收到ACK→重传)         |
  |---- DATA(seq=n, payload) ---> | (重传)
  |<--- ACK(seq=n) ------------- |
```

```c
#define RETRY_MAX   3
#define TIMEOUT_MS  200

bool reliable_send(const uint8_t *data, uint16_t len) {
    uint8_t seq = get_next_seq();
    for (int retry = 0; retry < RETRY_MAX; retry++) {
        send_frame(FRAME_DATA, seq, data, len);
        if (wait_ack(seq, TIMEOUT_MS) == 0) return true;
    }
    return false;
}
```

**方案二：滑动窗口 + 批量确认**

对于高速透传，单一ACK效率低。滑动窗口允许连续发送多个包后统一确认：

```
发送方 -> [seq=1] [seq=2] [seq=3] [seq=4]
接收方 -> ACK(累计确认seq=3，表示1,2,3都收到)
发送方 -> [seq=4重传] [seq=5]  (seq=3之前已确认，滑动窗口前移)
```

**方案三：FEC前向纠错**

在数据后附加冗余纠错码，接收方可自行纠正少量错误，无需重传。

- **Reed-Solomon编码**：适合突发错误(burst error)
- **汉明码(Hamming Code)**：纠正1位错误，检测2位错误
- **简单重复发送**：同一数据发3次，接收方3取2表决

```
数据: [A B C D] → RS(8,4)编码 → [A B C D P1 P2 P3 P4]
任意4个字节可恢复原始数据（容忍任意4字节丢失）
```

**方案四：XMODEM/YMODEM协议**

经典的文件传输协议，128/1024字节块+CRC校验+ACK/NAK机制：

```
SOH | 块号(1B) | 块号反码(1B) | 数据(128B) | CRC(1B)
```

HCI透传适配：蓝牙HCI UART层使用H4协议，Type(1B)+Payload，无确认机制，依赖上层HCI的重传逻辑。

### 硬件层面的可靠性增强

| 方法 | 说明 |
|------|------|
| 流控(RTS/CTS) | 硬件流控，防止缓冲区溢出 |
| 奇偶校验 | 检测单比特错误，不可纠正 |
| RS-485接口 | 差分信号，抗共模干扰 |
| 终端匹配电阻 | 120Ω消除反射 |
| 光耦隔离 | 隔离地线噪声 |
| TVS/ESD保护 | 保护IO口 |

### MCU实现要点

```c
// DMA+空闲中断接收整帧
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    // Size=接收到的字节数
    // 在空闲中断中处理完整帧
    if (Size > 0) {
        process_frame(rx_dma_buf, Size);
    }
    HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_dma_buf, RX_BUF_SIZE);
}
```

### 总结：可靠性层次

| 层次 | 机制 | 适用场景 |
|------|------|----------|
| 基本 | 波特率匹配 + 奇偶校验 | 短距PCB板内UART |
| 传输层 | ACK/NAK + 序列号 + 超时重传 | 模块间通信 |
| 会话层 | 滑动窗口 + 批量确认 | 高速透传 |
| 纠错层 | FEC / RS编码 | 无线/噪声环境 |
| 文件级 | XMODEM / ZMODEM | File Transfer |
| 硬件级 | RS-485 + 流控 + 隔离 | 工业现场 |

---

## Q201. HTTP/2多路复用

### HTTP/1.1的瓶颈

HTTP/1.1虽然支持持久连接(Keep-Alive)，但存在队头阻塞(Head-of-Line Blocking)：一个连接同一时间只能处理一个请求-响应，前一个慢请求阻塞后续所有请求。浏览器通过开启6-8个并发TCP连接缓解，但增加连接建立成本。

### HTTP/2核心改进

**1. 二进制分帧层（Binary Framing Layer）**

HTTP/2在单个TCP连接上建立二进制分帧层，将HTTP消息分解为独立的帧(Frame)，交错发送：

```
Stream 1: [HEADERS] [DATA] [DATA]         (GET /index.html)
Stream 3: [HEADERS] [DATA]                 (GET /style.css)
Stream 5: [HEADERS] [DATA] [DATA] [DATA]  (GET /app.js)

TCP发送顺序(交织): H1 D1 H3 D3 H5 D1 D5 H1 D5 D5
```

帧类型：HEADERS, DATA, PRIORITY, RST_STREAM, SETTINGS, PUSH_PROMISE, PING, GOAWAY, WINDOW_UPDATE, CONTINUATION

**2. 多路复用（Multiplexing）**

一个TCP连接承载多个并发Stream(逻辑通道)，每个Stream有独立ID(奇数=客户端发起，偶数=服务端推送)。彻底解决HTTP/1.x队头阻塞。

**3. 头部压缩（HPACK）**

使用静态字典(61个常见头)+动态字典+哈夫曼编码，压缩重复的HTTP头部。后续请求只发送差异部分，头部开销降低85%+。

**4̥X服务端推送（Server Push）**

服务器可主动推送客户端尚未请求但即将需要的资源(PUSH_PROMISE帧)：
```
Client -> GET /index.html
Server -> PUSH_PROMISE(style.css) + DATA(index.html) + DATA(style.css)
```

### MCU上的HTTP/2

- **nghttp2**：C语言HTTP/2库，支持客户端/服务端
- 嵌入式场景通常不需要完整的HTTP/2多路复用(MCU多为单一数据流)
- 但可以利用HPACK压缩减少MQTT-over-WebSocket头部开销
- AWS IoT Core支持HTTP/2连接(MQTT已迁移到MQTT5，部分场景仍可用)

### 与HTTP/1.1对比

| 特性 | HTTP/1.1 | HTTP/2 |
|------|----------|--------|
| 连接模型 | 每连接一请求 | 一连接多Stream |
| 头部 | 明文文本 | 二进制压缩(HPACK) |
| 队头阻塞 | 连接级 | 仅TCP层(HTTP/3解决) |
| 服务器推送 | 不支持 | 支持 |
| 流量控制 | TCP级 | Stream级+连接级 |

---

## Q202. QUIC协议

### 设计目标

QUIC(Quick UDP Internet Connections)是Google设计、已标准化为RFC 9000的传输层协议，解决TCP+TLS的痛点，是HTTP/3的基础传输层。

### 核心特性

**1. 0-RTT / 1-RTT连接建立**

```
首次连接(1-RTT):                        重复连接(0-RTT):
Client -> ClientHello                     Client -> ClientHello + 加密数据
Server -> ServerHello + 加密数据           Server -> ServerHello + 加密数据

对比: TCP+TLS1.3需2-3RTT, QUIC仅1RTT, 0-RTT可立即发送数据
```

**2. 消除队头阻塞**

TCP的队头阻塞是连接级的：一个丢包阻塞所有后续数据(即使属于不同流)。QUIC基于UDP，每个Stream独立传输，丢包仅影响该Stream：

```
Stream 1: [P1] [P2丢失] [P3]  <- 阻塞Stream 1但不阻塞Stream 2
Stream 2: [Q1] [Q2] [Q3]      <- 正常传输
```

**3. 连接迁移（Connection Migration）**

QUIC使用64位Connection ID标识连接，而非TCP的四元组(IP+Port)。即使客户端IP切换(Wi-Fi→4G)，只要CID不变，连接无缝保持。对移动设备(IoT网关)意义重大。

**4. 加密全面覆盖**

QUIC强制加密，相当于"TLS 1.3内置于传输层"。几乎所有包(包括ACK)都加密，仅少量包明文(版本协商、Initial包部分)。

### QUIC数据包结构

```
QUIC长包头(Initial/Handshake):
| Header Form(1b, Long=1) | Fixed Bit(1b) | Long Packet Type(2b) |
| Type-Specific Bits(4b) | Version(32b) |
| DCIL(8b) | Dest Connection ID(0-20B) |
| SCIL(8b) | Src Connection ID(0-20B) |
| Length(变长) | Packet Number(变长) | Payload(加密) |

QUIC短包头(1-RTT):
| Header Form(1b, Short=0) | Fixed Bit(1b) | Spin Bit(1b) |
| Reserved(2b) | Key Phase(1b) | Packet Number(2b) |
| Dest Connection ID(变长) | Payload(加密) |
```

### MCU/嵌入式应用

- QUIC实现复杂度高，MCU原生支持较少
- **picoquic** / **quiche**(Cloudflare) / **lsquic**(LiteSpeed)是C语言实现
- 在资源充足的嵌入式Linux网关(ARM Cortex-A + RAM > 64MB)上可使用
- 5G IoT模组(如移远RG500Q)支持QUIC硬件加速
- QUIC在IoT场景的优势：连接迁移(移动/漫游场景)、低延迟建连

---

## Q203. gRPC在嵌入式的应用

### gRPC概述

gRPC是Google开源的高性能RPC框架，基于HTTP/2传输 + Protobuf序列化，支持四种服务模式：一元RPC、服务端流式、客户端流式、双向流式。

```protobuf
service SensorService {
  rpc ReportData(SensorData) returns (Ack);             // 一元RPC
  rpc StreamData(stream SensorData) returns (Ack);      // 客户端流式
  rpc SubscribeAlerts(Empty) returns (stream Alert);    // 服务端流式
  rpc Chat(stream Message) returns (stream Message);    // 双向流式
}
```

### 嵌入式gRPC实现

**nanopb + gRPC-lite / microRPC**：

由于完整gRPC依赖完整HTTP/2，在MCU上不可行。替代方案：

1. **nanopb** + 自定义RPC层：用nanopb序列化，通过UART/SPI/CAN传输，自建请求ID匹配
2. **Embedded RPC(erpc)**：NXP开源，适合MCU间RPC，支持多种传输层(UART/SPI/USB)
3. **gRPC-Gateway**方案：MCU用MQTT/CoAP上报到边缘网关，网关对外提供gRPC服务

### 实际场景

```
MCU(传感器集合) -> MQTT -> 边缘网关(ARM Linux)
                              ├── gRPC Server (对外API)
                              ├── RESTful API (gRPC-Gateway自动生成)
                              └── MQTT Bridge (内部订阅MCU数据)

云端/移动App通过gRPC调用边缘网关获取数据，网关从MQTT订阅实时传感器数据。
```

### 优势与局限

| 优势 | 局限 |
|------|------|
| 强类型接口(.proto定义) | MCU上难原生运行完整gRPC |
| 自动生成客户端/服务端代码 | HTTP/2开销大 |
| 双向流式传输 | 需代理网关架构 |
| 多语言互操作 | 调试复杂(二进制) |
| 内置负载均衡、拦截器 | 学习曲线陡峭 |

### 在IoT边缘的应用

```
边缘网关(如树莓派/Gateway): 运行gRPC Server
  └── DeviceService: 设备注册/管理
  └── DataService: 历史数据查询(服务端流式)
  └── OTA Service: 固件升级(客户端流式上传文件)
  └── CommandService: 设备控制指令
  
所有接口通过protoc + nanopb-generator自动生成
```

---

## Q204. AMQP协议

### AMQP概述

AMQP(Advanced Message Queuing Protocol, 高级消息队列协议)是ISO/IEC 19464标准的面向消息的中间件协议，RabbitMQ是其最著名的实现。

### AMQP 0-9-1模型

```
Publisher → Exchange → [Routing Key匹配] → Queue → Consumer
                       ├─ Queue1(binding key: error.*)
                       └─ Queue2(binding key: *.log)
```

核心概念：
- **Exchange**：消息路由器，根据routing key分发
- **Queue**：消息缓冲区(FIFO)
- **Binding**：Exchange→Queue的绑定规则
- **Virtual Host(vhost)**：逻辑隔离单元(类似MQTT的topic前缀空间)

### 四种Exchange类型

| 类型 | 路由规则 | 例子 |
|------|---------|------|
| Direct | routing key精确匹配 | `key="error" → Queue_Error` |
| Topic | routing key通配符匹配 | `key="sensor.temp.#"` |
| Fanout | 广播到所有绑定Queue | 忽略routing key |
| Headers | 根据消息Header属性匹配 | `x-match=all, format=json` |

### AMQP报文结构

```
AMQP帧:
| Frame Type(1B) | Channel(2B) | Size(4B) | Payload(N B) | Frame End(1B, 0xCE) |

Method帧 Payload 示例 (Basic.Publish):
| Class ID(2B, 0x003C) | Method ID(2B, 0x0028) |
| ticket(2B, 0) | exchange(shortstr) | routing-key(shortstr) |
| mandatory(1b) | immediate(1b) |
```

### AMQP vs MQTT

| 维度 | AMQP | MQTT |
|------|------|------|
| 设计目标 | 企业消息中间件 | 物联网轻量级消息 |
| 复杂度 | 高(帧类型多、Channel管理) | 低(仅14种报文) |
| 消息路由 | 丰富(Exchange+Binding) | 仅Topic订阅 |
| QoS机制 | 确认+事务 | QoS 0/1/2 |
| 安全性 | SASL认证 | 用户名密码+TLS |
| 协议开销 | 大(帧头8字节) | 小(固定头2字节) |
| 适用场景 | 金融交易、企业ESB | IoT设备、移动推送 |

### 嵌入式应用

AMQP在MCU上直接实现极少(协议复杂度高)。通常方案：
- **Edge Gateway**：运行RabbitMQ Client(如amqp-cpp)，MCU通过MQTT/Modbus连接到网关
- **RabbitMQ MQTT Plugin**：RabbitMQ内置MQTT适配，IoT设备可用MQTT协议，消息在AMQP生态中流转
- **Azure IoT Hub**：原生支持AMQP(AMQP 1.0)，但也提供MQTT适配

```python
# AMQP客户端示例 (pika/RabbitMQ)
channel.exchange_declare(exchange='sensors', exchange_type='topic')
channel.queue_declare(queue='temp_alerts')
channel.queue_bind(exchange='sensors', queue='temp_alerts', routing_key='temp.alert.#')
channel.basic_publish(exchange='sensors', routing_key='temp.alert.high', body='35.5C')
```

---

## Q205. OPC UA工业协议

### OPC UA概述

OPC UA(OPC Unified Architecture, IEC 62541)是工业4.0核心通信协议，替代传统OPC Classic(基于COM/DCOM)，提供平台无关、安全的工业数据交换标准。

### 核心特点

```
客户端/服务端架构:
  SCADA/MES -> OPC UA Client -> OPC UA Server -> PLC/传感器/驱动器

关键技术:
  ├── 地址空间模型(Address Space): 面向对象的数据模型
  ├── 信息模型(Information Model): 语义化(节点ID, 类型, 引用)
  ├── 安全模型: 证书认证 + 加密 + 签名 + 用户/角色
  ├── 传输: UA TCP(二进制, 高效) 或 HTTPS/SOAP(Web服务)
  └── Pub/Sub扩展: 支持UDP组播(大规模数据分发)
```

### 地址空间模型

```
UA Server地址空间:
├── Objects/
│   ├── Device1/
│   │   ├── Temperature (Variable, Int16, 当前值)
│   │   ├── Config (Object)
│   │   │   └── SamplingRate (Variable, UInt32)
│   │   └── SetAlarmThreshold (Method, 可调用)
│   └── Device2/
├── Types/
│   └── ... (数据类型定义)
└── Views/
    └── MaintenanceView (按角度组织数据)
```

每个节点(Node)由NodeId(标识符)、NodeClass(类型：Object/Variable/Method等)、Attributes(属性)和References(与其他节点的关系)组成。

### 安全机制

OPC UA内置多层安全：
1. **应用层认证**：X.509证书交换+签名
2. **用户认证**：用户名/密码、X.509证书、Kerberos
3. **传输安全**：AES-256/SHA-256加密(高于TLS独立实现)
4. **会话管理**：SecureChannel概念，可续期

### 信息模型与配套规范

OPC UA定义了一系列配套规范(Companion Specifications)：
- **PLCopen**：PLC功能块映射
- **EUROMAP 77/83**：塑料/橡胶机械
- **VDMA**：机器人、机床
- **PackML**：包装机械

### MCU/嵌入式实现

- **open62541**：开源C99实现，可在STM32F4+(FreeRTOS+lwIP)上运行，Flash约300KB，RAM约50KB
- **UA-.NETStandard**：.NET平台(不适用纯MCU)
- 协议二进制编码快、占用小(对比XML/SOAP)

```c
// open62541 创建温度变量示例
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
attr.accessLevel = UA_ACCESSLEVELMASK_READ;
UA_Int16 temp = 25;
UA_Variant_setScalar(&attr.value, &temp, &UA_TYPES[UA_TYPES_INT16]);

UA_NodeId tempNodeId = UA_NODEID_STRING(1, "temperature");
UA_QualifiedName tempName = UA_QUALIFIEDNAME(1, "Temperature");
UA_Server_addVariableNode(server, tempNodeId, parentNodeId,
    UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
    tempName, UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
```

### 应用场景

- **SCADA/HMI**：生产数据可视化
- **MES集成**：生产执行系统获取实时数据
- **设备诊断**：远程访问PLC诊断信息
- **云连接**：通过UA Pub/Sub对接AWS IoT / Azure IoT Central

---

## Q206. EtherCAT实时以太网

### 概述

EtherCAT(Ethernet for Control Automation Technology)是由Beckhoff开发的实时工业以太网协议，属于IEC 61158标准。核心创新："飞读飞写"(Processing on the Fly)。

### 工作原理

```
Master → EtherCAT Frame(1个以太网帧承载所有从站数据)
         ┌─────────────────────────────────────────────┐
         │ ECAT Hdr | Slave1 OUT | Slave1 IN | Slave2  │
         │           |  (写数据)  |  (读数据)  | OUT|IN | ...
         └─────────────────────────────────────────────┘
                    ↓             ↓            ↓
             Slave1: 读取OUT   Slave2: 读取OUT  
                     写入IN            写入IN
                     硬件即时处理      硬件即时处理
                    ↓             ↓            ↓
         <- 帧回传到Master（Slave1/2已将IN数据更新到帧中）
```

**核心优势**：帧经从站时，从站高速硬件(ESC, EtherCAT Slave Controller)直接提取/插入数据，帧不停止、不缓存，延迟极低(单从站<1us，1000节点~30us)。

### 协议栈层次

```
应用层: CoE(CANopen over EtherCAT), SoE(SERCOS), EoE(Ethernet), FoE(File over EtherCAT)
数据链路层: EtherCAT协议(帧类型0x88A4)
物理层: 标准以太网(100BASE-TX), EBUS(LVDS, 背板)
```

### FMMU与SyncManager

ESC芯片内置FMMU(现场总线内存管理单元)和SyncManager：
- **FMMU**：将逻辑地址空间映射到物理内存地址，支持位级映射
- **SyncManager**：管理主站-从站的数据一致性(双缓冲切换)

### 分布式时钟(DC)

EtherCAT支持<1us精度的分布式时钟同步，通过以下机制：
1. Master选择DC参考时钟(第一个有DC能力的从站)
2. 广播测量传输延迟和时钟偏移
3. 从站硬件补偿延迟，全部同步到参考时钟

### MCU实现

- 从站需专用ESC芯片(如Beckhoff ET1100/ET1200, Microchip LAN9252)或内置ESC的MCU(Infineon XMC4800, TI AM335x PRU-ICSS)
- 裸机/RTOS上运行EtherCAT从站协议栈：**SOEM**(开源的Simple Open EtherCAT Master) / **SSC**(Beckhoff官方从站代码生成器)
- CoE模式最常用：复用CANopen对象字典，通过SDO配置、PDO映射过程数据

### 与其他工业以太网对比

| 特性 | EtherCAT | PROFINET IRT | Ethernet/IP |
|------|----------|-------------|-------------|
| 拓扑 | 菊花链/环形 | 星型(需交换机) | 星型 |
| 周期时间 | 50us-1ms | 250us-1ms | 1-10ms |
| 时钟同步 | DC(<1us) | PTCP | IEEE 1588 |
| 从站成本 | 低(ESC芯片) | 高(专用ASIC) | 低(标准MAC+软件) |
| 应用 | 运动控制、机器人 | 流程自动化 | 离散制造 |

---

## Q207. PROFINET

### 概述

PROFINET是PI组织(PROFIBUS & PROFINET International)推出的工业以太网标准，是PROFIBUS的以太网演进版本。PROFINET不是固定协议，而是两类模式：

### PROFINET RT (Real-Time, 软实时)

基于标准以太网硬件，通过优先级(VLAN 802.1Q, 优先级6)和优化的软件协议栈实现1-10ms周期：

```
标准以太网帧(PROFINET RT):
| MAC DA | MAC SA | VLAN(0x8100, Prio=6) | EtherType(0x8892) | PN RT Data | FCS |

数据区包含:
  └── FrameID(标识数据类型: 实时/告警/非周期)
  └── 周期过程数据(IO Data)
  └── Provider/Consumer Status
```

基于Provider/Consumer模型，无需主站轮询，IO设备主动推送数据。

### PROFINET IRT (Isochronous Real-Time, 硬实时)

需专用硬件(ASIC如西门子ERTEC)，将通信周期划分为IRT通道(确定性同步)和开放通道(非实时TCP/IP)：

```
IRT周期划分:
|<── IRT Phase(硬实时) ──>|<── Open Phase(TCP/IP) ──>|<── IRT Phase ──>
    同步运动控制/安全           诊断/Web/配置              下一个周期
```

周期时间可低至31.25us，抖动<1us。依赖精确的PTCP(Precision Transparent Clock Protocol)时间同步。

### PROFINET应用关系

```
IO Controller (PLC) <-> IO Device (驱动器/IO模块/编码器)
    ├── AR(Application Relation): 设备间通信关系
    ├── CR(Communication Relation): 具体通道(IOCAR/IOXAR/AlarmCR/RecordCR)
    │    ├── IOCAR: 周期IO数据交换
    │    ├── RecordCR: 非周期参数读写
    │    └── AlarmCR: 诊断报警
    └── 设备模型: DAP(Device Access Point) + 子模块
```

### 安全通信 (PROFIsafe)

PROFIsafe作为PROFINET上的安全层(黑色通道原理)实现SIL 3功能安全：
- 安全数据包：序列号+时间戳+CRC24校验
- 看门狗监控通信超时
- 无需专用安全总线(如AS-i Safety)，与标准数据同网传输

### MCU/嵌入式实现

- **从站开发**：商用协议栈(Port工业自动化、Hilscher netX、Renesas R-IN32M)
- **西门子平台**：ERTEC 200P/200P-2 ASIC
- **开源替代**：无成熟开源PROFINET从站(协议专利保护+认证要求)
- 需通过PI认证测试实验室认证

### PROFINET vs EtherCAT

| 维度 | PROFINET | EtherCAT |
|------|----------|----------|
| 拓扑 | 星型/树形(交换机) | 菊花链/环形 |
| 成本 | 较高(需交换机) | 较低(无需交换机) |
| 实时性能 | RT~1ms, IRT~31us | <1us per slave |
| 非实时通信 | 内置(开放通道) | 需EoE隧道 |
| 主要市场 | 欧洲自动化(Siemens生态) | 全球运动控制 |
| Web集成 | 好(开放通道原生态) | 需额外网关 |

---

## Q208. BACnet楼宇协议

### 概述

BACnet(Building Automation and Control Networks)是ASHRAE/ANSI/ISO(ISO 16484-5)标准的楼宇自动化协议，广泛应用于HVAC、照明、消防、安防等楼宇管理。

### BACnet互联网络模型

BACnet的核心设计理念：在网络层统一设备模型，支持多种物理层/数据链路层（BACnet Inter-network）：

```
BACnet应用层
BACnet网络层（路由）
    ├── BACnet/IP (UDP 47808, BVLL)
    ├── BACnet Ethernet (ISO 8802-3)
    ├── BACnet MS/TP (RS-485, 令牌传递)
    ├── BACnet ARCNET
    ├── BACnet LonTalk
    └── BACnet Zigbee
```

**BBMD(BACnet Broadcast Management Device)**：在IP子网间转发广播，解决BACnet依赖广播(UDP广播不能跨路由器)的问题。

### 面向对象的设备模型

```
Device对象
├── AnalogInput(AI):      温度传感器值 + 单位 + 上下限
├── AnalogOutput(AO):     阀门开度(0-100%)
├── BinaryInput(BI):      门磁状态(开/关)
├── BinaryOutput(BO):     风机启停控制
├── Schedule:             周一至周五08:00-18:00运行时间表
├── TrendLog:             历史数据记录
└── NotificationClass:    告警目标列表(发给谁)
```

每个对象有Properties(属性)标识符，如`Present_Value`(85), `Units`(117), `Reliability`(103)。

### BACnet服务（五大类）

| 类别 | 服务 | 说明 |
|------|------|------|
| 对象访问 | ReadProperty/WriteProperty | 读写对象属性(最常用) |
| 对象访问 | ReadPropertyMultiple | 批量读，减少网络交互 |
| 报警与事件 | COV/COVP | 值变化订阅(Change of Value) |
| 报警与事件 | EventNotification | 告警主动推送 |
| 文件传输 | AtomicReadFile/AtomicWriteFile | 趋势数据/配置导入导出 |
| 设备管理 | Who-Is/I-Am | 设备发现(类似ARP) |

### COV订阅机制

```
Client -> SubscribeCOV(Device3, AnalogInput1) -> Server
Server -> (温度变化超COV增量) -> COVNotification(新值25.6C) -> Client
```

COV减轻轻量级客户端轮询负担，适用于电池供电的无线传感器。

### 嵌入式实现

- **BACnet Stack**(开源C库, SourceForge)：支持Server/Client，可在Linux/FreeRTOS上运行
- 典型嵌入式场景：BACnet网关(Wi-Fi/BLE传感器→BACnet/IP上报到BMS)
- MS/TP物理层：RS-485半双工，令牌传递MAC(9600-115200bps)
- BACnet/IP与MQTT的有趣结合：趋势是BMS网关同时支持BACnet(南向)和MQTT(北向云端)

### 市场地位

BACnet是楼宇自控领域的"Modbus"，霍尼韦尔、江森、西门子等BMS系统均支持。中国标准GB/T 50636。新版BACnet/SC(安全连接，基于TLS 1.3 WebSocket)解决了传统BACnet安全薄弱的问题。

---

## Q209. DLMS电表协议

### DLMS/COSEM概述

DLMS(Device Language Message Specification)/COSEM(Companion Specification for Energy Metering)是IEC 62056系列标准定义的智能仪表通信协议，是智能电网中电表、水表、气表和热量表的数据交换标准。

### COSEM对象模型

```
物理设备(Physical Device)
  └── 逻辑设备(Logical Device)
       └── COSEM接口对象(COSEM Interface Objects)
            ├── 数据(Data): "寄存器"对象，存储计量数据
            ├── 寄存器(Register): 单一值(如总有功电能)
            ├── 扩展寄存器(Extended Register): 时间戳+值
            ├── 需量寄存器(Demand Register): 最大值+时间
            ├── 曲线数据(Profile Generic): 历史负荷曲线
            ├── 时钟(Clock): 实时时钟
            ├── 关联(Association): 安全认证
            └── SAP(服务访问点): DLMS/COSEM封装
```

每个COSEM对象通过`OBIS(Object Identification System)`编码唯一标识：
```
OBIS Code: A-B:C.D.E.F  (6组)
例: 1-0:1.8.0.255  →  总有功电能(+A), 费率1, 当前值
    1-0:2.8.0.255  →  总无功电能
    0-0:1.0.0.255  →  时钟
    0-1:24.2.1.255 →  瞬时电压 L1
```

### DLMS通信架构

DLMS定义了三层模型：

**1. COSEM应用层**
- xDLMS服务：GET/SET/ACTION/EVENT_NOTIFICATION
- 访问权限控制（读/写/无访问）
- 选择性访问(指定要读的属性子集)

**2. COSEM传输层(Wrapper)**
- **HDLC(High-Level Data Link Control)**：串口/光电接口(IEC 62056-21)标准
- **TCP-UDP/IP Wrapper**：网络通信

**3. 物理接口选择**
- 光电接口(ANSI C12.18/IEC 62056-21)：红外+磁吸，本地抄表
- RS-485：集中器-电表组网(Modbus-RTU兼容)
- PLC(G3-PLC/PRIME)：电力线载波
- RF Mesh(Wi-SUN)：无线抄表网络
- NB-IoT/LTE-M：电信运营商抄表

### DLMS安全

基于DLMS/COSEM安全套件：
- 应用层加密(AES-GCM-128)，保护数据机密性和完整性
- 四级安全策略：0=无安全, 1=认证, 2=认证+加密, 3=认证+加密+数字签名
- HLS(高级安全)：基于TLS/DTLS的传输层安全 + ECDH密钥协商

### MCU实现

- **Gurux DLMS**：开源C++/Java/ANSI C库，支持Server/Client
- 电表SoC(如瑞能RN821x/Microchip 71M6541)内置DLMS协议栈
- 内存需求：Flash~256KB, RAM~64KB(取决于数据量)
- HES(Head-End System, 前置机)通过DLMS与集中器/电表通信

### 报表示例 (DLMS PDU)

```
GET Request (读取OBIS 1-0:1.8.0.255, 总有功电能):
  [C0 01 C1]    // LLC层
  [00 01 00 30 00 01 00 10 60]  // GET Request Normal
  // class_id=1, obis=1-0:1.8.0.255, attribute=2(值)

GET Response:
  [C0 01 C1]    // LLC层  
  [00 01 00 30 00 01 00 10 61]  // GET Response Normal
  // Data=0x06 0x0000001A3F -> 67263 (67.263 kWh)
```

---

## Q210. ISO 15118充电桩协议

### 概述

ISO 15118是电动汽车(EV)与充电桩(EVSE)之间的通信协议标准，定义智能充电(V2G, Vehicle-to-Grid)的数字通信接口。

### 协议分层

```
ISO 15118-2: 充电通信(V2G应用层消息)
ISO 15118-3: 物理层和数据链路层(Powerline Communication, HomePlug GreenPHY, 基于PLC)
ISO 15118-8: 无线通信(Wi-Fi)
ISO 15118-20: 双向充电(V2G, V2H, V2L), 感应充电(WPT)
```

### 通信流程

```
物理连接: EV插枪 -> PLC建立(HomePlug GreenPHY)
  ↓
SLAC(信号电平衰减特性): EV检测CP信号, EVSE发送匹配请求
  ↓
SLAC匹配 -> AVLN建立(车内局域网, 点对点IP连接)
  ↓
SDP(SECC发现协议): EV通过mDNS发现EVSE的IP+端口+安全策略
  ↓
TLS建立: 双向证书认证(EV和EVSE互相验证X.509证书)
  ↓
V2G消息(SDP请求->会话建立->充电参数协商->开始充电->计费->停止)
```

### 核心V2G消息(DIN SPEC 70121 / ISO 15118-2)

| 消息 | 方向 | 说明 |
|------|------|------|
| SessionSetupReq/Res | EV->EVSE | 建立会话, 传递VehicleID |
| ServiceDiscovery | EV->EVSE | 查询支持的服务类型 |
| ServiceDetail | EV->EVSE | 询问特定服务详情 |
| ChargeParameterDiscovery | EV->EVSE | 协商充电功率/电压/电流 |
| PowerDelivery | EV->EVSE | 开始/停止能量传输 |
| MeteringReceipt | EVSE->EV | 计费信息(充电量) |
| CertificateUpdate | EVSE->EV | 更新合约证书(PnC) |
| Authorization | EV->EVSE | 外部认证(EIM)或PnC(Plug & Charge) |

### Plug & Charge (PnC，即插即充)

ISO 15118的核心便利功能：

```
EV插枪 -> 自动TLS握手(双向证书) -> 
EVSE根据合约证书自动识别用户 -> 
无需刷卡/App(自动扣费) -> 开始充电
```

依赖Vehicle OEM、充电运营商(MO)、充电网络之间的PKI信任链。

### 物理层：HomePlug GreenPHY

- 基于PLC(电力线载波)，利用充电线缆(Control Pilot, CP)作为物理介质
- 频率：2-30MHz
- 速率：约10Mbps PHY层
- 优势：无需额外数据线，所有支持ISO 15118的充电桩兼容

### MCU实现参考

- **Qualcomm QCA7000/7005**：GreenPHY PLC Modem芯片(SPI接口)，广泛用于EV/EVSE
- **Vector vSECC**：充电桩协议栈商业方案
- **RiseV2G**：开源V2G实现C++(主要用于参考)
- 嵌入式需求：Linux板级(ARM A系列 + RAM > 128MB，因PLC需要大量内存管理)

### 中国市场与国标的关系

- 中国国标GB/T 27930：CAN总线通信(250kbps)，功能简单(充电仅基础)，无PnC
- ChaoJi(新国标)正在引入PLC通信，向ISO 15118靠拢
- GB/T与ISO 15118在安全(PKI)、V2G方面差距较大

---

## Q211. AWS IoT/Azure IoT Hub对接

### AWS IoT Core

**设备注册与认证：**

```
设备 -> X.509证书(设备独有, CA签发) -> TLS双向认证(mTLS) -> AWS IoT Core
        或
设备 -> Custom Authorizer(Lambda + Token -> 灵活认证) -> Enable IoT Policy
```

**MQTT Topic结构：**

```
$aws/things/THING_NAME/shadow/update        # 设备影子更新
$aws/things/THING_NAME/shadow/get/accepted  # 获取设备影子
$aws/things/THING_NAME/jobs/notify          # OTA Job通知
$aws/events/presence/connected/CLIENT_ID    # 设备上线事件
```

**设备影子(Device Shadow)**：AWS存储设备预期状态和上报状态，断线期间云端记录期望值，重连后同步。

**规则引擎(Rules Engine)**：从MQTT主题筛选数据 → 路由到DynamoDB/S3/Lambda/Kinesis/Timestream。

### Azure IoT Hub

**设备注册与认证：**

```
设备 -> SAS Token(Shared Access Signature, HMAC-SHA256, 1小时-7天) -> IoT Hub
        或
设备 -> X.509 CA证书认证(批量设备自动化注册, DPS)
```

**DPS(Device Provisioning Service)**：设备出厂预烧录注册组密钥，首次上电自动分配到指定IoT Hub(支持异地就近分配，实现全球部署)。

**IoT Hub Endpoint(内置端点)**：

```c
// 连接字符串格式
"HostName=xxx.azure-devices.cn;DeviceId=DEV001;SharedAccessKey=xxxx"
```

**MQTT Topic结构**：

```
devices/{device_id}/messages/events/        # 设备->云端遥测(D2C)
devices/{device_id}/messages/devicebound/   # 云端->设备命令(C2D)
$iothub/twin/PATCH/properties/reported/     # 设备孪生上报
$iothub/twin/PATCH/properties/desired/       # 云端期望更新
$iothub/methods/POST/{method_name}/         # 直接方法调用
```

### MCU对接实现

**AWS IoT (以ESP32为例)**：

```c
// AWS IoT Core MQTT 连接配置
// Endpoint: a1b2c3d4e5-ats.iot.cn-north-1.amazonaws.com.cn
// 端口: 8883 (TLS)

esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtts://YOUR_ENDPOINT:8883",
    .broker.verification.certificate = aws_root_ca_pem,
    .credentials = {
        .client_cert = device_cert_pem,     // 设备证书
        .client_key = device_private_key,    // 设备私钥
    },
};
```

**Azure IoT (Azure SDK for Embedded C)**：

```c
// azure-sdk-for-c iothub_client_sample_mqtt
az_iot_hub_client client;
az_iot_hub_client_init(&client, hostname, device_id, &options);

// 发送遥测
az_iot_hub_client_telemetry_publish_topic_get(&client, NULL, mqtt_topic, sizeof(mqtt_topic), NULL);
mqtt_client_publish(topic, payload, len, QoS1);
```

### 对比总结

| 功能 | AWS IoT Core | Azure IoT Hub |
|------|-------------|---------------|
| 认证 | X.509/TLS mTLS + Custom Auth + IAM | SAS Token + X.509 + TPM |
| 设备配置 | IoT Core + AWS IoT Device Management | DPS(Device Provisioning Service) |
| OTA | FreeRTOS OTA / AWS IoT Jobs | Device Update for IoT Hub |
| 规则引擎 | AWS IoT Rules(SQL) | Azure Stream Analytics |
| 边缘 | Greengrass(FreeRTOS+Linux) | IoT Edge(Docker容器) |
| 协议 | MQTT + HTTP + WebSocket | MQTT + AMQP + HTTPS |
| 免费层 | 250万消息/月 | 8000条/天(Free) |

---

## Q212. OTA安全签名验证

### OTA安全威胁模型

```
攻击面:
├── 传输中固件被篡改(中间人攻击)
├── 下载到恶意固件(服务器被黑)
├── 固件降级攻击(回滚到有漏洞的旧版本)
├── 下载过程中断导致烧录不完整
└── 烧录到错误的设备
```

### 签名验证完整流程

```
1. 固件构建(云端/CI):
   原始固件 -> SHA-256哈希 -> 私钥签名(ECDSA/RSA) -> 签名附加到固件包末尾
   
   固件包 = [固件内容] + [版本号] + [公钥指纹] + [签名]
   
2. MCU下载验证:
   接收完整固件 -> 提取签名 -> 用预置公钥验签 -> 
   验签通过 -> 校验版本号(防降级) -> 烧录 -> 重启
   验签失败 -> 丢弃 + 告警
```

### 签名算法选择

| 算法 | 密钥长度 | 签名长度 | 验签速度(Cortex-M4@168MHz) | 推荐度 |
|------|---------|---------|---------------------------|--------|
| ECDSA P-256 | 32B | 64B | ~50ms | 推荐 |
| Ed25519 | 32B | 64B | ~30ms | 推荐 |
| RSA-2048 | 256B | 256B | ~500ms | 不推荐 |

**推荐ECDSA P-256**：平衡了安全级别(128-bit)和验签速度。

### MCU代码实现

```c
#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"

#define FIRMWARE_VERSION    0x01000200  // v1.0.2

// 预烧录在Flash末尾(受写保护)：
// - 公钥(64 bytes, secp256r1未压缩格式)
// - 最低允许版本号(防降级)
// - 签名(signature, 64 bytes, 在固件包末尾)

int ota_verify(const uint8_t *fw_data, size_t fw_size) {
    if (fw_size < 128) return -1;
    
    // 分离签名(最后64字节)
    size_t payload_size = fw_size - 64;
    const uint8_t *signature = fw_data + payload_size;
    
    // 检查版本号(防降级攻击)
    uint32_t new_version = *((uint32_t *)(fw_data + fw_size - 68)); // 签名前4B
    if (new_version < MIN_ALLOWED_VERSION) return -2;
    
    // SHA-256 哈希
    uint8_t hash[32];
    mbedtls_sha256(fw_data, payload_size, hash, 0);
    
    // ECDSA 验签
    mbedtls_ecdsa_context ecdsa;
    mbedtls_ecdsa_init(&ecdsa);
    mbedtls_ecp_group_load(&ecdsa.grp, MBEDTLS_ECP_DP_SECP256R1);
    mbedtls_ecp_point_read_binary(&ecdsa.grp, &ecdsa.Q, pub_key, 64);
    
    int ret = mbedtls_ecdsa_read_signature(&ecdsa, hash, 32, 
                                             signature, 64);
    mbedtls_ecdsa_free(&ecdsa);
    
    return (ret == 0) ? 0 : -3;
}
```

### 双区(Bank-Swap)安全升级

```
Flash Bank A (运行中)    Flash Bank B (下载新固件)
     ↓                          ↓
下载+验签通过             烧录到Bank B
     ↓                          ↓
设置Bootloader标志 ->  重启 -> Bootloader检测标志 -> 切换到Bank B
     ↓ (如果新固件运行失败)
看门狗超时 -> Bootloader回滚到Bank A
```

双区方案确保OTA失败时设备保持可用，不会变砖。

### 完整安全要求

1. 签名验签(ECDSA/Ed25519)
2. 版本号防降级
3. 双区Bootloader(Fail-safe)
4. 完整性校验(SHA-256)
5. 端到端加密(AES-GCM, 保护固件机密性)
6. 安全引导(Secure Boot, 从Boot ROM开始链式验证)
7. 加密固件存储(Flash读保护Level 2)

---

## Q213. 证书管理PKI

### PKI基础概念

PKI(Public Key Infrastructure)是管理数字证书的整套体系，确保公钥可信绑定到特定实体：

```
CA(Certificate Authority, 证书颁发机构)
  ├── 根CA(自签名, 离线存储 -> 最高信任锚)
  ├── 中间CA(由根CA签发 -> 签发终端证书)
  └── 终端实体证书(设备/服务器/App)

信任链:
  根CA证书(预置在设备Trust Store)
    -> 验证中间CA证书签名
      -> 验证终端实体证书签名
        -> 信任设备公钥
```

### X.509证书结构

```
Certificate:
  Version: v3
  Serial Number: 0x5A3F...
  Signature Algorithm: sha256WithRSAEncryption
  Issuer: CN=Intermediate CA, O=Company
  Validity:
    Not Before: 2025-01-01 00:00:00 UTC
    Not After:  2035-01-01 00:00:00 UTC
  Subject: CN=DEV-001, O=Company
  Subject Public Key Info:
    Algorithm: id-ecPublicKey
    Public Key: 04:AB:CD:... (65B, secp256r1)
  Extensions:
    Basic Constraints: CA=FALSE
    Key Usage: Digital Signature, Key Encipherment
    Extended Key Usage: TLS Web Client Authentication (1.3.6.1.5.5.7.3.2)
  Signature: 0x3A:B2:... (CA私钥签名)
```

### IoT设备证书生命周期

```
1. 设备出厂 (Factory Provisioning)
   设备生成密钥对(安全元件SE/TPM内) -> CSR发送到工厂PKI -> 
   签发设备证书 + 中间CA链 + 根CA -> 烧录到设备安全存储
   
2. 首次上云 (Onboarding)
   设备出示证书链 -> 云平台验证根CA信任 -> 验证吊销状态(OCSP/CRL) -> 
   允许注册
   
3. 正常运行 (Operational)
   定期检查证书到期 -> 到期前30天触发自动续期(EST/SCEP/CMPv2协议) -> 
   新证书下发 -> 无缝切换
   
4. 证书吊销 (Revocation)
   设备遗失/权限取消 -> 云平台将证书序列号加入CRL -> 
   或OCSP Stapling推送吊销状态
```

### MCU上的证书存储

| 存储位置 | 安全性 | 容量 | 场景 |
|---------|--------|------|------|
| 内部Flash | 低(可读, 除非Level2保护) | 大(数百KB) | 一般设备 |
| 安全元件(SE/ATECC608A) | 高(硬件防篡改) | 小(8-16KB) | 安全要求高 |
| TPM 2.0 | 高(可信平台) | 中等 | 标准安全 |
| 外部EEPROM | 低(明文) | 大 | 不推荐 |

### 证书格式与编码

- **PEM**：Base64编码，头尾有`-----BEGIN CERTIFICATE-----`标记，文本方便调试
- **DER**：二进制TLV编码(ASN.1编码规则)，适合存储(占用小)

```c
// 设备证书链存储结构
const uint8_t device_cert_chain[] = {
    // Device Certificate(DER)
    // Intermediate CA Certificate(DER)
    // 根CA不需要存储(云端已信任)
};
```

### 最佳实践

1. **密钥永不出安全元件**：签名在SE内完成，私钥不暴露给MCU内存
2. **证书撤销检查**：启用OCSP或CRL，避免被盗设备证书继续使用
3. **证书过期预警**：NTP同步后检查证书链中min(Not After)
4. **离线场景**：长有效期设备证书(5-20年) + OCSP Stapling
5. **安全引导链**：Boot ROM -> Bootloader签名 -> App签名 -> OTA固件签名，环环验证

---

## Q214. 边缘计算网关架构

### 定义与价值

边缘计算网关是部署在IoT设备(南向)和云平台(北向)之间的中间计算节点，将云计算能力下沉到网络边缘：

```
南向(现场设备层)        边缘计算网关         北向(云平台/应用)
┌─────────────┐      ┌──────────────┐      ┌──────────────┐
│ 传感器/PLC   │ UART │ 协议转换     │ MQTT │ AWS IoT      │
│ BLE/Tag     │ BLE  │ (Modbus->MQTT)│HTTP/2│ Azure IoT    │
│ Camera      │ Wi-Fi│ 数据清洗     │      │ 本地App/SCADA│
│ RS-485设备  │ RS485│ 规则引擎     │      │ 企业ERP      │
│ CAN设备     │ CAN  │ 本地存储     │      │ 第三方API    │
└─────────────┘      └──────────────┘      └──────────────┘
```

### 核心功能模块

**1. 协议转换层**

```c
// 统一数据模型(中间表示)
typedef struct {
    char device_id[32];
    uint32_t timestamp;
    char metric[32];
    double value;
    int quality; // 0=GOOD, 1=UNCERTAIN, 2=BAD
} unified_data_t;

// Modbus RTU -> 统一模型
unified_data_t modbus_to_unified(uint8_t slave_addr, uint16_t reg_addr, uint16_t value) {
    return (unified_data_t){
        .device_id = format("PLC_%d", slave_addr),
        .timestamp = time(NULL),
        .metric = lookup_metric_table(reg_addr), // 0x0010->"temperature"
        .value = value * 0.1, // Modbus 1位小数
        .quality = 0,
    };
}
```

**2. 数据清洗与预处理**

```
原始数据 -> 去噪(滑动平均/卡尔曼) -> 
死区过滤(|dv|<threshold忽略) -> 
数据压缩(时序压缩/降采样) -> 
本地存储 + 上传云端
```

**3. 规则引擎**

```json
{
  "rule": "temperature > 80",
  "actions": [
    {"type": "mqtt_publish", "topic": "alerts/temp"},
    {"type": "local_relay", "pin": 12, "value": 1},
    {"type": "store_event", "db": "sqlite", "table": "alarms"}
  ]
}
```

**4. 离线自治**
网关断网时：本地缓存数据(SQLite/TimescaleDB) -> 本地规则引擎继续运作 -> 网络恢复后批量补传(断点续传)

### 硬件参考架构

```
┌──────────────────────────────────────────────────┐
│ Tier 1: MCU网关 (基于STM32MP1/IMX6ULL)            │
│   Linux OS + Docker/OCI容器                       │
│   ├── Modbus RTU/TCP Driver                      │
│   ├── BLE Mesh Manager                           │
│   ├── MQTT Bridge (Mosquito/NanoMQ)              │
│   └── Edge DB (BerkeleyDB/SQLite)                │
│                                                    │
│ Tier 2: 边缘服务器 (基于Jetson/Intel x86)         │
│   Docker Compose                                  │
│   ├── Node-RED (低代码数据流编排)                 │
│   ├── InfluxDB/TimescaleDB (时序数据库)           │
│   ├── Grafana (本地仪表盘)                        │
│   ├── TensorRT/ONNX Runtime (边缘AI推理)          │
│   └── AWS Greengrass / Azure IoT Edge Runtime     │
└──────────────────────────────────────────────────┘
```

### 云平台集成能力

| 平台 | 边缘运行时 | 容器支持 | 特色功能 |
|------|----------|---------|---------|
| AWS Greengrass | Nucleus + Components | Docker + Lambda | ML Inference, Stream Manager |
| Azure IoT Edge | IoT Edge Runtime | Docker | Module Twins, Deployment Manifest |
| EdgeX Foundry | 开源微服务架构 | Docker Compose | 中立、可定制、设备服务多 |

### 数据流设计要点

1. **优先级队列**：告警->高优先级MQTT QoS1，遥测->低优先级QoS0/批量上传
2. **断点续传**：SQLite记录`last_sent_seq`，恢复后从断点继续
3. **离线存储策略**：FIFO循环覆盖(防止写满)，或按时间/容量阈值触发清理
4. **本地+云端双重时间戳**(UTC)修正传输延迟

---

## Q215. TSN时间敏感网络

### 概述

TSN(Time-Sensitive Networking)是IEEE 802.1任务组定义的一系列以太网增强标准，在标准以太网上提供确定性延迟、时间同步和高可靠性。它是工业4.0、车载网络、专业音视频的关键网络技术。

### TSN核心标准

| IEEE标准 | 名称 | 功能 |
|----------|------|------|
| 802.1AS-Rev | 时间同步(Timing and Sync) | gPTP，亚微秒级全网时钟同步 |
| 802.1Qbv | 时间感知整形(TAS) | 按时间槽调度流量(门控) |
| 802.1Qbu/802.3br | 帧抢占(Frame Preemption) | 高优先级帧打断低优先级帧传输 |
| 802.1Qav | 信用整形(CBS) | AVB流平滑突发流量 |
| 802.1CB | 帧复制与消除(FRER) | 冗余路径零丢包 |
| 802.1Qci | 逐流过滤与监管(PSFP) | 防止非法流量占用带宽 |
| 802.1Qcc | 集中配置模型 | SDN式流预留/调度 |

### 时间同步 (802.1AS, gPTP)

```
GM(主时钟, Grandmaster)  ->  Bridge(边界时钟/透明时钟)  ->  End Station

同步原理:
  Master -> Sync(精确发送时间t1) -> Slave(接收时间t2)
  Slave  -> Pdelay_Req(t3) -> Master(接收时间t4)
  Master -> Pdelay_Resp(t4) -> Slave
  
  路径延迟: path_delay = ((t4 - t1) - (t3 - t2)) / 2
  时钟偏移: offset = (t2 - t1) - path_delay
  
  目标: 全网同步<1us (通过硬件时间戳+多跳累积误差消除)
```

### 时间感知整形 (802.1Qbv, TAS)

将通信周期切成时间槽，每个VLAN优先级流量在指定窗口发送：

```
周期 500us:
| Gate 1(TC7)   | Gate 2(TC5) | Gate 3(TC0)   | Guard Band |
| 运动控制 Iso  | 过程数据    | Best Effort   | 保护间隔   |
| 实时/周期性   | 周期数据    | TCP/IP/Web    | 阻止突发   |
|<--- 100us --->|<-- 200us -->|<-- 150us --->|<- 50us -->|
```

通过传输门控列表(Gate Control List, GCL)定义每个队列的精确开关时间。

### 帧抢占 (802.1Qbu/802.3br)

Express帧(高优先级)可打断Preemptable帧(低优先级)的传输：

```
正常:      [████████████ Low Frame ████████████]
抢占后:    [██ Low Frame ██] [■■ Express ■■] [████ Low Remain ████]
           (被打断)           (插队先发)      (恢复继续)
```

解决低优先级巨型帧阻塞高优先级流量的"公交阻塞"问题。

### 在嵌入式/MCU中的应用

- MCU通常不支持TSN硬件(需要支持TSN的MAC+交换芯片)
- **NXP i.MX RT1170** / **STM32MP15x** 部分支持IEEE 1588硬件时间戳(TSN基础)
- 边缘计算网关或工业控制器中，使用TSN兼容交换芯片(如Microchip LAN9668, NXP SJA1105)构建TSN网络
- 车载领域：TSN作为骨干网连接ADAS控制器、网关和域控制器

### TSN vs 传统实时以太网

| 特性 | TSN | EtherCAT | PROFINET IRT |
|------|-----|----------|-------------|
| 标准 | IEEE 802.1(开放) | IEC 61158(Beckhoff) | IEC 61784(Siemens) |
| 硬件 | 标准以太网+TSN功能 | 专用ESC ASIC | 专用ASIC |
| 兼容性 | 与标准TCP/IP共存 | 需EoE隧道 | 需开放通道 |
| 多厂商 | 易(标准开放) | 中(需授权) | 低(Siemens生态) |
| 应用定位 | 融合网络(IT+OT) | 纯OT实时总线 | OT自动化总线 |

---

## Q216. 5G NR模组AT指令

### 模组架构

```
MCU(主控) <- UART/USB -> 5G NR模组 -> 5G基站
                  ↓
          AT指令 / QMI / MBIM / PPP

物理接口选项:
  UART (4线, 3.3V TTL, 常用921600-3000000bps)
  USB   (CDC-ACM / MBIM, 高速数据)
  PCIe  (M.2, 直接连接到模组)
  SDIO / SPI (少数模组支持)
```

### 基础AT指令流程

**1. 模组启动确认**

```
AT\r\n                         <- 测试串口通信
  OK
    
AT+CPIN?\r\n                   <- 检查SIM卡状态
  +CPIN: READY                  <- SIM就绪
  OK
    
AT+CSQ\r\n                     <- 查询信号强度
  +CSQ: 23,99                  <- RSSI=-73dBm(-113+23x2)
  OK
    
AT+CEREG?\r\n                  <- EPS注册状态
  +CEREG: 0,1                  <- 已注册(home network)
  OK
```

**2. 网络注册与附着**

```
AT+COPS?\r\n                   <- 查询运营商
  +COPS: 0,0,"CHINA MOBILE",7  <- LTE/5G NR模式(7)
  OK

AT+CGDCONT=1,"IP","cmnet"\r\n  <- 定义PDP上下文(APN)
  OK

AT+CGACT=1,1\r\n               <- 激活PDP上下文
  OK
```

**3. 5G网络注册与附着 (NR5G NSA模式)**

```
AT+C5GREG=2\r\n                <- 启用5G NSA注册URC
  OK

// 自动URC上报:
+C5GREG: 1   // 5G NSA注册成功(ENDC双连接: LTE锚点+NR扩展)
```

**4. Socket数据通信**

```
AT+QIOPEN=1,0,"TCP","121.37.158.8",1883,0,0\r\n  <- 建立TCP连接
  OK
  +QIOPEN: 0,0         <- Connect ID=0, 成功

AT+QISEND=0\r\n        <- 指定发送缓冲区长度
> hello 5G module      <- 输入数据, Ctrl+Z(0x1A)发送

AT+QIRD=0,1500\r\n     <- 读取接收数据
  +QIRD: 15
  hello from server
```

### 数据模式切换

**AT命令模式 -> 数据模式 -> AT命令模式：**

```
AT+QIDAT=0\r\n               <- 切换到数据模式(Connect ID=0)
> (此时串口为透明数据模式)     <- 自由收发数据
 +++                         <- 发送"+++"(1秒静默+1秒内+1秒静默)退出
OK                           <- 回到AT命令模式
```

### MQTT集成命令

移远模组(EC2x/EG2x/BG9x/RG5xx系列)内置MQTT AT命令：

```
AT+QMTOPEN=0,"broker.emqx.io",1883\r\n    <- 打开MQTT网络
  OK
  +QMTOPEN: 0,0

AT+QMTCONN=0,"client_001","user","pass"\r\n  <- 连接Broker
  OK
  +QMTCONN: 0,0,0

AT+QMTSUB=0,1,"sensor/temp",1\r\n            <- 订阅Topic, QoS1
  OK
  +QMTSUB: 0,1,0,1

AT+QMTPUB=0,0,0,0,"sensor/data"\r\n          <- 发布消息
> {"temp":25.6}
  OK
  +QMTPUB: 0,0,0
```

### 模组选型

| 模组 | 芯片 | 支持网络 | 接口 | 适用场景 |
|------|------|---------|------|---------|
| 移远RM500Q | 骁龙X55 | 5G NR SA/NSA | USB3.0/PCIe | 高速CPE |
| 广和通FG150 | 骁龙X55 | 5G NR SA/NSA | USB3.0 | 工业路由 |
| 移远RG500U | 展锐V516 | 5G NR SA/NSA | USB2.0 | 低成本5G |
| 中移ML307A | 翱捷ASR | Cat.1 | UART | 低成本IoT |

### 实际MCU交互

```c
// 带超时+回显匹配的AT指令发送
int at_send_cmd(const char *cmd, const char *expect, uint32_t timeout_ms) {
    uart_send(cmd, strlen(cmd));
    uart_send("\r\n", 2);
    
    uint32_t start = HAL_GetTick();
    while (HAL_GetTick() - start < timeout_ms) {
        if (strstr(rx_buf, expect)) return 0;
        HAL_Delay(10);
    }
    return -1; // 超时
}

// 顺序发送初始化
at_send_cmd("AT", "OK", 1000);
at_send_cmd("AT+CPIN?", "READY", 5000);
at_send_cmd("AT+CGDCONT=1,\"IP\",\"cmnet\"", "OK", 3000);
at_send_cmd("AT+CGACT=1,1", "OK", 15000);  // 激活较长超时
```

### 常见问题

1. **URC处理**：URC(Unsolicited Result Code, 如`+QIURC: "recv",0,15`)随时可能插入响应中，需异步处理回调
2. **模组供电**：5G模组峰值电流可达2-3A(3.8V)，需独立DCDC(LM25119)供电
3. **散热**：5G持续数据业务下功耗高(3-8W)，需散热片/风冷
4. **双SIM**：工业模组通常支持双SIM卡槽(AT+QUIMSLOT切换)

---

## Q217. NB-IoT eDRX/PSM省电模式

### NB-IoT省电机制全景

NB-IoT在Release 13引入三种省电模式，解决IoT设备超长待机需求(10年+电池)：

```
连接态 (RRC Connected)
    ↓ 无数据传输(不活动定时器超时)
空闲态 (RRC Idle)
    ↓ 可选: eDRX周期(延长监听间隔) 或 PSM(深度休眠)
  ┌──────────────────────────────────────────────┐
  │ PSM: 进入深度休眠                           │
  │   - 射频完全关闭                            │
  │   - 功耗~5uA                                │
  │   - 仍注册在核心网(不丢失IP地址)            │
  │   - 周期:T3412(可达413天)                    │
  │   - 下行数据被缓存，直到设备主动上行唤醒    │
  └──────────────────────────────────────────────┘
    ↓ TAU定时器到期
  TAU(跟踪区更新)或主动上行 -> 返回连接态
```

### PSM (Power Saving Mode)

```
工作原理:
  设备发TAU/数据 -> 进入Active Timer(T3324, 2s-186min) -> 
  等待下行 -> 超时 -> 进入PSM -> 射频关闭(UE不可达) ->
  T3412扩展定时器(可达413天)到期 -> 自动醒来 -> TAU -> Active
  
AT指令:
  AT+CPSMS=1,,,"01000011","00000010"
  // T3324 = 01000011 = 67秒(二进制x2s)
  // T3412 = 00000010 = 2小时(二进制x10min)
  
AT+CPSMS?  -> +CPSMS: 1,,,"01000011","00000010"
```

**PSM典型功耗**：休眠时<5uA，极大延长电池寿命。

### eDRX (Extended Discontinuous Reception)

```
工作原理:
  设备周期性短暂打开接收窗口(PTW, Paging Time Window)，其余时间休眠。
  eDRX周期远大于LTE DRX(2.56s)：可达2621.44秒(~43分钟)
  
  周期循环:
  ┌────────┐ ┌────────┐ ┌────────┐
  │  PTW   │ │ 休眠    │ │  PTW   │
  │ 寻呼窗  │ │ (省电)  │ │ 寻呼窗  │
  └────────┘ └────────┘ └────────┘
  <-------------- eDRX周期 -------------->
  
AT指令:
  AT+CEDRXS=2,4,"0101"
  // Access Tech=4(E-UTRAN NB-S1 mode)
  // 模式=2(启用eDRX)
  // eDRX值="0101"=5
  // 实际周期=10.24s (NB-IoT eDRX查找表, 5->10.24s)
```

### 三种模式对比

| 模式 | 下行可达 | 功耗 | 延迟 | 适用场景 |
|------|---------|------|------|---------|
| 无省电(DRX=2.56s) | 实时 | 高(mA级) | 低(<2.56s) | 市电/车载 |
| eDRX(10.24s) | 较实时 | 低(~50uA) | 中等(10s级) | 远程控制 |
| eDRX(2621.44s) | 准可及 | 很低(~20uA) | 高(44分钟) | 定时上报 |
| PSM | 不可达 | 极低(<5uA) | 极高(主动上行) | 纯上报传感器 |

### 低功耗设计实践

```c
// NB-IoT PSM省电发送流程
void nbiot_send_sensor_data(float temp, float hum) {
    // 1. 唤醒模组(拉高PWRKEY)
    modem_power_on();
    
    // 2. 等待注册(网络附着)
    while (!is_network_registered()) { delay(100); }
    
    // 3. 创建UDP Socket + 发送数据
    nbiot_socket_open();
    nbiot_sendto(server_ip, server_port, payload, len);
    nbiot_socket_close();
    
    // 4. 等待T3324超时进入PSM
    // 发送数据后不要立即断电，等待Active Timer确保下行ACK
    
    // 5. 完全断电模组(省电)
    modem_power_off();  // MCU进入待机
    
    // 6. RTC闹钟唤醒(下一上报周期)
    enter_standby(T_REPORT_SECONDS);  // PSM超长周期
}
```

### PSM vs eDRX如何选择

- **PSM**：纯上行传感器(温度/水位/抄表)，无需实时下行，极致省电
- **eDRX**：需下行配置/控制，如路灯远程开关(10分钟延迟可接受)
- **混合**：PSM + eDRX同时启用，在PTW期间用eDRX保持可达，其他时间PSM

### 实际部署经验

1. T3324不宜太短(影响PSM电源节省)，建议30秒-2分钟
2. 不同运营商对T3412的支持上限差异大(电信最大310小时，联通可能截断)
3. 模组在进入PSM前需要发送数据(否则不进PSM)
4. NB-IoT信号弱时(RSPR<-125dBm)功耗飙升(50-200mA)，应考虑信号采集策略
5. 部分海外运营商限制T3412(如Vodafone限制为214小时)

---

## Q218. RFID/NFC协议

### 技术体系

```
RFID (Radio Frequency Identification) / NFC (Near Field Communication)

            RFID                               NFC(13.56MHz)
低频(LF)       高频(HF-13.56MHz)  超高频(UHF)
125/134kHz      ISO 14443 A/B    860-960MHz
ISO 11784/85   ISO 15693         ISO 18000-6C
读取1-10cm     读取1-10cm        读取1-15m
动物/门禁      NFC/银行卡       仓储/物流
```

### ISO 14443 A (NFC-A) 基本原理

NFC基于电磁感应(Inductive Coupling)，读卡器天线产生的13.56MHz磁场同时为卡片供电和传输数据。

**通信流程：**

```
读卡器(PCD, Proximity Coupling Device)
    ↓ 持续发13.56MHz载波 -> 为卡片供电
    ↓ REQA(0x26, 短帧7bit) 
卡片(PICC) -> ATQA(2bytes, 包含UID大小信息)
    ↓ ANTICOLLISION + SELECT(Cascade Level 1 多卡防冲突)
卡片 -> SAK(Select Acknowledge, 1byte)
    ↓ (如果SAK显示UID不全，继续Cascade Level 2/3读完整UID)
    ↓ RATS(Request for Answer To Select)
卡片 -> ATS(Answer To Select, 含传输速率参数)
    ↓ PPS(Protocol and Parameter Selection, 可选)
    ↓ APDU Exchange (ISO 7816-4, 应用层数据交换)
```

### NFC三大模式

| 模式 | 角色 | 说明 | 应用 |
|------|------|------|------|
| Reader/Writer | NFC主动读卡 | 读写NFC标签/卡片 | 门禁、信息读取 |
| Peer-to-Peer(P2P) | 两NFC设备通信 | 双向数据交换(Android Beam已弃用) | 配对、传输 |
| Card Emulation | NFC模拟卡片 | 设备模拟NFC标签/卡 | Apple Pay、门禁模拟 |

### NFC NDEF数据格式

NDEF(NFC Data Exchange Format)是NFC Forum定义的标准数据封装格式：

```
NDEF Message:
  NDEF Record 1:
    Header: MB(1b) | ME(1b) | CF(1b) | SR(1b) | IL(1b) | TNF(3b)
    Type Length(1B)
    Payload Length(1B 或 4B, 取决于SR标志)
    Type: "T"(Text), "U"(URI), "Sp"(Smart Poster)
    Payload: 实际数据(如 "Hello NFC")
  NDEF Record 2: ...
```

TNF(Type Name Format)：0x01=NFC Forum Well-Known Type, 0x02=MIME Type, 0x04=NFC Forum External Type。

### MCU实现方案

**基本方案：**
- **PN532**(NXP)：SPI/I2C/UART接口，支持Reader/Writer + P2P + Card Emulation
- **RC522**(NXP)：SPI接口，低成本，仅支持Reader/Writer(ISO 14443A/MIFARE)
- **ST25R3911B**(ST)：高性能Reader，支持ISO 14443A/B, ISO 15693, FeliCa

```c
// PN532 读取MIFARE卡UID示例
uint8_t pn532_get_firmware[] = {0x00, 0x00, 0xFF, 0x02, 0xFE, 0xD4, 0x02, 0x2A, 0x00};
pn532_spi_transfer(pn532_get_firmware);

// 寻卡
uint8_t inListPassiveTarget[] = {0x00, 0x00, 0xFF, 0x04, 0xFC, 0xD4, 0x4A, 0x01, 0x00, 0xE1, 0x00};
pn532_spi_transfer(inListPassiveTarget);
// 响应含卡片UID + ATQA + SAK
```

### UHF RFID (ISO 18000-6C)

工作于860-960MHz超高频段，读取距离可达1-15米。广泛用于仓储物流、服装零售。

**核心芯片**：Impinj Indy R2000(高性能读卡器)、PR9200(低功耗读卡器)、MagicRF M100(国产)。

**标签存储结构**：
- EPC区(电子产品代码，96-496bit)：唯一身份标识
- TID区(标签识别号)：芯片型号+序列号
- User区：用户自定义数据
- Reserved区：Kill/Access密码

### 应用场景总结

- **门禁考勤**：NFC(MIFARE Classic/Ultralight/DesFire)
- **移动支付**：NFC Card Emulation(Apple Pay/交通卡)
- **产品防伪**：NFC(NTAG 213/215/216) + 唯一ID + 云端校验
- **仓储盘点**：UHF RFID快速批量读取(300+标签/秒)
- **工业追踪**：UHF RFID抗金属标签(On-Metal Tag)

---

## Q219. 红外遥控编码解码

### 红外遥控物理层

红外遥控使用波长940nm(典型)的红外LED作为发射器，红外接收头(如VS1838B)作为接收器。

```
调制方式: 38kHz载波(常用) + ASK(幅移键控)
编码: NEC / RC-5 / RC-6 / Sony SIRC 等多种协议

载波：38kHz方波(占空比1/3或1/2)
  |_|¯|_|¯|_|¯| (周期26.3us)
```

### NEC协议详解（最常用）

```
帧格式:
| Leader(9ms) | Leader(4.5ms) | Address(8bit) | ~Address(8bit) | Command(8bit) | ~Command(8bit) | Stop(560us) |

逻辑0: 560us载波 + 560us空闲 (总1.12ms)
逻辑1: 560us载波 + 1690us空闲 (总2.25ms)
重复码: 9ms载波 + 2.25ms空闲 + 560us停止

┌──────┐     ┌──┐ ┌┐ ┌──┐
│9ms   │4.5ms│L0│ │L1│ │... 
└──────┘     └──┘ └┘ └──┘
 Leader         Data bits...
```

**地址码反码验证**：Address ^ ~Address == 0xFF → 有效

### MCU解码实现（定时器捕获）

```c
// 使用STM32定时器输入捕获
// 每个边沿触发中断，记录脉宽

#define NEC_LEADER_HIGH_MIN    8000 // us
#define NEC_LEADER_HIGH_MAX    10000
#define NEC_LEADER_LOW         4500
#define NEC_BIT_HIGH           560
#define NEC_BIT_LOW_0          560  // 逻辑0低电平
#define NEC_BIT_LOW_1          1690 // 逻辑1低电平
#define NEC_REPEAT_LOW         2250

volatile uint32_t last_capture = 0;
volatile uint8_t nec_bits[4];
volatile int bit_count = 0;
volatile bool frame_ready = false;

void TIM_IRQHandler(void) {
    uint32_t capture = TIM_GetCapture();
    uint32_t width = capture - last_capture; // 脉宽(us)
    last_capture = capture;

    // 根据当前电平状态 + 脉宽判断
    if (gpio_pin == HIGH) {
        // 刚进入高电平：检查前一个低电平宽度
        if (state == IDLE) {
            if (width >= NEC_LEADER_HIGH_MIN && width <= NEC_LEADER_HIGH_MAX) {
                state = LEADER; // 收到引导码高电平
            }
        } else if (state == LEADER) {
            if (width >= NEC_LEADER_LOW - 500 && width <= NEC_LEADER_LOW + 500) {
                state = DATA; // 引导码低电平有效，进入数据
                bit_count = 0;
                memset(nec_bits, 0, 4);
            }
        } else if (state == DATA) {
            // 前一个低电平判断 bit 0/1
            if (width >= 400 && width <= 700) {
                // bit = 0，不置位
            } else if (width >= 1500 && width <= 1900) {
                nec_bits[bit_count / 8] |= (1 << (bit_count % 8)); // bit = 1
            }
            bit_count++;
            if (bit_count == 32) {
                // 验证反码
                if ((nec_bits[0] ^ nec_bits[1]) == 0xFF &&
                    (nec_bits[2] ^ nec_bits[3]) == 0xFF) {
                    frame_ready = true;
                }
                state = IDLE;
            }
        }
    }
}
```

### 其他常见红外协议对比

| 协议 | 载波 | 地址 | 数据 | 特点 |
|------|------|------|------|------|
| NEC | 38kHz | 8+8bit(反码) | 8+8bit(反码) | 最通用，带重复码 |
| RC-5 | 36kHz | 5bit | 6bit | 曼彻斯特编码，Philips |
| RC-6 | 36kHz | 8bit | 8bit | 模式位区分不同设备 |
| Sony SIRC | 40kHz | 5/8/13bit | 7/8/10bit | 脉宽调制，无载波的脉冲表示 |
| Samsung | 38kHz | 16bit | 16bit+16bit | 类似NEC但引导码不同 |

### 红外学习与克隆

MCU可通过以下方式实现红外学习：

```c
// 学习模式：记录所有边沿时间
typedef struct {
    uint32_t carrier_freq;  // 载波频率
    uint16_t edge_times[256]; // 边沿时间序列(us)
    uint16_t edge_count;
} ir_learned_code_t;

// 播放：按记录的边沿时间重放38kHz载波
void ir_playback(ir_learned_code_t *code) {
    for (int i = 0; i < code->edge_count; i++) {
        if (i % 2 == 0) {
            pwm_start(38000, 33); // 发送载波
        } else {
            pwm_stop(); // 关闭载波
        }
        delay_us(code->edge_times[i]);
    }
}
```

### 实际应用注意

1. **发射功率**：IR LED需100-500mA脉冲电流，使用三极管/MOSFET驱动
2. **接收头选型**：VS1838B(通用)/TSOP38238(高抗干扰)/TSOP34838(低电压3.3V)
3. **遮挡与角度**：红外直线传播，接收头前应加滤光片(840nm-960nm带通)
4. **多协议兼容**：设备应同时支持NEC + 扩展NEC(16位地址) + 重复码

---

## Q220. 蓝牙Mesh vs Zigbee对比

### 蓝牙Mesh概述

蓝牙Mesh基于BLE 4.0+的广播信道（37-39），不是点对点连接，而是通过Managed Flooding广播消息实现Mesh组网。

### 网络架构对比

| 维度 | 蓝牙Mesh | Zigbee |
|------|---------|--------|
| 底层标准 | BLE 4.0/5.0 (2.4GHz) | IEEE 802.15.4 (2.4GHz/868/915MHz) |
| 网络层协议 | Mesh Profile (SIG) | Zigbee PRO (CSA) |
| 消息传播 | Managed Flooding(泛洪) | AODV + 树路由(路由) |
| 最大跳数 | 127(TTL) | 15-30 |
| 最大节点 | ~32767(虚拟) | ~65535(理论) |
| 通信方式 | 广播/组播(全网络) | 单播/组播/广播 |
| 功耗模型 | Friend Node(LPN) | End Device(休眠) |
| 手机集成 | 原生(BLE) | 需网关/Zigbee Dongle |
| 开发成本 | 低(手机即Provisioner) | 中(需Coordinator) |
| 吞吐量 | ~10kbps(广播模式) | ~20kbps(实际) |

### Managed Flooding vs 路由

**蓝牙Mesh Managed Flooding：**
```
源节点 → 广播消息 → 所有邻接节点转发(中继) → 所有邻接节点再转发
         (TTL减1)    (丢包/重复包检测)        (直到TTL=0)
```

优点：简单、无路由表开销、动态拓扑适应强。
缺点：空中带宽占用大、大量广播包。

缓解机制：
- **TTL限制**：每中继一次减1，到0停止。
- **消息缓存**：每个节点维护Network PDU缓存(最少缓存255条)，避免重复中继。
- **心跳控制**：限制中继速率。

**Zigbee路由：**
- 树路由(Cskip)+ AODV按需路由
- 维护路由表，邻居表，更高效但开销大
- 适合稳定拓扑的大规模网络

### 低功耗策略

| | 蓝牙Mesh (LPN) | Zigbee (End Device) |
|------|---------|---------|
| 休眠方式 | LPN + Friend Node | 周期性休眠 |
| 消息接收 | Friend缓存，LPN定期查询 | Coordinator/Router缓存，定期唤醒查询 |
| 低功耗节点 | LPN(Low Power Node) | End Device |
| 始终保持在线 | Friend Node | Router/Coordinator |

### 手机集成体验

这是蓝牙Mesh最大的杀手级优势：

```
Zigbee: 手机 ──Wi-Fi──> Zigbee Gateway ──Zigbee──> 灯泡
         (无法直接通信，必须有网关中转)

蓝牙Mesh: 手机 ──BLE──> 灯泡
          (原生BLE，无需网关！)
```

用户直接用手机连接和控制蓝牙Mesh设备，无需购买额外的网关。同时Apple HomeKit、Google Home等也通过Thread/BLE支持蓝牙Mesh。

### 应用场景选择

**Zigbee适用：**
- 大型智能家居/楼宇(200+设备)
- 工业传感器网络(稳定拓扑)
- 需要高可靠性(AODV路由+ACK)
- 已有Zigbee生态(Amazon Echo Plus, Philips Hue, IKEA TRADFRI)

**蓝牙Mesh适用：**
- 手机直接控制的场景(照明、信标)
- 中小规模(<100设备)
- 无需网关的低成本方案
- 商业照明(DALI over Bluetooth Mesh)
- 资产追踪(蓝牙AoA/AoD定位)

### 共存策略

两者均工作在2.4GHz ISM频段，使用自适应跳频减少干扰：
- BLE: 37个数据信道(2402-2480MHz)，自适应跳频(AFH)
- Zigbee: 16个信道(11-26, 间隔5MHz)，可选择避开Wi-Fi信道

### MCU选型

**蓝牙Mesh**：nRF52840(推荐), ESP32-C3(低成本), CC2652R, DA1469x

**Zigbee**：CC2538/CC2652R + Z-Stack, EFR32MG21 + EmberZNet, nRF52840 + ZBOSS

**双模芯片(同时支持BLE Mesh + Zigbee)**：nRF52840, EFR32MG21 — 在同一设备上支持两种协议需分时复用(Time-Sliced Multiprotocol)。

### 技术融合趋势

2022年起的Matter协议选择了Thread(基于802.15.4 + IP)为Mesh骨干，而非蓝牙Mesh。然而蓝牙Mesh在商业照明(Bluetooth NLC)、信标定位等场景持续发展。两者将长期共存，Matter集成将推动Zigbee/Thread融合。

---

> **本文档涵盖Q181-Q220共40个问题，覆盖TCP/UDP、IoT协议族、工业总线、边缘计算、安全OTA、省电策略等核心领域。每个答案均包含协议原理、帧格式图示、MCU代码示例和实际部署经验。**
