QT -= gui
QT += network

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG += c++11
CONFIG -= app_bundle console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/Libs/spdlog/include

SOURCES += \
        Config/softconfig.cpp \
        Server/udpserver.cpp \
        Log/logger.cpp \
        main.cpp \
        mainserver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Common/common.h \
    Config/softconfig.h \
    Server/udpserver.h \
    Log/logger.h \
    mainserver.h

LIBS += -L$$PWD/Libs/spdlog/lib \
-lspdlog

LIBS +=-luser32 -ladvapi32

# LOGO
RC_ICONS = logo.ico
