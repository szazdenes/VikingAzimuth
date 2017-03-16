#-------------------------------------------------
#
# Project created by QtCreator 2017-03-16T16:09:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VikingAzimuth
TEMPLATE = app
DESTDIR = ./bin
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
RCC_DIR = ./rcc
UI_DIR = ./ui


SOURCES += main.cpp\
        mainwindow.cpp \
    computation.cpp

HEADERS  += mainwindow.h \
    computation.h

FORMS    += mainwindow.ui
