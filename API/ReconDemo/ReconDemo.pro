#-------------------------------------------------
#
# Project created by QtCreator 2016-11-05T16:10:59
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ReconDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    reconclientsocket.cpp \
    reconserver.cpp

HEADERS  += mainwindow.h \
    reconserver.h \
    reconclientsocket.h

FORMS    += mainwindow.ui
