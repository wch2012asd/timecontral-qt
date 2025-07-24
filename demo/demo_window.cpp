#include "demo_window.h"
#include <QApplication>
#include <QLabel>

DemoWindow::DemoWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_timeContral(nullptr)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
{
    setWindowTitle("TimeContral 控件演示窗口");
    setMinimumSize(800, 200);
    resize(1000, 200);
    
    // 设置窗口半透明背景，保留标题栏
    setAttribute(Qt::WA_TranslucentBackground);
    // 保留标题栏，移除FramelessWindowHint，保留WindowStaysOnTopHint
    setWindowFlags(Qt::WindowStaysOnTopHint);
    
    setupUI();
    setupTimeContral();
    
}

DemoWindow::~DemoWindow()
{
}

void DemoWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    m_centralWidget->setStyleSheet("background: transparent;"); // 设置中央控件背景透明
    setCentralWidget(m_centralWidget);
    
    // 创建主布局
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    
    // 添加标题
    QLabel *titleLabel = new QLabel("TimeContral 时间轴控件演示");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px; color: white; background: transparent;");
    m_mainLayout->addWidget(titleLabel);
    
    // 创建时间轴控件
    m_timeContral = new TimeContral(this);
    m_mainLayout->addWidget(m_timeContral);
    
    m_mainLayout->addStretch();
}

void DemoWindow::setupTimeContral()
{
    // 设置更大的时间范围以支持滚动
    QDateTime today = QDateTime::currentDateTime();
    QDateTime startOfDay = QDateTime(today.date(), QTime(0, 0, 0));
    QDateTime endOfDay = QDateTime(today.date(), QTime(23, 59, 59));
    
    // 总时间范围：前后各扩展一天
    QDateTime totalStart = startOfDay.addDays(-1);
    QDateTime totalEnd = endOfDay.addDays(1);
    
    // 可见范围：显示完整的24小时
    QDateTime visibleStart = startOfDay;     // 从0点开始
    QDateTime visibleEnd = endOfDay;         // 到23:59结束
    
    m_timeContral->setTimeRange(totalStart, totalEnd);
    m_timeContral->setVisibleTimeRange(visibleStart, visibleEnd);
    m_timeContral->setCurrentTime(today);
}

