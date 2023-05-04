/*  Copyright (c) 2023, Beamagine
 *
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

        - Redistributions of source code must retain the above copyright notice,
          this list of conditions and the following disclaimer.
        - Redistributions in binary form must reproduce the above copyright notice,
          this list of conditions and the following disclaimer in the documentation and/or
          other materials provided with the distribution.
        - Neither the name of copyright holders nor the names of its contributors may be
          used to endorse or promote products derived from this software without specific
          prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
    COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
    EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "pclPointCloudViewerController.h"

#include "QDateTime"
#include "QString"
#include <QDebug>
#include <QElapsedTimer>

pcl::visualization::CloudViewer viewer ("Lidar Viewer");

#include <boost/numeric/ublas/matrix.hpp>
#include <vtkTransform.h>

bool point_cloud_ready = false;
bool first_frame_received = false;
int parameter = 0;
int initial_detection = 0;

pcl::PointCloud<pcl::PointXYZRGB>::Ptr data_to_show = pcl::PointCloud<pcl::PointXYZRGB>::Ptr(new pcl::PointCloud<pcl::PointXYZRGB> );

QVector<int> m_intensities;

double red_global = 0;
double green_global = 0;
double blue_global = 0;

bool enableAxis = true;
bool firstTime = true;

int global_point_size = 1;

bool point_selected = false;
QString selected_data = "";

void pointPickingEventOccurred (const pcl::visualization::PointPickingEvent& event, void* viewer_void)
{
    Q_UNUSED(viewer_void);
    try{
        point_selected = true;
        float x, y, z;
        int index = event.getPointIndex ();
        if ( index == -1)
        {
            return;
        }
        event.getPoint(x, y, z);

        QString selected = "";
        selected = QString("Point coordinate (%1,%2,%3)").arg(x).arg(y).arg(z);

        if(index < m_intensities.size()){
            selected += QString(" - Intensity %1").arg(m_intensities.at(index));
        }

        qDebug()<<selected;
        selected_data = selected;
    }catch(...){
        qDebug()<<"pointPickingEventOccurred Unhandled error";
    }
}

void keyboardEventOccurred(const pcl::visualization::KeyboardEvent &event, void *viewer_void){
    Q_UNUSED(viewer_void);

    if(event.getKeyCode() == '+' && event.keyDown()){
        if(global_point_size < 10){
            ++global_point_size;
        }
    }
    else if(event.getKeyCode() == '-' && event.keyDown()){
        if(global_point_size > 1){
            --global_point_size;
        }
    }
}

void viewerOneOff(pcl::visualization::PCLVisualizer& viewer){

    viewer.createViewPort(0.0, 0.0, 1.0, 1.0, parameter);
    viewer.setBackgroundColor(0, 0, 0, 1);
    viewer.initCameraParameters();
    viewer.setWindowBorders (0);
    viewer.setCameraPosition(0,0,-1500,0,-1,0);
    viewer.registerPointPickingCallback (pointPickingEventOccurred, (void*)&viewer);
    viewer.registerKeyboardCallback(keyboardEventOccurred, (void*)&viewer);

}

void centerCameraCallBack(pcl::visualization::PCLVisualizer& viewer){
    viewer.setCameraPosition(0,0,-1500,0,-1,0);
}

void updatePointCloud(pcl::visualization::PCLVisualizer& viewer){
    try{
        if(point_cloud_ready){

            point_cloud_ready = false;

            if(!first_frame_received){

                if(data_to_show->points.size() > 0){

                    viewer.addPointCloud<pcl::PointXYZRGB>(data_to_show, "Lidar Viewer");
                    first_frame_received = true;
                }

            }else{

                if(data_to_show->points.size() > 0){
                    viewer.removePointCloud("Lidar Viewer");
                    viewer.addPointCloud<pcl::PointXYZRGB>(data_to_show, "Lidar Viewer");
                    viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, global_point_size, "Lidar Viewer");
                }
            }
        }
    }
    catch(pcl::IOException& ex ){
        qDebug()<<"Error atupdatePointCloud "<<ex.what();
    }
    catch(pcl::UnhandledPointTypeException& ex){
        qDebug()<<"Error atupdatePointCloud "<<ex.what();
    }
    catch(pcl::BadArgumentException& ex){
        qDebug()<<"Error atupdatePointCloud "<<ex.what();
    }
    catch(pcl::InitFailedException& ex){
        qDebug()<<"Error atupdatePointCloud "<<ex.what();
    }
    catch(pcl::UnorganizedPointCloudException& ex){
        qDebug()<<"Error atupdatePointCloud "<<ex.what();
    }
    catch(...){
        qDebug()<<"Unhandled error at updatePointCloud";
    }

}

void changeBackgroundColorCallback(pcl::visualization::PCLVisualizer& viewer){
    try{
        viewer.setBackgroundColor(red_global, green_global, blue_global, 1);
    }
    catch(pcl::IOException& ex ){
        qDebug()<<"Error changeBackgroundColorCallback "<<ex.what();
    }
    catch(pcl::UnhandledPointTypeException& ex){
        qDebug()<<"Error changeBackgroundColorCallback "<<ex.what();
    }
    catch(pcl::BadArgumentException& ex){
        qDebug()<<"Error changeBackgroundColorCallback "<<ex.what();
    }
    catch(pcl::InitFailedException& ex){
        qDebug()<<"Error changeBackgroundColorCallback "<<ex.what();
    }
    catch(pcl::UnorganizedPointCloudException& ex){
        qDebug()<<"Error changeBackgroundColorCallback "<<ex.what();
    }
    catch(...){
        qDebug()<<"Unhandled error at changeBackgroundColorCallback";
    }
}

void enableAxisCallback(pcl::visualization::PCLVisualizer& viewer){
    try{
        if(enableAxis){
            viewer.addCoordinateSystem(2000.0);
        }else{
            viewer.removeCoordinateSystem();
        }
    }
    catch(pcl::IOException& ex ){
        qDebug()<<"Error enableAxisCallback "<<ex.what();
    }
    catch(pcl::UnhandledPointTypeException& ex){
        qDebug()<<"Error enableAxisCallback "<<ex.what();
    }
    catch(pcl::BadArgumentException& ex){
        qDebug()<<"Error enableAxisCallback "<<ex.what();
    }
    catch(pcl::InitFailedException& ex){
        qDebug()<<"Error enableAxisCallback "<<ex.what();
    }
    catch(pcl::UnorganizedPointCloudException& ex){
        qDebug()<<"Error enableAxisCallback "<<ex.what();
    }
    catch(...){
        qDebug()<<"Unhandled error at enableAxisCallback";
    }
}

pclPointCloudViewerController::pclPointCloudViewerController(QObject *parent) : QObject(parent)
{
    m_event_handlers.clear();

    m_controller_thread = new QThread();
    m_controller_thread->setObjectName("PointCloudViewerController");

    connect(m_controller_thread, SIGNAL(started()),this, SLOT(run()));

    m_color_type_to_show = visualizationTypes::RAINBOW;

}

void pclPointCloudViewerController::customEvent(QEvent *event)
{

    if(pclPointCloudViewerControllerShowUdpPointCloud *p_event = dynamic_cast<pclPointCloudViewerControllerShowUdpPointCloud*>(event)){
        onShowUpdPointCloudRequest(p_event);
    }

}

void pclPointCloudViewerController::doShowPointCloud(int32_t *pointcloud)
{
    pclPointCloudViewerControllerShowUdpPointCloud *request = new pclPointCloudViewerControllerShowUdpPointCloud(pointcloud, pointcloud[0]);
    QCoreApplication::postEvent(this, request);
}


void pclPointCloudViewerController::doChangeBackgroundColor(const double &red, const double &green, const double &blue)
{
    try{
        red_global = red;
        green_global = green;
        blue_global = blue;

        viewer.runOnVisualizationThreadOnce(changeBackgroundColorCallback);
    }
    catch(pcl::IOException& ex ){
        qDebug()<<"Error at simplePointCloudViewer::changeBackgroundColor"<<ex.what();
    }
    catch(pcl::UnhandledPointTypeException& ex){
        qDebug()<<"Error at simplePointCloudViewer::changeBackgroundColor"<<ex.what();
    }
    catch(pcl::BadArgumentException& ex){
        qDebug()<<"Error at simplePointCloudViewer::changeBackgroundColor "<<ex.what();
    }
    catch(pcl::InitFailedException& ex){
        qDebug()<<"Error at simplePointCloudViewer::changeBackgroundColor "<<ex.what();
    }
    catch(pcl::UnorganizedPointCloudException& ex){
        qDebug()<<"Error at simplePointCloudViewer::changeBackgroundColor "<<ex.what();
    }
    catch(...){
        qDebug()<<"Unhandled error at simplePointCloudViewer::changeBackgroundColor";
    }
}

void pclPointCloudViewerController::doCenterCamera(){
    viewer.runOnVisualizationThreadOnce(centerCameraCallBack);
}

void pclPointCloudViewerController::setColorType(int pcd_type)
{
    m_color_type_to_show = pcd_type;

}

void pclPointCloudViewerController::startController()
{
    try{
        if(!m_controller_thread->isRunning()){
            moveToThread(m_controller_thread);
            m_controller_thread->start();
        }
    }
    catch(...){
        qDebug()<<"Unhandled error a t pclPointCloudViewerController::startController";
    }
}

void pclPointCloudViewerController::stopController()
{

    try{
        if(m_controller_thread->isRunning()){
            m_controller_thread->exit(0);
        }
        m_event_handlers.clear();

    }catch(...){
        qDebug()<<"Unhandled error at pclPointCloudViewerController::stopController";
    }

}

void pclPointCloudViewerController::setEventHandler(const QEvent::Type type, const QObject *event_handler)
{
    m_event_handlers.insert(type, event_handler);
}


void pclPointCloudViewerController::setAxisEnabled(const bool &enable){
    enableAxis = enable;
    viewer.runOnVisualizationThreadOnce(enableAxisCallback);
}

void pclPointCloudViewerController::sendEvent(QEvent::Type event_type, QEvent *event)
{
    QList<const QObject *> list = getEventHandlers(event_type);
    for (int i = 0; i < list.size(); ++i){
        QCoreApplication::postEvent((QObject*)list.at(i), event);
    }
}

QList<const QObject *> pclPointCloudViewerController::getEventHandlers(const QEvent::Type &event_type)
{
    return m_event_handlers.values(event_type);
}

void pclPointCloudViewerController::onShowUpdPointCloudRequest(pclPointCloudViewerControllerShowUdpPointCloud *request)
{
    try{
        int32_t *point_cloud = (int32_t*)malloc((request->getBufferSize()*sizeof(int32_t)*5)+1);
        memcpy(point_cloud, request->getPointCloud(), (5*sizeof(int32_t)*request->getBufferSize())+1);

        if(request->getBufferSize() > 0){

            showUdpPointCloud(point_cloud, request->getBufferSize());

            pcl::copyPointCloud(*m_color_point_cloud, *data_to_show);

            point_cloud_ready = true;
        }

        free(point_cloud);

    }catch(...){
        qDebug()<<"pclPointcloudViewerController::onShowUdpPointCloudRequest Unhandled error";
    }
    request->releaseMemory();
}

void pclPointCloudViewerController::showUdpPointCloud(int32_t *point_cloud, uint32_t buff_size)
{
    try{

        m_color_point_cloud->clear();
        m_intensities.clear();

        int j = 1;

        pcl::PointXYZRGB curr_point;
        pcl::PointXYZI point_i;

        for(uint32_t i=0;i<buff_size; ++i){


            curr_point.x = point_cloud[j];
            curr_point.y = point_cloud[j+1];
            curr_point.z = point_cloud[j+2];


            m_intensities.push_back(point_cloud[j+3]);

            int32_t val = point_cloud[j+4];

            curr_point.rgb = *reinterpret_cast<float*>(&val);
            j+=5;

            m_color_point_cloud->points.push_back(curr_point);
        }

    }
    catch(pcl::IOException& ex ){
        qDebug()<<"Error at simplePointCloudViewer::showUdpPointCloud"<<ex.what();
    }
    catch(pcl::UnhandledPointTypeException& ex){
        qDebug()<<"Error at simplePointCloudViewer::showUdpPointCloud"<<ex.what();
    }
    catch(pcl::BadArgumentException& ex){
        qDebug()<<"Error at simplePointCloudViewer::showUdpPointCloud"<<ex.what();
    }
    catch(pcl::InitFailedException& ex){
        qDebug()<<"Error at simplePointCloudViewer::showUdpPointCloud"<<ex.what();
    }
    catch(pcl::UnorganizedPointCloudException& ex){
        qDebug()<<"Error at simplePointCloudViewer::showUdpPointCloud"<<ex.what();
    }
    catch(...){
        qDebug()<<"Unhandled error at simplePointCloudViewer::showUdpPointCloud";
    }

}

void pclPointCloudViewerController::run()
{
    m_color_point_cloud = pcl::PointCloud<pcl::PointXYZRGB>::Ptr(new pcl::PointCloud<pcl::PointXYZRGB>);

    viewer.runOnVisualizationThreadOnce(viewerOneOff);
    viewer.runOnVisualizationThread(updatePointCloud);

    //!loop where data selection gets detected
    m_message_timer = new QTimer();
    connect(m_message_timer, SIGNAL(timeout()), this, SLOT(timerTimeOut()));
    m_message_timer->start(50);

}

void pclPointCloudViewerController::timerTimeOut()
{
    m_message_timer->stop();
    if(point_selected){
        emit sendPointSelectedData(selected_data);
        point_selected = false;
        selected_data = "";
    }
    m_message_timer->start(50);

}
