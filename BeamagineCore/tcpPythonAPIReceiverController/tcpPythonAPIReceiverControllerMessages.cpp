#include "tcpPythonAPIReceiverControllerMessages.h"

const QEvent::Type tcpPythonAPIReceiverControllerExecuteStartRecordingRequest::TYPE               = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteStopRecordingRequest::TYPE                = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteChangeSensorPathRequest::TYPE             = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteEnableSensorDataCollectionRequest::TYPE   = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteInitializeRequest::TYPE                   = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteFindDevicesRequest::TYPE                  = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteGetStatusRequest::TYPE                    = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteGetSensorsRequest::TYPE                   = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteStartDeviceRequest::TYPE                  = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteStopDeviceRequest::TYPE                   = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteStartStreamRequest::TYPE                  = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteStopStreamRequest::TYPE                   = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecutePowerOffRequest::TYPE                     = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteFastInitRequest::TYPE                     = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteTerminateRequest::TYPE                    = static_cast<QEvent::Type>(QEvent::registerEventType());
