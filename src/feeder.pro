#-------------------------------------------------
#
# Project created by QtCreator 2014-09-07T01:22:54
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = feeder
TEMPLATE = app


SOURCES += main.cpp \
    maintray.cpp \
    rssreader.cpp \
    atomreader.cpp \
    settingsdialog.cpp \
    feedsource.cpp \
    general.cpp \
    menu.cpp

HEADERS  += \
    maintray.h \
    rssreader.h \
    atomreader.h \
    settingsdialog.h \
    feedsource.h \
    general.h \
    menu.h

FORMS += \
    settingsdialog.ui
