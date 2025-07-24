#ifndef TIMEPLAY_DEMO_WINDOW_H
#define TIMEPLAY_DEMO_WINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QDateTimeEdit>
#include "../timePlay/timeplaycontrol.h"

/**
 * @brief TimePlayControl控件的演示窗口
 */
class TimePlayDemoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TimePlayDemoWindow(QWidget *parent = nullptr);
    ~TimePlayDemoWindow();

private slots:
    void onPlayStateChanged(TimePlayControl::PlayState state);
    void onCurrentTimeChanged(const QDateTime &time);
    void onPlayClicked();
    void onPauseClicked();
    void onStepForwardClicked();
    void onStepBackwardClicked();
    void onSpeedChanged(int value);
    void onStepIntervalChanged(int value);
    void onStartTimeChanged(const QDateTime &time);
    void onEndTimeChanged(const QDateTime &time);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void setupTimePlayControl();
    void updateStatusLabels();
    
private:
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    TimePlayControl *m_timePlayControl;
    
    // 控制面板
    QGroupBox *m_controlGroup;
    QSlider *m_speedSlider;
    QSpinBox *m_stepIntervalSpinBox;
    QDateTimeEdit *m_startTimeEdit;
    QDateTimeEdit *m_endTimeEdit;
    
    // 状态显示
    QGroupBox *m_statusGroup;
    QLabel *m_playStateLabel;
    QLabel *m_currentTimeLabel;
    QLabel *m_timeRangeLabel;
    QLabel *m_infoLabel;
};

#endif // TIMEPLAY_DEMO_WINDOW_H