#ifndef SAVEDATAMANAGER_H
#define SAVEDATAMANAGER_H

#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QQueue>

#include <QObject>
#include <QThread>
#include <QMap>
#include <QCoreApplication>
#include <QTimer>

#include "saveDataManagerMessages.h"
#include "saveDataStructs.h"

#ifdef _WIN32

#else
#include <unistd.h>
#endif


class saveDataManager : public QObject
{
    Q_OBJECT
public:
    explicit saveDataManager(QObject *parent = 0);

    //! @brief  Starts the thread
    //! @param  none
    //! @return none
    void startController();

    //! @brief  Stops the thread
    //! @param  none
    //! @return none
    void stopController();

    void setEventHandler(const QEvent::Type type, const QObject* event_handler);

    //! @brief  Sets path to save png images
    //! @param  path Path to save images
    //! @return none
    void setPathToSaveData(const QString &path);

    void setMaxFramesToSave(uint8_t frames);

    void doSavePointerToPng(uint8_t* image_pointer, uint16_t width, uint16_t height, uint8_t channels, uint32_t time_stamp);

    void doSavePointCloudToBin(int32_t *data_buffer, int32_t number_of_points, uint32_t time_stamp);

    void setDataTypeToSave(uint8_t data_type);

    void setExecutorAvailable(bool available);

signals:

    void sendPointCloudToSave(pointcloudData data);

    void sendImageToSave(imageData data);

public slots:

    void run();

    void checkSenderTimeout();

private:
    //! @brief  Receives events to process requests
    //! @param  event even to process
    //! @return none
    void customEvent(QEvent *event);

    void doCheckTimeout();

    void sendEvent(QEvent::Type event_type, QEvent *event );

    QList<const QObject *> getEventHandlers(const QEvent::Type& event_type);

    void onSavePointerToPng( saveDataManagerSavePointerToPngRequest *request);

    void savePointerToPng(uint8_t *image_pointer, uint16_t width, uint16_t height, uint8_t channels, uint32_t time_stamp);

    void onSavePointCloudToBin( saveDataManagerSavePointCloudToBinRequest *request);

    void savePointCloudToBin(int32_t *data_buffer, int32_t number_of_points, uint32_t time_stamp);

private:

    QThread *m_controller_thread;

    QMultiMap <QEvent::Type, const QObject * > m_event_handlers;


    QQueue<imageData> m_images_queue;
    QQueue<pointcloudData> m_pointcloud_queue;

    QTimer *m_check_sender_timer;
    int m_sender_timer_time_ms;


    QString m_path_to_save_data;

    uint16_t m_max_frames_to_save;
    uint8_t m_current_frame_number;
    uint8_t m_data_type;

    bool m_is_executor_available;

};

#endif // SAVEDATAMANAGER_H
