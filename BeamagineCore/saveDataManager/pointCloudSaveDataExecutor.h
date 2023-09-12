#ifndef POINTCLOUDSAVEDATAEXECUTOR_H
#define POINTCLOUDSAVEDATAEXECUTOR_H

#include <QDateTime>
#include <QDir>
#include <QDebug>

#include <QObject>
#include <QThread>
#include <QMap>
#include <QCoreApplication>
#include <QTimer>

#ifdef _WIN32

#else
#include <unistd.h>
#endif

#include "pointCloudSaveDataExecutorMessages.h"


class pointCloudSaveDataExecutor : public QObject
{
    Q_OBJECT
public:
    explicit pointCloudSaveDataExecutor(QObject *parent = 0);

    //! @brief  Starts the thread
    //! @param  none
    //! @return none
    void startController();

    //! @brief  Stops the thread
    //! @param  none
    //! @return none
    void stopController();

    void setEventHandler(const QEvent::Type type, const QObject* event_handler);

    //! @brief  Sets path to save pcl data
    //! @param  path Path to save pcl data
    //! @return none
    void setPathToSavePcd(const QString &path);

    void doSaveBinaryData(int32_t *pointcloud_buffer, QString file_name);

    void doSaveBinaryData(tPointCloudUdp *pointcloud, QString file_name);

private:
    //! @brief  Receives events to process requests
    //! @param  event even to process
    //! @return none
    void customEvent(QEvent *event);

    void doCheckTimeout();

    void sendEvent(QEvent::Type event_type, QEvent *event );

    QList<const QObject *> getEventHandlers(const QEvent::Type& event_type);

    void onSaveBinaryDataRequest(pointCloudSaveDataExecutorSaveBinaryDataRequest *request);

    void onSaveBinaryDataRequest(pointCloudSaveDataExecutorSaveBinaryBufferDataRequest *request);

    void saveBinaryData(tPointCloudUdp *pointcloud, QString file_name);

    void saveBinaryData(int32_t *pointcloud_buffer, QString file_name);

    void sendSaveBinaryDataResponse();

signals:

    void executorIsAvailable(bool available);


public slots:

    void run();

    void availableTimeOut();

private:

    QThread *m_controller_thread;

    QMultiMap <QEvent::Type, const QObject * > m_event_handlers;

    QString m_path_to_save_pcd;
    QString m_full_file_name;

    QTimer *m_available_timer;
    int m_available_time_ms;

    bool m_is_available;
};

#endif // POINTCLOUDSAVEDATAEXECUTOR_H
