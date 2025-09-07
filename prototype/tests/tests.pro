QT += testlib core
QT -= gui

TARGET = test_bloodgasanalyzer
TEMPLATE = app

CONFIG += c++23 console
CONFIG -= app_bundle

SOURCES += \
    test_bloodgasanalyzer.cpp \
    ../src/bloodgasanalyzer.cpp

HEADERS += \
    ../src/bloodgasanalyzer.h

INCLUDEPATH += ../src
