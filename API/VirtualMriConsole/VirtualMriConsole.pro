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
    SampleDataProtocol.cpp \
    databin.cpp \
    mask.cpp \
    EcnuRawDataFile.cpp \
    EcnuRawDataReader.cpp \
    virtualconsole.cpp \
    communicator.cpp \
    myevent.cpp \
    scantask.cpp \
    datapackage.cpp

HEADERS  += mainwindow.h \
    SampleDataProtocol.h \
    databin.h \
    mask.h \
    EcnuRawDataFile.h \
    EcnuRawDataReader.h \
    RawDataReader.h \
    virtualconsole.h \
    communicator.h \
    myevent.h \
    scantask.h \
    datapackage.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$[YAP_ROOT]/shared/ \
        $$[YAP_ROOT]/ \
        $$[THIRDPARTY]/LOG4CPLUS2/include/ \
        $$[THIRDPARTY]/boost/

DEPENDPATH += $$[THIRDPARTY]/log4cplus2/include

win32:CONFIG(release, debug|release): {
LIBS += -L$$[YAP_ROOT]/LIB/ -lClient -lImplement -lYap
LIBS += -L$$[THIRDPARTY]/log4cplus2/lib/ -llog4cplusud
LIBS += -L$$[THIRDPARTY]/boost/lib64-msvc-14.0/ -lboost_thread-vc140-mt-1_61
}
else:win32:CONFIG(debug, debug|release): {
LIBS += -L$$[YAP_ROOT]/LIB/ -lYapd -lClientd -lImplementd
LIBS += -L$$[THIRDPARTY]/log4cplus2/lib/ -llog4cplusud
LIBS += -L$$[THIRDPARTY]/boost/lib64-msvc-14.0/ -lboost_thread-vc140-mt-gd-1_61
}
else:unix: {
LIBS += -L$$[YAP_ROOT]/LIB/ -lClientd -lImplementd -lYapd
LIBS += -L$$[THIRDPARTY]/log4cplus2/ -llog4cplus

}
