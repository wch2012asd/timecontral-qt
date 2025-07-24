#include "integrated_demo_window.h"
#include <QApplication>
#include <QGroupBox>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

IntegratedDemoWindow::IntegratedDemoWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_dateSection(nullptr)
    , m_datePicker(nullptr)
    , m_dateInfoLabel(nullptr)
    , m_timelineSection(nullptr)
    , m_timeContral(nullptr)
    , m_timelineInfoLabel(nullptr)
    , m_playSection(nullptr)
    , m_timePlayControl(nullptr)
    , m_playInfoLabel(nullptr)
    , m_currentDate(QDate::currentDate())
    , m_currentHour(12)
    , m_baseDateTime(QDateTime::currentDateTime())
{
    setWindowTitle("集成演示 - 日期控制器与时间轴播放控件联动");
    setMinimumSize(1000, 800);
    resize(1200, 900);
    
    // 设置窗口半透明背景，保留标题栏
    setAttribute(Qt::WA_TranslucentBackground);
    
    setupUI();
    setupConnections();
    
    // 初始化时间轴
    updateTimelineForDate(m_currentDate);
}

IntegratedDemoWindow::~IntegratedDemoWindow()
{
}

void IntegratedDemoWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    m_centralWidget->setStyleSheet("background: transparent;");
    setCentralWidget(m_centralWidget);
    
    // 创建主布局
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(15);
    
    // 添加标题
    QLabel *titleLabel = new QLabel("集成演示 - 日期控制器与时间轴播放控件联动", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        font-size: 18px; 
        font-weight: bold; 
        margin: 15px; 
        padding: 12px;
        color: white; 
        background-color: rgba(44, 62, 80, 150);
        border-radius: 8px;
        border: 1px solid rgba(255, 255, 255, 100);
    )");
    m_mainLayout->addWidget(titleLabel);
    
    // 1. 日期选择器部分
    m_dateSection = new QWidget(this);
    m_dateSection->setStyleSheet(R"(
        background-color: rgba(52, 73, 94, 120);
        border: 2px solid rgba(255, 255, 255, 100);
        border-radius: 10px;
    )");
    QVBoxLayout *dateLayout = new QVBoxLayout(m_dateSection);
    dateLayout->setContentsMargins(15, 15, 15, 15);
    
    QLabel *dateLabel = new QLabel("1. 选择日期", this);
    dateLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white; margin-bottom: 10px;");
    dateLayout->addWidget(dateLabel);
    
    m_datePicker = new DatePicker(this);
    m_datePicker->setSelectedDate(m_currentDate);
    dateLayout->addWidget(m_datePicker, 0, Qt::AlignCenter);
    
    m_dateInfoLabel = new QLabel("选择日期后，时间轴将调整到该日期的12:00", this);
    m_dateInfoLabel->setStyleSheet("color: rgba(255, 255, 255, 180); font-size: 12px; margin-top: 5px;");
    m_dateInfoLabel->setAlignment(Qt::AlignCenter);
    dateLayout->addWidget(m_dateInfoLabel);
    
    m_mainLayout->addWidget(m_dateSection);
    
    // 2. 时间轴部分
    m_timelineSection = new QWidget(this);
    m_timelineSection->setStyleSheet(R"(
        background-color: rgba(52, 73, 94, 120);
        border: 2px solid rgba(255, 255, 255, 100);
        border-radius: 10px;
    )");
    QVBoxLayout *timelineLayout = new QVBoxLayout(m_timelineSection);
    timelineLayout->setContentsMargins(15, 15, 15, 15);
    
    QLabel *timelineLabel = new QLabel("2. 时间轴显示", this);
    timelineLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white; margin-bottom: 10px;");
    timelineLayout->addWidget(timelineLabel);
    
    m_timeContral = new TimeContral(this);
    m_timeContral->setMinimumHeight(120);
    timelineLayout->addWidget(m_timeContral);
    
    m_timelineInfoLabel = new QLabel("时间轴显示选定日期的24小时时间线", this);
    m_timelineInfoLabel->setStyleSheet("color: rgba(255, 255, 255, 180); font-size: 12px; margin-top: 5px;");
    m_timelineInfoLabel->setAlignment(Qt::AlignCenter);
    timelineLayout->addWidget(m_timelineInfoLabel);
    
    m_mainLayout->addWidget(m_timelineSection);
    
    // 3. 播放控制部分
    m_playSection = new QWidget(this);
    m_playSection->setStyleSheet(R"(
        background-color: rgba(52, 73, 94, 120);
        border: 2px solid rgba(255, 255, 255, 100);
        border-radius: 10px;
    )");
    QVBoxLayout *playLayout = new QVBoxLayout(m_playSection);
    playLayout->setContentsMargins(15, 15, 15, 15);
    
    QLabel *playLabel = new QLabel("3. 播放控制", this);
    playLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white; margin-bottom: 10px;");
    playLayout->addWidget(playLabel);
    
    m_timePlayControl = new TimePlayControl(this);
    playLayout->addWidget(m_timePlayControl, 0, Qt::AlignCenter);
    
    m_playInfoLabel = new QLabel("使用播放控件控制时间轴：上一个/下一个小时，播放时每3秒前进1小时", this);
    m_playInfoLabel->setStyleSheet("color: rgba(255, 255, 255, 180); font-size: 12px; margin-top: 5px;");
    m_playInfoLabel->setAlignment(Qt::AlignCenter);
    playLayout->addWidget(m_playInfoLabel);
    
    m_mainLayout->addWidget(m_playSection);
    
    m_mainLayout->addStretch();
}

void IntegratedDemoWindow::setupConnections()
{
    // 连接日期选择器信号
    connect(m_datePicker, &DatePicker::dateChanged, this, &IntegratedDemoWindow::onDateChanged);
    connect(m_datePicker, &DatePicker::dateSelected, this, &IntegratedDemoWindow::onDateSelected);
    
    // 连接播放控制器信号
    connect(m_timePlayControl, &TimePlayControl::playStateChanged, this, &IntegratedDemoWindow::onPlayStateChanged);
    connect(m_timePlayControl, &TimePlayControl::currentTimeChanged, this, &IntegratedDemoWindow::onCurrentTimeChanged);
    connect(m_timePlayControl, &TimePlayControl::stepForwardClicked, this, &IntegratedDemoWindow::onStepForwardClicked);
    connect(m_timePlayControl, &TimePlayControl::stepBackwardClicked, this, &IntegratedDemoWindow::onStepBackwardClicked);
    connect(m_timePlayControl, &TimePlayControl::playClicked, this, &IntegratedDemoWindow::onPlayClicked);
    connect(m_timePlayControl, &TimePlayControl::pauseClicked, this, &IntegratedDemoWindow::onPauseClicked);
    
    // 设置播放控制器参数
    m_timePlayControl->setStepInterval(3600); // 1小时 = 3600秒
    m_timePlayControl->setPlaySpeed(1.0);     // 每3秒前进1小时，通过定时器间隔控制
}

void IntegratedDemoWindow::onDateChanged(const QDate &date)
{
    m_currentDate = date;
    updateTimelineForDate(date);
    m_dateInfoLabel->setText(QString("已选择日期: %1，时间轴已调整到 %2 12:00")
                            .arg(date.toString("yyyy年MM月dd日"))
                            .arg(date.toString("yyyy-MM-dd")));
}

void IntegratedDemoWindow::onDateSelected(const QDate &date)
{
    m_currentDate = date;
    m_currentHour = 12; // 重置到12点
    updateTimelineForDate(date);
    moveTimelineBubbleToHour(12);
    
    m_dateInfoLabel->setText(QString("✅ 确认选择日期: %1，时间轴设置为 12:00")
                            .arg(date.toString("yyyy年MM月dd日")));
}

void IntegratedDemoWindow::onPlayStateChanged(TimePlayControl::PlayState state)
{
    QString stateText;
    switch (state) {
    case TimePlayControl::Stopped:
        stateText = "停止";
        break;
    case TimePlayControl::Playing:
        stateText = "播放中 - 每3秒前进1小时";
        // 设置播放控制器的定时器间隔为3秒
        m_timePlayControl->setPlaySpeed(1.0/3.0); // 每3秒更新一次
        break;
    case TimePlayControl::Paused:
        stateText = "暂停";
        break;
    }
    
    m_playInfoLabel->setText(QString("播放状态: %1").arg(stateText));
}

void IntegratedDemoWindow::onCurrentTimeChanged(const QDateTime &time)
{
    // 当播放控制器的时间改变时，更新时间轴气泡位置
    int hour = time.time().hour();
    if (hour != m_currentHour) {
        m_currentHour = hour;
        moveTimelineBubbleToHour(hour);
        
        m_timelineInfoLabel->setText(QString("时间轴当前时间: %1 %2:00")
                                    .arg(m_currentDate.toString("yyyy-MM-dd"))
                                    .arg(hour, 2, 10, QChar('0')));
    }
}

void IntegratedDemoWindow::onStepForwardClicked()
{
    // 下一个小时
    m_currentHour = (m_currentHour + 1) % 24;
    moveTimelineBubbleToHour(m_currentHour);
    
    // 更新播放控制器的当前时间
    QDateTime newTime(m_currentDate, QTime(m_currentHour, 0));
    m_timePlayControl->setCurrentTime(newTime);
    
    m_playInfoLabel->setText(QString("⏭ 前进到 %1:00").arg(m_currentHour, 2, 10, QChar('0')));
}

void IntegratedDemoWindow::onStepBackwardClicked()
{
    // 上一个小时
    m_currentHour = (m_currentHour - 1 + 24) % 24;
    moveTimelineBubbleToHour(m_currentHour);
    
    // 更新播放控制器的当前时间
    QDateTime newTime(m_currentDate, QTime(m_currentHour, 0));
    m_timePlayControl->setCurrentTime(newTime);
    
    m_playInfoLabel->setText(QString("⏮ 后退到 %1:00").arg(m_currentHour, 2, 10, QChar('0')));
}

void IntegratedDemoWindow::onPlayClicked()
{
    m_playInfoLabel->setText("▶ 开始播放 - 每3秒前进1小时");
}

void IntegratedDemoWindow::onPauseClicked()
{
    m_playInfoLabel->setText("⏸ 暂停播放");
}

void IntegratedDemoWindow::updateTimelineForDate(const QDate &date)
{
    // 设置时间轴的时间范围为选定日期的00:00到23:59
    QDateTime startTime(date, QTime(0, 0, 0));
    QDateTime endTime(date, QTime(23, 59, 59));
    QDateTime currentTime(date, QTime(12, 0, 0)); // 默认设置为12:00
    
    m_timeContral->setTimeRange(startTime, endTime);
    m_timeContral->setVisibleTimeRange(startTime, endTime);
    m_timeContral->setCurrentTime(currentTime);
    
    // 更新播放控制器的时间范围
    m_timePlayControl->setTimeRange(startTime, endTime);
    m_timePlayControl->setCurrentTime(currentTime);
    
    m_baseDateTime = startTime;
    m_currentHour = 12;
}

void IntegratedDemoWindow::moveTimelineBubbleToHour(int hour)
{
    // 移动时间轴气泡到指定小时
    QDateTime targetTime(m_currentDate, QTime(hour, 0, 0));
    m_timeContral->setCurrentTime(targetTime);
    
    m_timelineInfoLabel->setText(QString("时间轴气泡移动到: %1 %2:00")
                                .arg(m_currentDate.toString("yyyy-MM-dd"))
                                .arg(hour, 2, 10, QChar('0')));
}

void IntegratedDemoWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 获取客户区域（排除标题栏）
    QRect clientRect = centralWidget()->geometry();
    
    // 绘制半透明背景
    QLinearGradient gradient(0, clientRect.top(), 0, clientRect.bottom());
    gradient.setColorAt(0, QColor(52, 73, 94, 80));    // 深蓝灰色
    gradient.setColorAt(0.3, QColor(44, 62, 80, 60));  // 更深的蓝灰色
    gradient.setColorAt(0.7, QColor(52, 73, 94, 70));  // 深蓝灰色
    gradient.setColorAt(1, QColor(44, 62, 80, 90));    // 更深的蓝灰色
    
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(clientRect.adjusted(5, 5, -5, -5), 15, 15);
    
    // 添加玻璃效果边框
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255, 255, 255, 100), 2));
    painter.drawRoundedRect(clientRect.adjusted(6, 6, -6, -6), 13, 13);
    
    // 添加内部高光效果
    QLinearGradient highlightGradient(0, clientRect.top(), 0, clientRect.top() + clientRect.height() * 0.3);
    highlightGradient.setColorAt(0, QColor(255, 255, 255, 30));
    highlightGradient.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(QBrush(highlightGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(clientRect.adjusted(8, 8, -8, -clientRect.height() * 0.7), 10, 10);
}