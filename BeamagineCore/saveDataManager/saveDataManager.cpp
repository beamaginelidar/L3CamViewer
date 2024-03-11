#include "saveDataManager.h"


saveDataManager::saveDataManager(QObject *parent): QObject(parent)
{
    m_path_to_save_data = QDir::homePath() + "/data/";
    m_max_frames_to_save = 1000;
    m_current_frame_number = 0;
    m_is_executor_available = false;
    m_sender_timer_time_ms = 3;

    m_event_handlers.clear();

    m_images_queue.clear();
    m_pointcloud_queue.clear();
    m_float_binary_queue.clear();

    m_controller_thread = new QThread();
    m_controller_thread->setObjectName("saveDataManager");

    connect(m_controller_thread, SIGNAL(started()),this, SLOT(run()));
}

void saveDataManager::startController()
{
    try{
        if(!m_controller_thread->isRunning()){
            moveToThread(m_controller_thread);
            m_controller_thread->start();
        }
    }
    catch(...){
        qDebug()<<"Unhandled error at saveDataManager::startController";
    }
}

void saveDataManager::stopController()
{
    try{
        if(m_controller_thread->isRunning()){
            m_controller_thread->exit(0);
        }
        m_event_handlers.clear();

    }catch(...){
        qDebug()<<"Unhandled error at saveDataManager::stopController";
    }
}

void saveDataManager::setEventHandler(const QEvent::Type type, const QObject *event_handler)
{
    m_event_handlers.insert(type, event_handler);

}

void saveDataManager::setPathToSaveData(const QString &path)
{
    m_path_to_save_data = path;
}

void saveDataManager::setMaxFramesToSave(uint8_t frames)
{
    m_max_frames_to_save = frames;
}

void saveDataManager::doSavePointerToPng(uint8_t *image_pointer, uint16_t width, uint16_t height, uint8_t channels, uint32_t time_stamp)
{
    int buff_size = width*height*channels;
    uint8_t *image_buffer = (uint8_t*)malloc(buff_size);
    memcpy(image_buffer, image_pointer, buff_size);

    saveDataManagerSavePointerToPngRequest *request = new saveDataManagerSavePointerToPngRequest(image_buffer, width, height, channels, time_stamp);

    QCoreApplication::postEvent(this, request);
    free(image_buffer);
}

void saveDataManager::doSavePointCloudToBin(int32_t *data_buffer, int32_t number_of_points, uint32_t time_stamp)
{
    int buff_size = ((data_buffer[0] * 5) + 1) * sizeof(int32_t);
    int32_t *pointcloud_buffer = (int32_t*)malloc(buff_size);
    memcpy(pointcloud_buffer, data_buffer, buff_size);

    saveDataManagerSavePointCloudToBinRequest *request = new saveDataManagerSavePointCloudToBinRequest(pointcloud_buffer, number_of_points, time_stamp);

    QCoreApplication::postEvent(this, request);
    free(pointcloud_buffer);
}

void saveDataManager::doSaveFloatDataToBin(float *data_buffer, int buffer_size, uint32_t time_stamp)
{
    float *buffer = (float*)malloc(buffer_size);
    memcpy(buffer, data_buffer, buffer_size);

    saveDataManagerSaveFloatBufferRequest *request = new saveDataManagerSaveFloatBufferRequest(data_buffer, buffer_size, time_stamp);

    QCoreApplication::postEvent(this, request);
    free(buffer);
}

void saveDataManager::setDataTypeToSave(uint8_t data_type)
{
    m_data_type = data_type;
}

void saveDataManager::setExecutorAvailable(bool available)
{
    m_is_executor_available = available;
}

void saveDataManager::run()
{
    m_check_sender_timer = new QTimer();
    connect(m_check_sender_timer, SIGNAL(timeout()), this, SLOT(checkSenderTimeout()));
    m_check_sender_timer->start(m_sender_timer_time_ms);
}

void saveDataManager::checkSenderTimeout()
{
    m_check_sender_timer->stop();

    if(m_current_frame_number > 0 && m_is_executor_available){

        switch(m_data_type){
        case images:
            emit sendImageToSave(m_images_queue.dequeue());
            --m_current_frame_number;
            m_is_executor_available = false;
            break;
        case pointcloud:
            emit sendPointCloudToSave(m_pointcloud_queue.dequeue());
            --m_current_frame_number;
            m_is_executor_available = false;
            break;
        case binaryFloat:
            emit sendBufferToSave(m_float_binary_queue.dequeue());
            --m_current_frame_number;
            m_is_executor_available = false;
            break;
        }
    }

    m_check_sender_timer->start(m_sender_timer_time_ms);
}

void saveDataManager::customEvent(QEvent *event)
{

    if(saveDataManagerSavePointerToPngRequest *p_event = dynamic_cast<saveDataManagerSavePointerToPngRequest*>(event)){
        onSavePointerToPng(p_event);
    }
    else if(saveDataManagerSavePointCloudToBinRequest* p_event = dynamic_cast<saveDataManagerSavePointCloudToBinRequest*>(event)){
        onSavePointCloudToBin(p_event);
    }
    else if(saveDataManagerSaveFloatBufferRequest *p_event = dynamic_cast<saveDataManagerSaveFloatBufferRequest*>(event)){
        onSaveFloatBuffer(p_event);
    }

}


void saveDataManager::doCheckTimeout()
{
    //!do nothing
}

void saveDataManager::sendEvent(QEvent::Type event_type, QEvent *event)
{
    QList<const QObject *> list = getEventHandlers(event_type);
    for (int i = 0; i < list.size(); ++i){
        QCoreApplication::postEvent((QObject*)list.at(i), event);
    }
}

QList<const QObject *> saveDataManager::getEventHandlers(const QEvent::Type &event_type)
{
    return m_event_handlers.values(event_type);
}

void saveDataManager::onSavePointerToPng(saveDataManagerSavePointerToPngRequest *request)
{
    savePointerToPng(request->getImagePointer(), request->getImageWidth(), request->getImageHeight(), request->getImageChannels(), request->getTimeStamp());

    request->releaseMemory();
}

void saveDataManager::onSavePointCloudToBin(saveDataManagerSavePointCloudToBinRequest *request)
{
    savePointCloudToBin(request->getPointCloudBuffer(), request->getNumberPoints(), request->getTimeStamp());

    request->releaseMemory();
}

void saveDataManager::onSaveFloatBuffer(saveDataManagerSaveFloatBufferRequest *request)
{
    saveFloatBuffer(request->getFloatBuffer(), request->getBufferSize(), request->getTimeStamp());

    request->releaseMemory();
}


void saveDataManager::savePointCloudToBin(int32_t *data_buffer, int32_t number_of_points, uint32_t time_stamp)
{
    Q_UNUSED(number_of_points)
    if(m_current_frame_number < m_max_frames_to_save){

        int size = ((data_buffer[0] * 5) + 1) * sizeof(int32_t);
        pointcloudData data;

        data.pointcloud_buffer = (int32_t*)malloc(size);
        memcpy(data.pointcloud_buffer, data_buffer, size);

        data.timestamp = time_stamp;

        m_pointcloud_queue.enqueue(data);
        m_current_frame_number++;

    }else{
        qDebug()<<"Point Cloud Buffer full";

    }
}

void saveDataManager::savePointerToPng(uint8_t *image_pointer, uint16_t width, uint16_t height, uint8_t channels, uint32_t time_stamp){

    if(m_current_frame_number < m_max_frames_to_save){

        int buff_size = width*height*channels;
        imageData data;

        data.image_buffer = (uint8_t*)malloc(buff_size);
        memcpy(data.image_buffer, image_pointer, buff_size);

        data.timestamp = time_stamp;
        data.image_channels = channels;
        data.image_height = height;
        data.image_width = width;


        m_images_queue.enqueue(data);
        m_current_frame_number++;

    }else{
        qDebug()<<"Image Buffer full"<<width;
    }
}

void saveDataManager::saveFloatBuffer(float *buffer, int buffer_size, uint32_t time_stamp)
{
    if(m_current_frame_number < m_max_frames_to_save){

        binaryFloatData data;

        data.data_buffer = (float*)malloc(buffer_size);
        memcpy(data.data_buffer, buffer, buffer_size);

        data.data_size = buffer_size;
        data.timestamp = time_stamp;

        m_float_binary_queue.enqueue(data);
        m_current_frame_number++;
    }else{
        qDebug()<<"Binary float buffer is full";
    }
}

