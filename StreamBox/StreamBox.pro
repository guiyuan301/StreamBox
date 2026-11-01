#-------------------------------------------------
#
# Project created by QtCreator 2026-04-06T13:14:16
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StreamBox
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    VIEW/loginwindow.cpp \
    VIEW/mainwindow.cpp \
    VIEW/custom_widgets.cpp \
    VIEW/splashscreen.cpp \
    DBdate/dbsqlite3.cpp \
    MODEL/usermodel.cpp \
    DBdate/cleanup.cpp \
    CONTROLLER/usercontroller.cpp \
    MODEL/videomodel.cpp \
    CONTROLLER/videocontroller.cpp \
    VIEW/videocardwidget.cpp \
    VIEW/playwindow.cpp \
    Video/audiodecode.cpp \
    Video/videodecode.cpp \
    Video/livevideoaudio.cpp

HEADERS += \
    VIEW/loginwindow.h \
    VIEW/mainwindow.h \
    VIEW/custom_widgets.h \
    VIEW/splashscreen.h \
    DBdate/dbsqlite3.h \
    MODEL/usermodel.h \
    DBdate/cleanup.h \
    CONTROLLER/usercontroller.h \
    MODEL/videomodel.h \
    CONTROLLER/videocontroller.h \
    VIEW/videocardwidget.h \
    VIEW/playwindow.h \
    Video/audiodecode.h \
    Video/videodecode.h \
    Video/livevideoaudio.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pic.qrc

RC_ICONS += pic/StreamBox.ico
DESTDIR = $$PWD/bin
LIBS += $$PWD/lib/sqlite3.lib #引用依赖 这个不是QT原身自带的库
INCLUDEPATH += ./ffmpeglib/include
LIBS += $$PWD/ffmpeglib/lib/avcodec.lib  \
        $$PWD/ffmpeglib/lib/avdevice.lib  \
        $$PWD/ffmpeglib/lib/avfilter.lib  \
        $$PWD/ffmpeglib/lib/avformat.lib  \
        $$PWD/ffmpeglib/lib/avutil.lib  \
        $$PWD/ffmpeglib/lib/swresample.lib  \
        $$PWD/ffmpeglib/lib/swscale.lib
