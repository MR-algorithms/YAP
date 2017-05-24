#-------------------------------------------------
#
# Project created by QtCreator 2016-09-18T16:46:46
#
#-------------------------------------------------

QT       -= gui

QT += widgets

TARGET = YapClient
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/.. \
    D:/ThirdParty/boost \
    $$PWD/../../shared

DEPENDPATH += $$PWD/..

SOURCES += ModuleManager.cpp \
    PipelineCompiler.cpp \
    PipelineConstructor.cpp \
    ProcessorAgent.cpp \
    ../../shared/Client/DataHelper.cpp \
    ../../shared/Client/stdafx.cpp \
    ../../shared/Implement/CompositeProcessor.cpp \
    ../../shared/Implement/DataObject.cpp \
    ../../shared/Implement/ProcessorImpl.cpp \
    ../../shared/Implement/VariableSpace.cpp \
    ../../shared/Implement/LogImpl.cpp \
    ../../shared/Implement/LogUserImpl.cpp \
    Preprocessor.cpp \
    VdfParser.cpp \
    ScanFileParser.cpp

HEADERS += ModuleManager.h \
    PipelineCompiler.h \
    PipelineConstructor.h \
    ProcessorAgent.h \
    ../../shared/Client/DataHelper.h \
    ../../shared/Client/stdafx.h \
    ../../shared/Client/targetver.h \
    ../../shared/Implement/CompositeProcessor.h \
    ../../shared/Implement/ContainerImpl.h \
    ../../shared/Implement/DataObject.h \
    ../../shared/Implement/ProcessorImpl.h \
    ../../shared/Implement/YapImplement.h \
    ../../shared/Implement/VariableSpace.h \
    ../../shared/Implement/LogImpl.h \
    ../../shared/Implement/LogUserImpl.h \
    Preprocessor.h \
    VdfParser.h \
    ScanFileParser.h \
    ../../shared/Interface/Interfaces.h \
    ../../shared/Interface/smartptr.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \

    ../../shared/Interface/Implement/README.md

win32:CONFIG(release, debug|release): LIBS += -L/ThirdParty/log4cplus2/lib/ -llog4cplus
else:win32:CONFIG(debug, debug|release): LIBS += -L/ThirdParty/log4cplus2/lib/ -llog4cplusu
else:unix: LIBS += -L/ThirdParty/log4cplus2/lib/ -llog4cplus

INCLUDEPATH += d:/ThirdParty/log4cplus2/include
DEPENDPATH += d:/ThirdParty/log4cplus2/include
