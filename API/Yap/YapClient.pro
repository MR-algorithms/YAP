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
    ProcessorManagerAgent.cpp \
    ../../shared/Interface/Client/DataHelper.cpp \
    ../../shared/Interface/Client/stdafx.cpp \
    ../../shared/Interface/Implement/CompositeProcessor.cpp \
    ../../shared/Interface/Implement/DataObject.cpp \
    ../../shared/Interface/Implement/ProcessorImpl.cpp \
    ../../shared/Interface/Implement/SharedObjectImpl.cpp \
    ../../shared/Interface/Implement/VariableManager.cpp

HEADERS += ProcessorManagerAgent.h \
    ModuleManager.h \
    PipelineCompiler.h \
    PipelineConstructor.h \
    ProcessorAgent.h \
    ../../shared/Interface/Client/DataHelper.h \
    ../../shared/Interface/Client/stdafx.h \
    ../../shared/Interface/Client/targetver.h \
    ../../shared/Interface/Implement/CompositeProcessor.h \
    ../../shared/Interface/Implement/ContainerImpl.h \
    ../../shared/Interface/Implement/DataObject.h \
    ../../shared/Interface/Implement/ProcessorImpl.h \
    ../../shared/Interface/Implement/SharedObjectImpl.h \
    ../../shared/Interface/Implement/YapImplement.h \
    ../../shared/Interface/IContainer.h \
    ../../shared/Interface/IData.h \
    ../../shared/Interface/IMemory.h \
    ../../shared/Interface/IProcessor.h \
    ../../shared/Interface/IProperty.h \
    ../../shared/Interface/Implement/VariableManager.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \

    ../../shared/Interface/Implement/README.md
