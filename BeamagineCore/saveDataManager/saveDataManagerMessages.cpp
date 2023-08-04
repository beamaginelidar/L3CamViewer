#include "saveDataManagerMessages.h"

const QEvent::Type saveDataManagerSavePointerToPngRequest::TYPE              = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type saveDataManagerSavePointCloudToBinRequest::TYPE           = static_cast<QEvent::Type>(QEvent::registerEventType());
