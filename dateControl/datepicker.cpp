#include "datepicker.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QDebug>

DatePicker::DatePicker(QWidget *parent)
    : QWidget(parent)
    , m_dateButton(nullptr)
    , m_calendarPopup(nullptr)
    , m_calendar(nullptr)
    , m_mainLayout(nullptr)
    , m_selectedDate(QDate::currentDate())
    , m_calendarVisible(false)
    , m_showAnimation(nullptr)
    , m_hideAnimation(nullptr)
{
    setupUI();
    updateButtonText();
}

DatePicker::~DatePicker()
{
    if (m_calendarPopup) {
        m_calendarPopup->deleteLater();
    }
}

void DatePicker::setupUI()
{
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建日期按钮 - 半透明样式
    m_dateButton = new QPushButton(this);
    m_dateButton->setMinimumSize(200, 40);
    m_dateButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(74, 144, 226, 180), stop:0.5 rgba(53, 122, 189, 180), stop:1 rgba(46, 109, 164, 180));
            color: white;
            border: 1px solid rgba(255, 255, 255, 100);
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(91, 160, 242, 200), stop:0.5 rgba(74, 144, 226, 200), stop:1 rgba(53, 122, 189, 200));
            border: 1px solid rgba(255, 255, 255, 150);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(53, 122, 189, 220), stop:0.5 rgba(46, 109, 164, 220), stop:1 rgba(31, 71, 136, 220));
            border: 1px solid rgba(255, 255, 255, 80);
        }
    )");
    
    connect(m_dateButton, &QPushButton::clicked, this, &DatePicker::onButtonClicked);
    m_mainLayout->addWidget(m_dateButton);
    
    // 创建日历弹出窗口 - 半透明背景
    m_calendarPopup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
    m_calendarPopup->setAttribute(Qt::WA_TranslucentBackground);
    m_calendarPopup->setFixedSize(420, 550);
    m_calendarPopup->setWindowOpacity(0.95);
    m_calendarPopup->hide();
    
    // 在弹出窗口中创建日历控件
    m_calendar = new DateControl(m_calendarPopup);
    m_calendar->setGeometry(10, 10, 400, 530);
    m_calendar->setSelectedDate(m_selectedDate);
    
    // 连接日历信号
    connect(m_calendar, &DateControl::dateClicked, this, &DatePicker::onDateClicked);
    connect(m_calendar, &DateControl::dateSelectionChanged, this, &DatePicker::onDateSelectionChanged);
    
    // 安装事件过滤器以处理点击外部关闭
    m_calendarPopup->installEventFilter(this);
    qApp->installEventFilter(this);
    
    // 创建动画效果
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(m_calendarPopup);
    m_calendarPopup->setGraphicsEffect(effect);
    
    m_showAnimation = new QPropertyAnimation(effect, "opacity", this);
    m_showAnimation->setDuration(200);
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);
    
    m_hideAnimation = new QPropertyAnimation(effect, "opacity", this);
    m_hideAnimation->setDuration(150);
    m_hideAnimation->setStartValue(1.0);
    m_hideAnimation->setEndValue(0.0);
    
    connect(m_hideAnimation, &QPropertyAnimation::finished, m_calendarPopup, &QWidget::hide);
}

void DatePicker::setSelectedDate(const QDate &date)
{
    if (!date.isValid() || m_selectedDate == date)
        return;
        
    m_selectedDate = date;
    if (m_calendar) {
        m_calendar->setSelectedDate(date);
        m_calendar->setCurrentDate(date);
    }
    updateButtonText();
    emit dateChanged(date);
}

QDate DatePicker::selectedDate() const
{
    return m_selectedDate;
}

void DatePicker::setButtonText(const QString &text)
{
    if (m_dateButton) {
        m_dateButton->setText(text);
    }
}

QString DatePicker::buttonText() const
{
    return m_dateButton ? m_dateButton->text() : QString();
}

void DatePicker::onButtonClicked()
{
    if (m_calendarVisible) {
        hideCalendar();
    } else {
        showCalendar();
    }
}

void DatePicker::showCalendar()
{
    if (m_calendarVisible)
        return;
        
    // 计算弹出位置
    QPoint popupPos = calculatePopupPosition();
    m_calendarPopup->move(popupPos);
    
    // 显示弹出窗口
    m_calendarPopup->show();
    m_calendarPopup->raise();
    m_calendarPopup->activateWindow();
    
    // 播放显示动画
    m_showAnimation->start();
    
    m_calendarVisible = true;
}

void DatePicker::hideCalendar()
{
    if (!m_calendarVisible)
        return;
        
    // 播放隐藏动画
    m_hideAnimation->start();
    
    m_calendarVisible = false;
}

QPoint DatePicker::calculatePopupPosition()
{
    // 获取按钮在屏幕上的位置
    QPoint buttonPos = m_dateButton->mapToGlobal(QPoint(0, 0));
    QSize buttonSize = m_dateButton->size();
    QSize popupSize = m_calendarPopup->size();
    
    // 默认在按钮下方显示
    QPoint popupPos(buttonPos.x(), buttonPos.y() + buttonSize.height() + 5);
    
    // 检查屏幕边界
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    
    // 如果右边超出屏幕，向左调整
    if (popupPos.x() + popupSize.width() > screenGeometry.right()) {
        popupPos.setX(screenGeometry.right() - popupSize.width());
    }
    
    // 如果左边超出屏幕，向右调整
    if (popupPos.x() < screenGeometry.left()) {
        popupPos.setX(screenGeometry.left());
    }
    
    // 如果下方超出屏幕，在按钮上方显示
    if (popupPos.y() + popupSize.height() > screenGeometry.bottom()) {
        popupPos.setY(buttonPos.y() - popupSize.height() - 5);
    }
    
    // 如果上方也超出屏幕，调整到屏幕内
    if (popupPos.y() < screenGeometry.top()) {
        popupPos.setY(screenGeometry.top());
    }
    
    return popupPos;
}

void DatePicker::onDateClicked(const QDate &date)
{
    setSelectedDate(date);
    emit dateSelected(date);
    
    // 延迟关闭日历，让用户看到选择效果
    QTimer::singleShot(300, this, &DatePicker::hideCalendar);
}

void DatePicker::onDateSelectionChanged(const QDate &date)
{
    // 实时更新按钮文本，但不关闭日历
    m_selectedDate = date;
    updateButtonText();
    emit dateChanged(date);
}

void DatePicker::updateButtonText()
{
    if (m_dateButton) {
        QString dateText = QString("📅 %1").arg(m_selectedDate.toString("yyyy-MM-dd"));
        m_dateButton->setText(dateText);
    }
}

bool DatePicker::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        
        // 如果点击在弹出窗口外部，关闭日历
        if (m_calendarVisible && obj != m_calendarPopup && 
            !m_calendarPopup->geometry().contains(mouseEvent->globalPos())) {
            
            // 检查是否点击在按钮上
            QPoint buttonPos = m_dateButton->mapToGlobal(QPoint(0, 0));
            QRect buttonRect(buttonPos, m_dateButton->size());
            
            if (!buttonRect.contains(mouseEvent->globalPos())) {
                hideCalendar();
            }
        }
    }
    
    return QWidget::eventFilter(obj, event);
}

void DatePicker::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // 基类绘制
    QWidget::paintEvent(event);
}