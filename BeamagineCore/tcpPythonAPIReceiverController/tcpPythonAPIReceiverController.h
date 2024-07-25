#pragma once
#ifndef tcpPythonAPIReceiverCONTROLLER_H
#define tcpPythonAPIReceiverCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QCoreApplication>
#include <QString>

#ifdef _WIN32
#include <winsock2.h>
#include <iptypes.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <tcpPythonAPIReceiverControllerMessages.h>

class tcpPythonAPIReceiverController : public QObject
{
    Q_OBJECT
public:
    explicit tcpPythonAPIReceiverController(QObject *parent = 0);

    //! @brief  Retruns instance of the object
    //! @param  none
    //! @return Instance of this class
    static tcpPythonAPIReceiverController* Instance();

    ~tcpPythonAPIReceiverController();

    void setPort(uint16_t port);

    void setAddress(QString server_address);

    void initializeServer();

    QString processRequest(QByteArray data);

    void doCloseServer();

    void sendPathCommand(QString path);

    //! @brief  Starts the thread
    //! @param  none
    //! @return none
    void startController();

    //! @brief  Stops the thread
    //! @param  none
    //! @return none
    void stopController();

    void setEventHandler(const QEvent::Type type, const QObject* event_handler);

    void sendStartRecordingRequest(const QString &request);

    void sendStopRecordingRequest();

    void sendChangeDataCollectionPath(const QString &request);

    void sendEnableSensorDataCollection(const QString &request);

    void sendDisableSensorDataCollection(const QString &request);

public slots:
    void run();

private:

    void customEvent(QEvent *event);

    void readHttpMessagesThread();

    void sendEvent(QEvent::Type event_type, QEvent *event );

    QList<const QObject *> getEventHandlers(const QEvent::Type& event_type);

private:

    static tcpPythonAPIReceiverController* m_instance;                    //! Instance of the class

    QThread *m_controller_thread;

    QMultiMap <QEvent::Type, const QObject * > m_event_handlers;

    QString m_server_address;

    QString last_message_received;

    int m_mask_size;

    int m_socket_fd;
    int m_client_socket_fd;

    uint16_t m_port;

    bool m_server_started;
    bool m_server_initialized;

    struct addrinfo *m_result = NULL;
    struct sockaddr_in m_server, m_client;

    bool m_send_message;
};

#endif // tcpProtobufReceiverCONTROLLER_H
