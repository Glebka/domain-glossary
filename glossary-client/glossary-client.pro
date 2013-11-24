#-------------------------------------------------
#
# Project created by QtCreator 2013-09-27T23:24:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = glossary-client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    textedit.cpp \
    logindialog.cpp \
    texteditor.cpp \
    addlinkdialog.cpp \
    common.cpp \
    requestbuilder.cpp \
    clientsideproxy.cpp

HEADERS  += mainwindow.h \
    textedit.h \
    logindialog.h \
    texteditor.h \
    addlinkdialog.h \
    common.h \
    requestbuilder.h \
    clientsideproxy.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    addlinkdialog.ui

RESOURCES += \
    resources.qrc
