#-------------------------------------------------
#
# Project created by QtCreator 2016-11-05T15:41:55
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VirtualMriConsole
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SampleDataProtocol.cpp

HEADERS  += mainwindow.h \
    SampleDataProtocol.h

FORMS    += mainwindow.ui
