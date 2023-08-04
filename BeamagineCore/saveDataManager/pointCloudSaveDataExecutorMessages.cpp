#include "pointCloudSaveDataExecutorMessages.h"

const QEvent::Type pointCloudSaveDataExecutorSaveBinaryDataRequest::TYPE     = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type pointCloudSaveDataExecutorSaveBinaryDataResponse::TYPE    = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type pointCloudSaveDataExecutorSaveBinaryBufferDataRequest::TYPE = static_cast<QEvent::Type>(QEvent::registerEventType());
