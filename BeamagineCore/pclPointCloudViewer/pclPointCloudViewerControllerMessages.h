#pragma once

#ifndef POINTCLOUDVIEWERCONTROLLERMESSAGES
#define POINTCLOUDVIEWERCONTROLLERMESSAGES
#include <QEvent>
#include <QString>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/file_io.h>


class pclPointCloudViewerControllerShowUdpPointCloud : public QEvent{
  public:
    pclPointCloudViewerControllerShowUdpPointCloud(int32_t *pointcloud, int32_t buff_size) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_point_cloud = (int32_t*)malloc((buff_size*sizeof(int32_t)*5)+1);
        memcpy(m_point_cloud, pointcloud, (sizeof(int32_t)*buff_size*5)+1);
        m_buff_size = buff_size;
    }
    static const QEvent::Type TYPE;
    int32_t *getPointCloud(){return m_point_cloud;}
    int32_t getBufferSize(){return m_buff_size;}
    void releaseMemory(){free(m_point_cloud);}
private:
    int32_t m_buff_size;
    int32_t *m_point_cloud;
};

class pclPointCloudViewerControllerPointSelectedNotification : public QEvent{
public:
    pclPointCloudViewerControllerPointSelectedNotification(QString point_selected) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_point_selected = point_selected;
    }
    static const QEvent::Type TYPE;
    QString getPointSelected(){return m_point_selected;}
private:
    QString m_point_selected;
};

#endif // POINTCLOUDVIEWERCONTROLLERMESSAGES
