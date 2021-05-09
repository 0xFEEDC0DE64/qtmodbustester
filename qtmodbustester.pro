QT = core network serialbus gui widgets

CONFIG += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += main.cpp \
    changevaluesdialog.cpp \
    changevaluesmodel.cpp \
    mainwindow.cpp \
    modbustablemodel.cpp

FORMS += \
    changevaluesdialog.ui \
    mainwindow.ui

HEADERS += \
    changevaluesdialog.h \
    changevaluesmodel.h \
    mainwindow.h \
    modbustablemodel.h
