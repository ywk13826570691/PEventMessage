TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS +=

include($$PWD/server/server.pri)
include("$$PWD/include/include.pri")
include($$PWD/event/event.pri)
include($$PWD/client/client.pri)


LIBS += -lpthread

INCLUDEPATH += $$PWD/include
