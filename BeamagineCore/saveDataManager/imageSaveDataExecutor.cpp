#include "imageSaveDataExecutor.h"

imageSaveDataExecutor::imageSaveDataExecutor(QObject *parent) : QObject(parent)
{
    m_width = 2248;
    m_height = 2048;

    m_path_to_save_images = QDir::homePath() + "/images/";
    m_save_lossless = false;
    m_is_available = true;

    m_event_handlers.clear();

    m_controller_thread = new QThread();
    m_controller_thread->setObjectName("imageSaveDataExecutor");

    connect(m_controller_thread, SIGNAL(started()),this, SLOT(run()));
}

void imageSaveDataExecutor::startController()
{
    try{
        if(!m_controller_thread->isRunning()){
            moveToThread(m_controller_thread);
            m_controller_thread->start();
        }
    }
    catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::startController";
    }
}

void imageSaveDataExecutor::stopController()
{
    try{
        if(m_controller_thread->isRunning()){
            m_controller_thread->exit(0);
        }
        m_event_handlers.clear();

    }catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::stopController";
    }
}

void imageSaveDataExecutor::setEventHandler(const QEvent::Type type, const QObject *event_handler)
{
    m_event_handlers.insert(type, event_handler);

}


void imageSaveDataExecutor::doSaveDataPng(cv::Mat image_to_save, QDateTime time_stamp){

    try{
        m_is_available = false;
        cv::Mat image = image_to_save.clone();
        imageSaveDataExecutorSaveMatImage *request = new imageSaveDataExecutorSaveMatImage(image, time_stamp);
        QCoreApplication::postEvent(this, request);

    }catch(...){

    }
}

void imageSaveDataExecutor::doSaveDataPng(cv::Mat image_to_save, QDateTime time_stamp, QString camStr){

    try{
        m_is_available = false;
        cv::Mat image = image_to_save.clone();
        imageSaveDataExecutorSaveMatImageString *request = new imageSaveDataExecutorSaveMatImageString(image, time_stamp, camStr);
        QCoreApplication::postEvent(this, request);
    }catch(...){

    }
}

void imageSaveDataExecutor::doSaveDataPng(uint8_t *image_data_pointer, const int &image_type, const QDateTime &time_stamp){

    try{
        m_is_available = false;
        int size = m_height*m_width;
        uint8_t *data_pointer = (uint8_t *)malloc(size);
        memcpy(data_pointer, image_data_pointer, size);
        imageSaveDataExecutorSaveDataPngRequest *request = new imageSaveDataExecutorSaveDataPngRequest(data_pointer, image_type, time_stamp, size);
        free(data_pointer);
        QCoreApplication::postEvent(this, request);

    }catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::doSaveDataPng";
    }
}

void imageSaveDataExecutor::doSaveDataPngRgb(uint8_t *image_data_pointer, const int &image_type, const QDateTime &time_stamp){

    try{
        m_is_available = false;
        int size = m_height * m_width * 3;
        uint8_t *data_pointer = (uint8_t *)malloc(size);
        memcpy(data_pointer, image_data_pointer, size);
        imageSaveDataExecutorSaveDataPngRgbRequest *request = new imageSaveDataExecutorSaveDataPngRgbRequest(data_pointer, image_type, time_stamp, size);
        free(data_pointer);
        QCoreApplication::postEvent(this, request);

    }catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::doSaveDataPng";
    }
}

void imageSaveDataExecutor::doSaveDataPng(uint8_t *image_0_pointer, uint8_t *image_45_pointer, uint8_t *image_90_pointer, uint8_t *image_135_pointer, const QDateTime &time_stamp){

    try{
        m_is_available = false;

        uint8_t* data_pointer_0   = (uint8_t*)malloc(m_width*m_height);
        uint8_t* data_pointer_45  = (uint8_t*)malloc(m_width*m_height);
        uint8_t* data_pointer_90  = (uint8_t*)malloc(m_width*m_height);
        uint8_t* data_pointer_135 = (uint8_t*)malloc(m_width*m_height);

        memcpy(data_pointer_0,  image_0_pointer,   (m_width*m_height));
        memcpy(data_pointer_45, image_45_pointer,  (m_width*m_height));
        memcpy(data_pointer_90, image_90_pointer,  (m_width*m_height));
        memcpy(data_pointer_135,image_135_pointer, (m_width*m_height));

        imageSaveDataExecutorSaveMultiDataPngRequest *request = new imageSaveDataExecutorSaveMultiDataPngRequest(data_pointer_0,
                                                                                                                 data_pointer_45,
                                                                                                                 data_pointer_90,
                                                                                                                 data_pointer_135, time_stamp,(m_width*m_height));
        free(data_pointer_0);
        free(data_pointer_45);
        free(data_pointer_90);
        free(data_pointer_135);

        QCoreApplication::postEvent(this, request);

    }catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::doSaveDataPng 4 pointers";
    }
}

void imageSaveDataExecutor::doSaveDataPngStokes(uint8_t* image_s1, uint8_t* image_s2, const QDateTime &time_stamp){
    try{
        m_is_available = false;

        int size = m_height*m_width;
        uint8_t* image_s1_copy = (uint8_t*)malloc(size);
        uint8_t* image_s2_copy = (uint8_t*)malloc(size);

        memcpy(image_s1_copy, image_s1, size);
        memcpy(image_s2_copy, image_s2, size);

        imageSaveDataExecutorSaveDataPngStokesRequest *request = new imageSaveDataExecutorSaveDataPngStokesRequest(image_s1, image_s2, size, time_stamp);

        QCoreApplication::postEvent(this, request);

        free(image_s1_copy);
        free(image_s2_copy);

    }catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::saveDataPngStokes";
    }
}

void imageSaveDataExecutor::doSaveLossLess(bool lossless)
{
    m_save_lossless = lossless;
}

void imageSaveDataExecutor::doSavePointerToPng(uint8_t *image_pointer, uint16_t width, uint16_t height, uint8_t channels, QString file_name)
{
    m_is_available = false;

    int size_to_save = width*height*channels*sizeof(uint8_t);
    uint8_t *image_buffer = (uint8_t*)malloc(size_to_save);
    memcpy(image_buffer, image_pointer, size_to_save);
    imageSaveDataExecutorSavePointerToPngRequest *request = new imageSaveDataExecutorSavePointerToPngRequest(image_buffer, width, height, channels, file_name);
    QCoreApplication::postEvent(this, request);
    free(image_buffer);
}

void imageSaveDataExecutor::doSaveFloatData(float *buffer_data, int buffer_size, QString file_name)
{
    m_is_available = false;

    float *data_buffer = (float*)malloc(buffer_size);
    memcpy(data_buffer, buffer_data, buffer_size);
    imageSaveDataExecutorSaveFloatBufferRequest *request = new imageSaveDataExecutorSaveFloatBufferRequest(data_buffer, buffer_size, file_name);
    QCoreApplication::postEvent(this, request);
    free(data_buffer);
}

void imageSaveDataExecutor::run()
{
    m_available_timer = new QTimer();
    m_available_time_ms = 3;
    m_is_available = true;
    connect(m_available_timer, SIGNAL(timeout()), this, SLOT(availableTimeOut()));
    m_available_timer->start(m_available_time_ms);
}

void imageSaveDataExecutor::availableTimeOut()
{
    m_available_timer->stop();

    if(m_is_available){
        emit executorIsAvailable(m_is_available);
    }

    m_available_timer->start(m_available_time_ms);
}


void imageSaveDataExecutor::customEvent(QEvent *event)
{
    //----PNG DATA
    if(imageSaveDataExecutorSaveDataPngRequest *p_event = dynamic_cast<imageSaveDataExecutorSaveDataPngRequest*>(event)){
        onSaveDataPng(p_event);
    }
    else if(imageSaveDataExecutorSaveMultiDataPngRequest* p_event = dynamic_cast<imageSaveDataExecutorSaveMultiDataPngRequest*>(event)){
        onSaveDataPng(p_event);
    }
    else if(imageSaveDataExecutorSaveDataPngRgbRequest *p_event = dynamic_cast<imageSaveDataExecutorSaveDataPngRgbRequest*>(event)){
        onSaveDataPng(p_event);
    }
    else if(imageSaveDataExecutorSaveDataPngStokesRequest *p_event = dynamic_cast<imageSaveDataExecutorSaveDataPngStokesRequest*>(event)){
        onSaveStokesImages(p_event);
    }
    else if(imageSaveDataExecutorSaveMatImage *p_event = dynamic_cast<imageSaveDataExecutorSaveMatImage*>(event)){
        onSaveImage(p_event);
    }
    else if(imageSaveDataExecutorSaveMatImageString *p_event = dynamic_cast<imageSaveDataExecutorSaveMatImageString*>(event)){
        onSaveImage(p_event);
    }
    else if(imageSaveDataExecutorSavePointerToPngRequest *p_event = dynamic_cast<imageSaveDataExecutorSavePointerToPngRequest*>(event)){
        onSavePointerToPng(p_event);
    }
    else if(imageSaveDataExecutorSaveFloatBufferRequest *p_event = dynamic_cast<imageSaveDataExecutorSaveFloatBufferRequest*>(event)){
        onSaveFloatPointer(p_event);
    }
}

void imageSaveDataExecutor::doCheckTimeout()
{
    //!do nothing
}

void imageSaveDataExecutor::sendEvent(QEvent::Type event_type, QEvent *event)
{
    QList<const QObject *> list = getEventHandlers(event_type);
    for (int i = 0; i < list.size(); ++i){
        QCoreApplication::postEvent((QObject*)list.at(i), event);
    }
}

QList<const QObject *> imageSaveDataExecutor::getEventHandlers(const QEvent::Type &event_type)
{
    return m_event_handlers.values(event_type);
}

void imageSaveDataExecutor::setPathToSaveImages(const QString &path)
{
    m_path_to_save_images = path;
}

void imageSaveDataExecutor::setImageSize(const int &height, const int &width){
    m_height = height;
    m_width = width;
}

void imageSaveDataExecutor::onSaveDataPng(imageSaveDataExecutorSaveDataPngRequest *request){

    try{
        uint8_t *image_data_pointer = (uint8_t*) malloc(request->getImageSize());
        memcpy(image_data_pointer,request->getImageDataPointer(),request->getImageSize());

        int image_type = request->getImageType();
        QDateTime time_stamp = request->getTimeStamp();

        saveDataPng(image_data_pointer, image_type, time_stamp);

        free(image_data_pointer);
        request->freeMemory();
    }catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::onSaveDataPng";
    }

}

void imageSaveDataExecutor::onSaveStokesImages(imageSaveDataExecutorSaveDataPngStokesRequest *request){
    uint8_t* image_s1 = (uint8_t*)malloc(m_height*m_width);
    uint8_t* image_s2 = (uint8_t*)malloc(m_height*m_width);
    memcpy(image_s1, request->getImageS1(), m_height*m_width);
    memcpy(image_s2, request->getImageS2(), m_height*m_width);

    QDateTime time_stamp = request->getTimeStamp();

    saveStokesImages(image_s1, image_s2, time_stamp);
    free(image_s1);
    free(image_s2);

    request->releaseMemory();
}

void imageSaveDataExecutor::onSaveImage(imageSaveDataExecutorSaveMatImage *request){

    QDateTime time_stamp = request->getTimeStamp();
    cv::Mat image = request->getImage().clone();

    QString file_name  = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_thermal.png";

    cv::imwrite(file_name.toStdString(), image);

    m_is_available = true;

}

void imageSaveDataExecutor::onSaveImage(imageSaveDataExecutorSaveMatImageString *request){

    QDateTime time_stamp = request->getTimeStamp();
    cv::Mat image = request->getImage().clone();

    QString file_name  = m_path_to_save_images + request->getCamStr() + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_thermal.png";

    cv::imwrite(file_name.toStdString(), image);

    m_is_available = true;
}

void imageSaveDataExecutor::onSaveDataPng(imageSaveDataExecutorSaveDataPngRgbRequest *request){
    try{
        uint8_t *image_data_pointer = (uint8_t*) malloc(m_height*m_width*3);
        memcpy(image_data_pointer,request->getImageDataPointer(),(m_height*m_width*3));

        int image_type = request->getImageType();
        QDateTime time_stamp = request->getTimeStamp();

        saveDataPngRgb(image_data_pointer, image_type, time_stamp);

        free(image_data_pointer);
        request->freeMemory();
    }catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::onSaveDataPng";
    }
}

void imageSaveDataExecutor::onSaveDataPng(imageSaveDataExecutorSaveMultiDataPngRequest *request){
    try{
        uint8_t *image_0_pointer   = (uint8_t*) malloc(m_height*m_width);
        uint8_t *image_45_pointer  = (uint8_t*) malloc(m_height*m_width);
        uint8_t *image_90_pointer  = (uint8_t*) malloc(m_height*m_width);
        uint8_t *image_135_pointer = (uint8_t*) malloc(m_height*m_width);

        memcpy(image_0_pointer, request->getImage0DataPointer(),(m_height*m_width));
        memcpy(image_45_pointer, request->getImage45DataPointer(),(m_height*m_width));
        memcpy(image_90_pointer, request->getImage90DataPointer(),(m_height*m_width));
        memcpy(image_135_pointer, request->getImage135DataPointer(),(m_height*m_width));

        QDateTime time_stamp = request->getTimeStamp();

        saveDataPng(image_0_pointer, image_45_pointer, image_90_pointer, image_135_pointer, time_stamp);

        free(image_0_pointer);
        free(image_45_pointer);
        free(image_90_pointer);
        free(image_135_pointer);

        request->freeMemory();
    }catch(...){
        qDebug()<<"Unhandled error at imageSaveDataExecutor::onSaveDataPng";
    }
}

void imageSaveDataExecutor::onSavePointerToPng(imageSaveDataExecutorSavePointerToPngRequest *request){

    int size_to_save = request->getImageHeight()*request->getImageWidth()*request->getImageChannels()*sizeof(uint8_t);
    uint8_t *image_buffer = (uint8_t*)malloc(size_to_save);
    memcpy(image_buffer, request->getImagePointer(), size_to_save);

    savePointerToPng(image_buffer, request->getImageWidth(), request->getImageHeight(), request->getImageChannels(), request->getImageName());

    free(image_buffer);
    request->releaseMemory();

    sendSavePointerToPngResponse();
}

void imageSaveDataExecutor::convertYuv2Rgb(uint8_t *src_pointer, uint8_t *dst_pointer, uint32_t buff_size)
{
    double y0_value, y1_value, u_value, v_value;
    double r_value, g_value, b_value;

    int rgb_index = 0;

    for(uint32_t i=0; i<buff_size; ++i){

        y0_value = src_pointer[i];
        u_value =  src_pointer[i+1] - 127;
        y1_value = src_pointer[i+2];
        v_value =  src_pointer[i+3] -127;
        i+=3;

        r_value =  (y0_value + (1.5701 * v_value));
        g_value =  (y0_value - (0.1870 * u_value) - (0.4664 * v_value));
        b_value =  (y0_value + (1.8556 * u_value));


        if( r_value < 0) {r_value = 0; }
        if( r_value > 255 ) {r_value = 255;}
        if( g_value < 0) {g_value = 0;}
        if( g_value > 255 ) {g_value = 255;}
        if( b_value < 0) {b_value = 0;}
        if( b_value > 255 ) {b_value = 255;}

        dst_pointer[rgb_index] =   (uint8_t)b_value;
        dst_pointer[rgb_index+1] = (uint8_t)g_value;
        dst_pointer[rgb_index+2] = (uint8_t)r_value;

        r_value = (y1_value + (1.5701 * v_value));
        g_value = (y1_value - (0.1870 * u_value) - (0.4664 * v_value));
        b_value = (y1_value + (1.8556 * u_value));

        rgb_index+=3;

        if( r_value < 0) {r_value = 0;}
        if( r_value > 255 ) {r_value = 255;}
        if( g_value < 0) {g_value = 0;}
        if( g_value > 255 ) {g_value = 255;}
        if( b_value < 0) {b_value = 0;}
        if( b_value > 255 ) {b_value = 255;}

        dst_pointer[rgb_index] =   (uint8_t)b_value;
        dst_pointer[rgb_index+1] = (uint8_t)g_value;
        dst_pointer[rgb_index+2] = (uint8_t)r_value;

        rgb_index+=3;
    }
}

void imageSaveDataExecutor::savePointerToPng(uint8_t *image_pointer, uint16_t width, uint16_t height, uint8_t channels, QString file_name)
{
    cv::Mat image_to_save;

    QString ext = ".png";

    if(m_save_lossless){
        ext = ".bmp";
    }

    m_full_path_saved = m_path_to_save_images + file_name + ext;
    uint8_t* final_pointer = NULL;

    switch(channels){
    case 1:
        //!mono
        image_to_save = cv::Mat(height, width, CV_8UC1, image_pointer);
        cv::imwrite(m_full_path_saved.toStdString(), image_to_save);
        break;
    case 2:
        //!convert from YUV to RGB
        final_pointer = (uint8_t*)malloc(sizeof(uint8_t)*(height*width*3));
        convertYuv2Rgb(image_pointer, final_pointer, (height*width*2));
        image_to_save = cv::Mat(height, width, CV_8UC3, final_pointer);
        cv::imwrite(m_full_path_saved.toStdString(), image_to_save);
        free(final_pointer);
        break;
    case 3:
        //!rgb
        image_to_save = cv::Mat(height, width, CV_8UC3, image_pointer);
        cv::cvtColor(image_to_save, image_to_save, cv::COLOR_RGB2BGR);
        cv::imwrite(m_full_path_saved.toStdString(), image_to_save);
        break;
    case 4:
        //!thermal (rgba)
        image_to_save = cv::Mat(height, width, CV_8UC4, image_pointer);
        cv::imwrite(m_full_path_saved.toStdString(), image_to_save);
        break;
    }

    m_is_available = true;

}

void imageSaveDataExecutor::sendSavePointerToPngResponse()
{
    imageSaveDataExecutorSavePointerToPngResponse *response = new imageSaveDataExecutorSavePointerToPngResponse(m_full_path_saved);
    sendEvent(response->TYPE,response);
}

void imageSaveDataExecutor::onSaveFloatPointer(imageSaveDataExecutorSaveFloatBufferRequest *request)
{
    int size_to_save = request->getBufferSize();

    float *data_buffer = (float*)malloc(size_to_save);
    memcpy(data_buffer, request->getBuffer(), size_to_save);

    saveFloatPointer(data_buffer, size_to_save, request->getFileName());

    free(data_buffer);
    request->releaseMemory();

    m_is_available = true;
    sendSaveFloatBufferResponse();

}

void imageSaveDataExecutor::saveFloatPointer(float *data_buffer, int size_to_save, QString file_name)
{
    QString final_name = m_path_to_save_images + file_name + ".bin";

    FILE *file_handler = std::fopen(final_name.toStdString().c_str(), "wb");

    //! write all data to file
    std::fwrite(data_buffer, size_to_save, 1, file_handler);

    std::fclose(file_handler);
}

void imageSaveDataExecutor::sendSaveFloatBufferResponse()
{
    imageSaveDataExecutorSavePointerToPngResponse *response = new imageSaveDataExecutorSavePointerToPngResponse(m_full_path_saved);
    sendEvent(response->TYPE,response);
}

int imageSaveDataExecutor::saveDataPng(uint8_t *image_0_data, uint8_t *image_45_data, uint8_t *image_90_data, uint8_t *image_135_data, const QDateTime &time_stamp){
    try{
        uint8_t *image_0_pointer = (uint8_t*) malloc(m_height*m_width);
        memcpy(image_0_pointer,image_0_data,(m_height*m_width));

        uint8_t *image_45_pointer = (uint8_t*) malloc(m_height*m_width);
        memcpy(image_45_pointer,image_45_data,(m_height*m_width));

        uint8_t *image_90_pointer = (uint8_t*) malloc(m_height*m_width);
        memcpy(image_90_pointer,image_90_data,(m_height*m_width));

        uint8_t *image_135_pointer = (uint8_t*) malloc(m_height*m_width);
        memcpy(image_135_pointer,image_135_data,(m_height*m_width));


        QString file_name_0_deg   = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_0_deg.png";
        QString file_name_45_deg  = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_45_deg.png";
        QString file_name_90_deg  = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_90_deg.png";
        QString file_name_135_deg = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_135_deg.png";


        cv::Mat image_to_save_0(m_height, m_width, CV_8UC1, image_0_pointer);
        cv::Mat image_to_save_45(m_height, m_width, CV_8UC1, image_45_pointer);
        cv::Mat image_to_save_90(m_height, m_width, CV_8UC1, image_90_pointer);
        cv::Mat image_to_save_135(m_height, m_width, CV_8UC1, image_135_pointer);

        cv::imwrite(file_name_0_deg.toStdString(),  image_to_save_0);
        cv::imwrite(file_name_45_deg.toStdString(),  image_to_save_45);
        cv::imwrite(file_name_90_deg.toStdString(),  image_to_save_90);
        cv::imwrite(file_name_135_deg.toStdString(), image_to_save_135);

        free(image_0_pointer);
        free(image_45_pointer);
        free(image_90_pointer);
        free(image_135_pointer);


    }catch (...){
        return -1;
    }

    m_is_available = true;
    return 0;

}

int imageSaveDataExecutor::saveDataPngRgb(uint8_t* image_data, const int &image_type, const QDateTime &time_stamp){
    try{
        int data_size = m_height*m_width*3;
        uint8_t* image_copied = (uint8_t*)malloc(data_size);
        memcpy(image_copied, image_data, data_size);
        cv::Mat image_to_save(m_height, m_width, CV_8UC3, image_copied);

        QString ext = ".png";
        if(m_save_lossless){
            ext = ".bmp";
        }

        if(image_type == 2){ //aolp
            QString file_name_aolp = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_aolp" + ext;
            cv::imwrite(file_name_aolp.toStdString(),image_to_save);
        }else if(image_type == 4){ //hsv
            QString file_name_hsv = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_hsv" + ext;
            cv::imwrite(file_name_hsv.toStdString(),image_to_save);
        }
        else if(image_type == 0){
            QString file_name = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + ext;
            cv::imwrite(file_name.toStdString(),image_to_save);
        }

        free(image_copied);
    }catch(...){
        qDebug()<<"Unhandled error at saveDataExecutor::saveDataPng";
    }

    m_is_available = true;
    return 0;
}


int imageSaveDataExecutor::saveStokesImages(uint8_t* image_s1_data, uint8_t* image_s2_data, const QDateTime &time_stamp){
    try{
        int data_size = m_height*m_width;
        uint8_t* image_s1 = (uint8_t*)malloc(data_size);
        uint8_t* image_s2 = (uint8_t*)malloc(data_size);

        memcpy(image_s1, image_s1_data, data_size);
        memcpy(image_s2, image_s2_data, data_size);

        cv::Mat image_s1_cv = cv::Mat(m_height, m_width, CV_8UC1, image_s1);
        cv::Mat image_s2_cv = cv::Mat(m_height, m_width, CV_8UC1, image_s2);

        cv::Mat image_s1_color = cv::Mat(m_height, m_width, CV_8UC3);
        cv::Mat image_s2_color = cv::Mat(m_height, m_width, CV_8UC3);

        cv::applyColorMap(image_s1_cv, image_s1_color, cv::COLORMAP_JET);
        cv::applyColorMap(image_s2_cv, image_s2_color, cv::COLORMAP_JET);

        cv::cvtColor(image_s1_color, image_s1_color, cv::COLOR_BGR2RGB);
        cv::cvtColor(image_s2_color, image_s2_color, cv::COLOR_BGR2RGB);

        QString file_name_s1 = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_S1.png";
        QString file_name_s2 = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_S2.png";

        cv::imwrite(file_name_s1.toStdString(),image_s1_color);
        cv::imwrite(file_name_s2.toStdString(),image_s2_color);

        free(image_s1);
        free(image_s2);
    }catch(...){
        qDebug()<<"Unhandled error at saveDataExecutor::saveStokesImages";
    }

    m_is_available = true;
    return 0;
}

int imageSaveDataExecutor::saveDataPng(uint8_t *image_data, const int &image_type, const QDateTime &time_stamp){

    int ret = 0;
    try{

        QString ext = ".png";
        if(m_save_lossless){
            ext = ".bmp";
        }

        QString file_name;

        if(image_type == 0){

            file_name = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + ext;

            uint8_t *image_data_pointer = (uint8_t*) malloc(m_height*m_width);
            memcpy(image_data_pointer,image_data,(m_height*m_width));

            cv::Mat image_cv = cv::Mat(m_height, m_width, CV_8UC1, image_data_pointer);
            cv::imwrite(file_name.toStdString(), image_cv);

            free(image_data_pointer);

        }else if(image_type == 3){

            int data_size = m_height*m_width;
            uint8_t* image_dolp = (uint8_t*)malloc(data_size);

            memcpy(image_dolp, image_data, data_size);

            cv::Mat image_cv = cv::Mat(m_height, m_width, CV_8UC1, image_dolp);

            cv::Mat image_dolp_color = cv::Mat(m_height, m_width, CV_8UC3);

            cv::applyColorMap(image_cv, image_dolp_color, cv::COLORMAP_JET);

            //cv::cvtColor(image_dolp_color, image_dolp_color, cv::COLOR_BGR2RGB);

            file_name = m_path_to_save_images + time_stamp.toString("yyyyMMdd_hhmmss_zzz") + "_dolp" + ext;

            cv::imwrite(file_name.toStdString(),image_dolp_color);

            free(image_dolp);

        }

        ret = 0;

    }catch (...){

        ret =  -1;
    }

    m_is_available = true;

    return ret;
}
