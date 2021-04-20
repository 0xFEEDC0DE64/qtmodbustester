QT = core network serialbus gui widgets

CONFIG += c++latest

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += main.cpp \
    mainwindow.cpp \
    modbustablemodel.cpp

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h \
    modbustablemodel.h
