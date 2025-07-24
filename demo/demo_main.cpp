#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QFont>
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include "demo_window.h"
#include "date_demo_window.h"
#include "timeplay_demo_window.h"
#include "integrated_demo_window.h"

class DemoSelector : public QWidget
{
    Q_OBJECT
public:
    DemoSelector(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("演示选择器");
        setFixedSize(320, 300);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *titleLabel = new QLabel("请选择要演示的控件:", this);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; margin: 20px;");
        layout->addWidget(titleLabel);
        
        QPushButton *integratedBtn = new QPushButton("🔗 集成演示（推荐）", this);
        integratedBtn->setStyleSheet("QPushButton { padding: 12px; font-size: 13px; font-weight: bold; background-color: #e74c3c; }");
        connect(integratedBtn, &QPushButton::clicked, this, &DemoSelector::showIntegratedDemo);
        layout->addWidget(integratedBtn);
        
        QPushButton *timeBtn = new QPushButton("时间轴控件演示", this);
        timeBtn->setStyleSheet("QPushButton { padding: 10px; font-size: 12px; }");
        connect(timeBtn, &QPushButton::clicked, this, &DemoSelector::showTimeDemo);
        layout->addWidget(timeBtn);
        
        QPushButton *dateBtn = new QPushButton("日期控制器演示", this);
        dateBtn->setStyleSheet("QPushButton { padding: 10px; font-size: 12px; }");
        connect(dateBtn, &QPushButton::clicked, this, &DemoSelector::showDateDemo);
        layout->addWidget(dateBtn);
        
        QPushButton *timePlayBtn = new QPushButton("时间轴播放控件演示", this);
        timePlayBtn->setStyleSheet("QPushButton { padding: 10px; font-size: 12px; }");
        connect(timePlayBtn, &QPushButton::clicked, this, &DemoSelector::showTimePlayDemo);
        layout->addWidget(timePlayBtn);
        
        layout->addStretch();
    }

private slots:
    void showIntegratedDemo()
    {
        //hide();
        IntegratedDemoWindow *integratedWindow = new IntegratedDemoWindow();
        integratedWindow->setAttribute(Qt::WA_DeleteOnClose);
        integratedWindow->show();
        connect(integratedWindow, &QWidget::destroyed, this, &QWidget::show);
    }
    
    void showTimeDemo()
    {
        //hide();
        DemoWindow *timeWindow = new DemoWindow();
        timeWindow->setAttribute(Qt::WA_DeleteOnClose);
        timeWindow->show();
        connect(timeWindow, &QWidget::destroyed, this, &QWidget::show);
    }
    
    void showDateDemo()
    {
        //hide();
        DateDemoWindow *dateWindow = new DateDemoWindow();
        dateWindow->setAttribute(Qt::WA_DeleteOnClose);
        dateWindow->show();
        connect(dateWindow, &QWidget::destroyed, this, &QWidget::show);
    }
    
    void showTimePlayDemo()
    {
        //hide();
        TimePlayDemoWindow *timePlayWindow = new TimePlayDemoWindow();
        timePlayWindow->setAttribute(Qt::WA_DeleteOnClose);
        timePlayWindow->show();
        connect(timePlayWindow, &QWidget::destroyed, this, &QWidget::show);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("Control Demo");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Control Project");
    
    // 设置中文本地化
    QLocale::setDefault(QLocale::Chinese);
    
    // 设置应用程序字体
    QFont font = app.font();
    font.setFamily("Microsoft YaHei");
    font.setPointSize(9);
    app.setFont(font);
    
    // 设置样式
    if (QStyleFactory::keys().contains("Fusion")) {
        app.setStyle(QStyleFactory::create("Fusion"));
    }
    
    // 设置简单的应用程序样式表
    QString styleSheet = R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QWidget {
            background-color: #ffffff;
        }
        QPushButton {
            padding: 8px;
            min-width: 120px;
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 4px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #21618c;
        }
        QGroupBox {
            font-weight: bold;
            margin-top: 1ex;
            padding-top: 10px;
            border: 2px solid #bdc3c7;
            border-radius: 5px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
    )";
    
    app.setStyleSheet(styleSheet);
    
    // 创建并显示选择器窗口
    DemoSelector selector;
    selector.show();
    
    return app.exec();
}

#include "demo_main.moc"