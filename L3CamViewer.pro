#-------------------------------------------------
#
# Project created by QtCreator 2022-01-02T18:55:29
#
#-------------------------------------------------
unix{
QMAKE_CXXFLAGS += -std=gnu++14
}

CONFIG += c++14
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
        BeamagineCore/saveDataManager/imageSaveDataExecutor.cpp \
        BeamagineCore/saveDataManager/imageSaveDataExecutorMessages.cpp \
        BeamagineCore/saveDataManager/pointCloudSaveDataExecutor.cpp \
        BeamagineCore/saveDataManager/pointCloudSaveDataExecutorMessages.cpp \
        BeamagineCore/saveDataManager/saveDataManager.cpp \
        BeamagineCore/saveDataManager/saveDataManagerMessages.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        BeamagineCore/pclPointCloudViewer/pclPointCloudViewerController.h \
        BeamagineCore/pclPointCloudViewer/pclPointCloudViewerControllerMessages.h \
        BeamagineCore/udpReceiverController/udpReceiverControllerMessages.h \
        BeamagineCore/udpReceiverController/udpreceivercontroller.h \
        BeamagineCore/saveDataManager/imageSaveDataExecutor.h \
        BeamagineCore/saveDataManager/imageSaveDataExecutorMessages.h \
        BeamagineCore/saveDataManager/pointCloudSaveDataExecutor.h \
        BeamagineCore/saveDataManager/pointCloudSaveDataExecutorMessages.h \
        BeamagineCore/saveDataManager/saveDataManager.h \
        BeamagineCore/saveDataManager/saveDataManagerMessages.h \
        BeamagineCore/saveDataManager/saveDataStructs.h \
        BeamagineCore/beam_aux.h \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += \
        libs/libL3Cam/ \
        BeamagineCore/udpReceiverController/ \
        BeamagineCore/pclPointCloudViewer/ \
        BeamagineCore/saveDataManager/ \
        BeamagineCore/


# TODO: Modify INCLUDEPATH to match your environment

unix {

#OPENCV
INCLUDEPATH += $(HOME)/openCV4.5.5/lib/include/opencv4/ \

LIBS += -L$$PWD/libs/opencv4/ \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_imgcodecs \
    -lopencv_dnn

#PCL
INCLUDEPATH += $(HOME)/pcl-1.9.0/libs/include/pcl-1.9 \
               /usr/include/eigen3/ \
               /usr/include/vtk-7.1/


LIBS += -L$$PWD/libs/pcl-1.9.0 \
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

#LibL3cam
LIBS += -L$$PWD/libs/libL3Cam/ -lL3Cam

INCLUDEPATH += $$PWD/libs/libL3Cam
DEPENDPATH += $$PWD/libs/libL3Cam

PRE_TARGETDEPS += $$PWD/libs/libL3Cam/libL3Cam.a
}

win32{
#OPENCV
INCLUDEPATH += 'D:/OpenCV/opencv/build/include/'

LIBS += -L"$$PWD/libs/opencv4/" -lopencv_world440

#PCL
INCLUDEPATH += 'C:/Program Files/PCL 1.9.1/include/pcl-1.9/' \
               'C:/Program Files/PCL 1.9.1/3rdParty/Eigen/eigen3' \
               'C:/Program Files/PCL 1.9.1/3rdParty/VTK/include/vtk-8.1' \
               'C:/Program Files/PCL 1.9.1/3rdParty/Boost/include/boost-1_68' \
               'C:/Program Files/PCL 1.9.1/3rdParty/FLANN/include/'

LIBS += -L"$$PWD/libs/pcl-1.9.1/3rdParty/boost/" -llibboost_system-vc141-mt-x64-1_68 \
        -llibboost_filesystem-vc141-mt-x64-1_68 \
        -llibboost_thread-vc141-mt-x64-1_68 \
        -llibboost_date_time-vc141-mt-x64-1_68 \
        -llibboost_iostreams-vc141-mt-x64-1_68 \
        -llibboost_serialization-vc141-mt-x64-1_68 \
        -llibboost_chrono-vc141-mt-x64-1_68 \
        -llibboost_atomic-vc141-mt-x64-1_68 \
        -llibboost_regex-vc141-mt-x64-1_68

LIBS += -L"$$PWD/libs/pcl-1.9.1/3rdParty/qhull/" -lqhullstatic

LIBS += -L"$$PWD/libs/pcl-1.9.1/3rdParty/openni/" -lOpenNI2

LIBS += -L"$$PWD/libs/pcl-1.9.1/3rdParty/flann/" -lflann_cpp_s

LIBS += -L"$$PWD/libs/pcl-1.9.1/3rdParty/vtk/" -lvtkCommonCore-8.1 \
        -lvtkRenderingCore-8.1 \
        -lvtkCommonDataModel-8.1 \
        -lvtkCommonMath-8.1 \
        -lvtkRenderingLOD-8.1 \
        -lvtkCommonExecutionModel-8.1 \
        -lvtkRenderingFreeType-8.1 \
        -lvtksys-8.1 \
        -lvtkCommonTransforms-8.1 \
        -lvtkCommonColor-8.1 \
        -lvtkCommonSystem-8.1 \
        -lvtkCommonMisc-8.1

LIBS += -L"$$PWD/libs/pcl-1.9.1/pcl-1.9/" -lpcl_visualization_release \
        -lpcl_common_release


#LibL3cam
LIBS += -L$$PWD/libs/libL3Cam/ -lL3Cam

INCLUDEPATH += $$PWD/libs/libL3Cam
DEPENDPATH += $$PWD/libs/libL3Cam

PRE_TARGETDEPS += $$PWD/libs/libL3Cam/L3Cam.lib

LIBS += -L$$PWD/libs/libL3Cam/win_dependencies/ -liphlpapi
DEPENDPATH += $$PWD/libs/libL3Cam/win_dependencies
PRE_TARGETDEPS += $$PWD/libs/libL3Cam/win_dependencies/iphlpapi.lib


LIBS += -lopengl32 \
        -lglu32 \
        -lwsock32 \
        -lws2_32 \
        -luser32 \
        -lgdi32
}

qmx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $(HOME)/l3cam
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

