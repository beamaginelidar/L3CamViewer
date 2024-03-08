/*  Copyright (c) 2023, Beamagine
 *
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

        - Redistributions of source code must retain the above copyright notice,
          this list of conditions and the following disclaimer.
        - Redistributions in binary form must reproduce the above copyright notice,
          this list of conditions and the following disclaimer in the documentation and/or
          other materials provided with the distribution.
        - Neither the name of copyright holders nor the names of its contributors may be
          used to endorse or promote products derived from this software without specific
          prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
    COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
    EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "udpreceivercontroller.h"
#include <QDebug>

#ifdef _WIN32
#include <chrono>
#include <thread>
#else
#include <unistd.h>
#endif

#include <QDateTime>

udpReceiverController::udpReceiverController(QObject *parent) : QObject(parent)
{
    m_udp_port = 6000;
    m_address = "127.0.0.1";

    m_is_reading_image = false;
    m_is_pointcloud_ready = false;
    m_is_reading_pointcloud = false;
    m_read_image = false;
    m_read_rgb = false;
    m_read_pointcloud = false;
    m_error_code = 0;

    m_event_handlers.clear();

    m_controller_thread = new QThread();
    m_controller_thread->setObjectName("udpReceiverController");

    connect(m_controller_thread, SIGNAL(started()),this, SLOT(run()));

}

void udpReceiverController::setIpAddress(QString address)
{
    m_address = address;
}

void udpReceiverController::setPort(qint16 port)
{
    m_udp_port = port;
}

void udpReceiverController::doReadPointcloud(bool read)
{
    m_read_pointcloud = read;
}

void udpReceiverController::doReadImageRgb(bool read)
{
    m_read_rgb = read;
}

void udpReceiverController::startController()
{
    try{
        if(!m_controller_thread->isRunning()){
            moveToThread(m_controller_thread);
            m_controller_thread->start();
        }
    }
    catch(...){
        qDebug()<<"Unhandled error at udpReceiverController::startController";
    }
}

void udpReceiverController::stopController()
{
    try{
        if(m_controller_thread->isRunning()){
            m_controller_thread->exit(0);
        }
        m_event_handlers.clear();

    }catch(...){
        qDebug()<<"Unhandled error at udpReceiverController::stopController";
    }
}

void udpReceiverController::setEventHandler(const QEvent::Type type, const QObject *event_handler)
{
    m_event_handlers.insert(type, event_handler);
}

void udpReceiverController::run()
{
    if(initializeSocket() == 0){
        if(m_read_pointcloud){
            readPointcloud();
        }
        else if(m_read_rgb){
            readRgbImage();
        }
    }else{
        qDebug()<<"Error initializing UDP receiving socket"<<m_error_code;
    }
}

void udpReceiverController::customEvent(QEvent *event)
{
    Q_UNUSED(event);
}


void udpReceiverController::readRgbImage(){

    m_image_buffer = NULL;
    int bytes_count = 0;
    socklen_t socket_len = sizeof(m_socket);
    char* buffer;
    buffer = (char*)malloc(8000);
    m_is_reading_image = false;
    detectionImage curr_det;
    m_image_detections = 0;

    m_image_height = 5000;
    m_image_width = 5000;
    m_image_channels = 3;

    m_image_data_size = m_image_height*m_image_width*m_image_channels;

    m_image_buffer = (char*)malloc(m_image_data_size);
    uint8_t* image_pointer = (uint8_t*)malloc(m_image_data_size);

    memset(m_image_buffer, 0, m_image_data_size);
    memset(image_pointer, 0, m_image_data_size);

    while(1){

#ifdef _WIN32
        int size_read = recvfrom(m_udp_socket, buffer, 8000, 0, (struct sockaddr*)&m_socket, &socket_len);
#else
        int size_read = recvfrom(m_socket_descriptor, buffer, 8000, 0, (struct sockaddr*)&m_socket, &socket_len);
#endif

        if(size_read == 0){
            continue;
        }

        if(size_read == 11){

            memcpy(&m_image_height, &buffer[1], 2);
            memcpy(&m_image_width, &buffer[3], 2);
            memcpy(&m_image_channels, &buffer[5], 1);
            memcpy(&m_timestamp, &buffer[6], 4);
            memcpy(&m_image_detections, &buffer[10], 1);

            m_image_data_size = m_image_height*m_image_width*m_image_channels;
            memset(m_image_buffer,0,m_image_data_size);

            m_is_reading_image = true;
            m_detections.clear();

            bytes_count = 0;

        }else if(size_read == 1 && bytes_count == m_image_data_size ){

            m_is_reading_image = false;
            bytes_count = 0;

            memcpy(image_pointer, m_image_buffer, m_image_data_size);
            emit imageRgbReadyToShow(image_pointer, m_image_height, m_image_width, m_image_channels, m_detections, m_timestamp);

            m_image_detections = 0;

        }else if(size_read > 0 && m_is_reading_image){

            if(m_image_detections > 0){
                //!read detections packages
                memcpy(&curr_det.confidence, &buffer[0], 2);
                memcpy(&curr_det.box.x, &buffer[2], 2);
                memcpy(&curr_det.box.y, &buffer[4], 2);
                memcpy(&curr_det.box.height, &buffer[6], 2);
                memcpy(&curr_det.box.width, &buffer[8], 2);
                memcpy(&curr_det.label, &buffer[10], 2);
                memcpy(&curr_det.red, &buffer[12], 1);
                memcpy(&curr_det.green, &buffer[13], 1);
                memcpy(&curr_det.blue, &buffer[14], 1);
                m_detections.push_back(curr_det);
                --m_image_detections;
                continue;
            }

            memcpy(&m_image_buffer[bytes_count], buffer, size_read);
            bytes_count+=size_read;

        }

    }

    free(buffer);
    free(m_image_buffer);
    free(image_pointer);
}

void udpReceiverController::readThermalData()
{
    m_temperatures_pointer = NULL;
    int bytes_count = 0;
    int float_pointer_cnt;
    socklen_t socket_len = sizeof(m_socket);
    char *buffer;
    buffer = (char*)malloc(8000);
    m_is_reading_image = false;

    m_image_height = 1000;
    m_image_width = 1000;

    m_image_data_size = m_image_height * m_image_width * m_image_channels * sizeof(float);

    m_temperatures_pointer = (float*)malloc(m_image_data_size);

    while (1)
    {
        int size_read = recvfrom(m_socket_descriptor, buffer, 8000, 0, (struct sockaddr *)&m_socket, &socket_len);

        if (size_read == 9) // Header
        {
            memcpy(&m_image_height, &buffer[1], 2);
            memcpy(&m_image_width, &buffer[3], 2);
            memcpy(&m_timestamp, &buffer[5], 4);


            m_image_data_size = m_image_height * m_image_width * sizeof(float);

            m_is_reading_image = true;
            bytes_count = 0;
            float_pointer_cnt = 0;
        }
        else if (size_read == 1) // End, send image
        {
            m_is_reading_image = false;
            bytes_count = 0;
            float_pointer_cnt = 0;

            //saveBinaryThermalData(thermal_data_pointer, m_image_data_size, m_timestamp);
        }
        else if (size_read > 0 && m_is_reading_image)
        {
            memcpy(&m_temperatures_pointer[float_pointer_cnt], buffer, size_read);
            bytes_count += size_read;
            float_pointer_cnt += (size_read / 4);
        }
    }

    free(m_temperatures_pointer);
}

void udpReceiverController::readPointcloud(){

    int points_received = 1;
    socklen_t socket_len = sizeof(m_socket);
    char* buffer = NULL;
    buffer = (char*)malloc(64004);
    m_is_reading_pointcloud = false;
    int buffer_size = sizeof(int32_t)*(1440001);
    m_pointcloud_data = (int32_t*)malloc(buffer_size); //! (480*150*5 )+ 1
    memset(m_pointcloud_data, 0, buffer_size);


    while(1){

#ifdef _WIN32
        int size_read = recvfrom(m_udp_socket, buffer, 64004, 0, (struct sockaddr*)&m_socket, &socket_len);
#else
        int size_read = recvfrom(m_socket_descriptor, buffer, 64004, 0, (struct sockaddr*)&m_socket, &socket_len);
#endif
        if(size_read == 0){
            continue;
        }

        if(size_read == 17){
            memcpy(&m_pointcloud_size, &buffer[1], 4);
            //if(m_pointcloud_data != NULL){
            //    free(m_pointcloud_data);
            //    m_pointcloud_data = NULL;
            //}
            //m_pointcloud_data = (int32_t*)malloc( sizeof(int32_t)*(((m_pointcloud_size) * 5) + 1));
            memcpy(&m_pointcloud_data[0], &m_pointcloud_size, sizeof(int32_t));
            int32_t suma_1, suma_2;
            memcpy(&suma_1, &buffer[5], sizeof(int32_t));
            memcpy(&suma_2, &buffer[9], sizeof(int32_t));
            memcpy(&m_timestamp, &buffer[13], sizeof(uint32_t));
            m_is_reading_pointcloud = true;
            points_received = 1;
            emit pointcloudHeaderReceived(suma_1, suma_2);
        }
        else if(size_read == 1 && m_is_reading_pointcloud){

            m_is_reading_pointcloud = false;

            //if(m_pointcloud_data != NULL){
            int32_t *data_received = (int32_t*)malloc(sizeof(int32_t)*(m_pointcloud_size*5)+1);
            memcpy(&data_received[0], &m_pointcloud_data[0], sizeof(int32_t)*((m_pointcloud_size*5)+1));
            memset(m_pointcloud_data, 0, buffer_size);
            emit pointcloudReadyToShow(data_received, m_timestamp);
            //free(m_pointcloud_data);
            //m_pointcloud_data = NULL;
            //}
            points_received = 1;
        }
        else if(size_read > 0 && m_is_reading_pointcloud){

            //if(m_pointcloud_data != NULL){
            int32_t points = 0;
            memcpy(&points, &buffer[0], 4); //!copy number points in the package
            memcpy(&m_pointcloud_data[points_received], &buffer[4], (sizeof(int32_t)*(points*5)));
            points_received+=points*5;
            //}

        }
    }
    free(m_pointcloud_data);
    free(buffer);

}

int udpReceiverController::initializeSocket()
{

#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2,2), &m_wsa) != 0){
        m_error_code = -1;
        return m_error_code;
    }
    m_udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_udp_socket == INVALID_SOCKET){
        m_error_code = -2;
        return m_error_code;
    }

    m_socket.sin_family = AF_INET;
    m_socket.sin_port = htons((int)m_udp_port);

    if (inet_pton(AF_INET, (char*)m_address.toStdString().c_str(), &(m_socket.sin_addr)) != 1) {
        m_error_code = -5;
        return m_error_code;
    }

    if(bind(m_udp_socket, (sockaddr*)&m_socket, sizeof(m_socket)) == SOCKET_ERROR){
        m_error_code = -3;
        return m_error_code;
    }
    //!set size for sockets
    int rcvbufsize = 134217728;
    if(0 != setsockopt(m_udp_socket, SOL_SOCKET,SO_RCVBUF,(char*)&rcvbufsize,sizeof(rcvbufsize))){
        m_error_code = -4;
        return m_error_code;
    }
#else
    if( (m_socket_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        qDebug()<<"Socket Error";
        m_error_code = -1;
        return m_error_code;
    }

    memset((char *) &m_socket, 0, sizeof(struct sockaddr_in));
    m_socket.sin_addr.s_addr = inet_addr ((char*)m_address.toStdString().c_str());

    m_socket.sin_family = AF_INET;
    m_socket.sin_port = htons((int)m_udp_port);

    if (inet_aton((char*)m_address.toStdString().c_str(), &m_socket.sin_addr) == 0)
    {
        qDebug()<<"inet_aton() failed";
        return -2;
    }

    if (bind(m_socket_descriptor, (struct sockaddr *)&m_socket, sizeof(struct sockaddr_in)) == -1)
    {
        qDebug()<<"Could not bind name to socket";
        close(m_socket_descriptor);
        return -3;
    }

    //!set size for sockets
    int rcvbufsize = 134217728;
    if(0 != setsockopt(m_socket_descriptor,SOL_SOCKET,SO_RCVBUF,(char*)&rcvbufsize,sizeof(rcvbufsize))){
        qDebug()<<"Error setting size to socket";
        return -4;
    }

#endif

    return 0;
}

void udpReceiverController::sendEvent(QEvent::Type event_type, QEvent *event)
{
    QList<const QObject *> list = getEventHandlers(event_type);
    for (int i = 0; i < list.size(); ++i){
        QCoreApplication::postEvent((QObject*)list.at(i), event);
    }
}

QList<const QObject *> udpReceiverController::getEventHandlers(const QEvent::Type &event_type)
{
    return m_event_handlers.values(event_type);
}
