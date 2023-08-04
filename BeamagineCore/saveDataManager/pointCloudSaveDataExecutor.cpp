#include "pointCloudSaveDataExecutor.h"

#include "fstream"

pointCloudSaveDataExecutor::pointCloudSaveDataExecutor(QObject *parent) : QObject(parent)
{
    m_path_to_save_pcd = QDir::homePath() + "/LiDAR/";
    m_is_available = true;

    m_event_handlers.clear();

    m_controller_thread = new QThread();
    m_controller_thread->setObjectName("PointCloudSaveDataExecutor");

    connect(m_controller_thread, SIGNAL(started()),this, SLOT(run()));
}

void pointCloudSaveDataExecutor::startController()
{
    try{
        if(!m_controller_thread->isRunning()){
            moveToThread(m_controller_thread);
            m_controller_thread->start();
        }
    }
    catch(...){
        qDebug()<<"Unhandled error at pointCloudSaveDataExecutor::startController";
    }
}

void pointCloudSaveDataExecutor::stopController()
{
    try{
        if(m_controller_thread->isRunning()){
            m_controller_thread->exit(0);
        }
        m_event_handlers.clear();

    }catch(...){
        qDebug()<<"Unhandled error at pointCloudSaveDataExecutor::stopController";
    }
}

void pointCloudSaveDataExecutor::setEventHandler(const QEvent::Type type, const QObject *event_handler)
{
    m_event_handlers.insert(type, event_handler);

}

void pointCloudSaveDataExecutor::setPathToSavePcd(const QString &path)
{
    m_path_to_save_pcd = path;
}

void pointCloudSaveDataExecutor::doSaveBinaryData(int32_t *pointcloud_buffer, QString file_name){
    m_is_available = false;

    int buff_size = ((pointcloud_buffer[0] * 5) + 1) * sizeof(int32_t);
    int32_t *tmp_buff = (int32_t*)malloc(buff_size);

    memcpy(tmp_buff, pointcloud_buffer, buff_size);

    pointCloudSaveDataExecutorSaveBinaryBufferDataRequest *request = new pointCloudSaveDataExecutorSaveBinaryBufferDataRequest(tmp_buff, file_name);

    QCoreApplication::postEvent(this, request);
    free(tmp_buff);
}

void pointCloudSaveDataExecutor::doSaveBinaryData(tPointCloudUdp *pointcloud, QString file_name){

    m_is_available = false;

    int size_to_copy = sizeof(int32_t)*pointcloud->size;

    tPointCloudUdp *point_cloud_to_save = (tPointCloudUdp*)malloc(sizeof(tPointCloudUdp));
    point_cloud_to_save->size = pointcloud->size;
    point_cloud_to_save->data_buffer = (int32_t*)malloc(size_to_copy);

    memcpy(point_cloud_to_save->data_buffer, pointcloud->data_buffer, size_to_copy);

    pointCloudSaveDataExecutorSaveBinaryDataRequest *request = new pointCloudSaveDataExecutorSaveBinaryDataRequest(point_cloud_to_save, file_name);

    QCoreApplication::postEvent(this, request);

    free(point_cloud_to_save->data_buffer);
    free(point_cloud_to_save);
}


void pointCloudSaveDataExecutor::customEvent(QEvent *event)
{

    //----Pointcloud data
    if(pointCloudSaveDataExecutorSaveBinaryDataRequest *p_event = dynamic_cast<pointCloudSaveDataExecutorSaveBinaryDataRequest*>(event)){
        onSaveBinaryDataRequest(p_event);
    }
    else if(pointCloudSaveDataExecutorSaveBinaryBufferDataRequest *p_event = dynamic_cast<pointCloudSaveDataExecutorSaveBinaryBufferDataRequest*>(event)){
        onSaveBinaryDataRequest(p_event);
    }

}


void pointCloudSaveDataExecutor::doCheckTimeout()
{
    //!do nothing
}

void pointCloudSaveDataExecutor::sendEvent(QEvent::Type event_type, QEvent *event)
{
    QList<const QObject *> list = getEventHandlers(event_type);
    for (int i = 0; i < list.size(); ++i){
        QCoreApplication::postEvent((QObject*)list.at(i), event);
    }
}

QList<const QObject *> pointCloudSaveDataExecutor::getEventHandlers(const QEvent::Type &event_type)
{
    return m_event_handlers.values(event_type);
}

void pointCloudSaveDataExecutor::onSaveBinaryDataRequest(pointCloudSaveDataExecutorSaveBinaryDataRequest *request)
{
    int size_to_copy = sizeof(int32_t)*request->getDataToSave()->size;

    tPointCloudUdp *point_cloud_to_save = (tPointCloudUdp*)malloc(sizeof(tPointCloudUdp));

    point_cloud_to_save->size = request->getDataToSave()->size;
    point_cloud_to_save->data_buffer = (int32_t*)malloc(size_to_copy);

    memcpy(point_cloud_to_save->data_buffer, request->getDataToSave()->data_buffer, size_to_copy);

    saveBinaryData(point_cloud_to_save, request->getFileName());

    free(point_cloud_to_save->data_buffer);
    free(point_cloud_to_save);
    request->releaseMemory();

    sendSaveBinaryDataResponse();
}

void pointCloudSaveDataExecutor::onSaveBinaryDataRequest(pointCloudSaveDataExecutorSaveBinaryBufferDataRequest *request)
{
    int size_to_copy = (( request->getPointcloud()[0] * 5) +1 )*sizeof(int32_t);
    int32_t *pointcloud_buffer = (int32_t*)malloc(size_to_copy);

    memcpy(pointcloud_buffer, request->getPointcloud(), size_to_copy);

    saveBinaryData(pointcloud_buffer, request->getFileName());

    free(pointcloud_buffer);
    request->releaseMemory();
}

void pointCloudSaveDataExecutor::saveBinaryData(tPointCloudUdp *pointcloud, QString file_name)
{
    int32_t data_size = pointcloud->size/5;

    m_full_file_name = m_path_to_save_pcd + file_name  + ".bin";

    FILE *file_handler = std::fopen(m_full_file_name.toStdString().c_str(), "wb");

    //!write datasize
    std::fwrite(&data_size, sizeof(int32_t), 1, file_handler);
    //!write points
    std::fwrite(pointcloud->data_buffer, (sizeof(int32_t)*pointcloud->size), 1, file_handler);

    std::fclose(file_handler);

    m_is_available = true;
}

void pointCloudSaveDataExecutor::saveBinaryData(int32_t *pointcloud_buffer, QString file_name)
{
    int32_t data_size = (( pointcloud_buffer[0] * 5) +1 )*sizeof(int32_t);

    m_full_file_name = m_path_to_save_pcd + file_name + ".bin";

    FILE *file_handler = std::fopen(m_full_file_name.toStdString().c_str(), "wb");

    //!write all data
    std::fwrite(pointcloud_buffer, data_size, 1, file_handler);

    std::fclose(file_handler);

    m_is_available = true;

}

void pointCloudSaveDataExecutor::sendSaveBinaryDataResponse()
{
    pointCloudSaveDataExecutorSaveBinaryDataResponse *response = new pointCloudSaveDataExecutorSaveBinaryDataResponse(m_full_file_name);
    sendEvent(response->TYPE, response);
}

void pointCloudSaveDataExecutor::run()
{
    m_available_timer = new QTimer();
    m_available_time_ms = 10;
    m_is_available = true;
    connect(m_available_timer, SIGNAL(timeout()), this, SLOT(availableTimeOut()));
    m_available_timer->start(m_available_time_ms);
}

void pointCloudSaveDataExecutor::availableTimeOut()
{
    m_available_timer->stop();

    if(m_is_available){
        emit executorIsAvailable(m_is_available);
    }

    m_available_timer->start(m_available_time_ms);
}
