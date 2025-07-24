#include "timeplay_demo_window.h"
#include <QApplication>
#include <QMessageBox>
#include <QSplitter>
#include <QPainter>
#include <QPaintEvent>

TimePlayDemoWindow::TimePlayDemoWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_timePlayControl(nullptr)
    , m_controlGroup(nullptr)
    , m_speedSlider(nullptr)
    , m_stepIntervalSpinBox(nullptr)
    , m_startTimeEdit(nullptr)
    , m_endTimeEdit(nullptr)
    , m_statusGroup(nullptr)
    , m_playStateLabel(nullptr)
    , m_currentTimeLabel(nullptr)
    , m_timeRangeLabel(nullptr)
    , m_infoLabel(nullptr)
{
    setWindowTitle("时间轴播放控件演示 - TimePlayControl Demo");
    setMinimumSize(800, 600);
    resize(1000, 700);
    
    // 设置窗口半透明背景，保留标题栏
    setAttribute(Qt::WA_TranslucentBackground);
    // 保留标题栏，不设置FramelessWindowHint
    
    setupUI();
    setupTimePlayControl();
    updateStatusLabels();
}

TimePlayDemoWindow::~TimePlayDemoWindow()
{
}

void TimePlayDemoWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    m_centralWidget->setStyleSheet("background: transparent;");
    setCentralWidget(m_centralWidget);
    
    // 创建主布局
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(30, 30, 30, 30);
    m_mainLayout->setSpacing(20);
    
    // 添加标题 - 半透明背景
    QLabel *titleLabel = new QLabel("时间轴播放控件演示", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        font-size: 20px; 
        font-weight: bold; 
        margin: 20px; 
        padding: 15px;
        color: white; 
        background-color: rgba(44, 62, 80, 150);
        border-radius: 10px;
        border: 1px solid rgba(255, 255, 255, 100);
    )");
    m_mainLayout->addWidget(titleLabel);
    
    // 创建水平分割器
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    m_mainLayout->addWidget(splitter);
    
    // 左侧：时间轴播放控件
    QWidget *leftPanel = new QWidget(this);
    leftPanel->setStyleSheet(R"(
        background-color: rgba(52, 73, 94, 120);
        border: 2px solid rgba(255, 255, 255, 100);
        border-radius: 10px;
    )");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *controlLabel = new QLabel("时间轴播放控件", this);
    controlLabel->setAlignment(Qt::AlignCenter);
    controlLabel->setStyleSheet(R"(
        font-size: 16px; 
        font-weight: bold; 
        color: white;
        margin-bottom: 20px;
    )");
    leftLayout->addWidget(controlLabel);
    
    // 创建时间轴播放控件
    m_timePlayControl = new TimePlayControl(this);
    leftLayout->addWidget(m_timePlayControl);
    
    leftLayout->addStretch();
    splitter->addWidget(leftPanel);
    
    // 右侧：控制面板和状态显示
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setMaximumWidth(350);
    rightPanel->setMinimumWidth(300);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    // 控制面板
    m_controlGroup = new QGroupBox("控制面板", this);
    m_controlGroup->setStyleSheet(R"(
        QGroupBox { 
            font-size: 14px; 
            font-weight: bold; 
            color: white;
            background-color: rgba(52, 73, 94, 120);
            border: 2px solid rgba(255, 255, 255, 100);
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 5px 10px;
            background-color: rgba(41, 128, 185, 150);
            border-radius: 5px;
        }
    )");
    QVBoxLayout *controlLayout = new QVBoxLayout(m_controlGroup);
    controlLayout->setContentsMargins(15, 20, 15, 15);
    
    // 播放速度控制
    QLabel *speedLabel = new QLabel("播放速度:", this);
    speedLabel->setStyleSheet("color: white; font-size: 12px;");
    controlLayout->addWidget(speedLabel);
    
    m_speedSlider = new QSlider(Qt::Horizontal, this);
    m_speedSlider->setRange(1, 50);
    m_speedSlider->setValue(10);
    m_speedSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            border: 1px solid rgba(255, 255, 255, 100);
            height: 8px;
            background: rgba(255, 255, 255, 50);
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: rgba(0, 255, 255, 200);
            border: 1px solid rgba(0, 255, 255, 255);
            width: 18px;
            margin: -5px 0;
            border-radius: 9px;
        }
    )");
    connect(m_speedSlider, &QSlider::valueChanged, this, &TimePlayDemoWindow::onSpeedChanged);
    controlLayout->addWidget(m_speedSlider);
    
    // 步进间隔控制
    QLabel *stepLabel = new QLabel("步进间隔(秒):", this);
    stepLabel->setStyleSheet("color: white; font-size: 12px;");
    controlLayout->addWidget(stepLabel);
    
    m_stepIntervalSpinBox = new QSpinBox(this);
    m_stepIntervalSpinBox->setRange(1, 3600);
    m_stepIntervalSpinBox->setValue(60);
    m_stepIntervalSpinBox->setStyleSheet(R"(
        QSpinBox {
            background: rgba(255, 255, 255, 100);
            border: 1px solid rgba(255, 255, 255, 150);
            border-radius: 4px;
            padding: 5px;
            color: white;
        }
    )");
    connect(m_stepIntervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &TimePlayDemoWindow::onStepIntervalChanged);
    controlLayout->addWidget(m_stepIntervalSpinBox);
    
    // 时间范围设置
    QLabel *timeRangeLabel = new QLabel("时间范围:", this);
    timeRangeLabel->setStyleSheet("color: white; font-size: 12px; margin-top: 10px;");
    controlLayout->addWidget(timeRangeLabel);
    
    m_startTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(-3600), this);
    m_startTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    m_startTimeEdit->setStyleSheet(R"(
        QDateTimeEdit {
            background: rgba(255, 255, 255, 100);
            border: 1px solid rgba(255, 255, 255, 150);
            border-radius: 4px;
            padding: 5px;
            color: white;
        }
    )");
    connect(m_startTimeEdit, &QDateTimeEdit::dateTimeChanged,
            this, &TimePlayDemoWindow::onStartTimeChanged);
    controlLayout->addWidget(m_startTimeEdit);
    
    m_endTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3600), this);
    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    m_endTimeEdit->setStyleSheet(R"(
        QDateTimeEdit {
            background: rgba(255, 255, 255, 100);
            border: 1px solid rgba(255, 255, 255, 150);
            border-radius: 4px;
            padding: 5px;
            color: white;
        }
    )");
    connect(m_endTimeEdit, &QDateTimeEdit::dateTimeChanged,
            this, &TimePlayDemoWindow::onEndTimeChanged);
    controlLayout->addWidget(m_endTimeEdit);
    
    rightLayout->addWidget(m_controlGroup);
    
    // 状态显示面板
    m_statusGroup = new QGroupBox("状态信息", this);
    m_statusGroup->setStyleSheet(R"(
        QGroupBox { 
            font-size: 14px; 
            font-weight: bold; 
            color: white;
            background-color: rgba(52, 73, 94, 120);
            border: 2px solid rgba(255, 255, 255, 100);
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 5px 10px;
            background-color: rgba(41, 128, 185, 150);
            border-radius: 5px;
        }
    )");
    QVBoxLayout *statusLayout = new QVBoxLayout(m_statusGroup);
    statusLayout->setContentsMargins(15, 20, 15, 15);
    
    m_playStateLabel = new QLabel("播放状态: 停止", this);
    m_playStateLabel->setStyleSheet("color: white; font-size: 12px; padding: 5px; background-color: rgba(255, 255, 255, 50); border-radius: 3px;");
    statusLayout->addWidget(m_playStateLabel);
    
    m_currentTimeLabel = new QLabel("当前时间: ", this);
    m_currentTimeLabel->setStyleSheet("color: white; font-size: 12px; padding: 5px; background-color: rgba(255, 255, 255, 50); border-radius: 3px;");
    statusLayout->addWidget(m_currentTimeLabel);
    
    m_timeRangeLabel = new QLabel("时间范围: ", this);
    m_timeRangeLabel->setStyleSheet("color: white; font-size: 12px; padding: 5px; background-color: rgba(255, 255, 255, 50); border-radius: 3px;");
    statusLayout->addWidget(m_timeRangeLabel);
    
    rightLayout->addWidget(m_statusGroup);
    
    // 信息显示标签
    m_infoLabel = new QLabel("使用播放控件来控制时间轴播放", this);
    m_infoLabel->setAlignment(Qt::AlignCenter);
    m_infoLabel->setStyleSheet(R"(
        font-size: 14px; 
        color: white; 
        margin: 20px; 
        padding: 15px; 
        background-color: rgba(236, 240, 241, 150); 
        border-radius: 8px;
        border: 2px solid rgba(189, 195, 199, 120);
    )");
    rightLayout->addWidget(m_infoLabel);
    
    rightLayout->addStretch();
    splitter->addWidget(rightPanel);
    
    // 设置分割器比例
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
}

void TimePlayDemoWindow::setupTimePlayControl()
{
    // 连接信号
    connect(m_timePlayControl, &TimePlayControl::playStateChanged,
            this, &TimePlayDemoWindow::onPlayStateChanged);
    connect(m_timePlayControl, &TimePlayControl::currentTimeChanged,
            this, &TimePlayDemoWindow::onCurrentTimeChanged);
    connect(m_timePlayControl, &TimePlayControl::playClicked,
            this, &TimePlayDemoWindow::onPlayClicked);
    connect(m_timePlayControl, &TimePlayControl::pauseClicked,
            this, &TimePlayDemoWindow::onPauseClicked);
    connect(m_timePlayControl, &TimePlayControl::stepForwardClicked,
            this, &TimePlayDemoWindow::onStepForwardClicked);
    connect(m_timePlayControl, &TimePlayControl::stepBackwardClicked,
            this, &TimePlayDemoWindow::onStepBackwardClicked);
    
    // 设置初始值
    m_timePlayControl->setTimeRange(m_startTimeEdit->dateTime(), m_endTimeEdit->dateTime());
    m_timePlayControl->setPlaySpeed(m_speedSlider->value() / 10.0);
    m_timePlayControl->setStepInterval(m_stepIntervalSpinBox->value());
}

void TimePlayDemoWindow::onPlayStateChanged(TimePlayControl::PlayState state)
{
    updateStatusLabels();
    
    QString stateText;
    switch (state) {
    case TimePlayControl::Stopped:
        stateText = "停止";
        break;
    case TimePlayControl::Playing:
        stateText = "播放中";
        break;
    case TimePlayControl::Paused:
        stateText = "暂停";
        break;
    }
    
    m_infoLabel->setText(QString("播放状态已更改为: %1").arg(stateText));
}

void TimePlayDemoWindow::onCurrentTimeChanged(const QDateTime &time)
{
    updateStatusLabels();
}

void TimePlayDemoWindow::onPlayClicked()
{
    m_infoLabel->setText("▶ 开始播放");
}

void TimePlayDemoWindow::onPauseClicked()
{
    m_infoLabel->setText("⏸ 暂停播放");
}

void TimePlayDemoWindow::onStepForwardClicked()
{
    m_infoLabel->setText("›› 向前步进");
}

void TimePlayDemoWindow::onStepBackwardClicked()
{
    m_infoLabel->setText("‹‹ 向后步进");
}

void TimePlayDemoWindow::onSpeedChanged(int value)
{
    double speed = value / 10.0;
    m_timePlayControl->setPlaySpeed(speed);
    m_infoLabel->setText(QString("播放速度设置为: %1x").arg(speed, 0, 'f', 1));
}

void TimePlayDemoWindow::onStepIntervalChanged(int value)
{
    m_timePlayControl->setStepInterval(value);
    m_infoLabel->setText(QString("步进间隔设置为: %1秒").arg(value));
}

void TimePlayDemoWindow::onStartTimeChanged(const QDateTime &time)
{
    m_timePlayControl->setTimeRange(time, m_endTimeEdit->dateTime());
    updateStatusLabels();
}

void TimePlayDemoWindow::onEndTimeChanged(const QDateTime &time)
{
    m_timePlayControl->setTimeRange(m_startTimeEdit->dateTime(), time);
    updateStatusLabels();
}

void TimePlayDemoWindow::updateStatusLabels()
{
    if (m_playStateLabel) {
        QString stateText;
        switch (m_timePlayControl->playState()) {
        case TimePlayControl::Stopped:
            stateText = "停止";
            break;
        case TimePlayControl::Playing:
            stateText = "播放中";
            break;
        case TimePlayControl::Paused:
            stateText = "暂停";
            break;
        }
        m_playStateLabel->setText(QString("播放状态: %1").arg(stateText));
    }
    
    if (m_currentTimeLabel) {
        QString currentText = QString("当前时间: %1")
            .arg(m_timePlayControl->currentTime().toString("yyyy-MM-dd hh:mm:ss"));
        m_currentTimeLabel->setText(currentText);
    }
    
    if (m_timeRangeLabel) {
        QString rangeText = QString("时间范围: %1 ~ %2")
            .arg(m_timePlayControl->startTime().toString("hh:mm:ss"))
            .arg(m_timePlayControl->endTime().toString("hh:mm:ss"));
        m_timeRangeLabel->setText(rangeText);
    }
}

void TimePlayDemoWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 获取客户区域（排除标题栏）
    QRect clientRect = centralWidget()->geometry();
    
    // 绘制半透明背景
    QLinearGradient gradient(0, clientRect.top(), 0, clientRect.bottom());
    gradient.setColorAt(0, QColor(52, 73, 94, 100));   // 深蓝灰色
    gradient.setColorAt(0.3, QColor(44, 62, 80, 80));  // 更深的蓝灰色
    gradient.setColorAt(0.7, QColor(52, 73, 94, 90));  // 深蓝灰色
    gradient.setColorAt(1, QColor(44, 62, 80, 110));   // 更深的蓝灰色
    
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(clientRect.adjusted(5, 5, -5, -5), 15, 15);
    
    // 添加玻璃效果边框
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255, 255, 255, 120), 2));
    painter.drawRoundedRect(clientRect.adjusted(6, 6, -6, -6), 13, 13);
    
    // 添加内部高光效果
    QLinearGradient highlightGradient(0, clientRect.top(), 0, clientRect.top() + clientRect.height() * 0.4);
    highlightGradient.setColorAt(0, QColor(255, 255, 255, 40));
    highlightGradient.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(QBrush(highlightGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(clientRect.adjusted(8, 8, -8, -clientRect.height() * 0.6), 10, 10);
}