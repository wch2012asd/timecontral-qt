#ifndef DEMO_WINDOW_H
#define DEMO_WINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include "timecontral.h"
/**
 * @brief TimeContral控件的简单演示窗口
 */
class DemoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DemoWindow(QWidget *parent = nullptr);
    ~DemoWindow();

private:
    void setupUI();
    void setupTimeContral();
    void addSampleTimeItems();
    
private:
    TimeContral *m_timeContral;
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
};

#endif // DEMO_WINDOW_H
