#include "tcpPythonAPIReceiverController.h"
#include <QFile>
#include <QDebug>

#ifdef _WIN32
#else
#include <pthread.h>
#include <sys/ioctl.h>
#endif
#include <pythonAPICodes.h>

std::mutex mtx;

struct tcp_thread_data{
#ifdef _WIN32
    SOCKET socket_d;
#else
    int socket_d;
#endif
};

struct server_thread_data{
#ifdef _WIN32
    SOCKET server_socket_fd;
#else
    int server_socket_fd;
#endif
    struct sockaddr_in client_sockadd;
};

tcp_thread_data *data_for_protobuf_tcp_thread = NULL;
server_thread_data *data_for_server_thread = NULL;

#ifdef _WIN32
HANDLE new_api_connection_thread_handler;
HANDLE accepting_connection_thread_handler;
#else
pthread_t new_api_connection_thread_handler;
pthread_t accepting_connection_thread_handler;
#endif

tcpPythonAPIReceiverController* tcpPythonAPIReceiverController::m_instance = NULL;

bool server_thread_started = true;

bool send_response = false;

QString response_api = "";

#ifdef _WIN32
DWORD WINAPI newAPIConnectionThread(void* params)
#else
void *newAPIConnectionThread(void *params)
#endif
{
    struct tcp_thread_data *data = (struct tcp_thread_data *) params;
    bool thread_alive = true;
    int size_rec = 0;

#ifdef _WIN32
    SOCKET socket_d = data->socket_d;
#else
    int socket_d = data->socket_d;
#endif

    if(socket_d >= 0){

        char *buffer = (char*)malloc(sizeof(char)*2000);

        while(thread_alive){

#ifdef _WIN32
            size_rec = recv(socket_d, buffer, 2000, 0);
#else
            size_rec = read(socket_d , buffer, 2000);
#endif

            //qDebug()<<"Reading finished size "<<size_rec;

            if ( size_rec > 0 ){

                mtx.lock();
                char *recv_buf = (char*)malloc(sizeof(char)*size_rec);
                memcpy(&recv_buf[0], &buffer[0], sizeof(char)*size_rec);

                QByteArray data((const char*)recv_buf, size_rec);

                tcpPythonAPIReceiverController::Instance()->processRequest(data);

                free(recv_buf);

                mtx.unlock();

            }

            if(send_response)
            {
                //qDebug()<<"Sending response"<<response_api;
                send(socket_d, (char*)response_api.toStdString().c_str(), response_api.size(), 0);
                response_api = "";
                send_response = false;
            }
        }
        free(buffer);
    }

#ifdef _WIN32
    closesocket(socket_d);
    socket_d = INVALID_SOCKET;
    WSACleanup();
    return 0;
#else
    shutdown(socket_d, SHUT_RDWR);
    close(socket_d);
    free(data);
    pthread_exit(0);
#endif

}

#ifdef _WIN32
DWORD WINAPI acceptingConnectionThread(void *params){
#else
void *acceptingConnectionThread(void *params){
#endif

    int addrlen = sizeof(sockaddr_in);
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    struct server_thread_data *data = (struct server_thread_data *) params;

#ifdef _WIN32
    SOCKET socket_fd = data->server_socket_fd;
    SOCKET client_socket_fd;
#else
    int socket_fd = data->server_socket_fd;
    int client_socket_fd = -1;
#endif

    sockaddr_in client_add = data->client_sockadd;
    server_thread_started = true;

    while(server_thread_started){

#ifdef _WIN32

        int size = sizeof(client_add);
        client_socket_fd = accept(socket_fd,(struct sockaddr*)&client_add, &size);
        if (client_socket_fd == INVALID_SOCKET) {
            closesocket(client_socket_fd);
            WSACleanup();
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
#else
        if ((client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_add, (socklen_t*)&addrlen)) < 0){
            close(client_socket_fd);
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 500000; // 500ms timewait

        setsockopt(client_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif

        data_for_protobuf_tcp_thread = (struct tcp_thread_data*)malloc( sizeof(struct tcp_thread_data ));
        data_for_protobuf_tcp_thread->socket_d = client_socket_fd;

#ifdef _WIN32
        new_api_connection_thread_handler = CreateThread(NULL, 0, newAPIConnectionThread, (void*)data_for_protobuf_tcp_thread, 0, NULL);
#else
        pthread_create (&new_api_connection_thread_handler, NULL, newAPIConnectionThread, (void*)data_for_protobuf_tcp_thread);
#endif

    }
#ifdef _WIN32
    closesocket(socket_fd);
    socket_fd = INVALID_SOCKET;
    WSACleanup();
    return 0;
#else
    close(socket_fd);
    pthread_exit(0);
#endif

}


tcpPythonAPIReceiverController::tcpPythonAPIReceiverController(QObject *parent) : QObject(parent)
{
    m_server_address = "0.0.0.0";
    m_port = 54548;
    m_server_started = false;

    m_send_message = true;
    m_server_initialized = false;

    m_event_handlers.clear();

    m_controller_thread = new QThread();
    m_controller_thread->setObjectName("tcpPythonAPIReceiverController");

    connect(m_controller_thread, SIGNAL(started()),this, SLOT(run()));
}

tcpPythonAPIReceiverController *tcpPythonAPIReceiverController::Instance()
{
    if(m_instance == NULL){
        m_instance = new tcpPythonAPIReceiverController();
    }
    return m_instance;
}

tcpPythonAPIReceiverController::~tcpPythonAPIReceiverController()
{
    server_thread_started = false;

#ifdef _WIN32
    closesocket(m_socket_fd);
#else
    close(m_socket_fd);
#endif
}

void tcpPythonAPIReceiverController::setPort(uint16_t port)
{
    m_port = port;
}

void tcpPythonAPIReceiverController::setAddress(QString server_address)
{
    m_server_address = server_address;
}

void tcpPythonAPIReceiverController::initializeServer()
{
    m_server_started = false;
    m_server_initialized = false;

    m_server.sin_family = AF_INET;
    m_server.sin_addr.s_addr = INADDR_ANY;
    m_server.sin_port = htons(m_port);

#ifdef _WIN32
    int error = WSAStartup(MAKEWORD(2,2), &m_wsa);

    if (error != 0)
    {
        m_server_started = false;
        m_server_initialized = false;
        return;
    }
#endif

    if ( (m_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
        m_server_started = false;
        m_server_initialized = false;
#ifdef _WIN32
        WSACleanup();
#endif
        qDebug()<<"tcpPythonAPIReceiverController::initializeServer Error creating socket";
        return;
    }

    if( bind(m_socket_fd ,(struct sockaddr *)&m_server , sizeof(m_server)) == SOCKET_ERROR)
    {
        m_server_started = false;
        m_server_initialized = false;
#ifdef _WIN32
        closesocket(m_socket_fd);
        WSACleanup();
#else
        close(m_socket_fd);
#endif
        qDebug()<<"tcpPythonAPIReceiverController::initializeServer Error Binding socket";
        return;
    }

    if (listen(m_socket_fd, 1) < 0){
        m_server_started = false;
        m_server_initialized = false;
#ifdef _WIN32
        closesocket(m_socket_fd);
        WSACleanup();
#else
        close(m_socket_fd);
#endif
        qDebug()<<"tcpPythonAPIReceiverController::initializeServer Error Listening socket";
        return;
    }

    m_server_started = true;
    m_server_initialized = true;

}

void tcpPythonAPIReceiverController::startController()
{
    try{
        if(!m_controller_thread->isRunning()){
            moveToThread(m_controller_thread);
            m_controller_thread->start();
        }
    }
    catch(...){
        qDebug()<<"Unhandled error at tcpPythonAPIReceiverController::startController";
    }
}

void tcpPythonAPIReceiverController::stopController()
{
    try{
        if(m_controller_thread->isRunning()){
            m_controller_thread->exit(0);
        }
        m_event_handlers.clear();

    }catch(...){
        qDebug()<<"Unhandled error at tcpPythonAPIReceiverController::stopController";
    }
}

void tcpPythonAPIReceiverController::setEventHandler(const QEvent::Type type, const QObject *event_handler)
{
    m_event_handlers.insert(type, event_handler);
}

void tcpPythonAPIReceiverController::sendStartRecordingRequest(const QString &request)
{
    int frames = request.split("_")[1].toInt();
    tcpPythonAPIReceiverControllerExecuteStartRecordingRequest *command = new tcpPythonAPIReceiverControllerExecuteStartRecordingRequest(frames);
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendStopRecordingRequest()
{
    tcpPythonAPIReceiverControllerExecuteStopRecordingRequest *request = new tcpPythonAPIReceiverControllerExecuteStopRecordingRequest();
    sendEvent(request->TYPE, request);
}

void tcpPythonAPIReceiverController::sendChangeDataCollectionPath(const QString &request)
{
    QStringList params_list = request.split("_");

    int sensor = params_list[1].toInt();

    QString path = "";
    //!in case the path has '_' in the middle

    for(int i=2; i<params_list.size(); ++i){
        path += params_list[i];
        if(i<params_list.size() - 1){
            path += "_";
        }
    }

    tcpPythonAPIReceiverControllerExecuteChangeSensorPathRequest *command = new tcpPythonAPIReceiverControllerExecuteChangeSensorPathRequest(sensor, path);
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendEnableSensorDataCollection(const QString &request)
{
    int sensor = request.split("_")[1].toInt();
    tcpPythonAPIReceiverControllerExecuteEnableSensorDataCollectionRequest * command = new tcpPythonAPIReceiverControllerExecuteEnableSensorDataCollectionRequest(sensor, true);
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendDisableSensorDataCollection(const QString &request)
{
    int sensor = request.split("_")[1].toInt();
    tcpPythonAPIReceiverControllerExecuteEnableSensorDataCollectionRequest * command = new tcpPythonAPIReceiverControllerExecuteEnableSensorDataCollectionRequest(sensor, false);
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendInitializeRequest()
{
    tcpPythonAPIReceiverControllerExecuteInitializeRequest *command = new tcpPythonAPIReceiverControllerExecuteInitializeRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendFindDevicesRequest()
{
    tcpPythonAPIReceiverControllerExecuteFindDevicesRequest *command = new tcpPythonAPIReceiverControllerExecuteFindDevicesRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendGetStatusRequest()
{
    tcpPythonAPIReceiverControllerExecuteGetStatusRequest *command = new tcpPythonAPIReceiverControllerExecuteGetStatusRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendGetSensorsRequest()
{
    tcpPythonAPIReceiverControllerExecuteGetSensorsRequest *command = new tcpPythonAPIReceiverControllerExecuteGetSensorsRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendStartDeviceRequest()
{
    tcpPythonAPIReceiverControllerExecuteStartDeviceRequest *command = new tcpPythonAPIReceiverControllerExecuteStartDeviceRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendStopDeviceRequest()
{
    tcpPythonAPIReceiverControllerExecuteStopDeviceRequest *command = new tcpPythonAPIReceiverControllerExecuteStopDeviceRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendStartStreamRequest()
{
    tcpPythonAPIReceiverControllerExecuteStartStreamRequest *command = new tcpPythonAPIReceiverControllerExecuteStartStreamRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendStopStreamRequest()
{
    tcpPythonAPIReceiverControllerExecuteStopStreamRequest *command = new tcpPythonAPIReceiverControllerExecuteStopStreamRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendPowerOffRequest()
{
    tcpPythonAPIReceiverControllerExecutePowerOffRequest *command = new tcpPythonAPIReceiverControllerExecutePowerOffRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendFastInitRequest()
{
    tcpPythonAPIReceiverControllerExecuteFastInitRequest *command = new tcpPythonAPIReceiverControllerExecuteFastInitRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::sendTerminateRequest()
{
    tcpPythonAPIReceiverControllerExecuteTerminateRequest *command = new tcpPythonAPIReceiverControllerExecuteTerminateRequest();
    sendEvent(command->TYPE, command);
}

void tcpPythonAPIReceiverController::doSendResponse(const int &error)
{
    //qDebug()<<__func__<<error;
    response_api = QString("%1").arg(error);
    send_response = true;
}

void tcpPythonAPIReceiverController::run()
{
    readHttpMessagesThread();
}

void tcpPythonAPIReceiverController::customEvent(QEvent *event)
{

}

void tcpPythonAPIReceiverController::readHttpMessagesThread(){

    data_for_server_thread = (struct server_thread_data*)malloc(sizeof(struct server_thread_data));
    data_for_server_thread->client_sockadd = m_client;
    data_for_server_thread->server_socket_fd = m_socket_fd;

#ifdef _WIN32
    accepting_connection_thread_handler = CreateThread(NULL, 0, acceptingConnectionThread, (void*)data_for_server_thread, 0, NULL);
#else
    pthread_create(&accepting_connection_thread_handler, NULL, acceptingConnectionThread, (void*)data_for_server_thread);
#endif

}

void tcpPythonAPIReceiverController::sendEvent(QEvent::Type event_type, QEvent *event)
{
    QList<const QObject *> list = getEventHandlers(event_type);
    for (int i = 0; i < list.size(); ++i){
        QCoreApplication::postEvent((QObject*)list.at(i), event);
    }
}

QList<const QObject *> tcpPythonAPIReceiverController::getEventHandlers(const QEvent::Type &event_type)
{
    return m_event_handlers.values(event_type);
}

QString tcpPythonAPIReceiverController::processRequest(QByteArray data){

    QString response = "";
    QString request = QString(data.data());

    //qDebug()<<"Request received "<<request;
    int code = request.split("_")[0].toInt();

    switch(code){
    case CHANGE_DATA_COLLECTION_PATH:
        m_instance->sendChangeDataCollectionPath(request);
        break;
    case ENABLE_SENSOR_DATA_COLLECTION:
        m_instance->sendEnableSensorDataCollection(request);
        break;
    case DISABLE_SENSOR_DATA_COLLECTION:
        m_instance->sendDisableSensorDataCollection(request);
        break;
    case START_RECORDING_REQUEST:
        m_instance->sendStartRecordingRequest(request);
        break;
    case STOP_RECORDING_REQUEST:
        m_instance->sendStopRecordingRequest();
        break;
    case INITIALIZE:
        m_instance->sendInitializeRequest();
        break;
    case FIND_DEVICES:
        m_instance->sendFindDevicesRequest();
        break;
    case GET_STATUS:
        m_instance->sendGetStatusRequest();
        break;
    case GET_SENSORS:
        m_instance->sendGetSensorsRequest();
        break;
    case START_DEVICE:
        m_instance->sendStartDeviceRequest();
        break;
    case STOP_DEVICE:
        m_instance->sendStopDeviceRequest();
        break;
    case START_STREAM:
        m_instance->sendStartStreamRequest();
        break;
    case STOP_STREAM:
        m_instance->sendStopStreamRequest();
        break;
    case POWER_OFF:
        m_instance->sendPowerOffRequest();
        break;
    case FAST_INIT:
        m_instance->sendFastInitRequest();
        break;
    case TERMINATE:
        m_instance->sendTerminateRequest();
        break;
    }

    return response;

}

void tcpPythonAPIReceiverController::doCloseServer()
{
    server_thread_started = false;
}


