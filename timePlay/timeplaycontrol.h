#ifndef TIMEPLAYCONTROL_H
#define TIMEPLAYCONTROL_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>
#include <QDateTime>

/**
 * @brief 时间轴播放控件
 * 
 * 提供播放、暂停、前进、后退等时间轴控制功能
 */
class TimePlayControl : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 播放状态枚举
     */
    enum PlayState {
        Stopped,    // 停止
        Playing,    // 播放中
        Paused      // 暂停
    };

public:
    explicit TimePlayControl(QWidget *parent = nullptr);
    ~TimePlayControl() override;

    // 播放控制
    void play();
    void pause();
    void stop();
    void stepForward();
    void stepBackward();
    
    // 状态获取
    PlayState playState() const;
    bool isPlaying() const;
    
    // 时间设置
    void setCurrentTime(const QDateTime &time);
    QDateTime currentTime() const;
    
    void setTimeRange(const QDateTime &startTime, const QDateTime &endTime);
    QDateTime startTime() const;
    QDateTime endTime() const;
    
    // 播放速度设置
    void setPlaySpeed(double speed);
    double playSpeed() const;
    
    // 步进间隔设置（秒）
    void setStepInterval(int seconds);
    int stepInterval() const;

signals:
    void playStateChanged(PlayState state);
    void currentTimeChanged(const QDateTime &time);
    void playClicked();
    void pauseClicked();
    void stopClicked();
    void stepForwardClicked();
    void stepBackwardClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onPlayButtonClicked();
    void onStepBackwardClicked();
    void onStepForwardClicked();
    void onPlayTimer();

private:
    void setupUI();
    void updateButtonStates();
    void drawBackground(QPainter &painter);
    void createCircularButton(QPushButton *button, const QString &iconText);
    
private:
    class Private;
    Private *d;
};

#endif // TIMEPLAYCONTROL_H