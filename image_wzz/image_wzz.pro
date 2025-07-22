#-------------------------------------------------
#
# Project created by QtCreator 2025-07-18T15:21:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = image_wzz
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
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui


INCLUDEPATH+=F:\coderuanjian\opencv\opencv-build\install\include
LIBS+=F:\coderuanjian\opencv\opencv-build\lib\libopencv_*.a
#INCLUDEPATH += F:/coderuanjian/opencv/build/include \
               F:/coderuanjian/opencv/build/include/opencv2 # 这个路径通常也需要

#LIBS += -LF:/coderuanjian/opencv/opencv-build/install/x64/mingw/lib

win32:CONFIG(release, debug|release): LIBS += \
    -lopencv_calib3d401.dll \
    -lopencv_core401.dll \
    -lopencv_dnn401.dll \
    -lopencv_features2d401.dll \
    -lopencv_flann401.dll \
    -lopencv_highgui401.dll \
    -lopencv_imgcodecs401.dll \
    -lopencv_imgproc401.dll \
    -lopencv_ml401.dll \
    -lopencv_objdetect401.dll \
    -lopencv_photo401.dll \
    -lopencv_stitching401.dll \
    -lopencv_video401.dll \
    -lopencv_videoio401.dll
INCLUDEPATH += F:/coderuanjian/opencv/build/include \
               F:/coderuanjian/opencv/build/include/opencv2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
