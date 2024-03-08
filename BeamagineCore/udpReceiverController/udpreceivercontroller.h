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

#ifndef UDPRECEIVERCONTROLLER_H
#define UDPRECEIVERCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QCoreApplication>

#include <stdint.h>

#ifdef _WIN32
#include <winsock2.h>
#include <iptypes.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <beam_aux.h>

#include <udpReceiverControllerMessages.h>

class udpReceiverController : public QObject
{
    Q_OBJECT
public:
    explicit udpReceiverController(QObject *parent = 0);

    void setIpAddress(QString address);

    void setPort(qint16 port);

    void doReadPointcloud(bool read);

    void doReadImageRgb(bool read);

    //! @brief  Starts the thread
    //! @param  none
    //! @return none
    void startController();

    //! @brief  Stops the thread
    //! @param  none
    //! @return none
    void stopController();

    void setEventHandler(const QEvent::Type type, const QObject* event_handler);

public slots:
    void run();

signals:
    void imageReadyToShow(uint8_t* image_data, uint16_t heigth, uint16_t width);

    void imageRgbReadyToShow(uint8_t* image_data, uint16_t heigth, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp);

    void pointcloudReadyToShow(int32_t *pointcloud_data, uint32_t timestamp);

    void pointcloudHeaderReceived(int32_t suma_1, int32_t suma_2);

    void detectionsImageReadyToShow(std::vector<detectionImage> detections);


private:
    void customEvent(QEvent *event);

    void startup();

    void readPointcloud();

    void readRgbImage();

    void readThermalData();

    int initializeSocket();

protected:

    void sendEvent(QEvent::Type event_type, QEvent *event );

    QList<const QObject *> getEventHandlers(const QEvent::Type& event_type);

protected:

    QThread *m_controller_thread;

    QMultiMap <QEvent::Type, const QObject * > m_event_handlers;

private:

    QString m_address;
    std::vector<detectionImage> m_detections;
    struct sockaddr_in m_socket;         //! local Socket

#ifdef _WIN32
    WSADATA m_wsa;
    SOCKET m_udp_socket;
#else
    int m_socket_descriptor;             //! Socket descriptor
#endif

    int m_image_data_size;
    int m_error_code;

    float *m_temperatures_pointer;
    uint32_t m_timestamp;

    int32_t m_pointcloud_size;
    int32_t *m_pointcloud_data;
    quint16 m_udp_port;

    uint16_t m_image_width;
    uint16_t m_image_height;
    uint8_t m_image_channels;
    uint8_t m_image_detections;
    char *m_image_buffer;

    bool m_is_pointcloud_ready;
    bool m_read_pointcloud;
    bool m_is_reading_image;
    bool m_read_image ;
    bool m_is_reading_pointcloud;
    bool m_read_rgb;
    bool m_is_reading_detections;
};

#endif // UDPRECEIVERCONTROLLER_H
