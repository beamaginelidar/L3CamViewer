#include "tcpPythonAPIReceiverControllerMessages.h"

const QEvent::Type tcpPythonAPIReceiverControllerExecuteStartRecordingRequest::TYPE               = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteStopRecordingRequest::TYPE                = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteChangeSensorPathRequest::TYPE             = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type tcpPythonAPIReceiverControllerExecuteEnableSensorDataCollectionRequest::TYPE   = static_cast<QEvent::Type>(QEvent::registerEventType());
