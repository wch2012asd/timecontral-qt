#include "timeplaycontrol.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QTimer>
#include <QDebug>

// 私有实现类
class TimePlayControl::Private
{
public:
    Private(TimePlayControl *q)
        : q(q)
        , playState(Stopped)
        , currentTime(QDateTime::currentDateTime())
        , startTime(QDateTime::currentDateTime().addSecs(-3600))
        , endTime(QDateTime::currentDateTime().addSecs(3600))
        , playSpeed(1.0)
        , stepInterval(60)
        , playTimer(new QTimer(q))
        , mainLayout(nullptr)
        , stepBackwardButton(nullptr)
        , playPauseButton(nullptr)
        , stepForwardButton(nullptr)
    {
        playTimer->setInterval(1000); // 1秒更新一次
        QObject::connect(playTimer, &QTimer::timeout, q, &TimePlayControl::onPlayTimer);
    }

    TimePlayControl *q;
    
    // 播放状态
    PlayState playState;
    QDateTime currentTime;
    QDateTime startTime;
    QDateTime endTime;
    double playSpeed;
    int stepInterval;
    
    // 定时器
    QTimer *playTimer;
    
    // UI组件
    QHBoxLayout *mainLayout;
    QPushButton *stepBackwardButton;
    QPushButton *playPauseButton;
    QPushButton *stepForwardButton;
};

TimePlayControl::TimePlayControl(QWidget *parent)
    : QWidget(parent)
    , d(new Private(this))
{
    setMinimumSize(300, 80);
    setMaximumHeight(100);
    setupUI();
    updateButtonStates();
}

TimePlayControl::~TimePlayControl()
{
    delete d;
}

void TimePlayControl::setupUI()
{
    // 创建主布局
    d->mainLayout = new QHBoxLayout(this);
    d->mainLayout->setContentsMargins(20, 15, 20, 15);
    d->mainLayout->setSpacing(30);
    
    // 创建后退按钮
    d->stepBackwardButton = new QPushButton(this);
    d->stepBackwardButton->setFixedSize(50, 50);
    createCircularButton(d->stepBackwardButton, "‹‹");
    connect(d->stepBackwardButton, &QPushButton::clicked, this, &TimePlayControl::onStepBackwardClicked);
    
    // 创建播放/暂停按钮
    d->playPauseButton = new QPushButton(this);
    d->playPauseButton->setFixedSize(50, 50);
    createCircularButton(d->playPauseButton, "▶");
    connect(d->playPauseButton, &QPushButton::clicked, this, &TimePlayControl::onPlayButtonClicked);
    
    // 创建前进按钮
    d->stepForwardButton = new QPushButton(this);
    d->stepForwardButton->setFixedSize(50, 50);
    createCircularButton(d->stepForwardButton, "››");
    connect(d->stepForwardButton, &QPushButton::clicked, this, &TimePlayControl::onStepForwardClicked);
    
    // 添加到布局
    d->mainLayout->addStretch();
    d->mainLayout->addWidget(d->stepBackwardButton);
    d->mainLayout->addWidget(d->playPauseButton);
    d->mainLayout->addWidget(d->stepForwardButton);
    d->mainLayout->addStretch();
}

void TimePlayControl::createCircularButton(QPushButton *button, const QString &iconText)
{
    button->setText(iconText);
    button->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: 3px solid rgba(0, 255, 255, 200);
            border-radius: 25px;
            color: rgba(0, 255, 255, 255);
            font-size: 18px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: rgba(0, 255, 255, 30);
            border: 3px solid rgba(0, 255, 255, 255);
        }
        QPushButton:pressed {
            background: rgba(0, 255, 255, 60);
            border: 3px solid rgba(0, 255, 255, 255);
        }
    )");
}

void TimePlayControl::play()
{
    if (d->playState != Playing) {
        d->playState = Playing;
        d->playTimer->start();
        updateButtonStates();
        emit playStateChanged(d->playState);
        emit playClicked();
    }
}

void TimePlayControl::pause()
{
    if (d->playState == Playing) {
        d->playState = Paused;
        d->playTimer->stop();
        updateButtonStates();
        emit playStateChanged(d->playState);
        emit pauseClicked();
    }
}

void TimePlayControl::stop()
{
    if (d->playState != Stopped) {
        d->playState = Stopped;
        d->playTimer->stop();
        d->currentTime = d->startTime;
        updateButtonStates();
        emit playStateChanged(d->playState);
        emit currentTimeChanged(d->currentTime);
        emit stopClicked();
    }
}

void TimePlayControl::stepForward()
{
    QDateTime newTime = d->currentTime.addSecs(d->stepInterval);
    if (newTime <= d->endTime) {
        setCurrentTime(newTime);
        emit stepForwardClicked();
    }
}

void TimePlayControl::stepBackward()
{
    QDateTime newTime = d->currentTime.addSecs(-d->stepInterval);
    if (newTime >= d->startTime) {
        setCurrentTime(newTime);
        emit stepBackwardClicked();
    }
}

TimePlayControl::PlayState TimePlayControl::playState() const
{
    return d->playState;
}

bool TimePlayControl::isPlaying() const
{
    return d->playState == Playing;
}

void TimePlayControl::setCurrentTime(const QDateTime &time)
{
    if (time != d->currentTime && time >= d->startTime && time <= d->endTime) {
        d->currentTime = time;
        emit currentTimeChanged(d->currentTime);
    }
}

QDateTime TimePlayControl::currentTime() const
{
    return d->currentTime;
}

void TimePlayControl::setTimeRange(const QDateTime &startTime, const QDateTime &endTime)
{
    if (startTime < endTime) {
        d->startTime = startTime;
        d->endTime = endTime;
        
        // 确保当前时间在范围内
        if (d->currentTime < d->startTime) {
            setCurrentTime(d->startTime);
        } else if (d->currentTime > d->endTime) {
            setCurrentTime(d->endTime);
        }
    }
}

QDateTime TimePlayControl::startTime() const
{
    return d->startTime;
}

QDateTime TimePlayControl::endTime() const
{
    return d->endTime;
}

void TimePlayControl::setPlaySpeed(double speed)
{
    if (speed > 0) {
        d->playSpeed = speed;
        // 更新定时器间隔 - 支持慢速播放
        int interval;
        if (speed >= 1.0) {
            interval = static_cast<int>(1000 / speed);
        } else {
            // 对于小于1的速度，增加间隔时间
            interval = static_cast<int>(1000 / speed);
        }
        d->playTimer->setInterval(interval);
    }
}

double TimePlayControl::playSpeed() const
{
    return d->playSpeed;
}

void TimePlayControl::setStepInterval(int seconds)
{
    if (seconds > 0) {
        d->stepInterval = seconds;
    }
}

int TimePlayControl::stepInterval() const
{
    return d->stepInterval;
}

void TimePlayControl::onPlayButtonClicked()
{
    if (d->playState == Playing) {
        pause();
    } else {
        play();
    }
}

void TimePlayControl::onStepBackwardClicked()
{
    stepBackward();
}

void TimePlayControl::onStepForwardClicked()
{
    stepForward();
}

void TimePlayControl::onPlayTimer()
{
    if (d->playState == Playing) {
        // 每次定时器触发时前进一个步进间隔
        QDateTime newTime = d->currentTime.addSecs(d->stepInterval);
        if (newTime <= d->endTime) {
            setCurrentTime(newTime);
        } else {
            // 到达结束时间，停止播放
            stop();
        }
    }
}

void TimePlayControl::updateButtonStates()
{
    // 更新播放/暂停按钮图标
    if (d->playState == Playing) {
        d->playPauseButton->setText("⏸");
    } else {
        d->playPauseButton->setText("▶");
    }
    
    // 更新按钮可用状态
    d->stepBackwardButton->setEnabled(d->currentTime > d->startTime);
    d->stepForwardButton->setEnabled(d->currentTime < d->endTime);
}

void TimePlayControl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    drawBackground(painter);
}

void TimePlayControl::drawBackground(QPainter &painter)
{
    // 绘制蓝色渐变背景，与图片样式一致
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(74, 144, 226, 200));   // 顶部亮蓝色
    gradient.setColorAt(0.5, QColor(41, 128, 185, 180)); // 中部蓝色
    gradient.setColorAt(1, QColor(30, 100, 160, 160));   // 底部深蓝色
    
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 25, 25);
    
    // 添加玻璃效果边框
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255, 255, 255, 100), 2));
    painter.drawRoundedRect(rect().adjusted(3, 3, -3, -3), 23, 23);
    
    // 添加内部高光效果
    QLinearGradient highlightGradient(0, 0, 0, height() * 0.4);
    highlightGradient.setColorAt(0, QColor(255, 255, 255, 60));
    highlightGradient.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(QBrush(highlightGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(5, 5, -5, -height() * 0.6), 20, 20);
}

void TimePlayControl::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    QWidget::resizeEvent(event);
}