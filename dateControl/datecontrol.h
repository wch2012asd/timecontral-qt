#ifndef DATECONTROL_H
#define DATECONTROL_H

#include <QWidget>
#include <QDate>
#include <QDateTime>
#include <QVector>
#include <QPair>
#include <QVariant>
#include <QColor>

class QMouseEvent;
class QWheelEvent;
class QPaintEvent;
class QResizeEvent;
class QPainter;
class QTimer;

/**
 * @brief 日期控制插件类
 * 
 * 提供一个可交互的日期选择和控制界面，支持月视图、年视图、周视图切换
 */
class DateControl : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 视图模式枚举
     */
    enum ViewMode {
        MonthView,    // 月视图
        YearView,     // 年视图
        WeekView      // 周视图
    };

public:
    explicit DateControl(QWidget *parent = nullptr);
    ~DateControl() override;

    // 日期设置
    void setCurrentDate(const QDate &date);
    QDate currentDate() const;
    
    void setSelectedDate(const QDate &date);
    QDate selectedDate() const;
    
    // 视图模式
    void setViewMode(ViewMode mode);
    ViewMode viewMode() const;
    
    // 日期范围
    void setDateRange(const QDate &minDate, const QDate &maxDate);
    QDate minDate() const;
    QDate maxDate() const;
    

    
    // 外观设置
    void setBackgroundColor(const QColor &color);
    void setHeaderColor(const QColor &color);
    void setSelectedColor(const QColor &color);
    void setTodayColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setWeekendColor(const QColor &color);
    
    // 显示选项
    void setShowWeekNumbers(bool show);
    bool isShowWeekNumbers() const;
    
    void setShowToday(bool show);
    bool isShowToday() const;
    
    // 导航控制
    void goToToday();
    void goToNextMonth();
    void goToPreviousMonth();
    void goToNextYear();
    void goToPreviousYear();
    
    // 动画效果
    void setAnimationEnabled(bool enabled);
    bool isAnimationEnabled() const;

signals:
    void dateClicked(const QDate &date);
    void dateDoubleClicked(const QDate &date);
    void dateSelectionChanged(const QDate &date);
    void currentDateChanged(const QDate &date);
    void viewModeChanged(ViewMode mode);
    void monthChanged(int year, int month);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onAnimationTimer();

private:
    // 绘制函数
    void drawBackground(QPainter &painter);
    void drawHeader(QPainter &painter);
    void drawMonthView(QPainter &painter);
    void drawYearView(QPainter &painter);
    void drawWeekView(QPainter &painter);
    void drawDateCell(QPainter &painter, const QDate &date, const QRect &rect, bool isSelected, bool isToday);
    void drawBottomDateDisplay(QPainter &painter);
    
    // 辅助函数
    QRect getDateCellRect(const QDate &date) const;
    QDate getDateAtPosition(const QPoint &pos) const;
    QRect getHeaderRect() const;
    QRect getCalendarRect() const;
    void drawMiniMonth(QPainter &painter, int year, int month, const QRect &rect);
    
    // 布局计算
    void calculateLayout();
    void updateCalendar();
    
    // 动画相关
    void startTransitionAnimation();

private:
    class Private;
    Private *d;
};

#endif // DATECONTROL_H