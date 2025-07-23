# TimeContral 时间轴控件

一个现代化的Qt时间轴控件，具有精美的磨砂玻璃效果和丰富的交互功能。适用于时间管理、项目规划、数据可视化等场景。

![TimeContral Demo](screenshot.png)

## ✨ 核心特性

### 🎨 现代化UI设计
- **磨砂玻璃背景**: 深天蓝色半透明磨砂质感，现代化视觉效果
- **优雅的时间气泡**: 半透明淡绿色气泡显示当前时间
- **平滑动画**: 丝滑的滚动和缩放动画效果
- **响应式布局**: 自适应窗口大小，完美显示24小时时间轴

### ⚡ 强大的交互功能
- **无限滚动**: 支持无限制时间范围滚动，查看任意时间段
- **智能拖拽**: 平滑的时间轴滚动，带阻尼效果
- **双击定位**: 双击时间轴快速定位到整点时间
- **滚轮缩放**: 鼠标滚轮缩放时间轴显示精度
- **实时更新**: 动态显示当前时间和日期

### 📅 灵活的时间管理
- **时间点标记**: 添加重要时间节点，支持自定义颜色和标签
- **时间段显示**: 可视化时间段，适合项目进度展示
- **多时间范围**: 支持跨天显示，自动更新日期信息
- **精确定位**: 时间精度到秒，支持多种时间格式

### 🔧 高度可定制
- **滚动模式**: 可选择无限滚动或限定范围滚动
- **外观定制**: 自定义背景、刻度、文本颜色
- **显示选项**: 可控制气泡、日期、刻度等元素显示
- **透明窗口**: 支持透明背景，完美融入桌面环境

## 文件结构

```
├── timecontral.h              # 时间轴控件头文件
├── timecontral.cpp            # 时间轴控件实现文件
├── timeContral_global.h       # 全局定义头文件
├── timeContral.pro           # 库项目文件
├── demo_window.h             # 演示窗口头文件
├── demo_window.cpp           # 演示窗口实现文件
├── demo_main.cpp             # 演示程序主文件
├── demo.pro                  # 演示程序项目文件
├── test_timecontral.pro      # 测试项目文件
├── test_main.cpp             # 测试主程序
├── test_timecontral_unit.h   # 单元测试头文件
├── test_timecontral_unit.cpp # 单元测试实现
└── README.md                 # 说明文档
```

## 编译和运行

### 编译演示程序

1. 确保已安装Qt 5.12或更高版本
2. 打开命令行，进入项目目录
3. 运行以下命令：

```bash
qmake demo.pro
make  # Linux/Mac
nmake # Windows (使用MSVC)
mingw32-make # Windows (使用MinGW)
```

4. 运行生成的可执行文件：

```bash
./bin/timecontral_demo  # Linux/Mac
bin\timecontral_demo.exe # Windows
```

### 编译测试程序

```bash
qmake test_timecontral.pro
make  # Linux/Mac
nmake # Windows (使用MSVC)
./test_timecontral  # 运行测试
```

### 编译库文件

```bash
qmake timeContral.pro
make  # Linux/Mac
nmake # Windows (使用MSVC)
```

## 🚀 快速开始

### 基本用法

```cpp
#include "timecontral.h"

// 创建时间轴控件
TimeContral *timeControl = new TimeContral(parent);

// 设置24小时显示范围
QDateTime today = QDateTime::currentDateTime();
QDateTime startOfDay = QDateTime(today.date(), QTime(0, 0, 0));
QDateTime endOfDay = QDateTime(today.date(), QTime(23, 59, 59));

// 启用无限滚动模式（默认）
timeControl->setInfiniteScrollEnabled(true);
timeControl->setVisibleTimeRange(startOfDay, endOfDay);

// 添加时间标记
timeControl->addTimePoint(QDateTime::currentDateTime(), "现在", Qt::red);
timeControl->addTimeSpan(
    QDateTime::currentDateTime().addHours(1),
    QDateTime::currentDateTime().addHours(3),
    "会议时间", Qt::blue
);

// 设置当前时间（显示气泡）
timeControl->setCurrentTime(QDateTime::currentDateTime());
```

### 高级配置

```cpp
// 限制滚动范围
timeControl->setInfiniteScrollEnabled(false);
timeControl->setTimeRange(startTime, endTime);

// 自定义外观
timeControl->setBackgroundColor(QColor(70, 130, 180, 200));
timeControl->setShowTimeBubble(true);
timeControl->setShowDateOnTimeline(true);

// 设置缩放级别
timeControl->setZoomLevel(1.5);
```

### 事件处理

```cpp
// 时间项交互
connect(timeControl, &TimeContral::timeItemClicked, [](int index) {
    qDebug() << "点击时间项:" << index;
});

connect(timeControl, &TimeContral::timeItemDoubleClicked, [](int index) {
    qDebug() << "双击时间项:" << index;
});

// 时间变化监听
connect(timeControl, &TimeContral::currentTimeChanged, [](const QDateTime &time) {
    qDebug() << "当前时间:" << time.toString("yyyy-MM-dd hh:mm:ss");
});

// 可见范围变化（滚动时触发）
connect(timeControl, &TimeContral::visibleTimeRangeChanged, 
        [](const QDateTime &start, const QDateTime &end) {
    qDebug() << "可见范围:" << start.toString() << "到" << end.toString();
});
```

## 演示程序功能

演示程序提供了完整的控件功能展示：

### 控制面板
- **时间范围设置**: 设置时间轴的最小和最大时间
- **可见范围设置**: 设置当前可见的时间范围
- **时间项管理**: 添加、删除时间点和时间段
- **缩放控制**: 放大、缩小、重置缩放级别

### 外观设置
- **颜色设置**: 自定义背景色、刻度色、文本色
- **显示选项**: 控制时间气泡和日期显示
- **格式设置**: 自定义时间显示格式
- **刻度高度**: 调整时间刻度的高度

### 演示功能
- **示例数据**: 加载预设的示例时间项
- **实时演示**: 自动更新当前时间并添加随机事件
- **数据导入导出**: 保存和加载时间轴数据

### 事件日志
- 显示所有用户操作和控件事件的日志
- 实时更新状态信息

## 📖 API 参考

### 时间范围管理
```cpp
// 设置总时间范围（用于限制滚动）
void setTimeRange(const QDateTime &minTime, const QDateTime &maxTime);
QDateTime minTime() const;
QDateTime maxTime() const;

// 设置可见时间范围（当前显示的时间段）
void setVisibleTimeRange(const QDateTime &startTime, const QDateTime &endTime);
QDateTime visibleStartTime() const;
QDateTime visibleEndTime() const;

// 滚动模式控制
void setInfiniteScrollEnabled(bool enabled);  // 启用/禁用无限滚动
bool isInfiniteScrollEnabled() const;
```

### 时间项管理
```cpp
// 添加时间标记
int addTimePoint(const QDateTime &time, const QString &label = QString(), 
                const QColor &color = Qt::blue, const QVariant &userData = QVariant());

int addTimeSpan(const QDateTime &startTime, const QDateTime &endTime, 
               const QString &label = QString(), const QColor &color = Qt::green,
               const QVariant &userData = QVariant());

// 管理时间项
bool removeTimeItem(int index);
void clearTimeItems();
int timeItemCount() const;
TimeItem timeItemAt(int index) const;

// 选中状态
void setCurrentTimeItem(int index);
int currentTimeItem() const;
```

### 当前时间控制
```cpp
void setCurrentTime(const QDateTime &time);  // 设置当前时间（显示气泡）
QDateTime currentTime() const;
```

### 外观定制
```cpp
// 颜色设置
void setBackgroundColor(const QColor &color);
void setScaleColor(const QColor &color);
void setTextColor(const QColor &color);

// 显示选项
void setShowTimeBubble(bool show);           // 显示/隐藏时间气泡
bool isShowTimeBubble() const;
void setShowDateOnTimeline(bool show);       // 显示/隐藏日期
bool isShowDateOnTimeline() const;

// 其他外观
void setScaleHeight(int height);             // 刻度高度
void setTimeFormat(const QString &format);   // 时间格式
```

### 缩放控制
```cpp
void zoomIn();                              // 放大
void zoomOut();                             // 缩小
void setZoomLevel(double level);            // 设置缩放级别 (0.1-10.0)
double zoomLevel() const;
```

### 信号列表
```cpp
// 交互信号
void timeItemClicked(int index);            // 时间项被点击
void timeItemDoubleClicked(int index);      // 时间项被双击
void currentTimeItemChanged(int index);     // 选中项改变

// 时间变化信号
void currentTimeChanged(const QDateTime &time);                    // 当前时间改变
void timeRangeChanged(const QDateTime &minTime, const QDateTime &maxTime);  // 时间范围改变
void visibleTimeRangeChanged(const QDateTime &startTime, const QDateTime &endTime);  // 可见范围改变
```

## 🎮 交互说明

### 鼠标操作
- **拖拽**: 水平拖拽滚动时间轴，查看不同时间段
- **双击**: 双击时间轴定位到对应的整点时间
- **滚轮**: 滚动缩放时间轴显示精度
- **悬停**: 鼠标悬停在时间项上显示详细信息

### 键盘操作
- 支持通过编程方式控制所有功能
- 可集成自定义快捷键

## 🎯 使用场景

### 项目管理
- 项目时间线展示
- 里程碑标记
- 进度可视化

### 数据分析
- 时间序列数据展示
- 事件时间轴
- 趋势分析

### 系统监控
- 实时状态显示
- 历史事件回放
- 性能指标时间轴

### 日程管理
- 日程安排可视化
- 时间块管理
- 会议时间展示

## 🔧 技术特点

### 性能优化
- 高效的绘制算法，支持大量时间项
- 智能的可见区域计算，只绘制必要内容
- 平滑的动画效果，60fps流畅体验

### 兼容性
- 支持Qt 5.12及以上版本
- 跨平台支持：Windows、Linux、macOS
- 支持高DPI显示屏

### 扩展性
- 模块化设计，易于扩展
- 丰富的信号系统，便于集成
- 支持自定义样式和主题

## 🐛 已知问题

- 在某些Linux发行版上可能需要额外的字体配置
- 高DPI屏幕上的文本渲染可能需要调整

## 📝 更新日志

### v1.0.0 (2025-01-23)
- ✨ 全新的磨砂玻璃UI设计
- 🚀 无限滚动功能
- 🎯 双击定位到整点
- 🎨 半透明气泡效果
- 📱 透明窗口支持
- ⚡ 平滑滚动动画
- 🔧 完整的API接口

## 📄 许可证

本项目采用MIT许可证，详见LICENSE文件。

## 🤝 贡献

欢迎提交问题报告和功能请求！

1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

## 📞 联系方式

如有问题或建议，请通过以下方式联系：

- 提交 Issue
- 发送邮件
- 创建 Discussion

---

⭐ 如果这个项目对你有帮助，请给个星标支持一下！