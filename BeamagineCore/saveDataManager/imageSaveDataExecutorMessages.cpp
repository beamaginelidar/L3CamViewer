#include "imageSaveDataExecutorMessages.h"

const QEvent::Type imageSaveDataExecutorSaveDataPngRequest::TYPE              = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSaveMultiDataPngRequest::TYPE         = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSaveDataPngRgbRequest::TYPE           = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSaveDataPngStokesRequest::TYPE        = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSaveMatImage::TYPE                    = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSaveMatImageString::TYPE              = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSavePointerToPngRequest::TYPE         = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSavePointerToPngResponse::TYPE        = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSaveFloatBufferRequest::TYPE          = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type imageSaveDataExecutorSaveFloatBufferResponse::TYPE         = static_cast<QEvent::Type>(QEvent::registerEventType());
