INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS                += $$PWD/configurator.h \
                          $$PWD/qhttpserver/qhttpconnection.h \
                          $$PWD/qhttpserver/qhttprequest.h \
                          $$PWD/qhttpserver/qhttpresponse.h \
                          $$PWD/qhttpserver/qhttpserver.h \
                          $$PWD/qhttpserver/qhttpserverapi.h \
                          $$PWD/qhttpserver/qhttpserverfwd.h \
                          $$PWD/qhttpserver/http-parser/http_parser.h

SOURCES                += $$PWD/configurator.cpp \
                          $$PWD/qhttpserver/qhttpconnection.cpp \
                          $$PWD/qhttpserver/qhttprequest.cpp \
                          $$PWD/qhttpserver/qhttpresponse.cpp \
                          $$PWD/qhttpserver/qhttpserver.cpp \
                          $$PWD/qhttpserver/http-parser/http_parser.c

QT                     += sql
