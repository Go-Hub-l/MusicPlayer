QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lrc.cpp \
    main.cpp \
    mvwidget.cpp \
    registerwidget.cpp \
    widget.cpp \
    ./Net/TCPNet.cpp \
    ./Kernel/TCPKernel.cpp \
    ./videoplayer/videoplayer.cpp

include (./sqlapi/sqlapi.pri)

INCLUDEPATH += ./sqlapi\
        ./Net\
        ./Kernel\
        ./videoplayer



HEADERS += \
    lrc.h \
    mvwidget.h \
    registerwidget.h \
    widget.h \
    ./Net/INet.h \
    ./Net/TCPNet.h\
    ./Kernel/IKernel.h\
    ./Kernel/TCPKernel.h \
    packdef.h\
    ./videoplayer/videoplayer.h

win32{
INCLUDEPATH += $$PWD/SDL2-2.0.16/include
INCLUDEPATH += $$PWD/ffmpeg-4.2.1-win32-dev/include
LIBS += $$PWD/ffmpeg-4.2.1-win32-dev/lib/avformat.lib \
        $$PWD/ffmpeg-4.2.1-win32-dev/lib/avcodec.lib \
        $$PWD/ffmpeg-4.2.1-win32-dev/lib/avdevice.lib \
        $$PWD/ffmpeg-4.2.1-win32-dev/lib/avfilter.lib \
        $$PWD/ffmpeg-4.2.1-win32-dev/lib/avutil.lib \
        $$PWD/ffmpeg-4.2.1-win32-dev/lib/postproc.lib \
        $$PWD/ffmpeg-4.2.1-win32-dev/lib/swresample.lib \
        $$PWD/ffmpeg-4.2.1-win32-dev/lib/swscale.lib \
        $$PWD/SDL2-2.0.16/lib/x86/SDL2.lib
}

FORMS += \
    lrc.ui \
    mvwidget.ui \
    registerwidget.ui \
    widget.ui

QT	+= core gui
QT	+= multimedia

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc

#网络库
LIBS += -lws2_32
