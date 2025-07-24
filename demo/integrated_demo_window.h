#ifndef INTEGRATED_DEMO_WINDOW_H
#define INTEGRATED_DEMO_WINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include "../dateControl/datepicker.h"
#include "../timePlay/timeplaycontrol.h"
#include "timecontral.h"

/**
 * @brief 集成演示窗口 - 日期控制器与时间轴播放控件联动
 */
class IntegratedDemoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit IntegratedDemoWindow(QWidget *parent = nullptr);
    ~IntegratedDemoWindow();

private slots:
    void onDateChanged(const QDate &date);
    void onDateSelected(const QDate &date);
    void onPlayStateChanged(TimePlayControl::PlayState state);
    void onCurrentTimeChanged(const QDateTime &time);
    void onStepForwardClicked();
    void onStepBackwardClicked();
    void onPlayClicked();
    void onPauseClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void setupConnections();
    void updateTimelineForDate(const QDate &date);
    void moveTimelineBubbleToHour(int hour);
    
private:
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    // 上部分：日期选择器
    QWidget *m_dateSection;
    DatePicker *m_datePicker;
    QLabel *m_dateInfoLabel;
    
    // 中部分：时间轴
    QWidget *m_timelineSection;
    TimeContral *m_timeContral;
    QLabel *m_timelineInfoLabel;
    
    // 下部分：播放控制
    QWidget *m_playSection;
    TimePlayControl *m_timePlayControl;
    QLabel *m_playInfoLabel;
    
    // 当前状态
    QDate m_currentDate;
    int m_currentHour;
    QDateTime m_baseDateTime;
};

#endif // INTEGRATED_DEMO_WINDOW_H