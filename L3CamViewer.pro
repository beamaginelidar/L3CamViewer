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
        imageviewerform.cpp \
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
        imageviewerform.h \
        mainwindow.h

FORMS += \
        imageviewerform.ui \
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
INCLUDEPATH += $(HOME)/openCV4.5.5/install/include/opencv4/ \

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
    -lpcl_io \
    -lpcl_visualization \
    -lpcl_kdtree \
    -lpcl_io \
    -lpcl_io_ply

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
PCL_BASE_PATH = "D:/PCL_1.13.1/PCL 1.13.1/"

INCLUDEPATH += $$PCL_BASE_PATH"/include/pcl-1.13/" \
               $$PCL_BASE_PATH"/3rdParty/Eigen/eigen3" \
               $$PCL_BASE_PATH"/3rdParty/VTK/include/vtk-9.2" \
               $$PCL_BASE_PATH"/3rdParty/Boost/include/boost-1_82" \
               $$PCL_BASE_PATH"/3rdParty/FLANN/include/"

LIBS += -L"$$PWD/libs/pcl-1.13.1/Windows/3rdParty/boost/" -llibboost_atomic-vc143-mt-x64-1_82\
        -llibboost_chrono-vc143-mt-x64-1_82 \
        -llibboost_date_time-vc143-mt-x64-1_82 \
        -llibboost_filesystem-vc143-mt-x64-1_82 \
        -llibboost_iostreams-vc143-mt-x64-1_82 \
        -llibboost_regex-vc143-mt-x64-1_82 \
        -llibboost_serialization-vc143-mt-x64-1_82 \
        -llibboost_system-vc143-mt-x64-1_82 \
        -llibboost_thread-vc143-mt-x64-1_82

LIBS += -L"$$PWD/libs/pcl-1.13.1/Windows/3rdParty/qhull/" -lqhullstatic

LIBS += -L"$$PWD/libs/pcl-1.13.1/Windows/3rdParty/openni/" -lOpenNI2

LIBS += -L"$$PWD/libs/pcl-1.13.1/Windows/3rdParty/flann/" -lflann_cpp_s

LIBS += -L"$$PWD/libs/pcl-1.13.1/Windows/3rdParty/vtk/"   -lvtkCommonColor-9.2 \
        -lvtkCommonCore-9.2 \
        -lvtkCommonDataModel-9.2 \
        -lvtkCommonExecutionModel-9.2 \
        -lvtkCommonMath-9.2 \
        -lvtkCommonMisc-9.2 \
        -lvtkCommonSystem-9.2 \
        -lvtkCommonTransforms-9.2 \
        -lvtkRenderingCore-9.2 \
        -lvtkRenderingFreeType-9.2 \
        -lvtkRenderingLOD-9.2 \
        -lvtksys-9.2 \


LIBS += -L"$$PWD/libs/pcl-1.13.1/Windows/pcl-1.13/"  -lpcl_common \
        -lpcl_visualization



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

