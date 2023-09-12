#ifndef IMAGESAVEDATAEXECUTOR_H
#define IMAGESAVEDATAEXECUTOR_H

#include <QDateTime>
#include <QDir>
#include <QDebug>

#include <QObject>
#include <QThread>
#include <QMap>
#include <QCoreApplication>
#include <QTimer>

#include "imageSaveDataExecutorMessages.h"

#ifdef _WIN32
#define PIXEL_FORMAT BGR8
#else
#define PIXEL_FORMAT RGB8
#include "unistd.h"
#endif

class imageSaveDataExecutor : public QObject
{
    Q_OBJECT
public:
    explicit imageSaveDataExecutor(QObject *parent = 0);

    //! @brief  Starts the thread
    //! @param  none
    //! @return none
    void startController();

    //! @brief  Stops the thread
    //! @param  none
    //! @return none
    void stopController();

    void setEventHandler(const QEvent::Type type, const QObject* event_handler);

    //! @brief  Saves single png image
    //! @param  image_data_pointer Pointer with image to save
    //! @param  image_type Type of image to save
    //! @param  time_stamp Time stamp to add to the image name
    //! @return none
    void doSaveDataPng(uint8_t* image_data_pointer, const int &image_type, const QDateTime &time_stamp);

    //! @brief  Saves single png image with string in the name
    //! @param  image_data_pointer Pointer with image to save
    //! @param  image_type Type of image to save
    //! @param  camStr String to append to name
    //! @param  time_stamp Time stamp to add to the image name
    //! @return none
    void doSaveDataPng(cv::Mat image_to_save, QDateTime time_stamp, QString camStr);

    //! @brief  Saves multiple png image
    //! @param  image_0_pointer Pointer with image to save
    //! @param  image_45_pointer Pointer with image to save
    //! @param  image_90_pointer Pointer with image to save
    //! @param  image_135_pointer Pointer with image to save
    //! @param  time_stamp Time stamp to add to the image name
    //! @return none
    void doSaveDataPng(uint8_t *image_0_pointer, uint8_t *image_45_pointer, uint8_t *image_90_pointer, uint8_t *image_135_pointer, const QDateTime &time_stamp);

    //! @brief  Saves single png RGB image
    //! @param  image_data_pointer Pointer with image to save
    //! @param  image_type Type of image to save
    //! @param  time_stamp Time stamp to add to the image name
    //! @return none
    void doSaveDataPngRgb(uint8_t *image_data_pointer, const int &image_type, const QDateTime &time_stamp);

    //! @brief  Saves stokes images
    //! @param  image_s1 Pointer with image data
    //! @param  image_S2 Pointer with image data
    //! @param  time_stamp time stamp to add to filename
    //! @return none
    void doSaveDataPngStokes(uint8_t *image_s1, uint8_t *image_s2, const QDateTime &time_stamp);

    //! @brief  Sets path to save png images
    //! @param  path Path to save images
    //! @return none
    void setPathToSaveImages(const QString &path);

    //! @brief  Sets image size
    //! @param  height Height of the image
    //! @param  width Width of the image
    //! @return none
    void setImageSize(const int &height, const int &width);

    void doSaveDataPng(cv::Mat image_to_save, QDateTime time_stamp);

    void doSaveLossLess(bool lossless);

    void doSavePointerToPng(uint8_t* image_pointer, uint16_t width, uint16_t height, uint8_t channels, QString file_name);

signals:

    void executorIsAvailable(bool available);

public slots:

    void run();

    void availableTimeOut();

private:
    //! @brief  Receives events to process requests
    //! @param  event even to process
    //! @return none
    void customEvent(QEvent *event);

    void doCheckTimeout();

    void sendEvent(QEvent::Type event_type, QEvent *event );

    QList<const QObject *> getEventHandlers(const QEvent::Type& event_type);

    //! @brief  Process save data png request
    //! @param  request Message of the request
    //! @return none
    void onSaveDataPng(imageSaveDataExecutorSaveDataPngRequest *request);

    //! @brief  Process save data multi images png request
    //! @param  request Message of the request
    //! @return none
    void onSaveDataPng(imageSaveDataExecutorSaveMultiDataPngRequest *request);

    //! @brief  Process save data png rgb request
    //! @param  request Message of the request
    //! @return none
    void onSaveDataPng(imageSaveDataExecutorSaveDataPngRgbRequest *request);

    //! @brief  Saves image png
    //! @param  image_data Pointer with image data to save
    //! @param  image_type type of image to save (raw, aolp, dolp)
    //! @param  time_stamp Timestamp to the image filename
    //! @return integer with result
    int saveDataPng(uint8_t *image_data, const int &image_type, const QDateTime &time_stamp);

    //! @brief  Saves multiple images png
    //! @param  image_0_data Pointer with image data to save
    //! @param  image_45_data Pointer with image data to save
    //! @param  image_90_data Pointer with image data to save
    //! @param  image_135_data Pointer with image data to save
    //! @param  time_stamp Timestamp to the image filename
    //! @return integer with result
    int saveDataPng(uint8_t *image_0_data, uint8_t *image_45_data, uint8_t *image_90_data, uint8_t *image_135_data, const QDateTime &time_stamp);

    //! @brief  Saves image png rgb
    //! @param  image_data Pointer with image data to save
    //! @param  image_type type of image to save (raw, aolp, dolp)
    //! @param  time_stamp Timestamp to the image filename
    //! @return integer with result
    int saveDataPngRgb(uint8_t* image_data, const int &image_type, const QDateTime &time_stamp);

    //! @brief  Process save stokes images request
    //! @param  request Message of the request
    //! @return none
    void onSaveStokesImages(imageSaveDataExecutorSaveDataPngStokesRequest *request);

    //! @brief  Saves stokes images
    //! @param  image_s2_data Pointer with S1 image data
    //! @param  image_S2_data Pointer with S2 image data
    //! @param  time_stamp Time stamp for the name of the files
    //! @return integer with result
    int saveStokesImages(uint8_t *image_s1_data, uint8_t *image_s2_data, const QDateTime &time_stamp);

    void onSaveImage(imageSaveDataExecutorSaveMatImage *request);

    void onSaveImage(imageSaveDataExecutorSaveMatImageString *request);

    void onSavePointerToPng(imageSaveDataExecutorSavePointerToPngRequest *request);

    void convertYuv2Rgb(uint8_t *src_pointer, uint8_t* dst_pointer, uint32_t buff_size);

    void savePointerToPng(uint8_t *image_pointer, uint16_t width, uint16_t height, uint8_t channels, QString file_name);

    void sendSavePointerToPngResponse();

    

private:

    QThread *m_controller_thread;

    QMultiMap <QEvent::Type, const QObject * > m_event_handlers;

    QTimer *m_available_timer;
    int m_available_time_ms;

    QString m_path_to_save_images;
    QString m_full_path_saved;

    int m_height;
    int m_width;

    bool m_save_lossless;
    bool m_is_available;

};

#endif // IMAGESAVEDATAEXECUTOR_H
