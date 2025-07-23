QT += core gui widgets

CONFIG += c++17

TARGET = timecontral_demo
TEMPLATE = app

# 包含主库的头文件
INCLUDEPATH += .

# 直接包含源文件
SOURCES += \
    timecontral.cpp \
    demo_window.cpp \
    demo_main.cpp

HEADERS += \
    timecontral.h \
    timeContral_global.h \
    demo_window.h

# 定义库符号
DEFINES += TIMECONTRAL_LIBRARY

# 设置输出目录
DESTDIR = $$PWD/bin

# 部署设置
target.path = $$[QT_INSTALL_EXAMPLES]/timecontral
INSTALLS += target