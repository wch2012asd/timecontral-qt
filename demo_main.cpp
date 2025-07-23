#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QFont>
#include <QLocale>
#include <QTranslator>
#include "demo_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("TimeContral Demo");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("TimeContral Project");
    
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
            background-color: #f0f0f0;
        }
        QPushButton {
            padding: 5px;
            min-width: 80px;
        }
        QGroupBox {
            font-weight: bold;
            margin-top: 1ex;
            padding-top: 10px;
        }
    )";
    
    app.setStyleSheet(styleSheet);
    
    // 创建并显示主窗口
    DemoWindow window;
    window.show();
    
    return app.exec();
}