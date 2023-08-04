#pragma once
#ifndef POINTCLOUDSAVEDATAEXECUTORMESSAGES_H
#define POINTCLOUDSAVEDATAEXECUTORMESSAGES_H

#include <QEvent>
#include <QDateTime>
#include <QString>
#include <beam_aux.h>

class pointCloudSaveDataExecutorSaveBinaryDataRequest : public QEvent{
public:
    pointCloudSaveDataExecutorSaveBinaryDataRequest(const tPointCloudUdp* pointcloud_data, const QString &file_name) : QEvent((QEvent::Type)(QEvent::registerEventType())){

        int size_to_copy = sizeof(int32_t)*pointcloud_data->size;

        m_point_cloud = (tPointCloudUdp*)malloc(sizeof(tPointCloudUdp));
        m_point_cloud->size = pointcloud_data->size;
        m_point_cloud->data_buffer = (int32_t*)malloc(size_to_copy);
        memcpy(m_point_cloud->data_buffer, pointcloud_data->data_buffer, size_to_copy);

        m_file_name = file_name;
    }

    static const QEvent::Type TYPE;
    tPointCloudUdp* getDataToSave(){return m_point_cloud;}
    QString getFileName(){return m_file_name;}

    void releaseMemory(){
        free(m_point_cloud->data_buffer);
        free(m_point_cloud);
    }
private:
    tPointCloudUdp* m_point_cloud;
    QString m_file_name;
};

class pointCloudSaveDataExecutorSaveBinaryBufferDataRequest : public QEvent{
public:
    pointCloudSaveDataExecutorSaveBinaryBufferDataRequest(int32_t *pointcloud_buffer, QString filename) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        int buff_size = sizeof(int32_t) * ((pointcloud_buffer[0] * 5) + 1);
        m_pointcloud_buffer = (int32_t*) malloc(buff_size);

        memcpy(m_pointcloud_buffer, pointcloud_buffer, buff_size);
        m_file_name = filename;
    }
    static const QEvent::Type TYPE;
    QString getFileName(){return m_file_name;}
    int32_t *getPointcloud(){return m_pointcloud_buffer;}
    void releaseMemory(){free(m_pointcloud_buffer);}
private:
    int32_t *m_pointcloud_buffer;
    QString m_file_name;
};

class pointCloudSaveDataExecutorSaveBinaryDataResponse : public QEvent{
public:
    pointCloudSaveDataExecutorSaveBinaryDataResponse(const QString &file_name) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_file_name = file_name;
    }
    static const QEvent::Type TYPE;
    QString getFileName(){return m_file_name;}
private:
    QString m_file_name;
};

#endif // pointCloudSaveDATAEXECUTORMESSAGES_H
