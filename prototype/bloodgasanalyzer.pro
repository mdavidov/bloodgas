QT += core widgets

TARGET = bloodgasanalyzer
TEMPLATE = app

CONFIG += c++23

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/bloodgasanalyzer.cpp

HEADERS += \
    src/mainwindow.h \
    src/bloodgasanalyzer.h

# Installation path
target.path = /usr/local/bin
INSTALLS += target