#-------------------------------------------------
#
# Project created by QtCreator 2016-01-23T19:54:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LiFE
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    guifile.cpp \
    filedetail.cpp \
    dirsizecounter.cpp \
    copydialog.cpp

HEADERS  += mainwindow.h \
    enhancedqt.h \
    aboutdialog.h \
    guifile.h \
    filedetail.h \
    dirsizecounter.h \
    copydialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    filedetail.ui \
    copydialog.ui

RESOURCES += \
    res.qrc
