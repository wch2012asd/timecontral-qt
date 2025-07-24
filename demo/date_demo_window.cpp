#include "date_demo_window.h"
#include <QApplication>
#include <QMessageBox>
#include <QGroupBox>
#include <QPainter>
#include <QPaintEvent>

DateDemoWindow::DateDemoWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_datePicker1(nullptr)
    , m_infoLabel(nullptr)
{
    setWindowTitle("气泡式日期选择器演示 - DatePicker Demo");
    setMinimumSize(600, 400);
    resize(700, 500);
    
    // 设置窗口半透明背景，保留标题栏
    setAttribute(Qt::WA_TranslucentBackground);
    // 保留标题栏，不设置FramelessWindowHint
    
    setupUI();
    setupDatePickers();
}

DateDemoWindow::~DateDemoWindow()
{
}

void DateDemoWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    m_centralWidget->setStyleSheet("background: transparent;");
    setCentralWidget(m_centralWidget);
    
    // 创建主布局
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(30, 30, 30, 30);
    m_mainLayout->setSpacing(20);
    
    // 添加标题 - 半透明背景
    QLabel *titleLabel = new QLabel("气泡式日期选择器演示", this);
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
    
    // 添加说明 - 半透明背景
    QLabel *descLabel = new QLabel("点击下方按钮打开日期选择器", this);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet(R"(
        font-size: 14px; 
        color: white; 
        margin-bottom: 20px;
        padding: 10px;
        background-color: rgba(127, 140, 141, 120);
        border-radius: 8px;
        border: 1px solid rgba(255, 255, 255, 80);
    )");
    m_mainLayout->addWidget(descLabel);
    
    // 创建日期选择器组 - 半透明背景
    QGroupBox *pickerGroup = new QGroupBox("日期选择器", this);
    pickerGroup->setStyleSheet(R"(
        QGroupBox { 
            font-size: 16px; 
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
    QVBoxLayout *pickerLayout = new QVBoxLayout(pickerGroup);
    pickerLayout->setSpacing(15);
    pickerLayout->setContentsMargins(20, 25, 20, 20);
    
    // 创建日期选择器
    m_datePicker1 = new DatePicker(this);
    m_datePicker1->setSelectedDate(QDate::currentDate());
    pickerLayout->addWidget(m_datePicker1);
    
    m_mainLayout->addWidget(pickerGroup);
    
    // 添加信息显示标签 - 半透明背景
    m_infoLabel = new QLabel("请点击上方按钮选择日期", this);
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
    m_mainLayout->addWidget(m_infoLabel);
    
    m_mainLayout->addStretch();
}

void DateDemoWindow::setupDatePickers()
{
    // 连接日期选择器的信号
    connect(m_datePicker1, &DatePicker::dateChanged, this, &DateDemoWindow::onDateChanged);
    connect(m_datePicker1, &DatePicker::dateSelected, this, &DateDemoWindow::onDateSelected);
}

void DateDemoWindow::onDateChanged(const QDate &date)
{
    QString message = QString("日期选择器的日期已更改为: %1")
        .arg(date.toString("yyyy年MM月dd日"));
    m_infoLabel->setText(message);
}

void DateDemoWindow::onDateSelected(const QDate &date)
{
    QString message = QString("✅ 日期选择器已确认选择: %1")
        .arg(date.toString("yyyy年MM月dd日"));
    m_infoLabel->setText(message);
    
    // 可选：显示消息框确认
    // QMessageBox::information(this, "日期确认", message);
}

void DateDemoWindow::paintEvent(QPaintEvent *event)
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