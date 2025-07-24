#ifndef DATE_DEMO_WINDOW_H
#define DATE_DEMO_WINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "../dateControl/datepicker.h"

/**
 * @brief DatePicker控件的演示窗口
 */
class DateDemoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DateDemoWindow(QWidget *parent = nullptr);
    ~DateDemoWindow();

private slots:
    void onDateChanged(const QDate &date);
    void onDateSelected(const QDate &date);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void setupDatePickers();
    
private:
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    DatePicker *m_datePicker1;
    
    QLabel *m_infoLabel;
};

#endif // DATE_DEMO_WINDOW_H