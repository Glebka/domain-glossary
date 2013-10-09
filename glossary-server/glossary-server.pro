#-------------------------------------------------
#
# Project created by QtCreator 2013-10-09T17:01:40
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = glossary-server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    listener.cpp \
    clientworker.cpp

OTHER_FILES += \
    contents.xml \
    config.xml

HEADERS += \
    listener.h \
    clientworker.h
