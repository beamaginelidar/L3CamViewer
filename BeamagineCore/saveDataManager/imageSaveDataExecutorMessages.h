#ifndef IMAGESAVEDATAEXECUTORMESSAGES_H
#define IMAGESAVEDATAEXECUTORMESSAGES_H

#include <QEvent>
#include <QDateTime>
#include <QString>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

class imageSaveDataExecutorSaveDataPngRequest : public QEvent{
public:
    imageSaveDataExecutorSaveDataPngRequest(uint8_t *image_data_pointer, const int &image_type, const QDateTime &time_stamp, const int &image_size): QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_image_data_pointer = (uint8_t*)malloc(image_size);
        memcpy(m_image_data_pointer, image_data_pointer, image_size);
        m_image_type = image_type;
        m_time_stamp = time_stamp;
        m_image_size = image_size;
    }
    static const QEvent::Type TYPE;
    uint8_t* getImageDataPointer(){return m_image_data_pointer;}
    int getImageType(){return m_image_type;}
    int getImageSize(){return m_image_size;}
    QDateTime getTimeStamp(){return m_time_stamp;}
    void freeMemory(){
        free(m_image_data_pointer);
    }
private:
    uint8_t *m_image_data_pointer;
    QDateTime m_time_stamp;
    int m_image_type;
    int m_image_size;
};

class imageSaveDataExecutorSaveDataPngRgbRequest: public QEvent{
public:
    imageSaveDataExecutorSaveDataPngRgbRequest(uint8_t *image_data_pointer, const int &image_type, const QDateTime &time_stamp, const int &image_size): QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_image_data_pointer = (uint8_t*)malloc(image_size);
        memcpy(m_image_data_pointer, image_data_pointer, image_size);
        m_image_type = image_type;
        m_time_stamp = time_stamp;

    }
    static const QEvent::Type TYPE;
    uint8_t* getImageDataPointer(){return m_image_data_pointer;}
    int getImageType(){return m_image_type;}
    QDateTime getTimeStamp(){return m_time_stamp;}

    void freeMemory(){
        free(m_image_data_pointer);
    }
private:
    uint8_t *m_image_data_pointer;
    QDateTime m_time_stamp;
    int m_image_type;
};

class imageSaveDataExecutorSaveMultiDataPngRequest : public QEvent{
public:
    imageSaveDataExecutorSaveMultiDataPngRequest(uint8_t *image_0_pointer, uint8_t *image_45_pointer, uint8_t *image_90_pointer, uint8_t *image_135_pointer, const QDateTime &time_stamp, const int &image_size): QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_image_0_pointer = (uint8_t*)malloc(image_size);
        m_image_45_pointer = (uint8_t*)malloc(image_size);
        m_image_90_pointer = (uint8_t*)malloc(image_size);
        m_image_135_pointer = (uint8_t*)malloc(image_size);
        memcpy(m_image_0_pointer, image_0_pointer, image_size);
        memcpy(m_image_45_pointer, image_45_pointer, image_size);
        memcpy(m_image_90_pointer, image_90_pointer, image_size);
        memcpy(m_image_135_pointer, image_135_pointer, image_size);
        m_time_stamp = time_stamp;
    }
    static const QEvent::Type TYPE;
    uint8_t* getImage0DataPointer(){return m_image_0_pointer;}
    uint8_t* getImage45DataPointer(){return m_image_45_pointer;}
    uint8_t* getImage90DataPointer(){return m_image_90_pointer;}
    uint8_t* getImage135DataPointer(){return m_image_135_pointer;}

    QDateTime getTimeStamp(){return m_time_stamp;}
    void freeMemory(){
        free(m_image_0_pointer);
        free(m_image_45_pointer);
        free(m_image_90_pointer);
        free(m_image_135_pointer);
    }
private:
    uint8_t *m_image_0_pointer;
    uint8_t *m_image_45_pointer;
    uint8_t *m_image_90_pointer;
    uint8_t *m_image_135_pointer;
    QDateTime m_time_stamp;
};


class imageSaveDataExecutorSavePointerToPngRequest : public QEvent{
public:
    imageSaveDataExecutorSavePointerToPngRequest(uint8_t *image_pointer, uint16_t width, uint16_t height, uint8_t channels, QString file_name) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        int size_to_copy = height*width*channels*sizeof(uint8_t);
        m_image_buffer = (uint8_t*)malloc(size_to_copy);
        memcpy(m_image_buffer, image_pointer, size_to_copy);

        m_width = width;
        m_height = height;
        m_channels = channels;
        m_file_name = file_name;
    }

    static const QEvent::Type TYPE;
    uint8_t* getImagePointer(){return m_image_buffer;}
    uint16_t getImageWidth(){return m_width;}
    uint16_t getImageHeight(){return m_height;}
    uint8_t  getImageChannels(){return m_channels;}
    QString  getImageName(){return m_file_name;}
    void releaseMemory(){free(m_image_buffer);}

private:
    uint8_t *m_image_buffer;
    uint16_t m_width;
    uint16_t m_height;
    uint8_t m_channels;
    QString m_file_name;
};

class imageSaveDataExecutorSavePointerToPngResponse : public QEvent{
public:
    imageSaveDataExecutorSavePointerToPngResponse(QString file_path) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_file_path = file_path;
    }
    static const QEvent::Type TYPE;
    QString  getFilePath(){return m_file_path;}

private:
    QString m_file_path;
};

class imageSaveDataExecutorSaveDataPngStokesRequest : public QEvent {
public:

    imageSaveDataExecutorSaveDataPngStokesRequest(uint8_t* image_s1, uint8_t* image_s2, const int &image_size, const QDateTime &time_stamp) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_image_s1 = (uint8_t*)malloc(image_size);
        m_image_s2 = (uint8_t*)malloc(image_size);
        memcpy(m_image_s1, image_s1, image_size);
        memcpy(m_image_s2, image_s2, image_size);
        m_date_time = time_stamp;
    }
    static const QEvent::Type TYPE;
    uint8_t* getImageS1(){return m_image_s1;}
    uint8_t* getImageS2(){return m_image_s2;}
    QDateTime getTimeStamp(){return m_date_time;}
    void releaseMemory(){
        free(m_image_s1);
        free(m_image_s2);
    }
private:
    uint8_t *m_image_s1;
    uint8_t *m_image_s2;
    QDateTime m_date_time;

};

class imageSaveDataExecutorSaveMatImage : public QEvent {
public:

    imageSaveDataExecutorSaveMatImage(cv::Mat image, const QDateTime &time_stamp) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_image = image.clone();
        m_date_time = time_stamp;
    }
    static const QEvent::Type TYPE;
    cv::Mat getImage(){return m_image;}
    QDateTime getTimeStamp(){return m_date_time;}

private:
    cv::Mat m_image;
    QDateTime m_date_time;

};

class imageSaveDataExecutorSaveMatImageString : public QEvent {
public:

    imageSaveDataExecutorSaveMatImageString(cv::Mat image, const QDateTime &time_stamp, QString camStr) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_image = image.clone();
        m_date_time = time_stamp;
        m_camStr = camStr;
    }
    static const QEvent::Type TYPE;
    cv::Mat getImage(){return m_image;}
    QDateTime getTimeStamp(){return m_date_time;}
    QString getCamStr(){return m_camStr;}

private:
    cv::Mat m_image;
    QDateTime m_date_time;
    QString m_camStr;

};

#endif // IMAGESAVEDATAEXECUTORMESSAGES_H
