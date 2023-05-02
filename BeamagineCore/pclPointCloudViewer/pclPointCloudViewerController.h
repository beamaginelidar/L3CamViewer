#pragma once

#ifndef PCLPOINTCLOUDVIEWERCONTROLLER_H
#define PCLPOINTCLOUDVIEWERCONTROLLER_H

#include <QThread>
#include <QMap>
#include <QCoreApplication>
#include <QTimer>
#include <QQueue>

//segmentation
//#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

//#include <pcl/common/centroid.h>

#include <pcl/point_cloud.h>
#include <pcl/common/common.h>

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/cloud_viewer.h>
#include <vtkRenderWindow.h>

#include "pclPointCloudViewerControllerMessages.h"

//#include "boost/math/special_functions/round.hpp"
#include "math.h"

#include <Eigen/Geometry>

#include <beam_aux.h>

class pclPointCloudViewerController : public QObject
{
    Q_OBJECT
public:
    explicit pclPointCloudViewerController(QObject *parent = 0);

    //! @brief
    //! @param
    //! @return
    void customEvent(QEvent *event);

    void doShowPointCloud(int32_t *pointcloud);

    //! @brief  Changes visualization window background color
    //! @param  red Red value of backgroud color in range 0-1
    //! @param  green Green value of backgroud color in range 0-1
    //! @param  blue Blue value of backgroud color in range 0-1
    //! @return none
    void doChangeBackgroundColor(const double &red, const double &green, const double &blue);

    //! @brief  Starts the thread
    //! @param  none
    //! @return none
    void startController();

    //! @brief  Stops the thread
    //! @param  none
    //! @return none
    void stopController();

    //! @brief  Enables or disables axis representation
    //! @param  enable If true shows axis else hides axis
    //! @return none
    void setAxisEnabled(const bool &enable);

    void setEventHandler(const QEvent::Type type, const QObject* event_handler);

    void doCenterCamera();

    void setColorType(int pcd_type);

signals:

    void sendPointSelectedData(QString data);

public slots:

    void run();

    void timerTimeOut();

private:

    void onShowUpdPointCloudRequest(pclPointCloudViewerControllerShowUdpPointCloud *request);

    void showUdpPointCloud(int32_t *point_cloud, uint32_t buff_size);


protected:

    void sendEvent(QEvent::Type event_type, QEvent *event );

    QList<const QObject *> getEventHandlers(const QEvent::Type& event_type);


protected:

    QThread *m_controller_thread;

    QMultiMap <QEvent::Type, const QObject * > m_event_handlers;

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr m_color_point_cloud;


public:

private:

    int m_color_type_to_show;

    QTimer *m_message_timer;

    bool m_send_message_enabled;

};

#endif // pclPointCloudViewerController_H
