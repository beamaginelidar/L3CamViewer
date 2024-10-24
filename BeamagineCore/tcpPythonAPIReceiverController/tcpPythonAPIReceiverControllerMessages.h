#pragma once
#ifndef tcpPythonAPIReceiverCONTROLLERMESSAGES_H
#define tcpPythonAPIReceiverCONTROLLERMESSAGES_H

#include <QEvent>
#include <QString>

class tcpPythonAPIReceiverControllerExecuteStartRecordingRequest: public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteStartRecordingRequest(const int &frames) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_frames = frames;
    }
    static const QEvent::Type TYPE;

    int getFrames(){return m_frames;}
private:

    int m_frames;
};

class tcpPythonAPIReceiverControllerExecuteStopRecordingRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteStopRecordingRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;
private:
};

class tcpPythonAPIReceiverControllerExecuteChangeSensorPathRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteChangeSensorPathRequest(const int &sensor, const QString &path) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_sensor = sensor;
        m_path = path;
    }
    static const QEvent::Type TYPE;
    int getSensor(){return m_sensor;}
    QString getPath(){return m_path;}
private:
    int m_sensor;
    QString m_path;
};

class tcpPythonAPIReceiverControllerExecuteEnableSensorDataCollectionRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteEnableSensorDataCollectionRequest(const int &sensor, const bool &enabled) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_sensor = sensor;
        m_enabled = enabled;
    }
    static const QEvent::Type TYPE;
    int getSensor(){return m_sensor;}
    bool isEnabled(){return m_enabled;}
private:
    int m_sensor;
    bool m_enabled;
};

class tcpPythonAPIReceiverControllerExecuteInitializeRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteInitializeRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }

    static const QEvent::Type TYPE;
private:

};
class tcpPythonAPIReceiverControllerExecuteFindDevicesRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteFindDevicesRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;

private:

};
class tcpPythonAPIReceiverControllerExecuteGetStatusRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteGetStatusRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;
private:

};
class tcpPythonAPIReceiverControllerExecuteGetSensorsRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteGetSensorsRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;

private:

};
class tcpPythonAPIReceiverControllerExecuteStartDeviceRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteStartDeviceRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;

private:

};
class tcpPythonAPIReceiverControllerExecuteStopDeviceRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteStopDeviceRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;
private:

};
class tcpPythonAPIReceiverControllerExecuteStartStreamRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteStartStreamRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
   static const QEvent::Type TYPE;
private:

};
class tcpPythonAPIReceiverControllerExecuteStopStreamRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteStopStreamRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;

private:

};
class tcpPythonAPIReceiverControllerExecutePowerOffRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecutePowerOffRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;
private:

};
class tcpPythonAPIReceiverControllerExecuteFastInitRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteFastInitRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;

private:

};

class tcpPythonAPIReceiverControllerExecuteTerminateRequest : public QEvent{
public:
    tcpPythonAPIReceiverControllerExecuteTerminateRequest() : QEvent((QEvent::Type)(QEvent::registerEventType())){

    }
    static const QEvent::Type TYPE;

private:

};


#endif // tcpPythonAPIReceiverCONTROLLERMESSAGES_H
