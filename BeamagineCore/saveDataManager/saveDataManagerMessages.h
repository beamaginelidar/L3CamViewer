#ifndef SAVEDATAMANAGERMESSAGES_H
#define SAVEDATAMANAGERMESSAGES_H

#include <QEvent>

class saveDataManagerSavePointerToPngRequest : public QEvent{
public:
    saveDataManagerSavePointerToPngRequest(uint8_t *image_pointer, uint16_t width, uint16_t height, uint8_t channels, uint32_t time_stamp) : QEvent((QEvent::Type)(QEvent::registerEventType())){

        int size_to_copy = height*width*channels*sizeof(uint8_t);
        m_image_buffer = (uint8_t*)malloc(size_to_copy);
        memcpy(m_image_buffer, image_pointer, size_to_copy);

        m_width = width;
        m_height = height;
        m_channels = channels;
        m_time_stamp = time_stamp;
    }

    static const QEvent::Type TYPE;
    uint8_t* getImagePointer(){return m_image_buffer;}
    uint16_t getImageWidth(){return m_width;}
    uint16_t getImageHeight(){return m_height;}
    uint8_t  getImageChannels(){return m_channels;}
    uint32_t  getTimeStamp(){return m_time_stamp;}
    void releaseMemory(){free(m_image_buffer);}

private:
    uint8_t *m_image_buffer;
    uint16_t m_width;
    uint16_t m_height;
    uint8_t m_channels;
    uint32_t m_time_stamp;
};

class saveDataManagerSavePointCloudToBinRequest : public QEvent{
public:
    saveDataManagerSavePointCloudToBinRequest(int32_t *point_cloud_buffer, int32_t pointcloud_size, uint32_t time_stamp) : QEvent((QEvent::Type)(QEvent::registerEventType())){

        Q_UNUSED(pointcloud_size)
 
        int size_to_copy = ((point_cloud_buffer[0] * 5) + 1)*sizeof(int32_t);
        m_point_cloud_buffer = (int32_t*)malloc(size_to_copy);
        memcpy(m_point_cloud_buffer, point_cloud_buffer, size_to_copy);

        m_size = size_to_copy;
        m_time_stamp = time_stamp;
    }

    static const QEvent::Type TYPE;
    int32_t *getPointCloudBuffer(){return m_point_cloud_buffer;}
    uint32_t getTimeStamp(){return m_time_stamp;}
    int32_t getNumberPoints(){return m_size;}
    void releaseMemory(){free(m_point_cloud_buffer);}

private:
    int32_t *m_point_cloud_buffer;
    int32_t m_size;
    uint32_t m_time_stamp;
};
#endif // SAVEDATAMANAGERMESSAGES_H
