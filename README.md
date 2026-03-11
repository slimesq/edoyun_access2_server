# edoyun_access2_server

易道云提升阶段考核项目——**群聊服务端**。基于 C++17 + Linux epoll 实现的 Reactor 模式 TCP 服务器，支持多客户端群聊消息转发和文件上传/下载。与配套客户端（edoyun_assess2_client）配合使用。

## 功能特性

- **群聊广播**：接收客户端消息后，广播给所有当前在线连接
- **文件上传**：接收客户端分块上传，以 SHA1 哈希为文件名存储到 `storage/` 目录，完成后向全体广播文件信息
- **文件下载**：根据客户端请求的 SHA1，从 `storage/` 读取文件分块推送给客户端
- **并发处理**：Reactor 单线程处理 I/O 事件，业务逻辑交由线程池异步执行
- **连接管理**：每个连接绑定 `UserContext`，保存连接级别的上传/下载状态

## 架构设计

项目分为两层：

```
edoyun_access2_server/
├── reactor/     # 网络框架层（与业务无关的通用 TCP 服务器）
└── business/    # 业务逻辑层（群聊协议解析与文件管理）
```

### 整体数据流

```
客户端 TCP 连接
    ↓
Acceptor（监听 accept）
    ↓
EventLoop（epoll 事件驱动）
    ↓ 数据可读
TcpConnection::handleMessageCallback()
    ↓
ChatServer::onMessage()  ← 解析 Train 协议头，读取 payload
    ↓ 按 msgType 分发
┌─────────────┬──────────────────┬────────────────────┐
MyTask        FileUploadTask     FileDownloadTask
（群聊广播）  （分块写文件）      （分块读文件推送）
    ↓ 提交到线程池执行
ThreadPool（N 个 worker 线程）
    ↓ 结果通过 EventLoop::runInLoop 回到主线程
EventLoop（发送响应给客户端）
```

## reactor 层

### 核心组件

| 组件 | 说明 |
|------|------|
| `TcpServer` | 服务器入口，持有 `Acceptor` 和 `EventLoop`，设置三个回调并启动 loop |
| `EventLoop` | epoll 事件循环，管理所有 `TcpConnection`，通过 eventfd 支持跨线程唤醒 |
| `Acceptor` | 封装 listen socket，负责 `accept()` 新连接 |
| `TcpConnection` | 封装单个客户端连接，持有 `SocketIO` 用于读写，支持 `sendInLoop()` 线程安全发送 |
| `ThreadPool` | 固定大小线程池 + `TaskQueue`，执行业务 Task |
| `Socket` / `SocketIO` | socket fd 的 RAII 封装与 IO 操作 |
| `InetAddress` | IP/端口地址封装 |
| `UserContext` | 绑定在 `TcpConnection` 上的用户状态，保存连接级别的文件传输上下文 |

### EventLoop 关键机制

- **epoll ET/LT**：通过 `epoll_wait` 监听 `accept fd` 和所有 `conn fd`
- **跨线程唤醒**：线程池 worker 执行完任务需回调到 EventLoop 线程时，通过写 `eventfd` 唤醒 epoll，再在主线程的 `doPengdingFunctors()` 中执行回调
- **`runInLoop()`**：任何线程都可安全地将 functor 投递到 EventLoop 线程执行，配合 `m_mutex` 保护 `m_pendings` 队列

### TcpConnection 的三个回调

```
onConnection  → ChatServer::onNewConnection  （新连接建立）
onMessage     → ChatServer::onMessage        （数据可读）
onClose       → ChatServer::onClose          （连接关闭）
```

## business 层

### 核心组件

| 组件 | 说明 |
|------|------|
| `ChatServer` | 应用层服务器，解析 Train 帧头，按 msgType 创建对应 Task 投入线程池 |
| `MyTask` | 群聊 Task，将消息广播给所有连接 |
| `FileUploadTask` | 文件上传 Task，调用 `FileUpload::writeFile()` 写入数据块 |
| `FileDownloadTask` | 文件下载 Task，循环调用 `FileDownload::readChunk()` 推送数据 |
| `FileUpload` | 服务端上传状态机，线程安全，按偏移量写文件，完成后触发广播 |
| `FileDownload` | 服务端下载状态机，按 SHA1 打开文件，分块读取 |
| `FileContext` | 保存在 `UserContext` 中，记录当前连接正在处理的上传/下载对象 |

### 通信协议（Train）

与客户端共享相同的二进制协议：

```
[payload_length : 8字节 LE] [msgType : 4字节 LE] [payload : N字节]
```

| MsgType | 值 | 方向 | 说明 |
|---------|-----|------|------|
| GroupChat | 1001 | 双向 | 群聊文字消息 |
| UploadBegin | 1002 | 客→服 | 开始上传：用户名、文件名、SHA1、文件大小 |
| UploadChunk | 1003 | 客→服 | 上传数据块：用户名、偏移量、数据 |
| DownloadBegin | 1004 | 客→服 | 请求下载：文件 SHA1 |
| DownloadEnd | 1005 | 服→客 | 下载完成通知 |
| DownloadChunk | 1007 | 服→客 | 下载数据块：SHA1、偏移量、数据 |
| FileStatus | 1008 | 服→客 | 状态回复：Uncompleted / Completed / Error |

### 文件传输流程

**上传流程**
1. 收到 `UploadBegin`：创建 `FileUpload` 对象，存入 `UserContext`，回复 `FileStatus(Uncompleted)`（表示"继续发块"）
2. 收到 `UploadChunk`：调用 `FileUpload::writeFile(offset, size, data)` 写入 `storage/<sha1>` 文件
3. 文件写完：`FileUpload` 状态变为 `Completed`，向所有连接广播包含文件信息的 GroupChat 消息

**下载流程**
1. 收到 `DownloadBegin`：创建 `FileDownload` 对象，存入 `UserContext`，提交 `FileDownloadTask`
2. Task 循环读块：每次调用 `readChunk(buf, maxSize)`，通过 `sendInLoop` 发送 `DownloadChunk`
3. 读取完毕：发送 `DownloadEnd`，清理下载上下文

## 项目结构

```
edoyun_access2_server/
├── main.cpp                          # 入口：忽略 SIGPIPE，启动 ChatServer(4线程, 队列10, :8888)
├── CMakeLists.txt                    # 顶层构建，依赖 reactor 和 business 两个子库
├── reactor/
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── Acceptor.h
│   │   ├── EventLoop.h               # epoll 事件循环，runInLoop 机制
│   │   ├── InetAddress.h
│   │   ├── Socket.h / SocketIO.h
│   │   ├── Task.h / TaskQueue.h
│   │   ├── TcpConnection.h           # 连接封装，三回调模型
│   │   ├── TcpServer.h
│   │   ├── ThreadPool.h
│   │   ├── UserContext.h
│   │   └── utils/                    # NonCopyable 等工具类
│   └── src/                          # 对应实现文件
└── business/
    ├── CMakeLists.txt
    ├── include/
    │   ├── ChatServer.h              # 协议分发入口
    │   ├── FileContext.h             # 连接级别文件状态
    │   ├── FileDownload.h / FileDownloadTask.h
    │   ├── FileUpload.h / FileUploadTask.h
    │   ├── MyTask.h                  # 群聊广播 Task
    │   └── utils/                    # LoadStatusType 等
    └── src/                          # 对应实现文件
```

## 环境要求

- Linux（依赖 epoll，不支持 Windows/macOS）
- GCC / Clang，C++17
- CMake >= 3.20

## 构建方式

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## 运行

```bash
./edoyun_access2_server
```

默认绑定 `192.168.32.129:8888`，4 个 worker 线程，任务队列容量 10。

如需修改地址或线程数，编辑 `main.cpp`：

```cpp
ChatServer server(/*threadNum=*/4, /*queSize=*/10, "192.168.32.129", 8888);
```

文件上传后会存储在运行目录下的 `storage/` 文件夹中，以文件 SHA1 哈希值命名。

## 注意事项

- 服务端在 `main()` 入口处忽略了 `SIGPIPE` 信号，防止向已断开的客户端写数据时进程崩溃
- 文件按 SHA1 去重存储：相同内容的文件只存一份
- 业务 Task 在线程池中执行，需将结果回传到 EventLoop 主线程才能安全发送，通过 `TcpConnection::sendInLoop()` 实现
