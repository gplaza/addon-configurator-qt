HEADERS += configurator.h \
           qhttpserver/qhttpconnection.h \
           qhttpserver/qhttprequest.h \
           qhttpserver/qhttpresponse.h \
           qhttpserver/qhttpserver.h \
           qhttpserver/qhttpserverapi.h \
           qhttpserver/qhttpserverfwd.h \
           qhttpserver/http-parser/http_parser.h

SOURCES += configurator.cpp \
           qhttpserver/qhttpconnection.cpp \
           qhttpserver/qhttprequest.cpp \
           qhttpserver/qhttpresponse.cpp \
           qhttpserver/qhttpserver.cpp \
           qhttpserver/http-parser/http_parser.c

OTHER_FILES += configurator.pri
