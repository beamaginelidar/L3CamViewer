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


#endif // tcpPythonAPIReceiverCONTROLLERMESSAGES_H
