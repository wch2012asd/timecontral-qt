#ifndef DATEPICKER_H
#define DATEPICKER_H

#include <QWidget>
#include <QPushButton>
#include <QDate>
#include <QVBoxLayout>
#include <QLabel>
#include "datecontrol.h"

class QPropertyAnimation;

/**
 * @brief 日期选择器控件 - 气泡弹出式日历
 */
class DatePicker : public QWidget
{
    Q_OBJECT

public:
    explicit DatePicker(QWidget *parent = nullptr);
    ~DatePicker() override;

    // 日期设置
    void setSelectedDate(const QDate &date);
    QDate selectedDate() const;
    
    // 外观设置
    void setButtonText(const QString &text);
    QString buttonText() const;

signals:
    void dateChanged(const QDate &date);
    void dateSelected(const QDate &date);

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onButtonClicked();
    void onDateClicked(const QDate &date);
    void onDateSelectionChanged(const QDate &date);
    void hideCalendar();

private:
    void setupUI();
    void showCalendar();
    void updateButtonText();
    QPoint calculatePopupPosition();
    
private:
    QPushButton *m_dateButton;
    QWidget *m_calendarPopup;
    DateControl *m_calendar;
    QVBoxLayout *m_mainLayout;
    
    QDate m_selectedDate;
    bool m_calendarVisible;
    
    QPropertyAnimation *m_showAnimation;
    QPropertyAnimation *m_hideAnimation;
};

#endif // DATEPICKER_H