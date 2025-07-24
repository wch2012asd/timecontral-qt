#ifndef TIMECONTRAL_H
#define TIMECONTRAL_H

#include "timeContral_global.h"
#include <QWidget>
#include <QDateTime>
#include <QVector>
#include <QPair>
#include <QVariant>

class QMouseEvent;
class QWheelEvent;
class QPaintEvent;
class QResizeEvent;
class QPainter;

/**
 * @brief 时间轴控件类
 * 
 * 这个控件提供了一个可交互的时间轴，支持缩放、拖动和选择时间点/时间段
 */
class TIMECONTRAL_EXPORT TimeContral : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 时间点或时间段的数据结构
     */
    struct TimeItem {
        QDateTime startTime;      // 开始时间
        QDateTime endTime;        // 结束时间（对于时间点，与开始时间相同）
        QString label;            // 标签文本
        QColor color;             // 颜色
        bool isPoint;             // 是否为时间点（true）或时间段（false）
        QVariant userData;        // 用户数据
    };

public:
    explicit TimeContral(QWidget *parent = nullptr);
    ~TimeContral() override;

    // 设置时间范围
    void setTimeRange(const QDateTime &minTime, const QDateTime &maxTime);
    QDateTime minTime() const;
    QDateTime maxTime() const;
    
    // 设置可见时间范围（视口）
    void setVisibleTimeRange(const QDateTime &startTime, const QDateTime &endTime);
    QDateTime visibleStartTime() const;
    QDateTime visibleEndTime() const;
    
    // 添加/删除时间项
    int addTimePoint(const QDateTime &time, const QString &label = QString(), 
                    const QColor &color = Qt::blue, const QVariant &userData = QVariant());
    int addTimeSpan(const QDateTime &startTime, const QDateTime &endTime, 
                   const QString &label = QString(), const QColor &color = Qt::green,
                   const QVariant &userData = QVariant());
    bool removeTimeItem(int index);
    void clearTimeItems();
    
    // 获取时间项
    int timeItemCount() const;
    TimeItem timeItemAt(int index) const;
    
    // 设置当前选中的时间项
    void setCurrentTimeItem(int index);
    int currentTimeItem() const;
    
    // 外观设置
    void setBackgroundColor(const QColor &color);
    void setScaleColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setScaleHeight(int height);
    void setTimeFormat(const QString &format);
    
    // 时间控制
    void setCurrentTime(const QDateTime &time);
    QDateTime currentTime() const;
    
    // 显示设置
    void setShowTimeBubble(bool show);
    bool isShowTimeBubble() const;
    void setShowDateOnTimeline(bool show);
    bool isShowDateOnTimeline() const;
    
    // 缩放控制
    void zoomIn();
    void zoomOut();
    void setZoomLevel(double level);
    double zoomLevel() const;
    
    // 滚动模式控制
    void setInfiniteScrollEnabled(bool enabled);
    bool isInfiniteScrollEnabled() const;

signals:
    void timeItemClicked(int index);
    void timeItemDoubleClicked(int index);
    void timeRangeChanged(const QDateTime &minTime, const QDateTime &maxTime);
    void visibleTimeRangeChanged(const QDateTime &startTime, const QDateTime &endTime);
    void currentTimeItemChanged(int index);
    
    // 时间变化信号
    void currentTimeChanged(const QDateTime &time);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // 坐标转换
    int timeToPos(const QDateTime &time) const;
    QDateTime posToTime(int pos) const;
    
    // 绘制函数
    void drawBackground(QPainter &painter);
    void drawTimeScale(QPainter &painter);
    void drawTimeItems(QPainter &painter);
    void drawCurrentTimeIndicator(QPainter &painter);
    void drawTimeBubble(QPainter &painter);
    void drawDateOnTimeline(QPainter &painter);
    
    // 查找时间项
    int findTimeItemAt(const QPoint &pos) const;
    
    // 更新布局
    void updateLayout();

private:
    class Private;
    Private *d;
};

#endif // TIMECONTRAL_H
