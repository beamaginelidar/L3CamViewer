#include "tcpPythonAPIReceiverController.h"
#include <QFile>
#include <QDebug>

#include <pthread.h>
#include <sys/ioctl.h>

#include <pythonAPICodes.h>

std::mutex mtx;

struct tcp_thread_data{
    int socket_d;
};

struct server_thread_data{
    int server_socket_fd;
    struct sockaddr_in client_sockadd;
};

tcp_thread_data *data_for_protobuf_tcp_thread = NULL;
server_thread_data *data_for_server_thread = NULL;

pthread_t new_api_connection_thread_handler;
pthread_t accepting_connection_thread_handler;

tcpPythonAPIReceiverController* tcpPythonAPIReceiverController::m_instance = NULL;

bool server_thread_started = true;

void *newAPIConnectionThread(void *params){

    struct tcp_thread_data *data = (struct tcp_thread_data *) params;

    int socket_d = data->socket_d;
    bool thread_alive = true;
    int size_rec = 0;

    if(socket_d >= 0){

        char *buffer = (char*)malloc(sizeof(char)*2000);

        while(thread_alive){

#ifdef _WIN32
        size_rec = recv(ClientSocket, buffer, 500, 0);
#else
        size_rec = read(socket_d , buffer, 2000);
#endif

            if ( size_rec > 0 ){

                mtx.lock();
                char *recv_buf = (char*)malloc(sizeof(char)*size_rec);
                memcpy(&recv_buf[0], &buffer[0], sizeof(char)*size_rec);

                QByteArray data((const char*)recv_buf, size_rec);

                tcpPythonAPIReceiverController::Instance()->processRequest(data);

                free(recv_buf);

                mtx.unlock();

            }else if(size_rec < 0){
                thread_alive = false;
            }

        }

        free(buffer);
    }

    close(socket_d);
    free(data);
    pthread_exit(0);
}

void *acceptingConnectionThread(void *params){

    int addrlen = sizeof(sockaddr_in);
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    struct server_thread_data *data = (struct server_thread_data *) params;

    int socket_fd = data->server_socket_fd;
    sockaddr_in client_add = data->client_sockadd;
    server_thread_started = true;

    while(server_thread_started){
        int client_socket_fd = -1;

        if ((client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_add, (socklen_t*)&addrlen)) < 0){
            close(client_socket_fd);
            sleep(5);
            continue;
        }

        data_for_protobuf_tcp_thread = (struct tcp_thread_data*)malloc( sizeof(struct tcp_thread_data ));
        data_for_protobuf_tcp_thread->socket_d = client_socket_fd;

        pthread_create (&new_api_connection_thread_handler, NULL, newAPIConnectionThread, (void*)data_for_protobuf_tcp_thread);
    }

    close(socket_fd);
    pthread_exit(0);
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
    close(m_socket_fd);
    close(m_client_socket_fd);
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

    qDebug()<<"tcpPythonAPIReceiverController::initializeServer Initializing python API server";

    m_server.sin_family = AF_INET;
    m_server.sin_addr.s_addr = INADDR_ANY;
    m_server.sin_port = htons(m_port);

    if ( (m_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        m_server_started = false;
        m_server_initialized = false;
        qDebug()<<"tcpPythonAPIReceiverController::initializeServer Error creating socket";
        return;
    }

    if (bind(m_socket_fd, (struct sockaddr *)&m_server, sizeof(m_server))<0 ){
        m_server_started = false;
        m_server_initialized = false;
        qDebug()<<"tcpPythonAPIReceiverController::initializeServer Error Binding socket";
        return;
    }

    if (listen(m_socket_fd, 1) < 0){
        m_server_started = false;
        m_server_initialized = false;
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

    pthread_create(&accepting_connection_thread_handler, NULL, acceptingConnectionThread, (void*)data_for_server_thread);
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

    qDebug()<<"Request received "<<request;

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
    }

    return response;

}

void tcpPythonAPIReceiverController::doCloseServer()
{
    server_thread_started = false;
}


