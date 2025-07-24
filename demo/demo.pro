QT += core gui widgets

CONFIG += c++17

TARGET = timecontral_demo
TEMPLATE = app

# 包含主库的头文件
INCLUDEPATH += . \
    ../dateControl \
    ../timePlay

# 直接包含源文件
SOURCES += \
    demo_window.cpp \
    demo_main.cpp \
    date_demo_window.cpp \
    timeplay_demo_window.cpp \
    integrated_demo_window.cpp \
    ../dateControl/datecontrol.cpp \
    ../dateControl/datepicker.cpp \
    ../timePlay/timeplaycontrol.cpp

HEADERS += \
    timeContral_global.h \
    demo_window.h \
    date_demo_window.h \
    timeplay_demo_window.h \
    integrated_demo_window.h \
    ../dateControl/datecontrol.h \
    ../dateControl/datepicker.h \
    ../timePlay/timeplaycontrol.h

# 定义库符号
DEFINES += TIMECONTRAL_LIBRARY

# 设置输出目录
DESTDIR = $$PWD/bin

# 部署设置
target.path = $$[QT_INSTALL_EXAMPLES]/timecontral
INSTALLS += target


win32: LIBS += -L$$PWD/../timeControl/bin/ -ltimeContral

INCLUDEPATH += $$PWD/../timeControl
DEPENDPATH += $$PWD/../timeControl/bin

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../timeControl/bin/timeContral.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../timeControl/bin/libtimeContral.a
