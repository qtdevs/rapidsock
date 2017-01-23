QT     *= network
CONFIG *= c++11

INCLUDEPATH *= $$PWD

HEADERS += \
    $$PWD/rapidclient.h \
    $$PWD/rapidclient_p.h \
    $$PWD/rapidserver.h \
    $$PWD/rapidserver_p.h

SOURCES += \
    $$PWD/rapidclient.cpp \
    $$PWD/rapidserver.cpp
