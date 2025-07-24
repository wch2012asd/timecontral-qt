#include "timecontral.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QDateTime>
#include <QColor>
#include <QFontMetrics>
#include <QToolTip>
#include <QDebug>
#include <QLinearGradient>
#include <QPolygon>
#include <QBrush>
#include <QPen>
#include <QFont>

// 私有实现类，用于隐藏实现细节
class TimeContral::Private
{
public:
    Private(TimeContral *q)
        : q(q)
        , m_minTime(QDateTime::fromString("2014-01-01 00:00:00", "yyyy-MM-dd hh:mm:ss"))
        , m_maxTime(QDateTime::fromString("2014-01-01 23:59:59", "yyyy-MM-dd hh:mm:ss"))
        , m_visibleStartTime(m_minTime)
        , m_visibleEndTime(m_maxTime)
        , m_currentIndex(-1)
        , m_isDragging(false)
        , m_lastMousePos(-1)
        , m_dragStartMousePos(-1)
        , m_backgroundColor(QColor(30, 100, 180))
        , m_scaleColor(Qt::white)
        , m_textColor(Qt::white)
        , m_scaleHeight(40)
        , m_timeFormat("hh:mm")
        , m_zoomLevel(1.0)
        , m_currentTime(QDateTime::fromString("2014-01-01 03:00:00", "yyyy-MM-dd hh:mm:ss"))
        , m_showTimeBubble(true)
        , m_showDateOnTimeline(true)
        , m_infiniteScrollEnabled(true)
    {
    }

    // 成员变量
    TimeContral *q;
    QDateTime m_minTime;
    QDateTime m_maxTime;
    QDateTime m_visibleStartTime;
    QDateTime m_visibleEndTime;
    QVector<TimeContral::TimeItem> m_timeItems;
    int m_currentIndex;
    bool m_isDragging;
    int m_lastMousePos;
    int m_dragStartMousePos;
    QColor m_backgroundColor;
    QColor m_scaleColor;
    QColor m_textColor;
    int m_scaleHeight;
    QString m_timeFormat;
    double m_zoomLevel;
    QDateTime m_currentTime;
    
    // 显示选项
    bool m_showTimeBubble;
    bool m_showDateOnTimeline;
    
    // 滚动选项
    bool m_infiniteScrollEnabled;
};

TimeContral::TimeContral(QWidget *parent)
    : QWidget(parent)
    , d(new Private(this))
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumHeight(100);
    setMaximumHeight(100);
}

TimeContral::~TimeContral()
{
    delete d;
}

void TimeContral::setTimeRange(const QDateTime &minTime, const QDateTime &maxTime)
{
    if (minTime >= maxTime)
        return;

    d->m_minTime = minTime;
    d->m_maxTime = maxTime;

    if (d->m_visibleStartTime < minTime)
        d->m_visibleStartTime = minTime;
    if (d->m_visibleEndTime > maxTime)
        d->m_visibleEndTime = maxTime;

    emit timeRangeChanged(d->m_minTime, d->m_maxTime);
    update();
}

QDateTime TimeContral::minTime() const
{
    return d->m_minTime;
}

QDateTime TimeContral::maxTime() const
{
    return d->m_maxTime;
}

void TimeContral::setVisibleTimeRange(const QDateTime &startTime, const QDateTime &endTime)
{
    if (startTime >= endTime)
        return;

    QDateTime start = startTime;
    QDateTime end = endTime;

    if (start < d->m_minTime)
        start = d->m_minTime;
    if (end > d->m_maxTime)
        end = d->m_maxTime;

    d->m_visibleStartTime = start;
    d->m_visibleEndTime = end;

    emit visibleTimeRangeChanged(d->m_visibleStartTime, d->m_visibleEndTime);
    update();
}

QDateTime TimeContral::visibleStartTime() const
{
    return d->m_visibleStartTime;
}

QDateTime TimeContral::visibleEndTime() const
{
    return d->m_visibleEndTime;
}

int TimeContral::addTimePoint(const QDateTime &time, const QString &label, 
                             const QColor &color, const QVariant &userData)
{
    TimeItem item;
    item.startTime = time;
    item.endTime = time;
    item.label = label;
    item.color = color;
    item.isPoint = true;
    item.userData = userData;

    d->m_timeItems.append(item);
    update();
    return d->m_timeItems.size() - 1;
}

int TimeContral::addTimeSpan(const QDateTime &startTime, const QDateTime &endTime, 
                            const QString &label, const QColor &color, const QVariant &userData)
{
    if (startTime >= endTime)
        return -1;

    TimeItem item;
    item.startTime = startTime;
    item.endTime = endTime;
    item.label = label;
    item.color = color;
    item.isPoint = false;
    item.userData = userData;

    d->m_timeItems.append(item);
    update();
    return d->m_timeItems.size() - 1;
}

bool TimeContral::removeTimeItem(int index)
{
    if (index < 0 || index >= d->m_timeItems.size())
        return false;

    d->m_timeItems.remove(index);
    
    if (d->m_currentIndex == index)
        d->m_currentIndex = -1;
    else if (d->m_currentIndex > index)
        d->m_currentIndex--;

    update();
    return true;
}

void TimeContral::clearTimeItems()
{
    d->m_timeItems.clear();
    d->m_currentIndex = -1;
    update();
}

int TimeContral::timeItemCount() const
{
    return d->m_timeItems.size();
}

TimeContral::TimeItem TimeContral::timeItemAt(int index) const
{
    if (index < 0 || index >= d->m_timeItems.size())
        return TimeItem();
    
    return d->m_timeItems.at(index);
}

void TimeContral::setCurrentTimeItem(int index)
{
    if (index < -1 || index >= d->m_timeItems.size())
        return;

    if (d->m_currentIndex != index) {
        d->m_currentIndex = index;
        emit currentTimeItemChanged(index);
        update();
    }
}

int TimeContral::currentTimeItem() const
{
    return d->m_currentIndex;
}

void TimeContral::setBackgroundColor(const QColor &color)
{
    d->m_backgroundColor = color;
    update();
}

void TimeContral::setScaleColor(const QColor &color)
{
    d->m_scaleColor = color;
    update();
}

void TimeContral::setTextColor(const QColor &color)
{
    d->m_textColor = color;
    update();
}

void TimeContral::setScaleHeight(int height)
{
    d->m_scaleHeight = height;
    update();
}

void TimeContral::setTimeFormat(const QString &format)
{
    d->m_timeFormat = format;
    update();
}

void TimeContral::setCurrentTime(const QDateTime &time)
{
    if (d->m_currentTime != time) {
        d->m_currentTime = time;
        emit currentTimeChanged(time);
        update();
    }
}

QDateTime TimeContral::currentTime() const
{
    return d->m_currentTime;
}

void TimeContral::setShowTimeBubble(bool show)
{
    d->m_showTimeBubble = show;
    update();
}

bool TimeContral::isShowTimeBubble() const
{
    return d->m_showTimeBubble;
}

void TimeContral::setShowDateOnTimeline(bool show)
{
    d->m_showDateOnTimeline = show;
    update();
}

bool TimeContral::isShowDateOnTimeline() const
{
    return d->m_showDateOnTimeline;
}

void TimeContral::zoomIn()
{
    setZoomLevel(d->m_zoomLevel * 1.2);
}

void TimeContral::zoomOut()
{
    setZoomLevel(d->m_zoomLevel / 1.2);
}

void TimeContral::setZoomLevel(double level)
{
    if (level < 0.1)
        level = 0.1;
    if (level > 10.0)
        level = 10.0;

    if (qFuzzyCompare(d->m_zoomLevel, level))
        return;

    d->m_zoomLevel = level;
    update();
}

double TimeContral::zoomLevel() const
{
    return d->m_zoomLevel;
}

void TimeContral::setInfiniteScrollEnabled(bool enabled)
{
    d->m_infiniteScrollEnabled = enabled;
}

bool TimeContral::isInfiniteScrollEnabled() const
{
    return d->m_infiniteScrollEnabled;
}

void TimeContral::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    drawBackground(painter);
    
    // 绘制时间刻度
    drawTimeScale(painter);
    
    // 绘制时间项
    drawTimeItems(painter);
    
    // 绘制当前时间气泡
    if (d->m_showTimeBubble) {
        drawTimeBubble(painter);
    }
    
    // 绘制时间轴上的日期
    if (d->m_showDateOnTimeline) {
        drawDateOnTimeline(painter);
    }
}

void TimeContral::drawBackground(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 创建深天蓝色半透明磨砂质感背景
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(70, 130, 180, 200));     // 钢蓝色，更深
    gradient.setColorAt(0.5, QColor(100, 149, 237, 180));  // 矢车菊蓝
    gradient.setColorAt(1, QColor(123, 104, 238, 160));    // 中紫罗兰色
    
    // 绘制圆角矩形背景
    painter.setBrush(QBrush(gradient));
    painter.setPen(QPen(QColor(255, 255, 255, 80), 1)); // 半透明白色边框
    painter.drawRoundedRect(rect(), 20, 20);
    
    // 添加磨砂质感的内部高光
    QLinearGradient frostGradient(0, 0, 0, height()/2);
    frostGradient.setColorAt(0, QColor(255, 255, 255, 60));  // 顶部高光
    frostGradient.setColorAt(0.3, QColor(255, 255, 255, 30)); // 中间过渡
    frostGradient.setColorAt(1, QColor(255, 255, 255, 0));   // 底部透明
    painter.setBrush(QBrush(frostGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(2, 2, -2, -height()/2), 18, 18);
    
    // 添加磨砂纹理效果
    painter.setOpacity(0.1);
    for (int i = 0; i < 3; i++) {
        QLinearGradient textureGradient(0, i*10, width(), i*10+5);
        textureGradient.setColorAt(0, QColor(255, 255, 255, 20));
        textureGradient.setColorAt(1, QColor(255, 255, 255, 0));
        painter.setBrush(QBrush(textureGradient));
        painter.drawRoundedRect(rect().adjusted(3, 3+i*8, -3, -3), 17, 17);
    }
    painter.setOpacity(1.0); // 恢复不透明度
}

void TimeContral::drawTimeScale(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 设置字体，减小字体大小
    QFont font = painter.font();
    font.setPointSize(8);
    font.setBold(false);
    painter.setFont(font);
    
    // 计算绘制区域，向上移动刻度位置为日期腾出空间
    int margin = 20;
    int scaleY = height() - 50;  // 时间刻度的Y位置
    
    // 绘制基于可见时间范围的动态刻度
    painter.setPen(QPen(Qt::white, 1));
    
    // 计算可见时间范围的小时数
    qint64 visibleTimeSpan = d->m_visibleEndTime.toMSecsSinceEpoch() - d->m_visibleStartTime.toMSecsSinceEpoch();
    double visibleHours = visibleTimeSpan / (1000.0 * 3600.0); // 转换为小时
    
    int availableWidth = width() - 2 * margin;
    double pixelsPerHour = availableWidth / visibleHours;
    
    // 找到可见范围内的第一个整小时
    QDateTime startHour = d->m_visibleStartTime;
    startHour = QDateTime(startHour.date(), QTime(startHour.time().hour(), 0, 0));
    if (startHour < d->m_visibleStartTime) {
        startHour = startHour.addSecs(3600); // 下一个小时
    }
    
    // 绘制时间刻度
    QDateTime currentHour = startHour;
    while (currentHour <= d->m_visibleEndTime) {
        // 计算这个小时在屏幕上的位置
        qint64 offsetMs = currentHour.toMSecsSinceEpoch() - d->m_visibleStartTime.toMSecsSinceEpoch();
        double offsetHours = offsetMs / (1000.0 * 3600.0);
        int x = margin + (int)(offsetHours * pixelsPerHour);
        
        if (x >= margin && x <= width() - margin) {
            int hour = currentHour.time().hour();
            
            // 绘制刻度线
            if (hour % 3 == 0) {
                // 主刻度（每3小时）
                painter.drawLine(x, scaleY - 6, x, scaleY + 6);
                
                // 绘制时间文本，格式为 hh:00
                QString timeText = QString("%1:00").arg(hour, 2, 10, QChar('0'));
                QFontMetrics fm(font);
                int textWidth = fm.width(timeText);
                painter.drawText(x - textWidth/2, scaleY + 20, timeText);
            } else {
                // 小刻度（每小时）
                painter.drawLine(x, scaleY - 3, x, scaleY + 3);
            }
        }
        
        currentHour = currentHour.addSecs(3600); // 下一个小时
    }
    
    // 绘制当前时间的高亮指示器（不包括文本）
    int currentX = timeToPos(d->m_currentTime);
    
    // 只有当前时间在可见范围内时才绘制高亮
    if (currentX >= 0 && currentX <= width()) {
        // 在精确的当前时间位置绘制指示器
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(180, 220, 50))); // 与气泡相同的绿色
        painter.drawEllipse(QPoint(currentX, scaleY), 4, 4);
        
        // 绘制从气泡到时间轴的连接线
        painter.setPen(QPen(QColor(180, 220, 50, 100), 1, Qt::DashLine));
        painter.drawLine(currentX, 50, currentX, scaleY - 5);
    }
}

void TimeContral::drawTimeItems(QPainter &painter)
{
    int y = height() - d->m_scaleHeight - 10;  // 时间项绘制的基准y坐标
    
    // 绘制所有时间项
    for (int i = 0; i < d->m_timeItems.size(); ++i) {
        const TimeItem &item = d->m_timeItems.at(i);
        
        // 检查时间项是否在可见范围内
        if ((item.endTime < d->m_visibleStartTime) || (item.startTime > d->m_visibleEndTime))
            continue;
        
        // 设置画笔和画刷
        painter.setPen(item.color);
        QBrush brush(item.color, (i == d->m_currentIndex) ? Qt::SolidPattern : Qt::NoBrush);
        painter.setBrush(brush);
        
        if (item.isPoint) {
            // 绘制时间点
            int x = timeToPos(item.startTime);
            int radius = (i == d->m_currentIndex) ? 6 : 4;
            painter.drawEllipse(QPoint(x, y), radius, radius);
            
            // 绘制标签
            if (!item.label.isEmpty()) {
                painter.setPen(d->m_textColor);
                painter.drawText(x + 5, y - 5, item.label);
            }
        } else {
            // 绘制时间段
            int x1 = timeToPos(item.startTime);
            int x2 = timeToPos(item.endTime);
            int height = 10;
            
            // 绘制矩形
            QRect rect(x1, y - height / 2, x2 - x1, height);
            painter.drawRect(rect);
            
            // 绘制标签
            if (!item.label.isEmpty()) {
                painter.setPen(d->m_textColor);
                painter.drawText(x1 + 5, y - height / 2 - 5, item.label);
            }
        }
    }
}

void TimeContral::drawCurrentTimeIndicator(QPainter &painter)
{
    // 这个函数现在由 drawTimeScale 中的高亮部分处理
    Q_UNUSED(painter);
}

void TimeContral::drawTimeBubble(QPainter &painter)
{
    if (!d->m_showTimeBubble)
        return;
        
    // 使用 timeToPos 函数来计算当前时间的精确位置
    int currentX = timeToPos(d->m_currentTime);
    
    // 如果当前时间不在可见范围内，不绘制气泡
    if (currentX < 0 || currentX > width())
        return;
    
    // 气泡位置和尺寸
    int y = 10; // 顶部位置
    QString text = d->m_currentTime.toString("yyyy-MM-dd hh") + ":00:00";
    
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(11);
    painter.setFont(font);
    
    QFontMetrics fm(font);
    int textWidth = fm.width(text) + 20;
    int bubbleWidth = textWidth;
    int bubbleHeight = 30;
    
    // 确保气泡不超出边界
    int bubbleX = currentX - bubbleWidth/2;
    if (bubbleX < 5) bubbleX = 5;
    if (bubbleX + bubbleWidth > width() - 5) bubbleX = width() - bubbleWidth - 5;
    
    QRect bubbleRect(bubbleX, y, bubbleWidth, bubbleHeight);
    
    // 绘制气泡，使用淡色半透明效果
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QColor(220, 240, 180, 180)); // 淡绿色，半透明
    painter.setPen(QPen(QColor(200, 220, 160, 120), 1)); // 更淡的绿色边框，透明
    painter.drawRoundedRect(bubbleRect, 8, 8);
    
    // 绘制气泡尖角，确保指向当前时间的精确位置
    int arrowX = currentX;
    // 如果气泡被边界限制移动了，箭头仍然指向原始位置
    if (arrowX < bubbleX + 10) arrowX = bubbleX + 10;
    if (arrowX > bubbleX + bubbleWidth - 10) arrowX = bubbleX + bubbleWidth - 10;
    
    QPolygon triangle;
    triangle << QPoint(arrowX - 6, y + bubbleHeight)
             << QPoint(arrowX + 6, y + bubbleHeight)
             << QPoint(arrowX, y + bubbleHeight + 8);
    painter.setBrush(QColor(220, 240, 180, 180)); // 与气泡相同的淡色半透明
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(triangle);
    
    // 绘制时间文本
    painter.setPen(QColor(40, 40, 40)); // 深灰色文字
    painter.drawText(bubbleRect, Qt::AlignCenter, text);
}

void TimeContral::drawDateOnTimeline(QPainter &painter)
{
    if (!d->m_showDateOnTimeline)
        return;
        
    // 设置字体
    QFont font = painter.font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);
    
    // 绘制日期文本，基于可见时间范围的中点
    painter.setPen(QPen(Qt::white));
    
    // 计算可见时间范围的中点时间
    qint64 midTimeMs = (d->m_visibleStartTime.toMSecsSinceEpoch() + 
                       d->m_visibleEndTime.toMSecsSinceEpoch()) / 2;
    QDateTime midTime = QDateTime::fromMSecsSinceEpoch(midTimeMs);
    
    QString dateText = midTime.toString("yyyy-MM-dd");
    QFontMetrics fm(font);
    int textWidth = fm.width(dateText);
    
    // 在时间轴底部绘制日期
    painter.drawText((width() - textWidth)/2, height() - 10, dateText);
}

int TimeContral::timeToPos(const QDateTime &time) const
{
    int margin = 20; // 与drawTimeScale中的边距保持一致
    int availableWidth = width() - 2 * margin;
    
    if (time <= d->m_visibleStartTime)
        return margin;
    if (time >= d->m_visibleEndTime)
        return width() - margin;
    
    qint64 totalTimeSpan = d->m_visibleEndTime.toMSecsSinceEpoch() - 
                          d->m_visibleStartTime.toMSecsSinceEpoch();
    qint64 timeOffset = time.toMSecsSinceEpoch() - 
                       d->m_visibleStartTime.toMSecsSinceEpoch();
    
    return margin + static_cast<int>(static_cast<double>(timeOffset) / totalTimeSpan * availableWidth);
}

QDateTime TimeContral::posToTime(int pos) const
{
    int margin = 20; // 与drawTimeScale中的边距保持一致
    int availableWidth = width() - 2 * margin;
    
    if (pos <= margin)
        return d->m_visibleStartTime;
    if (pos >= width() - margin)
        return d->m_visibleEndTime;
    
    qint64 totalTimeSpan = d->m_visibleEndTime.toMSecsSinceEpoch() - 
                          d->m_visibleStartTime.toMSecsSinceEpoch();
    qint64 timeOffset = static_cast<qint64>(static_cast<double>(pos - margin) / availableWidth * totalTimeSpan);
    
    return QDateTime::fromMSecsSinceEpoch(d->m_visibleStartTime.toMSecsSinceEpoch() + timeOffset);
}

int TimeContral::findTimeItemAt(const QPoint &pos) const
{
    // 将鼠标位置转换为时间
    QDateTime time = posToTime(pos.x());
    
    // 检查每个时间项
    for (int i = d->m_timeItems.size() - 1; i >= 0; --i) {
        const TimeItem &item = d->m_timeItems.at(i);
        
        if (item.isPoint) {
            // 对于时间点，检查鼠标是否在点的附近
            int x = timeToPos(item.startTime);
            if (qAbs(pos.x() - x) <= 5)
                return i;
        } else {
            // 对于时间段，检查鼠标是否在段内
            if (time >= item.startTime && time <= item.endTime)
                return i;
        }
    }
    
    return -1;
}

void TimeContral::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 检查是否点击了时间项
        int index = findTimeItemAt(event->pos());
        if (index != -1) {
            setCurrentTimeItem(index);
            emit timeItemClicked(index);
            return;
        }
        
        // 开始拖动滚动时间轴
        d->m_isDragging = true;
        d->m_lastMousePos = event->pos().x();
        // 记录气泡在屏幕上的固定位置（当前时间对应的屏幕位置）
        d->m_dragStartMousePos = timeToPos(d->m_currentTime);
        setCursor(Qt::ClosedHandCursor);
    }
    
    QWidget::mousePressEvent(event);
}

void TimeContral::mouseMoveEvent(QMouseEvent *event)
{
    if (d->m_isDragging && d->m_lastMousePos != -1) {
        // 计算鼠标移动的像素距离
        int pixelDelta = d->m_lastMousePos - event->pos().x();
        d->m_lastMousePos = event->pos().x();
        
        // 平滑滚动：基于像素距离计算时间偏移
        if (pixelDelta != 0) {
            // 计算可见时间范围
            qint64 timeSpan = d->m_visibleEndTime.toMSecsSinceEpoch() - 
                             d->m_visibleStartTime.toMSecsSinceEpoch();
            
            // 计算每像素对应的时间（毫秒），添加滚动阻尼系数
            double dampingFactor = 0.3; // 阻尼系数，值越小滚动越慢
            int availableWidth = width() - 40; // 减去边距
            double msPerPixel = static_cast<double>(timeSpan) / availableWidth * dampingFactor;
            
            // 计算时间偏移量
            qint64 timeOffset = static_cast<qint64>(pixelDelta * msPerPixel);
            
            // 计算新的可见时间范围
            QDateTime newStart = QDateTime::fromMSecsSinceEpoch(
                d->m_visibleStartTime.toMSecsSinceEpoch() + timeOffset);
            QDateTime newEnd = QDateTime::fromMSecsSinceEpoch(
                d->m_visibleEndTime.toMSecsSinceEpoch() + timeOffset);
            
            // 根据无限滚动设置决定是否检查边界
            if (d->m_infiniteScrollEnabled) {
                // 无限滚动：直接更新可见时间范围
                d->m_visibleStartTime = newStart;
                d->m_visibleEndTime = newEnd;
                
                emit visibleTimeRangeChanged(d->m_visibleStartTime, d->m_visibleEndTime);
                update();
            } else {
                // 有限滚动：检查边界
                if (newStart >= d->m_minTime && newEnd <= d->m_maxTime) {
                    d->m_visibleStartTime = newStart;
                    d->m_visibleEndTime = newEnd;
                    
                    emit visibleTimeRangeChanged(d->m_visibleStartTime, d->m_visibleEndTime);
                    update();
                }
            }
        }
    } else {
        // 检查鼠标是否悬停在时间项上
        int index = findTimeItemAt(event->pos());
        if (index != -1) {
            const TimeItem &item = d->m_timeItems.at(index);
            QString tip = item.label;
            if (tip.isEmpty()) {
                if (item.isPoint) {
                    tip = item.startTime.toString(d->m_timeFormat);
                } else {
                    tip = item.startTime.toString(d->m_timeFormat) + " - " + 
                          item.endTime.toString(d->m_timeFormat);
                }
            }
            QToolTip::showText(event->globalPos(), tip, this);
            setCursor(Qt::PointingHandCursor);
        } else {
            QToolTip::hideText();
            setCursor(Qt::ArrowCursor);
        }
    }
    
    QWidget::mouseMoveEvent(event);
}

void TimeContral::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 结束拖动
        d->m_isDragging = false;
        d->m_lastMousePos = -1;
        setCursor(Qt::ArrowCursor);
        
        // 检查是否是双击
        if (event->flags() & Qt::MouseEventCreatedDoubleClick) {
            int index = findTimeItemAt(event->pos());
            if (index != -1) {
                emit timeItemDoubleClicked(index);
            }
        }
    }
    
    QWidget::mouseReleaseEvent(event);
}

void TimeContral::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 检查是否双击了时间项
        int index = findTimeItemAt(event->pos());
        if (index != -1) {
            emit timeItemDoubleClicked(index);
            return;
        }
        
        // 双击时间轴切换当前时间，调整到整小时
        QDateTime clickedTime = posToTime(event->pos().x());
        
        // 将时间调整到整小时（分钟和秒设为0）
        QDateTime hourTime = QDateTime(clickedTime.date(), 
                                      QTime(clickedTime.time().hour(), 0, 0));
        
        setCurrentTime(hourTime);
    }
    
    QWidget::mouseDoubleClickEvent(event);
}

void TimeContral::wheelEvent(QWheelEvent *event)
{
    // 使用鼠标滚轮进行缩放
    QPoint numDegrees = event->angleDelta() / 8;
    
    if (!numDegrees.isNull()) {
        if (numDegrees.y() > 0) {
            // 向上滚动，放大
            setZoomLevel(d->m_zoomLevel * 1.1);
        } else {
            // 向下滚动，缩小
            setZoomLevel(d->m_zoomLevel / 1.1);
        }
    }
    
    event->accept();
}

void TimeContral::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    updateLayout();
}

void TimeContral::updateLayout()
{
    // 在控件大小改变时更新布局
    update();
}