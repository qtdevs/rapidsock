QT      += core gui widgets

TARGET   = rapidsock
TEMPLATE = app

CONFIG  += c++11

SOURCES += \
    main.cpp \
    serverwindow.cpp \
    clientwindow.cpp

HEADERS += \
    serverwindow.h \
    clientwindow.h

FORMS   += \
    serverwindow.ui \
    clientwindow.ui

# Core files
include(rapidsock/rapidsock.pri)
