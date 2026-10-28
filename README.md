# herrychat 在线互动聊天系统

一个仿微信的 C++ 全栈在线聊天系统，支持注册登录、好友管理、一对一单聊、群聊及群管理，客户端基于 Qt6 开发，服务端采用多进程微服务架构。

## 功能特性

- **账号体系**：邮箱验证码注册、登录、忘记密码，密码 XOR 加密存储
- **好友系统**：按用户昵称搜索、添加好友申请、同意/拒绝、双向好友关系
- **单聊**：TCP 长连接实时收发消息，在线即时推送、离线消息入库，消息气泡展示
- **群聊**：创建群聊、邀请好友进群、群消息广播、群聊记录持久化
- **群管理**：群主/管理员/普通成员三级角色；群主可踢人、设置/取消管理员、修改群名；成员可修改群昵称、退出群聊
- **个人中心**：修改头像（本地裁剪上传）、昵称、签名、性别
- **体验细节**：回车发送消息、密码明文/密文切换、错误码分类提示、消息重发、未读标记

## 系统架构

```
┌────────────┐      ┌──────────────────────────────────────────┐
│  Qt 客户端  │ HTTP │                GateServer                │
│(herrychat/)│─────▶│  网关（Boost.Beast + Asio，端口 8080）     │
└────────────┘      └───────┬──────────────┬───────────────────┘
                            │ gRPC         │ gRPC
                            ▼              ▼
                  ┌──────────────┐  ┌──────────────────┐
                  │ VarifyServer │  │   StatusServer   │
                  │ Node.js 验证码│  │   状态服务       │
                  │ (端口 50051)  │  │   (端口 50052)   │
                  └──────────────┘  └────────┬─────────┘
                                             │ gRPC
                                             ▼
                          ┌──────────────────────────────────┐
                          │ ChatServer / ChatServer2          │
                          │ TCP 长连接（端口 8990 / 8991）     │
                          └──────────────────────────────────┘
```

- **GateServer**：HTTP 网关，统一接收客户端请求，通过头文件路由表（LogicSystem）分发注册、登录、好友、用户信息等业务
- **VarifyServer**：Node.js + gRPC，通过 QQ 邮箱 SMTP 发送注册验证码，验证码写入 Redis（10 分钟过期）
- **StatusServer**：状态服务，维护用户在线状态，为 ChatServer 分配连接
- **ChatServer / ChatServer2**：TCP 长连接服务，负责单聊/群聊消息转发、在线推送与离线留存

## 技术栈

| 模块 | 技术 |
|---|---|
| 客户端 | C++17 / Qt6 Widgets / QSS |
| 网络 | Boost.Asio（异步 I/O）、Boost.Beast（HTTP）、TCP 长连接 |
| 服务间通信 | gRPC + Protobuf |
| 数据库 | MySQL 8.0（连接池、PreparedStatement 防注入、存储过程） |
| 缓存 | Redis（验证码、在线状态） |
| 验证码服务 | Node.js + nodemailer |

## 目录结构

```
herrychat1/
├── herrychat/          # Qt 客户端
├── gitserver/          # HTTP 网关服务（C++）
├── server/
│   ├── ChatServer/     # 聊天服务（TCP 长连接）
│   ├── ChatServer2/    # 聊天服务第二实例
│   └── StatusServer/   # 状态服务
├── VarityServer/       # Node.js 邮箱验证码服务
├── sql/                # 数据库建表脚本
└── config.ini.example  # 网关配置模板（复制为 config.ini 并填入真实信息）
```

## 环境依赖

- Qt 6.x（客户端编译）
- Visual Studio 2019/2022 + C++17（服务端编译）
- Boost（Asio、Beast）
- gRPC / Protobuf
- MySQL 8.0（连接池使用 mysqlcppconn）
- Redis
- Node.js + npm（验证码服务）

## 快速启动

### 1. 初始化数据库

导入 `sql/llfc2.sql` 到 MySQL（默认库名 `herry`），确认 `config.ini` 中 MySQL 端口、账号密码正确。

### 2. 启动 Redis

启动本地 Redis（默认端口 6390，密码见 `config.ini`）。

### 3. 启动验证码服务

```bash
cd VarityServer
npm install
node server.js
```

> 首次使用请将 `config.json.example` 复制为 `config.json`，填入你自己的 QQ 邮箱和 SMTP 授权码。

### 4. 启动服务端

分别编译并运行：
- `gitserver`（网关，端口 8080）
- `server/StatusServer`（端口 50052）
- `server/ChatServer`、`server/ChatServer2`（端口 8990 / 8991）

### 5. 启动客户端

用 Qt Creator 打开 `herrychat/herrychat.pro` 编译运行即可。
客户端读取运行目录下的 `config.ini`（`[GateServer] host/port`）连接网关，缺省时兜底 `127.0.0.1:8080`。

## 主要代码说明

- `herrychat/httpmgr.cpp`：HTTP 请求封装，QNetworkAccessManager 异步收发
- `herrychat/tcpmgr.cpp`：TCP 长连接管理，消息编解码与分发
- `gitserver/LogicSystem.cpp`：网关业务路由，注册、登录、验证码校验
- `server/ChatServer/LogicSystem.cpp`：聊天协议处理，好友、单聊、群聊、群管理
- `server/ChatServer/MysqlDao.cpp`：数据访问层，预编译语句防 SQL 注入
- `VarityServer/server.js`：gRPC 验证码服务，发信与 Redis 写入

## 数据库表（简要）

| 表 | 说明 |
|---|---|
| `user` | 用户信息（uid、昵称、密码、头像、签名） |
| `friend` | 好友关系（双向） |
| `chat_thread` | 会话（单聊 / 群聊，type 区分） |
| `chat_msg` | 消息记录 |
| `group_member` | 群成员与角色（群主 2 / 管理员 1 / 普通 0） |

完整建表语句见 `sql/llfc2.sql`。

## 说明

- 密码采用自定义 XOR 变换加密存储（`xor_code = 长度 % 255`），适合课程学习演示；生产环境请替换为 bcrypt/argon2 等标准方案
- 项目为课程设计作品，架构参考 C++ 项目实战教程，在此之上扩展了群聊、群管理、图片消息等完整功能

## License

仅用于学习交流。
