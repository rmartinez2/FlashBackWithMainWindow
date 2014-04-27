#-------------------------------------------------
#
# Project created by QtCreator 2014-04-23T22:57:30
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

TARGET = FlashBack
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    videothread.cpp \
    timeitem.cpp \
    slidervisual.cpp \
    sidemenu.cpp \
    seekerbarticker.cpp \
    seekerbar.cpp \
    recordings.cpp \
    recordingoptions.cpp \
    notificationwidget.cpp \
    logodetectionthread.cpp \
    cvmatviewer.cpp \
    cutratedetectionthread.cpp \
    channelselectionview.cpp \
    bsdetectionthread.cpp

HEADERS  += mainwindow.h \
    videothread.h \
    timeitem.h \
    slidervisual.h \
    sidemenu.h \
    seekerbarticker.h \
    seekerbar.h \
    recordings.h \
    recordingoptions.h \
    notificationwidget.h \
    logodetectionthread.h \
    cvmatviewer.h \
    cutratedetectionthread.h \
    channelselectionview.h \
    bsdetectionthread.h

FORMS    += mainwindow.ui \
    widget.ui

QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS

INCLUDEPATH += C:\\openCV_build_debug\\install\\include

LIBS += -LC:\\openCV_build_debug\\install\\lib\
    -llibopencv_core246 \
    -llibopencv_highgui246 \
    -llibopencv_imgproc246 \
    -llibopencv_calib3d246 \
    -llibopencv_contrib246 \
    -llibopencv_gpu246 \
    -llibopencv_ml246 \
    -llibopencv_objdetect \
    -llibopencv_video246 \
    -llibopencv_videostab246 \
    -llibopencv_flann246 \
    -llibopencv_features2d246 \
    -llibopencv_legacy246 \
    -llibopencv_nonfree246 \
    -llibopencv_photo246 \
    -llibopencv_stitching246 \
    -llibopencv_superres246


INCLUDEPATH += C:\\Users\\Rene\\Downloads\\ffmpeg-20140110-git-d9481dc-win32-dev\\ffmpeg-20140110-git-d9481dc-win32-dev\\include

LIBS += -LC:\\Users\\Rene\\Downloads\\ffmpeg-20140110-git-d9481dc-win32-dev\\ffmpeg-20140110-git-d9481dc-win32-dev\\lib \
    -lavcodec \
    -lavformat \
    -lavutil \
    -lswresample \
    -lpostproc \
    -lswscale \
    -lavdevice \
    -lavfilter




OTHER_FILES += \
    avutil-52.dll \
    avformat-55.dll \
    avfilter-4.dll \
    avdevice-55.dll \
    avcodec-55.dll \
