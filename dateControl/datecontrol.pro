QT += core widgets

CONFIG += c++11

# 最低 Qt 版本要求
lessThan(QT_MAJOR_VERSION, 5): error("需要 Qt 5.0 或更高版本")
equals(QT_MAJOR_VERSION, 5): lessThan(QT_MINOR_VERSION, 6): error("需要 Qt 5.6 或更高版本")

TARGET = datecontrol_demo
TEMPLATE = app

# 源文件
SOURCES += \
    datecontrol.cpp \
    datecontrol_demo.cpp \
    datecontrol_main.cpp

# 头文件
HEADERS += \
    datecontrol.h \
    datecontrol_demo.h

# 编译器设置
win32 {
    CONFIG += console
    QMAKE_CXXFLAGS += /utf-8
}

unix {
    QMAKE_CXXFLAGS += -std=c++11
}

# 输出目录
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc
UI_DIR = $$PWD/build/ui

# 创建输出目录
!exists($$DESTDIR) {
    system(mkdir $$DESTDIR)
}

!exists($$OBJECTS_DIR) {
    system(mkdir $$OBJECTS_DIR)
}

# 版本信息
VERSION = 1.0.0
QMAKE_TARGET_COMPANY = "DateControl Project"
QMAKE_TARGET_PRODUCT = "DateControl Demo"
QMAKE_TARGET_DESCRIPTION = "Date Control Widget Demo Application"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2024"