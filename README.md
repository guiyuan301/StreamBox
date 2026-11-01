# StreamBox 本地视频播放与直播录制工具

StreamBox 是一个基于 **Qt Widgets + C++11** 的桌面视频工具，用 FFmpeg 自研解码管线做播放，同时支持从摄像头 / 麦克风采集并录制成本地 MP4。界面走深色无边框自绘风格，前后共 4 个主页面 + 播放器 + 直播窗口，适合作为 Qt 综合练手项目。

## 功能一览

| 模块 | 说明 |
|---|---|
| 启动与登录 | 启动动画淡出后进入登录页，账号密码存于本地 SQLite，支持注册新用户、登录成功后淡入主界面 |
| 本地视频库 | 通过「添加视频」导入本机视频（可选封面），卡片网格展示，支持刷新、悬停高亮、右键删除记录 |
| 视频播放 | 独立播放窗口，含进度条、当前/总时长、音量、全屏，支持双击全屏、ESC 退出全屏、拖动无边框窗口 |
| 直播录制 | 打开摄像头 + 麦克风实时预览，点击「结束直播并生成回放」后由 `ffmpeg.exe` 把 H.264 + PCM 封装成 MP4 |
| 个人中心 | 显示用户 ID / 账号、修改昵称、退出登录 |
| 系统设置 | 首选项界面（占位实现，见下文「已知限制」） |

## 技术栈

- Qt **5.9.8 MinGW 32bit**（Widgets / Multimedia / MultimediaWidgets）
- C++11，qmake 工程
- FFmpeg 4.x 动态库（`avcodec-58` / `avformat-58` / `avfilter-7` / `avutil-56` / `avdevice-58` / `swscale-5` / `swresample-3` / `postproc-55`）
- SQLite3（`sqlite3.dll` + `sqlite3.lib`）

## 目录结构

```
StreamBox/
├── StreamBox.pro          # qmake 工程文件（DESTDIR 指向 bin/）
├── main.cpp               # 程序入口：数据库路径、三个核心页面装配
├── VIEW/                  # 界面层
│   ├── splashscreen.*     # 启动动画
│   ├── loginwindow.*      # 登录 / 注册（无边框可拖动）
│   ├── mainwindow.*       # 主窗口：影视 / 直播 / 个人中心 / 设置
│   ├── playwindow.*       # 播放器窗口
│   ├── videocardwidget.*  # 视频卡片控件
│   └── custom_widgets.*   # 通用自绘控件与样式辅助
├── CONTROLLER/            # 控制器层：usercontroller / videocontroller
├── MODEL/                 # 数据模型层：usermodel / videomodel
├── DBdate/                # SQLite 封装与退出清理
├── Video/                 # 解码与录制
│   ├── videodecode.*      # 视频解码线程（本地文件 / 摄像头）
│   ├── audiodecode.*      # 音频解码线程（本地文件 / 麦克风）
│   └── livevideoaudio.*   # 调用 ffmpeg.exe 合成 MP4
├── ffmpeglib/             # FFmpeg 头文件 + 导入库
├── lib/                   # sqlite3.lib
├── pic/                   # 图标与界面图片（.qrc 资源）
└── bin/                   # 运行目录（exe、DLL、ffmpeg.exe、StreamBox.db、录制输出）
```

## 编译与运行

### 1. 准备环境

安装 **Qt 5.9.8 for Windows (MinGW 32bit)**，安装时勾选 `Qt Multimedia`、`Qt Multimedia Widgets`。运行时需要 32 位 Windows 环境或 64 位系统上的 WOW64 支持。

### 2. 获取代码

```bash
git clone https://github.com/guiyuan301/StreamBox.git
```

### 3. 构建

1. 用 Qt Creator 打开 `StreamBox/StreamBox.pro`
2. 选择套件 `Desktop Qt 5.9.8 MinGW 32bit`
3. 直接点击构建（工程里 `DESTDIR = $$PWD/bin`，产物直接落在 `bin/`）

依赖的 DLL、`ffmpeg.exe`、`StreamBox.db` 已经在仓库里，**不需要额外下载**。

### 4. 运行

- 在 Qt Creator 里直接点运行；或
- 在 `bin/` 目录下双击 `StreamBox.exe`；或
- 想脱离 Qt 环境独立分发时，执行 `windeployqt StreamBox.exe` 把 Qt 运行库拷到 `bin/`。

## 使用说明

1. **注册 / 登录**：首次使用先点「没有账号？立即注册」，注册成功后会自动回填账号到登录页。
2. **导入视频**：进入「影视」页，点「+ 添加视频」，先选视频文件（`mp4 / avi / mkv / mov / wmv / flv`），随后可再选一张封面图（可取消）。记录保存在 `bin/StreamBox.db`，**不会复制视频文件**。
3. **播放**：点击卡片进入播放窗口；进度条可拖动跳转，右下角可切全屏，双击画面也能全屏，`ESC` 退出全屏。
4. **删除**：卡片右键选删除，只删数据库记录，不动磁盘上的原文件。
5. **直播录制**：进入「直播」页 →「开启直播」，预览窗口出现摄像头画面；点「结束直播并生成回放」，程序会把 `bin/temp.h264` 和 `bin/temp.aac` 交给 `ffmpeg.exe` 合成，输出到 `bin/dtbase_path/video_path/LiveRecord_年月日_时分秒.mp4`。

## 这个项目的优点

- **不用 QMediaPlayer 现成播放器**：自己控制 avformat → avcodec → sws_scale 的完整解码链路，视频帧转成 `QImage` 后推给界面，音频走 `QAudioOutput`，解码与播放分别在独立线程，界面不卡顿。
- **业务闭环完整**：注册登录 → 视频库 CRUD → 播放 → 采集录制 → 回放落盘，一条链路都能跑通，不是只有界面的空壳。
- **分层清晰**：VIEW / CONTROLLER / MODEL / DBdate / Video 五层分离，单体实例（`getInstance`）管理数据库与控制器，后续替换存储或解码实现时改动面小。
- **自绘 UI 风格统一**：无边框窗口、渐变按钮、淡入淡出切页、暗色主题卡片与细滚动条，全由 QSS + 事件重写实现，不依赖第三方 UI 库。
- **开箱即构建**：FFmpeg 头文件、导入库、运行时 DLL、`ffmpeg.exe`、数据库模板都在仓库里，clone 后只差一个 Qt 安装就能编译运行。

## 已知限制

有些地方是练手阶段的"半成品"，用之前先知道，避免误判为 bug：

| 位置 | 现状 |
|---|---|
| 倍速下拉框 | 只输出日志，未真正改变解码/播放速度（[playwindow.cpp](StreamBox/VIEW/playwindow.cpp) `onSpeedChanged`） |
| 系统设置页 | 复选框与「清理缓存」为界面占位，勾选不持久化，缓存数字是写死的 |
| 摄像头设备名 | 硬编码为 `video=Integrated Camera`（[videodecode.cpp](StreamBox/Video/videodecode.cpp)），换了机器要改成自己的摄像头名称 |
| 麦克风设备名 | 硬编码为 `audio=麦克风阵列 (Realtek(R) Audio)`（[audiodecode.cpp](StreamBox/Video/audiodecode.cpp)），同样需要按 `dshow` 设备名修改 |
| 账号密码 | 明文存放在 SQLite 的 `user` 表，仅适合本地演示，不要用于真实场景 |
| 视频素材 | 仓库不含示例视频，首页初始为空，需要自行「添加视频」 |
| 数据库记录 | 视频路径以绝对路径存储，移动文件或在别的机器打开会失效 |

## 后续可以加的东西

按投入产出排了个优先级，供继续迭代参考：

1. **设备枚举**：用 `avdevice_list_input_sources` 列出本机摄像头 / 麦克风，下拉框选择，去掉硬编码设备名（这是当前最容易踩坑的一处）。
2. **倍速播放落地**：按 `speed` 换算每帧 pts 的等待时长，音视频同步时间基准改用 `QElapsedTimer`。
3. **相对路径 + 文件校验**：入库时存相对 `bin/` 的路径，启动时检查文件是否存在，缺失的卡片置灰提示"文件已移动"。
4. **播放记忆**：把 `watch_history` 表接上，记录每个用户的播放进度，下次从头续播。
5. **设置持久化**：用 `QSettings` 存开机自启、关闭最小化、默认音量等选项；缓存清理改成真正扫描 `dtbase_path`。
6. **密码加盐哈希**：换成 SHA-256 + 随机盐，顺手补上登录失败次数限制。
7. **UI 细节**：封面自动抽帧（FFmpeg 截首帧当封面）、加载转圈、删除文件时可选同步删源文件。
8. **打包分发**：接一个 `windeployqt` + Inno Setup 脚本，产出免安装版。

## 贡献方式

```bash
git add <改动的文件>
git commit -m "fix: 描述这次改了什么"
git push
```

提交前建议在 Qt Creator 里完整构建一次，并确认 `bin/` 下没有被误加入版本库的 `StreamBox.exe`、`temp.*`、视频素材（`.gitignore` 已经排除）。

## 版本

StreamBox v1.0.0 · Copyright © 2026 StreamBox Inc.
