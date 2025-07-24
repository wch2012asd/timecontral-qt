#include "datecontrol.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QDate>
#include <QLocale>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QDebug>
#include <QApplication>

// 私有实现类
class DateControl::Private
{
public:
    Private(DateControl *q)
        : q(q)
        , currentDate(QDate::currentDate())
        , selectedDate(QDate::currentDate())
        , viewMode(MonthView)
        , minDate(QDate(1900, 1, 1))
        , maxDate(QDate(2100, 12, 31))
        , backgroundColor(QColor(41, 128, 185))  // 蓝色背景
        , headerColor(QColor(52, 152, 219))      // 更亮的蓝色头部
        , selectedColor(QColor(74, 144, 226))    // 选中日期的蓝色
        , todayColor(QColor(46, 204, 113))       // 今天的绿色
        , textColor(QColor(255, 255, 255))       // 白色文字
        , weekendColor(QColor(255, 255, 255))
        , showWeekNumbers(false)
        , showToday(true)

        , animationEnabled(true)
        , animationTimer(new QTimer(q))
        , animationProgress(0.0)
        , isAnimating(false)
        , cellWidth(0)
        , cellHeight(0)
        , headerHeight(50)
    {
        animationTimer->setSingleShot(false);
        animationTimer->setInterval(16); // ~60 FPS
        QObject::connect(animationTimer, &QTimer::timeout, q, &DateControl::onAnimationTimer);
    }

    DateControl *q;
    
    // 日期相关
    QDate currentDate;
    QDate selectedDate;
    ViewMode viewMode;
    QDate minDate;
    QDate maxDate;
    

    
    // 外观设置
    QColor backgroundColor;
    QColor headerColor;
    QColor selectedColor;
    QColor todayColor;
    QColor textColor;
    QColor weekendColor;
    
    // 显示选项
    bool showWeekNumbers;
    bool showToday;
    
    // 动画
    bool animationEnabled;
    QTimer *animationTimer;
    double animationProgress;
    bool isAnimating;
    QDate animationStartDate;
    QDate animationEndDate;
    
    // 布局
    int cellWidth;
    int cellHeight;
    int headerHeight;
    QRect calendarRect;
    QRect headerRect;
    
    // 交互状态
    QDate hoveredDate;
    bool isDragging;
    QPoint lastMousePos;
};

DateControl::DateControl(QWidget *parent)
    : QWidget(parent)
    , d(new Private(this))
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(300, 250);
    
    calculateLayout();
}

DateControl::~DateControl()
{
    delete d;
}

void DateControl::setCurrentDate(const QDate &date)
{
    if (!date.isValid() || d->currentDate == date)
        return;
        
    if (d->animationEnabled && isVisible()) {
        d->animationStartDate = d->currentDate;
        d->animationEndDate = date;
        startTransitionAnimation();
    }
    
    d->currentDate = date;
    emit currentDateChanged(date);
    emit monthChanged(date.year(), date.month());
    update();
}

QDate DateControl::currentDate() const
{
    return d->currentDate;
}

void DateControl::setSelectedDate(const QDate &date)
{
    if (!date.isValid() || d->selectedDate == date)
        return;
        
    d->selectedDate = date;
    emit dateSelectionChanged(date);
    update();
}

QDate DateControl::selectedDate() const
{
    return d->selectedDate;
}

void DateControl::setViewMode(ViewMode mode)
{
    if (d->viewMode == mode)
        return;
        
    d->viewMode = mode;
    calculateLayout();
    emit viewModeChanged(mode);
    update();
}

DateControl::ViewMode DateControl::viewMode() const
{
    return d->viewMode;
}

void DateControl::setDateRange(const QDate &minDate, const QDate &maxDate)
{
    if (!minDate.isValid() || !maxDate.isValid() || minDate >= maxDate)
        return;
        
    d->minDate = minDate;
    d->maxDate = maxDate;
    
    // 确保当前日期在范围内
    if (d->currentDate < minDate)
        setCurrentDate(minDate);
    else if (d->currentDate > maxDate)
        setCurrentDate(maxDate);
        
    update();
}

QDate DateControl::minDate() const
{
    return d->minDate;
}

QDate DateControl::maxDate() const
{
    return d->maxDate;
}



void DateControl::setBackgroundColor(const QColor &color)
{
    d->backgroundColor = color;
    update();
}

void DateControl::setHeaderColor(const QColor &color)
{
    d->headerColor = color;
    update();
}

void DateControl::setSelectedColor(const QColor &color)
{
    d->selectedColor = color;
    update();
}

void DateControl::setTodayColor(const QColor &color)
{
    d->todayColor = color;
    update();
}

void DateControl::setTextColor(const QColor &color)
{
    d->textColor = color;
    update();
}

void DateControl::setWeekendColor(const QColor &color)
{
    d->weekendColor = color;
    update();
}

void DateControl::setShowWeekNumbers(bool show)
{
    d->showWeekNumbers = show;
    calculateLayout();
    update();
}

bool DateControl::isShowWeekNumbers() const
{
    return d->showWeekNumbers;
}

void DateControl::setShowToday(bool show)
{
    d->showToday = show;
    update();
}

bool DateControl::isShowToday() const
{
    return d->showToday;
}



void DateControl::goToToday()
{
    setCurrentDate(QDate::currentDate());
    setSelectedDate(QDate::currentDate());
}

void DateControl::goToNextMonth()
{
    QDate nextMonth = d->currentDate.addMonths(1);
    if (nextMonth <= d->maxDate) {
        setCurrentDate(nextMonth);
    }
}

void DateControl::goToPreviousMonth()
{
    QDate prevMonth = d->currentDate.addMonths(-1);
    if (prevMonth >= d->minDate) {
        setCurrentDate(prevMonth);
    }
}

void DateControl::goToNextYear()
{
    QDate nextYear = d->currentDate.addYears(1);
    if (nextYear <= d->maxDate) {
        setCurrentDate(nextYear);
    }
}

void DateControl::goToPreviousYear()
{
    QDate prevYear = d->currentDate.addYears(-1);
    if (prevYear >= d->minDate) {
        setCurrentDate(prevYear);
    }
}

void DateControl::setAnimationEnabled(bool enabled)
{
    d->animationEnabled = enabled;
}

bool DateControl::isAnimationEnabled() const
{
    return d->animationEnabled;
}

void DateControl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    drawBackground(painter);
    
    // 绘制头部
    drawHeader(painter);
    
    // 根据视图模式绘制日历
    switch (d->viewMode) {
    case MonthView:
        drawMonthView(painter);
        break;
    case YearView:
        drawYearView(painter);
        break;
    case WeekView:
        drawWeekView(painter);
        break;
    }
    
    // 绘制底部选中日期显示区域
    drawBottomDateDisplay(painter);
}

void DateControl::drawBackground(QPainter &painter)
{
    // 绘制半透明蓝色渐变背景
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(52, 152, 219, 160));   // 顶部蓝色 - 更透明
    gradient.setColorAt(0.5, QColor(41, 128, 185, 140)); // 中部蓝色 - 更透明
    gradient.setColorAt(1, QColor(30, 100, 160, 120));   // 底部深蓝色 - 更透明
    
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 12, 12);
    
    // 添加玻璃效果边框
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255, 255, 255, 100), 2));
    painter.drawRoundedRect(rect().adjusted(3, 3, -3, -3), 10, 10);
    
    // 添加内部高光效果
    QLinearGradient highlightGradient(0, 0, 0, height() * 0.3);
    highlightGradient.setColorAt(0, QColor(255, 255, 255, 50));
    highlightGradient.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(QBrush(highlightGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(5, 5, -5, -height() * 0.7), 8, 8);
    
    // 绘制底部日期显示区域背景
    QRect bottomRect(rect().left() + 15, rect().bottom() - 55, rect().width() - 30, 40);
    
    // 底部区域的半透明深色背景
    painter.setBrush(QColor(0, 0, 0, 80));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(bottomRect, 8, 8);
    
    // 底部区域的玻璃效果边框
    painter.setPen(QPen(QColor(255, 255, 255, 120), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(bottomRect, 8, 8);
}

void DateControl::drawHeader(QPainter &painter)
{
    QRect headerRect = getHeaderRect();
    
    // 绘制左右导航箭头，样式与图片一致
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    
    // 左箭头 (‹‹) - 双箭头样式
    QRect leftArrowRect(headerRect.left() + 15, headerRect.center().y() - 12, 30, 24);
    QFont arrowFont = font();
    arrowFont.setPointSize(font().pointSize() + 4);
    arrowFont.setBold(true);
    painter.setFont(arrowFont);
    painter.drawText(leftArrowRect, Qt::AlignCenter, "‹‹");
    
    // 右箭头 (››) - 双箭头样式  
    QRect rightArrowRect(headerRect.right() - 45, headerRect.center().y() - 12, 30, 24);
    painter.drawText(rightArrowRect, Qt::AlignCenter, "››");
    
    // 绘制月份年份文本 - 居中显示
    painter.setPen(Qt::white);
    QFont headerFont = font();
    headerFont.setPointSize(font().pointSize() + 8);
    headerFont.setBold(false);
    painter.setFont(headerFont);
    
    QString headerText;
    if (d->viewMode == MonthView) {
        headerText = QString("%1年 %2月").arg(d->currentDate.year())
                                        .arg(d->currentDate.month());
    } else if (d->viewMode == YearView) {
        headerText = QString("%1年").arg(d->currentDate.year());
    } else {
        QDate startOfWeek = d->currentDate.addDays(-(d->currentDate.dayOfWeek() - 1));
        QDate endOfWeek = startOfWeek.addDays(6);
        headerText = QString("%1 - %2").arg(startOfWeek.toString("MM/dd"))
                                      .arg(endOfWeek.toString("MM/dd"));
    }
    
    painter.drawText(headerRect, Qt::AlignCenter, headerText);
}

void DateControl::drawMonthView(QPainter &painter)
{
    QRect calendarRect = getCalendarRect();
    
    // 计算网格
    int cols = d->showWeekNumbers ? 8 : 7;
    int rows = 7; // 1行星期标题 + 6行日期
    
    d->cellWidth = calendarRect.width() / cols;
    d->cellHeight = calendarRect.height() / rows;
    
    // 绘制星期标题
    painter.setPen(QColor(255, 255, 255, 180));
    QFont weekFont = font();
    weekFont.setBold(false);
    weekFont.setPointSize(font().pointSize() - 1);
    painter.setFont(weekFont);
    
    QStringList weekDays = {"日", "一", "二", "三", "四", "五", "六"};
    
    int startCol = d->showWeekNumbers ? 1 : 0;
    for (int i = 0; i < 7; ++i) {
        QRect weekRect(calendarRect.left() + (startCol + i) * d->cellWidth,
                      calendarRect.top(),
                      d->cellWidth, d->cellHeight);
        
        QColor weekColor = (i >= 5) ? d->weekendColor : d->textColor;
        painter.setPen(weekColor);
        painter.drawText(weekRect, Qt::AlignCenter, weekDays[i]);
    }
    
    // 绘制日期网格
    QDate firstDay = QDate(d->currentDate.year(), d->currentDate.month(), 1);
    QDate startDate = firstDay.addDays(-(firstDay.dayOfWeek() - 1));
    
    painter.setFont(font());
    
    for (int week = 0; week < 6; ++week) {
        // 绘制周数
        if (d->showWeekNumbers) {
            QDate weekDate = startDate.addDays(week * 7);
            int weekNumber = weekDate.weekNumber();
            
            QRect weekNumRect(calendarRect.left(),
                            calendarRect.top() + (week + 1) * d->cellHeight,
                            d->cellWidth, d->cellHeight);
            
            painter.setPen(d->textColor.lighter(150));
            painter.drawText(weekNumRect, Qt::AlignCenter, QString::number(weekNumber));
        }
        
        // 绘制日期
        for (int day = 0; day < 7; ++day) {
            QDate date = startDate.addDays(week * 7 + day);
            
            QRect cellRect(calendarRect.left() + (startCol + day) * d->cellWidth,
                          calendarRect.top() + (week + 1) * d->cellHeight,
                          d->cellWidth, d->cellHeight);
            
            bool isSelected = (date == d->selectedDate);
            bool isToday = (date == QDate::currentDate() && d->showToday);
            
            drawDateCell(painter, date, cellRect, isSelected, isToday);
        }
    }
}

void DateControl::drawYearView(QPainter &painter)
{
    // 年视图：显示12个月的小日历
    QRect calendarRect = getCalendarRect();
    
    int monthCols = 4;
    int monthRows = 3;
    int monthWidth = calendarRect.width() / monthCols;
    int monthHeight = calendarRect.height() / monthRows;
    
    painter.setFont(QFont(font().family(), font().pointSize() - 2));
    
    for (int month = 1; month <= 12; ++month) {
        int row = (month - 1) / monthCols;
        int col = (month - 1) % monthCols;
        
        QRect monthRect(calendarRect.left() + col * monthWidth,
                       calendarRect.top() + row * monthHeight,
                       monthWidth, monthHeight);
        
        // 绘制月份标题
        QString monthName = QLocale().monthName(month, QLocale::ShortFormat);
        QRect titleRect = monthRect;
        titleRect.setHeight(20);
        
        painter.setPen(d->headerColor);
        painter.drawText(titleRect, Qt::AlignCenter, monthName);
        
        // 绘制简化的月份日历
        QRect miniCalRect = monthRect.adjusted(5, 25, -5, -5);
        drawMiniMonth(painter, d->currentDate.year(), month, miniCalRect);
    }
}

void DateControl::drawWeekView(QPainter &painter)
{
    // 周视图：显示一周的详细信息
    QRect calendarRect = getCalendarRect();
    
    QDate startOfWeek = d->currentDate.addDays(-(d->currentDate.dayOfWeek() - 1));
    
    d->cellWidth = calendarRect.width() / 7;
    d->cellHeight = calendarRect.height();
    
    painter.setFont(font());
    
    for (int day = 0; day < 7; ++day) {
        QDate date = startOfWeek.addDays(day);
        
        QRect cellRect(calendarRect.left() + day * d->cellWidth,
                      calendarRect.top(),
                      d->cellWidth, d->cellHeight);
        
        bool isSelected = (date == d->selectedDate);
        bool isToday = (date == QDate::currentDate() && d->showToday);
        
        drawDateCell(painter, date, cellRect, isSelected, isToday);
    }
}



void DateControl::drawDateCell(QPainter &painter, const QDate &date, const QRect &rect, 
                              bool isSelected, bool isToday)
{
    // 计算圆形区域，与图片样式一致
    int circleSize = qMin(rect.width(), rect.height()) - 6;
    QRect circleRect(rect.center().x() - circleSize/2, 
                    rect.center().y() - circleSize/2, 
                    circleSize, circleSize);
    
    // 绘制选中状态的圆形背景 - 蓝色圆形
    if (isSelected) {
        // 绘制选中的蓝色圆形，与图片中的样式一致
        painter.setBrush(QColor(74, 144, 226)); 
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(circleRect);
        
        // 添加高光效果
        QRadialGradient highlight(circleRect.center(), circleSize/2);
        highlight.setColorAt(0, QColor(255, 255, 255, 30));
        highlight.setColorAt(1, QColor(255, 255, 255, 0));
        painter.setBrush(QBrush(highlight));
        painter.drawEllipse(circleRect);
    } else if (isToday) {
        // 今天的日期用不同颜色标识
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(255, 255, 255, 150), 2));
        painter.drawEllipse(circleRect);
    }
    
    // 绘制日期文本
    QColor textColor = Qt::white;
    if (date.month() != d->currentDate.month()) {
        textColor = QColor(255, 255, 255, 120); // 其他月份的日期显示为半透明
    }
    
    if (isSelected) {
        textColor = Qt::white;
    }
    
    painter.setPen(textColor);
    QFont dateFont = font();
    dateFont.setBold(isSelected);
    dateFont.setPointSize(font().pointSize() + (isSelected ? 1 : 0));
    painter.setFont(dateFont);
    painter.drawText(rect, Qt::AlignCenter, QString::number(date.day()));
}

void DateControl::drawMiniMonth(QPainter &painter, int year, int month, const QRect &rect)
{
    // 绘制年视图中的小月份日历
    QDate firstDay(year, month, 1);
    QDate startDate = firstDay.addDays(-(firstDay.dayOfWeek() - 1));
    
    int cellW = rect.width() / 7;
    int cellH = rect.height() / 6;
    
    for (int week = 0; week < 6; ++week) {
        for (int day = 0; day < 7; ++day) {
            QDate date = startDate.addDays(week * 7 + day);
            
            QRect cellRect(rect.left() + day * cellW,
                          rect.top() + week * cellH,
                          cellW, cellH);
            
            QColor textColor = d->textColor;
            if (date.month() != month) {
                textColor = textColor.lighter(200);
            }
            
            if (date == d->selectedDate) {
                painter.setBrush(d->selectedColor);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(cellRect.center(), 3, 3);
                textColor = Qt::white;
            }
            
            painter.setPen(textColor);
            painter.drawText(cellRect, Qt::AlignCenter, QString::number(date.day()));
        }
    }
}



QRect DateControl::getDateCellRect(const QDate &date) const
{
    if (d->viewMode != MonthView)
        return QRect();
    
    QDate firstDay = QDate(d->currentDate.year(), d->currentDate.month(), 1);
    QDate startDate = firstDay.addDays(-(firstDay.dayOfWeek() - 1));
    
    int daysDiff = startDate.daysTo(date);
    int week = daysDiff / 7;
    int day = daysDiff % 7;
    
    QRect calendarRect = getCalendarRect();
    int startCol = d->showWeekNumbers ? 1 : 0;
    
    return QRect(calendarRect.left() + (startCol + day) * d->cellWidth,
                calendarRect.top() + (week + 1) * d->cellHeight,
                d->cellWidth, d->cellHeight);
}

QDate DateControl::getDateAtPosition(const QPoint &pos) const
{
    if (d->viewMode != MonthView)
        return QDate();
    
    QRect calendarRect = getCalendarRect();
    if (!calendarRect.contains(pos))
        return QDate();
    
    int startCol = d->showWeekNumbers ? 1 : 0;
    int col = (pos.x() - calendarRect.left()) / d->cellWidth - startCol;
    int row = (pos.y() - calendarRect.top()) / d->cellHeight - 1; // 减去标题行
    
    if (col < 0 || col >= 7 || row < 0 || row >= 6)
        return QDate();
    
    QDate firstDay = QDate(d->currentDate.year(), d->currentDate.month(), 1);
    QDate startDate = firstDay.addDays(-(firstDay.dayOfWeek() - 1));
    
    return startDate.addDays(row * 7 + col);
}

QRect DateControl::getHeaderRect() const
{
    return QRect(10, 10, width() - 20, d->headerHeight);
}

QRect DateControl::getCalendarRect() const
{
    return QRect(10, d->headerHeight + 20, width() - 20, height() - d->headerHeight - 90);
}

void DateControl::calculateLayout()
{
    d->calendarRect = getCalendarRect();
    d->headerRect = getHeaderRect();
}

void DateControl::updateCalendar()
{
    calculateLayout();
    update();
}

void DateControl::startTransitionAnimation()
{
    if (!d->animationEnabled)
        return;
    
    d->isAnimating = true;
    d->animationProgress = 0.0;
    d->animationTimer->start();
}

void DateControl::onAnimationTimer()
{
    if (!d->isAnimating)
        return;
    
    d->animationProgress += 0.05; // 动画速度
    
    if (d->animationProgress >= 1.0) {
        d->animationProgress = 1.0;
        d->isAnimating = false;
        d->animationTimer->stop();
    }
    
    update();
}

void DateControl::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QRect headerRect = getHeaderRect();
        
        // 检查是否点击了左箭头
        QRect leftArrowRect(headerRect.left() + 15, headerRect.center().y() - 12, 30, 24);
        if (leftArrowRect.contains(event->pos())) {
            goToPreviousMonth();
            return;
        }
        
        // 检查是否点击了右箭头
        QRect rightArrowRect(headerRect.right() - 45, headerRect.center().y() - 12, 30, 24);
        if (rightArrowRect.contains(event->pos())) {
            goToNextMonth();
            return;
        }
        
        // 检查日期点击
        QDate clickedDate = getDateAtPosition(event->pos());
        if (clickedDate.isValid()) {
            setSelectedDate(clickedDate);
            emit dateClicked(clickedDate);
        }
    }
    
    QWidget::mousePressEvent(event);
}

void DateControl::mouseMoveEvent(QMouseEvent *event)
{
    QDate hoveredDate = getDateAtPosition(event->pos());
    if (hoveredDate != d->hoveredDate) {
        d->hoveredDate = hoveredDate;
        update();
    }
    
    QWidget::mouseMoveEvent(event);
}

void DateControl::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

void DateControl::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QDate clickedDate = getDateAtPosition(event->pos());
        if (clickedDate.isValid()) {
            emit dateDoubleClicked(clickedDate);
        }
    }
    
    QWidget::mouseDoubleClickEvent(event);
}

void DateControl::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    
    if (!numDegrees.isNull()) {
        if (numDegrees.y() > 0) {
            goToPreviousMonth();
        } else {
            goToNextMonth();
        }
    }
    
    event->accept();
}

void DateControl::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    calculateLayout();
}

void DateControl::drawBottomDateDisplay(QPainter &painter)
{
    // 绘制底部日期显示区域，与图片样式完全一致
    QRect bottomRect(rect().left() + 15, rect().bottom() - 55, rect().width() - 30, 40);
    
    // 绘制日历图标 - 更精致的设计
    QRect iconRect(bottomRect.left() + 12, bottomRect.center().y() - 10, 20, 20);
    painter.setPen(QPen(Qt::white, 1.5));
    painter.setBrush(Qt::NoBrush);
    
    // 日历图标主体
    painter.drawRoundedRect(iconRect.adjusted(2, 3, -2, -2), 2, 2);
    
    // 日历顶部的小孔
    painter.drawLine(iconRect.left() + 5, iconRect.top() + 1, iconRect.left() + 5, iconRect.top() + 6);
    painter.drawLine(iconRect.right() - 5, iconRect.top() + 1, iconRect.right() - 5, iconRect.top() + 6);
    
    // 日历内部的横线
    painter.drawLine(iconRect.left() + 3, iconRect.top() + 8, iconRect.right() - 3, iconRect.top() + 8);
    painter.drawLine(iconRect.left() + 3, iconRect.top() + 11, iconRect.right() - 3, iconRect.top() + 11);
    
    // 绘制选中的日期文本 - 与图片样式一致
    QRect textRect(iconRect.right() + 12, bottomRect.top(), 
                   bottomRect.width() - iconRect.width() - 24, bottomRect.height());
    
    painter.setPen(Qt::white);
    QFont dateFont = font();
    dateFont.setPointSize(font().pointSize() + 1);
    dateFont.setBold(false);
    painter.setFont(dateFont);
    
    // 使用与图片一致的日期格式
    QString dateText = d->selectedDate.toString("yyyy-MM-dd");
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, dateText);
}

void DateControl::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        setSelectedDate(d->selectedDate.addDays(-1));
        break;
    case Qt::Key_Right:
        setSelectedDate(d->selectedDate.addDays(1));
        break;
    case Qt::Key_Up:
        setSelectedDate(d->selectedDate.addDays(-7));
        break;
    case Qt::Key_Down:
        setSelectedDate(d->selectedDate.addDays(7));
        break;
    case Qt::Key_Home:
        goToToday();
        break;
    case Qt::Key_PageUp:
        goToPreviousMonth();
        break;
    case Qt::Key_PageDown:
        goToNextMonth();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}