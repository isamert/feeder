#-------------------------------------------------
#
# Project created by QtCreator 2014-09-07T01:22:54
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = feeder
TEMPLATE = app

target.path = /usr/bin

#-------------------------------------------------
#
# Install for Linux
#
#-------------------------------------------------
desktop.files = ../files/$${TARGET}.desktop
desktop.path = /usr/share/applications

icon.files = ../files/$${TARGET}.png
icon.path = /usr/share/pixmaps

INSTALLS += target desktop icon

SOURCES += main.cpp \
    maintray.cpp \
    rssreader.cpp \
    atomreader.cpp \
    settingsdialog.cpp \
    feedsource.cpp \
    general.cpp \
    menu.cpp \
    aboutdialog.cpp

HEADERS  += \
    maintray.h \
    rssreader.h \
    atomreader.h \
    settingsdialog.h \
    feedsource.h \
    general.h \
    menu.h \
    aboutdialog.h

FORMS += \
    settingsdialog.ui \
    aboutdialog.ui

RESOURCES += \
    resources.qrc


