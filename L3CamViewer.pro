#-------------------------------------------------
#
# Project created by QtCreator 2022-01-02T18:55:29
#
#-------------------------------------------------
CONFIG += c++14
QMAKE_CXXFLAGS += -std=gnu++14

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = L3CamViewer
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


SOURCES += \
        BeamagineCore/pclPointCloudViewer/pclPointCloudViewerController.cpp \
        BeamagineCore/pclPointCloudViewer/pclPointCloudViewerControllerMessages.cpp \
        BeamagineCore/udpReceiverController/udpReceiverControllerMessages.cpp \
        BeamagineCore/udpReceiverController/udpreceivercontroller.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        BeamagineCore/pclPointCloudViewer/pclPointCloudViewerController.h \
        BeamagineCore/pclPointCloudViewer/pclPointCloudViewerControllerMessages.h \
        BeamagineCore/udpReceiverController/udpReceiverControllerMessages.h \
        BeamagineCore/udpReceiverController/udpreceivercontroller.h \
        BeamagineCore/beam_aux.h \
        mainwindow.h


FORMS += \
        mainwindow.ui

INCLUDEPATH += \
        libs/libL3Cam/ \
        BeamagineCore/udpReceiverController/ \
        BeamagineCore/pclPointCloudViewer/ \
        BeamagineCore/

INCLUDEPATH += /usr/local/include/opencv4/ \


LIBS += -Llibs/opencv4/ \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_imgcodecs


INCLUDEPATH += /home/beamagine/pcl-1.9.0/libs/include/pcl-1.9 \
               /usr/include/eigen3/ \
               /usr/include/vtk-7.1/


LIBS += -Llibs/pcl-1.9.0 \
    -lpcl_common \
    -lpcl_features \
    -lpcl_filters \
    -lpcl_io \
    -lpcl_visualization

LIBS += -lboost_system \
    -lvtkCommonCore-7.1 \
    -lvtkRenderingCore-7.1 \
    -lvtkCommonDataModel-7.1 \
    -lvtkCommonMath-7.1 \
    -lvtkRenderingLOD-7.1 \
    -lvtkCommonExecutionModel-7.1 \
    -lvtkRenderingFreeType-7.1


qmx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/beamagine/l3cam
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

unix:!macx: LIBS += -L$$PWD/libs/libL3Cam/ -lL3Cam

INCLUDEPATH += $$PWD/libs/libL3Cam
DEPENDPATH += $$PWD/libs/libL3Cam

unix:!macx: PRE_TARGETDEPS += $$PWD/libs/libL3Cam/libL3Cam.a
