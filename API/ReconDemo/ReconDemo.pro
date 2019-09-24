#-------------------------------------------------
#
# Project created by QtCreator 2017-11-10T11:58:05
#
#-------------------------------------------------

QT       += core gui charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ReconDemo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    Geometry/algorithm.cpp \
    Geometry/cmatrix44.cpp \
    Geometry/Line3D.cpp \
    Geometry/Matrix33.cpp \
    Geometry/Plane.cpp \
    Geometry/Rect2D.cpp \
    Geometry/Rect3D.cpp \
    Geometry/Segment2D.cpp \
    Geometry/Segment3D.cpp \
    grayscalemapper.cpp \
    imageobject.cpp \
    imagewidget.cpp \
    imagewindowmapper.cpp \
    plot1d.cpp \
    rectcalc.cpp \
    imagelayoutwidget.cpp \
    datamanager.cpp \
    DataSample/reconclientsocket.cpp \
    DataSample/reconserver.cpp \
    DataSample/datapackage.cpp \
    Processors/ReceiveData.cpp \
    Processors/Display1D.cpp \
    Processors/Display2D.cpp \
    Processors/PSliceIterator.cpp \
    Processors/PFft2D.cpp

HEADERS += \
        mainwindow.h \
    Geometry/algorithm.h \
    Geometry/Angle.h \
    Geometry/cmatrix44.h \
    Geometry/geometry.h \
    Geometry/Line3D.h \
    Geometry/Matrix33.h \
    Geometry/Plane.h \
    Geometry/Rect2D.h \
    Geometry/Rect3D.h \
    Geometry/Segment2D.h \
    Geometry/Segment3D.h \
    grayscalemapper.h \
    imageobject.h \
    imagewidget.h \
    imagewindowmapper.h \
    plot1d.h \
    rectcalc.h \
    imagelayoutwidget.h \
    datamanager.h \
    StringHelper.h \
    DataSample/reconclientsocket.h \
    DataSample/reconserver.h \
    DataSample/SampleDataClient.h \
    DataSample/SampleDataProtocol.h \
    DataSample/CmrPackItemTypeHelper.h \
    DataSample/CmrPackType.h \
    DataSample/datapackage.h \
    Processors/ReceiveData.h \
    Processors/Display1D.h \
    Processors/Display2D.h \
    Processors/PSliceIterator.h \
    Processors/PFft2D.h

FORMS += \
        mainwindow.ui


INCLUDEPATH += $$[YAP_ROOT]/shared/ \
        $$[YAP_ROOT]/ \
		$$[THIRDPARTY]/log4cplus-1.2.1/include/ \
        $$[THIRDPARTY]/boost/\
        $$[THIRDPARTY]/FFTW/include/

DEPENDPATH += $$[THIRDPARTY]/log4cplus2/include

DEPENDPATH += $$[THIRDPARTY]/FFTW/include

win32:CONFIG(release, debug|release): {
LIBS += -L$$[YAP_ROOT]/LIB/ -lClient -lImplement -lYap
LIBS += -L$$[THIRDPARTY]/log4cplus-1.2.1/lib/win32/ -llog4cplusu
LIBS += -L$$[THIRDPARTY]/boost/lib64-msvc-14.0/ -lboost_thread-vc140-mt-1_61
LIBS += -L$$[THIRDPARTY]/FFTW/lib/ -llibfftw3-3
LIBS += -L$$[THIRDPARTY]/FFTW/lib/ -llibfftw3f-3
LIBS += -L$$[THIRDPARTY]/FFTW/lib/ -llibfftw3l-3
}
else:win32:CONFIG(debug, debug|release): {
LIBS += -L$$[YAP_ROOT]/LIB/ -lYapd -lClientd -lImplementd
LIBS += -L$$[THIRDPARTY]/log4cplus-1.2.1/lib/win32/ -llog4cplusud
LIBS += -L$$[THIRDPARTY]/boost/lib64-msvc-14.0/ -lboost_thread-vc140-mt-gd-1_61
LIBS += -L$$[THIRDPARTY]/FFTW/lib/ -llibfftw3-3
LIBS += -L$$[THIRDPARTY]/FFTW/lib/ -llibfftw3f-3
LIBS += -L$$[THIRDPARTY]/FFTW/lib/ -llibfftw3l-3
}
else:unix: {
LIBS += -L$$[YAP_ROOT]/LIB/ -lClientd -lImplementd -lYapd
LIBS += -L$$[THIRDPARTY]/log4cplus-1.2.1/ -llog4cplus

}

