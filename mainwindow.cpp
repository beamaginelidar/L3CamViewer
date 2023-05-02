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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

Q_DECLARE_METATYPE(uint16_t)
Q_DECLARE_METATYPE(uint8_t)
Q_DECLARE_METATYPE(int32_t)
Q_DECLARE_METATYPE(std::vector<detectionImage>)
Q_DECLARE_METATYPE(uint32_t)

#include <QSpinBox>

bool dev_initialized = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_status_connected_style  = "border-radius: 5px; background-color: rgb(114,255,158); color:rgb(0,0,0);";
    m_status_error_style = "border-radius: 5px; background-color: rgb(255,114,114); color:rgb(0,0,0);";
    m_status_alarm_style = "border-radius: 5px; background-color: rgb(255,255,158); color:rgb(0,0,0);";
    m_status_searching_style = "border-radius: 5px; background-color: rgb(114,158,255); color:rgb(0,0,0);";
    m_status_undefined_style = "border-radius: 5px; background-color: rgb(190,190,190); color:rgb(0,0,0);";

    ui->label_image_rgb->setBackgroundRole(QPalette::Base);
    ui->label_image_rgb->setScaledContents(true);

    ui->label_image_thermal->setBackgroundRole(QPalette::Base);
    ui->label_image_thermal->setScaledContents(true);


    ui->label_l3cam_status->setStyleSheet(m_status_undefined_style);

    m_searching_device = false;
    m_searching_status = false;
    m_search_timer = new QTimer();
    connect(m_search_timer, SIGNAL(timeout()), this, SLOT(searchTimerTimeOut()));
    connect(ui->spinBox_max_temperature, SIGNAL(valueChanged(int)), this, SLOT(valueChangedCode(int)));

    m_devices_connected = 0;
    m_sensors_connected = 0;

    m_rgb_sensor = NULL;
    m_thermal_sensor = NULL;
    m_lidar_sensor = NULL;
    m_allied_wide_sensor = NULL;
    m_allied_narrow_sensor = NULL;
    m_pol_sensor = NULL;

    m_device_started = false;
    m_device_streaming = false;

    m_rgb_image_reader = new udpReceiverController();
    m_thermal_image_reader = new udpReceiverController();
    m_pointcloud_reader = new udpReceiverController();
    m_rgb_pol_image_reader = new udpReceiverController();

    m_rgb_port = 6020;
    m_thermal_port = 6030;
    m_pcd_port = 6050;
    m_rgbp_port = 6060;

    m_max_distance = 180000;
    m_min_distance = 1500;
    m_max_intensity = 3000;
    m_min_intensity = 0;

    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<int32_t>("int32_t");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<std::vector<detectionImage> >("std::vector<detectionImage>");

    connect(m_pointcloud_reader, SIGNAL(pointcloudReadyToShow(int32_t*, uint32_t)), this, SLOT(pointCloudReadyToShow(int32_t*, uint32_t)));

    connect(m_rgb_image_reader, SIGNAL(imageRgbReadyToShow(uint8_t*, uint16_t, uint16_t, uint8_t, std::vector<detectionImage>, uint32_t)),
            this, SLOT(imageRgbReadyToShow(uint8_t*, uint16_t, uint16_t, uint8_t, std::vector<detectionImage>, uint32_t)));

    connect(m_thermal_image_reader, SIGNAL(imageRgbReadyToShow(uint8_t*, uint16_t, uint16_t, uint8_t, std::vector<detectionImage>, uint32_t )),
            this, SLOT(imageThermalReadyToShow(uint8_t*, uint16_t, uint16_t, uint8_t, std::vector<detectionImage>, uint32_t)));

    connect(m_rgb_pol_image_reader, SIGNAL(imageRgbReadyToShow(uint8_t*, uint16_t, uint16_t, uint8_t, std::vector<detectionImage>, uint32_t )),
            this, SLOT(imageRgbPolReadyToShow(uint8_t *, uint16_t , uint16_t , uint8_t , std::vector<detectionImage> , uint32_t )));

    m_thermal_status = deviceStatus::undefined;
    m_polarimetric_status = deviceStatus::undefined;
    m_rgb_status = deviceStatus::undefined;
    m_lidar_status = deviceStatus::undefined;
    m_allied_wide_status = deviceStatus::undefined;
    m_allied_narrow_status = deviceStatus::undefined;

    updateSensorStatus(m_thermal_status, ui->label_thermal_1_status_value);
    updateSensorStatus(m_polarimetric_status, ui->label_polarimetric_status_value);
    updateSensorStatus(m_rgb_status, ui->label_rgb_status_value);
    updateSensorStatus(m_lidar_status, ui->label_lidar_status_value);
    updateSensorStatus(m_allied_wide_status, ui->label_allied_wide_status_value);
    updateSensorStatus(m_allied_narrow_status, ui->label_allied_narrow_status_value);

    initializeGUI();

    m_library_version = QString("%1").arg(GET_VERSION());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::deviceDetected()
{
    ui->pushButton_start_device->setEnabled(true);

    ui->label_l3cam_status->setText("Connected");
    ui->label_l3cam_status->setStyleSheet(m_status_connected_style);

    int32_t status = 0;
    int error = 0;

    error = GET_DEVICE_STATUS(m_devices[0], &status);

    if(error != L3CAM_OK){
        qDebug()<<QString("Device status error %1 - %2").arg(error).arg(getBeamErrorDescription(error));
        addMessageToLogWindow(QString("Device status error %1 - %2").arg(error).arg(getBeamErrorDescription(error)) , logType::error);
        return;
    }

    qDebug()<<"Device status "<<status;
    uint8_t statusType = logType::verbose;
    if(status == 1 || status == 5 || status == 10)
        statusType = logType::warning;
    if(status == 2 || status == 3 || status == 6 || status == 7)
        statusType = logType::error;
    addMessageToLogWindow("Device status " + QString::number(status), statusType);

    error = GET_SENSORS_AVAILABLE(m_devices[0], m_sensors, &m_sensors_connected);

    qDebug()<<"Found"<<m_sensors_connected<<"sensors in device";
    addMessageToLogWindow("Found " + QString::number(m_sensors_connected) + " sensors in device");

    m_server_address = QString("%1").arg(GET_LOCAL_SERVER_ADDRESS(m_devices[0]));

    for(int i=0; i<m_sensors_connected; ++i){
        switch (m_sensors[i].sensor_type) {
        case sensor_lidar:
            m_lidar_sensor = &m_sensors[i];
            break;
        case sensor_econ_rgb:
            m_rgb_sensor = &m_sensors[i];
            break;
        case sensor_thermal:
            m_thermal_sensor = &m_sensors[i];
            break;
        case sensor_pol:
            m_pol_sensor = &m_sensors[i];
            break;
        case sensor_allied_wide:
            m_allied_wide_sensor = &m_sensors[i];
            break;
        case sensor_allied_narrow:
            m_allied_narrow_sensor = &m_sensors[i];
            break;
        }
    }

    initializeSystemStatus();

    initializeReceivers();
}

void MainWindow::initializeGUI()
{
    initializeRgbDefault();

    ui->pushButton_start_device->setEnabled(false);
    ui->pushButton_start_streaming->setEnabled(false);

    m_point_cloud_viewer = new pclPointCloudViewerController();
    m_point_cloud_viewer->startController();
    m_point_cloud_viewer->setAxisEnabled(true);

    (m_thermal_sensor != NULL) ? m_thermal_status = deviceStatus::no_error : m_thermal_status = deviceStatus::error_s;
    (m_lidar_sensor != NULL) ? m_lidar_status = deviceStatus::no_error : m_lidar_status = deviceStatus::error_s;
    (m_pol_sensor != NULL) ? m_polarimetric_status = deviceStatus::no_error : m_polarimetric_status = deviceStatus::error_s;
    (m_rgb_sensor != NULL) ? m_rgb_status = deviceStatus::no_error : m_rgb_status = deviceStatus::error_s;
    (m_allied_wide_sensor != NULL) ? m_allied_wide_status = deviceStatus::no_error : m_allied_wide_status = deviceStatus::error_s;
    m_allied_narrow_status = deviceStatus::error_s;

}

void MainWindow::initializeRgbDefault()
{
    m_rgb_contrast = 10;
    m_rgb_brightness = 0;
    m_rgb_staturation = 16;
    m_rgb_sharpness = 16;
    m_rgb_gamma = 220;
    m_rgb_gain = 0;
    m_rgb_white_balance = 5000;
    m_rgb_exposure_time = 156;

    m_rgb_auto_exp = true;
    m_rgb_auto_white = true;

    ui->horizontalSlider_rgb_bri->setValue(m_rgb_brightness);
    ui->horizontalSlider_contrast->setValue(m_rgb_contrast);
    ui->horizontalSlider_saturation->setValue(m_rgb_staturation);
    ui->horizontalSlider_sharpness->setValue(m_rgb_sharpness);
    ui->horizontalSlider_gamma->setValue(m_rgb_gamma);
    ui->horizontalSlider_gain->setValue(m_rgb_gain);
    ui->horizontalSlider_white_balance->setValue(m_rgb_white_balance);
    ui->horizontalSlider_exposure->setValue(m_rgb_exposure_time);

    ui->horizontalSlider_white_balance->setDisabled(m_rgb_auto_white);
    ui->horizontalSlider_exposure->setDisabled(m_rgb_auto_exp);

    ui->checkBox_rgb_exp->setChecked(m_rgb_auto_exp);
    ui->checkBox_rgb_white_bal->setChecked(m_rgb_auto_white);

}

void MainWindow::initializeReceivers()
{
    if(m_lidar_sensor != NULL){

        m_pointcloud_reader->setIpAddress(m_server_address);
        m_pointcloud_reader->setPort(m_pcd_port);
        m_pointcloud_reader->doReadPointcloud(true);
        m_pointcloud_reader->startController();
    }

    if(m_rgb_sensor != NULL || m_allied_narrow_sensor != NULL){

        ui->radioButton_fusion_mono->setEnabled(true);

        m_rgb_image_reader->setIpAddress(m_server_address);
        m_rgb_image_reader->setPort(m_rgb_port);
        m_rgb_image_reader->doReadImageRgb(true);
        m_rgb_image_reader->startController();
    }

    if(m_allied_wide_sensor!= NULL || m_pol_sensor != NULL){

        m_rgb_pol_image_reader->setIpAddress(m_server_address);
        m_rgb_pol_image_reader->setPort(m_rgbp_port);
        m_rgb_pol_image_reader->doReadImageRgb(true);
        m_rgb_pol_image_reader->startController();
    }

    if(m_thermal_sensor != NULL){
        ui->radioButton_fusion_thermal->setEnabled(true);

        m_thermal_image_reader->setIpAddress(m_server_address);
        m_thermal_image_reader->setPort(m_thermal_port);
        m_thermal_image_reader->doReadImageRgb(true);
        m_thermal_image_reader->startController();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(dev_initialized){
        int ret = QMessageBox::warning(this, "Terminate",
                                       "Are you sure you want to terminate?\n\n"
                                       "The app will close.",
                                       QMessageBox::No | QMessageBox::Yes,
                                       QMessageBox::Yes);

        if(ret == QMessageBox::Yes){
            int error = L3CAM_OK;
            if(m_device_streaming){
                error = STOP_STREAM(m_devices[0]);
                if(error == L3CAM_OK){
                    ui->pushButton_start_device->setEnabled(true);
                    ui->pushButton_start_streaming->setText("START STREAMING");
                    m_device_streaming = false;
                }else{
                    qDebug()<<"Stop stream response -"<<error<<getBeamErrorDescription(error);
                    addMessageToLogWindow("Stop stream response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
                    return;
                }
            }

            if(m_device_started){
                error = STOP_DEVICE(m_devices[0]);
                if(error == L3CAM_OK){
                    ui->pushButton_start_device->setText("START DEVICE");
                    m_device_started = false;
                    ui->pushButton_start_streaming->setEnabled(false);
                }else{
                    qDebug()<<"Stop device response -"<<error<<getBeamErrorDescription(error);
                    addMessageToLogWindow("Stop device response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
                    return;
                }
            }

            TERMINATE(m_devices[0]);

            ui->label_l3cam_status->setText("DISCONNECTED");
            ui->label_l3cam_status->setStyleSheet(m_status_undefined_style);

            event->accept();
        }
        else {
            event->ignore();
        }
    }
    else{
        event->accept();
    }
}

void MainWindow::on_pushButton_getVersion_clicked()
{
    const char *version = GET_VERSION();
    qDebug()<<version;
    addMessageToLogWindow(QString(version));
}

void MainWindow::on_pushButton_initialize_clicked()
{
    int error = INITIALIZE();
    qDebug()<<"initializing response "<<error;
    addMessageToLogWindow("initializing response " + QString::number(error), (error) ? logType::error : logType::verbose);

    if(!error){
        dev_initialized = true;
    }
}

void MainWindow::on_pushButton_findDevices_clicked()
{

    m_devices_connected = 0;
    int error = FIND_DEVICES(m_devices, &m_devices_connected);

    qDebug()<<"Finding devices response -"<<error<<"- num devices found"<<m_devices_connected;
    addMessageToLogWindow("Finding devices response - " + QString::number(error) + " - num devices found " + QString::number(m_devices_connected), (error) ? logType::error : logType::verbose);

    if(m_devices_connected != 0){
        qDebug()<<"Device address "<<m_devices[0].ip_address;
        addMessageToLogWindow("Device address " + QString(m_devices[0].ip_address));

        ui->pushButton_start_device->setEnabled(true);

        ui->label_l3cam_status->setText("Connected");
        ui->label_l3cam_status->setStyleSheet(m_status_connected_style);

    }
}

void MainWindow::on_pushButton_status_clicked()
{
    int32_t status = 0;
    int error = GET_DEVICE_STATUS(m_devices[0], &status);

    qDebug()<<"system status response -"<<error<<" - status -"<<status;
    uint8_t statusType = logType::verbose;
    if(status == 1 || status == 5 || status == 10)
        statusType = logType::warning;
    if(status == 2 || status == 3 || status == 6 || status == 7)
        statusType = logType::error;
    addMessageToLogWindow("Device status " + QString::number(status), statusType);
}

void MainWindow::on_pushButton_sensors_clicked()
{
    int error = GET_SENSORS_AVAILABLE(m_devices[0], m_sensors, &m_sensors_connected);

    qDebug()<<"Get sensors available response - "<<error;
    addMessageToLogWindow("Get sensors available response - " + QString::number(error), (error) ? logType::error : logType::verbose);

    qDebug()<<"Num sensors available -"<<m_sensors_connected;
    addMessageToLogWindow("Num sensors available - " + QString::number(m_sensors_connected));

    m_server_address = QString("%1").arg(GET_LOCAL_SERVER_ADDRESS(m_devices[0]));


    for(int i=0; i<m_sensors_connected; ++i){
        switch (m_sensors[i].sensor_type) {
        case sensor_lidar:
            m_lidar_sensor = &m_sensors[i];
            break;
        case sensor_econ_rgb:
            m_rgb_sensor = &m_sensors[i];
            break;
        case sensor_thermal:
            m_thermal_sensor = &m_sensors[i];
            break;
        case sensor_pol:
            m_pol_sensor = &m_sensors[i];
            break;
        case sensor_allied_wide:
            m_allied_wide_sensor = &m_sensors[i];
            break;
        case sensor_allied_narrow:
            m_allied_narrow_sensor = &m_sensors[i];
            break;

        }
    }

    initializeSystemStatus();

    initializeReceivers();
}

void MainWindow::on_pushButton_fast_init_clicked()
{
    //!set searching status GUI
    int error = 0;
    error = INITIALIZE();
    qDebug()<<"INITIALIZE -"<<error;
    addMessageToLogWindow("INITIALIZE - " + QString::number(error), (error) ? logType::error : logType::verbose);

    if(!error){
        dev_initialized = true;
    }

    if(error != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(error);
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
        return;
    }

    //!start searching thread
    ui->label_l3cam_status->setText("Searching...");
    m_search_timer->start(500);

    m_searching_device = true;
    m_searching_status = true;
}

void MainWindow::searchTimerTimeOut()
{
    m_search_timer->stop();

    if(m_searching_device){
        m_searching_status = !m_searching_status;
        (m_searching_status) ?  ui->label_l3cam_status->setStyleSheet(m_status_undefined_style) : ui->label_l3cam_status->setStyleSheet(m_status_searching_style);

        FIND_DEVICES(m_devices, &m_devices_connected);

        if(m_devices_connected > 0){
            m_searching_device = false;

            qDebug()<<"Device address "<<m_devices[0].ip_address;
            addMessageToLogWindow("Device address " + QString(m_devices[0].ip_address));

            deviceDetected();
            int error = L3CAM_OK;
            error = START_DEVICE(m_devices[0]);
            if(error == L3CAM_OK){
                ui->pushButton_start_device->setText("STOP DEVICE");
                m_device_started = true;
                ui->pushButton_start_streaming->setEnabled(true);

                error = START_STREAM(m_devices[0]);
                if(error == L3CAM_OK){
                    ui->pushButton_start_device->setEnabled(false);
                    ui->pushButton_start_streaming->setText("STOP STREAMING");
                    m_device_streaming = true;
                }else{
                    qDebug()<<"Start stream response -"<<error<<getBeamErrorDescription(error);
                    addMessageToLogWindow("Start stream response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
                    m_device_streaming = false;
                }
            }else{
                qDebug()<<"Start device response -"<<error<<getBeamErrorDescription(error);
                addMessageToLogWindow("Start device response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
            }
        }else{
            m_search_timer->start(500);
        }
    }
}

void MainWindow::on_checkBox_enable_temp_filter_clicked(bool checked)
{
    int error = ENABLE_THERMAL_CAMERA_TEMPERATURE_FILTER(m_devices[0], checked);
    if(error != L3CAM_OK){
        qDebug()<<"Error thermal filter"<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error thermal filter " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_apply_thermal_filter_clicked()
{
    int error = CHANGE_THERMAL_CAMERA_TEMPERATURE_FILTER(m_devices[0], ui->spinBox_min_temperature->value(), ui->spinBox_max_temperature->value());
    if(error != L3CAM_OK){
        qDebug()<<"Error changing thermal filter values"<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error changing thermal filter values " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_comboBox_thermal_color_currentIndexChanged(int index)
{
    thermalTypes value = thermal_WHITE;

    switch(index){
    case 0:
        value = thermal_WHITE;
        break;
    case 1:
        value = thermal_BLACK;
        break;
    case 2:
        value = thermal_IRON;
        break;
    case 3:
        value = thermal_COOL;
        break;
    case 4:
        value = thermal_AMBER;
        break;
    case 5:
        value = thermal_INDIGO;
        break;
    case 6:
        value = thermal_TYRIAN;
        break;
    case 7:
        value = thermal_GLORY;
        break;
    case 8:
        value = thermal_ENVY;
        break;
    case 9:
        value = thermal_WHITE_NEW;
        break;
    case 10:
        value = thermal_BLACK_NEW;
        break;
    case 11:
        value = thermal_SPECTRA;
        break;
    case 12:
        value = thermal_PRISM;
        break;
    case 13:
        value = thermal_TYRIAN_NEW;
        break;
    case 14:
        value = thermal_AMBER_NEW;
        break;
    case 15:
        value = thermal_IRON_NEW;
        break;
    case 16:
        value = thermal_HI;
        break;
    case 17:
        value = thermal_HILO;
        break;
    }

    int error = CHANGE_THERMAL_CAMERA_COLORMAP(m_devices[0], value);

    if(error != L3CAM_OK){
        qDebug()<<"Error changing thermal colormap - "<<error<<" - "<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error changing thermal colormap - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_checkBox_rgb_white_bal_clicked(bool checked)
{
    m_rgb_auto_white = checked;
    ui->horizontalSlider_white_balance->setDisabled(checked);
    ENABLE_RGB_CAMERA_AUTO_WHITE_BALANCE(m_devices[0], checked);
}

void MainWindow::on_checkBox_rgb_exp_clicked(bool checked)
{
    m_rgb_auto_exp = checked;
    ui->horizontalSlider_exposure->setDisabled(checked);
    ENABLE_RGB_CAMERA_AUTO_EXPOSURE_TIME(m_devices[0], checked);
}

void MainWindow::on_pushButton_set_rgb_protocol_clicked()
{
    streamingProtocols protocol;
    switch(ui->comboBox_rgb_protocol->currentIndex()){
    case 0:
        protocol = protocol_raw_udp;
        break;
    case 1:
        protocol = protocol_gstreamer;
        break;
    default:
        protocol = protocol_raw_udp;
        break;
    }

    if(m_rgb_sensor != NULL){
        m_rgb_sensor->protocol = protocol;
        CHANGE_STREAMING_PROTOCOL(m_devices[0], m_rgb_sensor);
    }
}

void MainWindow::on_pushButton_get_rgb_pipeline_clicked()
{
    char *pipeline;
    GET_RTSP_PIPELINE(m_devices[0], *m_rgb_sensor, &pipeline);
    QString string_pipeline = QString("%1").arg(pipeline);

    ui->lineEdit_rgb_rtsp->setText(string_pipeline);
}

void MainWindow::on_pushButton_set_rgb_image_size_clicked()
{
    econResolutions cur_reso = reso_640_480;

    switch (ui->comboBox_rgb_image_size->currentIndex()) {
    case 0:
        cur_reso = reso_640_480;
        break;
    case 1:
        cur_reso = reso_1280_720;
        break;
    case 2:
        cur_reso = reso_1920_1080;
        break;
    }

    CHANGE_RGB_CAMERA_RESOLUTION(m_devices[0], cur_reso);
}

void MainWindow::on_pushButton_set_framerate_clicked()
{
    CHANGE_RGB_CAMERA_FRAMERATE(m_devices[0], ui->spinBox_rgb_framerate->value());
}

void MainWindow::on_horizontalSlider_rgb_bri_valueChanged(int value)
{
    ui->label_rgb_bri_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_contrast_valueChanged(int value)
{
    ui->label_contrast_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_saturation_valueChanged(int value)
{
    ui->label_saturation_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_sharpness_valueChanged(int value)
{
    ui->label_sharpness_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_gamma_valueChanged(int value)
{
    ui->label_gamma_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_gain_valueChanged(int value)
{
    ui->label_gain_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_white_balance_valueChanged(int value)
{
    ui->label_white_balance_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_exposure_valueChanged(int value)
{
    ui->label_exposure_val->setText(QString("%1").arg(value));
}

void MainWindow::on_pushButton_rgb_default_clicked()
{
    m_rgb_contrast = 10;
    m_rgb_brightness = 0;
    m_rgb_staturation = 16;
    m_rgb_sharpness = 16;
    m_rgb_gamma = 220;
    m_rgb_gain = 0;
    m_rgb_white_balance = 5000;
    m_rgb_exposure_time = 156;

    m_rgb_auto_exp = true;
    m_rgb_auto_white = true;

    ui->horizontalSlider_rgb_bri->setValue(m_rgb_brightness);
    ui->horizontalSlider_contrast->setValue(m_rgb_contrast);
    ui->horizontalSlider_saturation->setValue(m_rgb_staturation);
    ui->horizontalSlider_sharpness->setValue(m_rgb_sharpness);
    ui->horizontalSlider_gamma->setValue(m_rgb_gamma);
    ui->horizontalSlider_gain->setValue(m_rgb_gain);
    ui->horizontalSlider_white_balance->setValue(m_rgb_white_balance);
    ui->horizontalSlider_exposure->setValue(m_rgb_exposure_time);

    ui->horizontalSlider_white_balance->setDisabled(m_rgb_auto_white);
    ui->horizontalSlider_exposure->setDisabled(m_rgb_auto_exp);

    ui->checkBox_rgb_exp->setChecked(m_rgb_auto_exp);
    ui->checkBox_rgb_white_bal->setChecked(m_rgb_auto_white);

    int result = SET_RGB_CAMERA_DEFAULT_SETTINGS(m_devices[0]);

    if(result != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(result);
        addMessageToLogWindow(QString(getBeamErrorDescription(result)), logType::error);
    }
}


void MainWindow::on_horizontalSlider_rgb_bri_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_BRIGHTNESS(m_devices[0], ui->horizontalSlider_rgb_bri->value());
    if(error != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(error);
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_contrast_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_CONTRAST(m_devices[0], ui->horizontalSlider_contrast->value());
    if(error != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(error);
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_saturation_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_SATURATION(m_devices[0], ui->horizontalSlider_saturation->value());
    if(error != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(error);
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_sharpness_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_SHARPNESS(m_devices[0], ui->horizontalSlider_sharpness->value());
    if(error != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(error);
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_gamma_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_GAMMA(m_devices[0], ui->horizontalSlider_gamma->value());
    if(error != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(error);
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_gain_sliderReleased()
{
    int error =  CHANGE_RGB_CAMERA_GAIN(m_devices[0], ui->horizontalSlider_gain->value());
    if(error != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(error);
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_white_balance_sliderReleased()
{
    if(m_rgb_auto_white == false){
        int error = CHANGE_RGB_CAMERA_WHITE_BALANCE(m_devices[0], ui->horizontalSlider_white_balance->value());
        if(error != L3CAM_OK){
            qDebug()<<getBeamErrorDescription(error);
            addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
        }
    }
}

void MainWindow::on_horizontalSlider_exposure_sliderReleased()
{
    if(m_rgb_auto_exp == false){
        int error = CHANGE_RGB_CAMERA_EXPOSURE_TIME(m_devices[0], ui->horizontalSlider_exposure->value());
        if(error != L3CAM_OK){
            qDebug()<<getBeamErrorDescription(error);
            addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
        }
    }
}

void MainWindow::on_pushButton_power_off_clicked()
{
    POWER_OFF_DEVICE(m_devices[0]);
}

void MainWindow::on_pushButton_start_device_clicked()
{
    int error = 0;
    if(m_device_started){
        error = STOP_DEVICE(m_devices[0]);
        if(error == L3CAM_OK){
            ui->pushButton_start_device->setText("START DEVICE");
            m_device_started = false;
            ui->pushButton_start_streaming->setEnabled(false);
        }else{
            qDebug()<<"Stop device response -"<<error<<getBeamErrorDescription(error);
            addMessageToLogWindow("Stop device response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
        }
    }else{
        error = START_DEVICE(m_devices[0]);
        if(error == L3CAM_OK){
            ui->pushButton_start_device->setText("STOP DEVICE");
            m_device_started = true;
            ui->pushButton_start_streaming->setEnabled(true);
        }else{
            qDebug()<<"Start device response -"<<error<<getBeamErrorDescription(error);
            addMessageToLogWindow("Start device response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
        }
    }
}

void MainWindow::on_pushButton_start_streaming_clicked()
{
    int error = 0;

    if(m_device_streaming){
        error = STOP_STREAM(m_devices[0]);
        if(error == L3CAM_OK){
            ui->pushButton_start_device->setEnabled(true);
            ui->pushButton_start_streaming->setText("START STREAMING");
            m_device_streaming = false;
        }else{
            qDebug()<<"Stop stream response -"<<error<<getBeamErrorDescription(error);
            addMessageToLogWindow("Stop stream response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
        }
    }else{
        error = START_STREAM(m_devices[0]);
        if(error == L3CAM_OK){
            ui->pushButton_start_device->setEnabled(false);
            ui->pushButton_start_streaming->setText("STOP STREAMING");
            m_device_streaming = true;
        }else{
            qDebug()<<"Start stream response -"<<error<<getBeamErrorDescription(error);
            addMessageToLogWindow("Start device response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
            m_device_streaming = false;
        }
    }
}

void MainWindow::on_radioButton_range3D_clicked(bool checked)
{
    int error = 0;
    ui->label_max_range->setText("Max distance:");
    ui->label_min_range->setText("Min distance:");
    ui->spinBox_max_range->setSuffix(" mm");
    ui->spinBox_min_range->setSuffix(" mm");
    if(checked){
        error = CHANGE_POINT_CLOUD_COLOR(m_devices[0], RAINBOW);
    }
    qDebug()<<"change pointcloud color response -"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow("Start device response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::on_radioButton_range3DZ_clicked(bool checked)
{
    int error = 0;
    ui->label_max_range->setText("Max distance:");
    ui->label_min_range->setText("Min distance:");
    ui->spinBox_max_range->setSuffix(" mm");
    ui->spinBox_min_range->setSuffix(" mm");
    if(checked){
        error = CHANGE_POINT_CLOUD_COLOR(m_devices[0], RAINBOW_Z);
    }
    qDebug()<<"change fusion pointcloud color response -"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow("change fusion pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::on_radioButton_intensity_clicked(bool checked)
{
    int error = 0;
    if(checked){
        error = CHANGE_POINT_CLOUD_COLOR(m_devices[0], INTENSITY);
    }
    ui->label_max_range->setText("Max intensity:");
    ui->label_min_range->setText("Min intensity:");
    ui->spinBox_max_range->setSuffix("");
    ui->spinBox_min_range->setSuffix("");
    qDebug()<<"change fusion pointcloud color response -"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow("change fusion pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::on_radioButton_fusion_thermal_clicked(bool checked)
{
    int error = 0;
    if(checked){
        error = CHANGE_POINT_CLOUD_COLOR(m_devices[0], THERMAL_FUSION);
    }
    qDebug()<<"change fusion pointcloud color response -"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow("change fusion pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::on_radioButton_fusion_mono_clicked(bool checked)
{
    int error = 0;
    if(checked){
        error = CHANGE_POINT_CLOUD_COLOR(m_devices[0], RGB_FUSION);
    }
    qDebug()<<"change fusion pointcloud color response -"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow("change fusion pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::pointCloudReadyToShow(int32_t *pointcloud, uint32_t timestamp)
{
    Q_UNUSED(timestamp)
    if(m_device_started){
        m_point_cloud_viewer->doShowPointCloud(pointcloud);
    }

    free(pointcloud);
}

void MainWindow::imageRgbReadyToShow(uint8_t *image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp)
{
    cv::Mat image_to_show;

    if(m_device_started){

        if(channels == 1){
            image_to_show = cv::Mat(height, width, CV_8UC1, image_data);
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_GRAY2RGB);
        }
        else if(channels == 3){
            image_to_show = cv::Mat(height, width, CV_8UC3, image_data);
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_BGR2RGB);
        }

        drawDetections(image_to_show, detections, 30);

        cv::resize(image_to_show, image_to_show, cv::Size(600,400));

        QImage image = QImage((uchar*) image_to_show.data, image_to_show.cols, image_to_show.rows, image_to_show.step,  QImage::Format_RGB888);

        ui->label_image_rgb->clear();
        QPixmap image_pix_map = QPixmap::fromImage(image);
        ui->label_image_rgb->setPixmap(image_pix_map);
        ui->label_image_rgb->adjustSize();
    }
    image_to_show.release();
    free(image_data);
}

void MainWindow::imageRgbPolReadyToShow(uint8_t *image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp)
{
    cv::Mat image_to_show;

    if(m_device_started){

        if(channels == 1){
            image_to_show = cv::Mat(height, width, CV_8UC1, image_data);
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_GRAY2RGB);
        }
        else if(channels == 3){
            image_to_show = cv::Mat(height, width, CV_8UC3, image_data);
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_BGR2RGB);
        }

        drawDetections(image_to_show, detections, 30);

        cv::resize(image_to_show, image_to_show, cv::Size(600,400));

        QImage image = QImage((uchar*) image_to_show.data, image_to_show.cols, image_to_show.rows, image_to_show.step,  QImage::Format_RGB888);

        ui->label_image_rgb_2->clear();
        QPixmap image_pix_map = QPixmap::fromImage(image);
        ui->label_image_rgb_2->setPixmap(image_pix_map);
        ui->label_image_rgb_2->adjustSize();
    }
    image_to_show.release();
    free(image_data);
}

void MainWindow::imageThermalReadyToShow(uint8_t *image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp)
{
    cv::Mat image_to_show;

    if(m_device_started){

        image_to_show = cv::Mat(height, width, CV_8UC3, image_data);
        cv::cvtColor(image_to_show, image_to_show, cv::COLOR_BGR2RGB);

        drawDetections(image_to_show, detections, 30);

        cv::resize(image_to_show, image_to_show, cv::Size(600,400));

        QImage image = QImage((uchar*) image_to_show.data, image_to_show.cols, image_to_show.rows, image_to_show.step,  QImage::Format_RGB888);

        ui->label_image_thermal->clear();
        QPixmap image_pix_map = QPixmap::fromImage(image);
        ui->label_image_thermal->setPixmap(image_pix_map);
        ui->label_image_thermal->adjustSize();
    }

    image_to_show.release();
    free(image_data);
}

void MainWindow::on_pushButton_set_th_protocol_clicked()
{
    streamingProtocols protocol;
    switch(ui->comboBox_th_protocol->currentIndex()){
    case 0:
        protocol = protocol_raw_udp;
        break;
    case 1:
        protocol = protocol_gstreamer;
        break;
    default:
        protocol = protocol_raw_udp;
        break;
    }

    if(m_thermal_sensor != NULL){
        m_thermal_sensor->protocol = protocol;
        CHANGE_STREAMING_PROTOCOL(m_devices[0], m_thermal_sensor);
    }
}

void MainWindow::on_pushButton_get_thermal_pipeline_clicked()
{
    char *pipeline;
    GET_RTSP_PIPELINE(m_devices[0], *m_thermal_sensor, &pipeline);
    QString string_pipeline = QString("%1").arg(pipeline);

    ui->lineEdit_ther_rtsp->setText(string_pipeline);
}

void MainWindow::on_pushButton_get_lidar_pipeline_clicked()
{
    char *pipeline;
    GET_RTSP_PIPELINE(m_devices[0], *m_lidar_sensor, &pipeline);
    QString string_pipeline = QString("%1").arg(pipeline);

    ui->lineEdit_depth_rtsp->setText(string_pipeline);
}

void MainWindow::on_pushButton_depth_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_lidar_sensor, (char*)ui->lineEdit_depth_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        qDebug()<<"Error setting depth pipeline"<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error setting depth pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_rgb_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_rgb_sensor, (char*)ui->lineEdit_rgb_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        qDebug()<<"Error setting rgb pipeline"<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error setting rgb pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_ther_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_thermal_sensor, (char*)ui->lineEdit_ther_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        qDebug()<<"Error setting thermal pipeline"<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error setting thermal pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_wide_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_allied_wide_sensor, (char*)ui->lineEdit_wide_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        qDebug()<<"Error setting allied wide pipeline"<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error setting allied wide pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_narrow_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_allied_narrow_sensor, (char*)ui->lineEdit_narrow_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        qDebug()<<"Error setting allied narrow pipeline"<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error setting allied narrow pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::drawDetections(cv::Mat &image_to_show, std::vector<detectionImage> detections, uint8_t threshold)
{

    int top, left, width, height;
    int baseline;

    for(uint i=0; i<detections.size(); i++){
        if (detections.at(i).confidence >= threshold){
            top = detections.at(i).box.y;
            left = detections.at(i).box.x;
            height = detections.at(i).box.height;
            width = detections.at(i).box.width;
            cv::rectangle(image_to_show, cv::Point(left, top), cv::Point(left+width, top+height), cv::Scalar(0,255,0), 2);

            std::string label = cv::format("%.2f", (detections.at(i).confidence/100.0));
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_ITALIC, 1.25, 1, &baseline);

            if (top <= labelSize.height) top = labelSize.height;
            cv::rectangle(image_to_show, cv::Point(left, top - round(1.5*labelSize.height)),
                          cv::Point(left+round(1.5*labelSize.width), top + baseline),
                          cv::Scalar(0,255,0), cv::FILLED);
            cv::putText(image_to_show, label, cv::Point(left, top), cv::FONT_ITALIC, 1.75, cv::Scalar(0,0,0), 1);

        }
    }
}

void MainWindow::valueChangedCode(int value)
{
    qDebug()<<"SLOT POR CODIGO"<<value;
    addMessageToLogWindow("SLOT POR CODIGO " + QString::number(value), logType::error);
}

void MainWindow::on_checkBox_show_axis_clicked(bool checked)
{
    m_point_cloud_viewer->setAxisEnabled(checked);
}

void MainWindow::on_pushButton_apply_filter_clicked()
{
    int error = CHANGE_DISTANCE_RANGE(m_devices[0], ui->min_distance_box->value(), ui->max_distance_box->value());
    if(error != L3CAM_OK){
        qDebug()<<getBeamErrorDescription(error);
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}


void MainWindow::on_pushButton_apply_color_ranges_clicked()
{
    int min_value = ui->spinBox_min_range->value();
    int max_value = ui->spinBox_max_range->value();

    if(ui->radioButton_intensity->isChecked()){
        m_max_intensity = max_value;
        m_min_intensity = min_value;
    }else{
        m_max_distance = max_value;
        m_min_distance = min_value;
    }

    CHANGE_POINT_CLOUD_COLOR_RANGE(m_devices[0], min_value, max_value);
}

void MainWindow::on_pushButton_set_lidar_protocol_clicked()
{
    streamingProtocols protocol;

    switch(ui->comboBox_lidar_protocol->currentIndex()){
    case 0:
        protocol = protocol_raw_udp;
        break;
    case 1:
        protocol = protocol_gstreamer;
        break;
    default:
        protocol = protocol_raw_udp;
        break;
    }

    if(m_lidar_sensor != NULL){
        m_lidar_sensor->protocol = protocol;
        CHANGE_STREAMING_PROTOCOL(m_devices[0], m_lidar_sensor);
    }
}

void MainWindow::on_pushButton_set_network_settings_clicked()
{
    if(ui->checkBox_dhcp->isChecked()){
        CHANGE_NETWORK_CONFIGURATION(m_devices[0], NULL, NULL, NULL, true);

    }else{
        CHANGE_NETWORK_CONFIGURATION(m_devices[0],
                (char*)ui->lineEdit_address->text().toStdString().c_str(),
                (char*)ui->lineEdit_netmask->text().toStdString().c_str(),
                (char*)ui->lineEdit_gateway->text().toStdString().c_str(),
                false);
    }
}

void MainWindow::on_pushButton_terminate_clicked()
{
    close();
}

void MainWindow::on_checkBox_allied_wide_exposure_clicked(bool checked)
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = ENABLE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME(m_devices[0], *m_allied_wide_sensor,checked);
    }else if(m_current_allied_camera == narrow_camera){
        error = ENABLE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME(m_devices[0], *m_allied_narrow_sensor,checked);
    }

    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
    if(!error){
        ui->horizontalSlider_allied_wide_exposure->setDisabled(checked);

        //!habilitar el rango del autoexposure time
        ui->horizontalSlider_allied_wide_max_exposure->setEnabled(checked);
        ui->horizontalSlider_allied_wide_min_exposure->setEnabled(checked);
    }
    else ui->checkBox_allied_wide_exposure->setChecked(!checked);
}

void MainWindow::on_radioButton_wide_camera_clicked(bool checked)
{
    if(checked){
        m_current_allied_camera = (uint8_t)wide_camera;
    }else{
        m_current_allied_camera = (uint8_t)narrow_camera;
    }
}

void MainWindow::on_radioButton_narrow_camera_clicked(bool checked)
{
    if(checked){
        m_current_allied_camera = (uint8_t)narrow_camera;
    }else{
        m_current_allied_camera = (uint8_t)wide_camera;
    }
}

void MainWindow::on_horizontalSlider_allied_wide_exposure_valueChanged(int value)
{
    ui->label_exposure_allied_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_allied_wide_exposure_sliderReleased()
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_EXPOSURE_TIME_US(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_allied_wide_exposure->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_EXPOSURE_TIME_US(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_allied_wide_exposure->value());
    }

    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_allied_wide_min_exposure_valueChanged(int value)
{
    ui->label_exposure_min_allied_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_allied_wide_min_exposure_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_allied_wide_min_exposure->value(), (float)ui->horizontalSlider_allied_wide_max_exposure->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_allied_wide_min_exposure->value(), (float)ui->horizontalSlider_allied_wide_max_exposure->value());
    }

    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_allied_wide_max_exposure_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_allied_wide_min_exposure->value(), (float)ui->horizontalSlider_allied_wide_max_exposure->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_allied_wide_min_exposure->value(), (float)ui->horizontalSlider_allied_wide_max_exposure->value());
    }

    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_allied_wide_max_exposure_valueChanged(int value)
{
    ui->label_exposure_max_allied_wide_val->setText(QString("%1").arg(value));
}


void MainWindow::on_checkBox_allied_wide_gain_clicked(bool checked)
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error =  ENABLE_ALLIED_CAMERA_AUTO_GAIN(m_devices[0], *m_allied_wide_sensor, checked);
    }else if(m_current_allied_camera == narrow_camera){
        error = ENABLE_ALLIED_CAMERA_AUTO_GAIN(m_devices[0], *m_allied_narrow_sensor, checked);
    }

    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
    if(!error){
        ui->horizontalSlider_gain_wide->setDisabled(checked);

        //!habilitar el rango del autoexposure time
        ui->horizontalSlider_allied_wide_max_gain->setEnabled(checked);
        ui->horizontalSlider_allied_wide_min_gain->setEnabled(checked);
    }
    else ui->checkBox_allied_wide_gain->setChecked(!checked);
}

void MainWindow::on_horizontalSlider_gain_wide_valueChanged(int value)
{
    ui->label_gain_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_gain_wide_sliderReleased()
{
    int error = L3CAM_OK;

    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_GAIN(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_gain_wide->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_GAIN(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_gain_wide->value());
    }

    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_allied_wide_min_gain_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_GAIN_RANGE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_allied_wide_min_gain->value(), (float)ui->horizontalSlider_allied_wide_max_gain->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_GAIN_RANGE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_allied_wide_min_gain->value(), (float)ui->horizontalSlider_allied_wide_max_gain->value());
    }

    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::on_horizontalSlider_allied_wide_min_gain_valueChanged(int value)
{
    ui->label_gain_min_allied_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_allied_wide_max_gain_valueChanged(int value)
{
    ui->label_gain_max_allied_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_allied_wide_max_gain_sliderReleased()
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_GAIN_RANGE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_allied_wide_min_gain->value(), (float)ui->horizontalSlider_allied_wide_max_gain->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_GAIN_RANGE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_allied_wide_min_gain->value(), (float)ui->horizontalSlider_allied_wide_max_gain->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_intensity_controller_target_wide_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_TARGET(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_intensity_controller_target_wide->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_TARGET(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_intensity_controller_target_wide->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_intensity_controller_target_wide_valueChanged(int value)
{
    ui->label_int_cont_targ_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_comboBox_intensity_controller_region_wide_currentIndexChanged(int index)
{

    int error = L3CAM_OK;
    if(index == 0){
        if(m_current_allied_camera == wide_camera){
            error = CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_REGION(m_devices[0], *m_allied_wide_sensor, allied_controller_region_auto_mode_region);
        }else if(m_current_allied_camera == narrow_camera){
            error = CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_REGION(m_devices[0], *m_allied_narrow_sensor, allied_controller_region_auto_mode_region);
        }
    }else if(index == 1){
        if(m_current_allied_camera == wide_camera){
            error = CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_REGION(m_devices[0], *m_allied_wide_sensor, allied_controller_region_full_image);
        }else if(m_current_allied_camera == narrow_camera){
            error = CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_REGION(m_devices[0], *m_allied_narrow_sensor, allied_controller_region_full_image);
        }
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_allied_wide_auto_mode_height_valueChanged(int value)
{
    ui->label_auto_mode_height_allied_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_allied_wide_auto_mode_height_sliderReleased()
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_MODE_REGION(m_devices[0], *m_allied_wide_sensor, (int)ui->horizontalSlider_allied_wide_auto_mode_height->value(), (int)ui->horizontalSlider_allied_wide_auto_mode_width->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_MODE_REGION(m_devices[0], *m_allied_narrow_sensor, (int)ui->horizontalSlider_allied_wide_auto_mode_height->value(), (int)ui->horizontalSlider_allied_wide_auto_mode_width->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_allied_wide_auto_mode_width_valueChanged(int value)
{
    ui->label_auto_width_allied_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_allied_wide_auto_mode_width_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_MODE_REGION(m_devices[0], *m_allied_wide_sensor, (int)ui->horizontalSlider_allied_wide_auto_mode_height->value(), (int)ui->horizontalSlider_allied_wide_auto_mode_width->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_MODE_REGION(m_devices[0], *m_allied_narrow_sensor, (int)ui->horizontalSlider_allied_wide_auto_mode_height->value(), (int)ui->horizontalSlider_allied_wide_auto_mode_width->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_max_buffers_count_wide_valueChanged(int value)
{
    ui->label_max_buffers_count_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_max_buffers_count_wide_sliderReleased()
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_MAX_DRIVER_BUFFERS_COUNT(m_devices[0], *m_allied_wide_sensor, (int)ui->horizontalSlider_max_buffers_count_wide->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_MAX_DRIVER_BUFFERS_COUNT(m_devices[0], *m_allied_narrow_sensor, (int)ui->horizontalSlider_max_buffers_count_wide->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_checkBox_allied_wide_white_bal_clicked(bool checked)
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = ENABLE_ALLIED_CAMERA_AUTO_WHITE_BALANCE(m_devices[0], *m_allied_wide_sensor, checked);
    }else if(m_current_allied_camera == narrow_camera){
        error = ENABLE_ALLIED_CAMERA_AUTO_WHITE_BALANCE(m_devices[0], *m_allied_narrow_sensor, checked);
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

    if(!error){
        ui->horizontalSlider_white_balance_auto_tol_allied_wide->setEnabled(checked);
        ui->horizontalSlider_white_balance_auto_rate_allied_wide->setEnabled(checked);

        ui->horizontalSlider_white_balance_ratio_allied_wide->setDisabled(checked);
        ui->comboBox_white_balance_ratio_selector->setDisabled(checked);
    }
    else ui->checkBox_allied_wide_white_bal->setChecked(!checked);
}

void MainWindow::on_horizontalSlider_white_balance_auto_tol_allied_wide_sliderReleased()
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_BALANCE_WHITE_AUTO_TOLERANCE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_white_balance_auto_tol_allied_wide->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_BALANCE_WHITE_AUTO_TOLERANCE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_white_balance_auto_tol_allied_wide->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_white_balance_auto_tol_allied_wide_valueChanged(int value)
{
    ui->label_white_balance_auto_tol_allied_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_white_balance_auto_rate_allied_wide_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_BALANCE_WHITE_AUTO_RATE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_white_balance_auto_rate_allied_wide->value());
    }else if(m_current_allied_camera == narrow_camera){
        error =CHANGE_ALLIED_CAMERA_BALANCE_WHITE_AUTO_RATE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_white_balance_auto_rate_allied_wide->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_white_balance_auto_rate_allied_wide_valueChanged(int value)
{
    ui->label_white_balance_auto_rate_allied_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_comboBox_white_balance_ratio_selector_currentIndexChanged(int index)
{

    int error = L3CAM_OK;
    if(index == 0){
        if(m_current_allied_camera == wide_camera){
            error = CHANGE_ALLIED_CAMERA_BALANCE_RATIO_SELECTOR(m_devices[0], *m_allied_wide_sensor, allied_balance_ratio_selector_red);
        }else if(m_current_allied_camera == narrow_camera){
            error = CHANGE_ALLIED_CAMERA_BALANCE_RATIO_SELECTOR(m_devices[0], *m_allied_narrow_sensor, allied_balance_ratio_selector_red);
        }
    }else if(index == 1){
        if(m_current_allied_camera == wide_camera){
            error = CHANGE_ALLIED_CAMERA_BALANCE_RATIO_SELECTOR(m_devices[0], *m_allied_wide_sensor, allied_balance_ratio_selector_blue);
        }else if(m_current_allied_camera == narrow_camera){
            error =  CHANGE_ALLIED_CAMERA_BALANCE_RATIO_SELECTOR(m_devices[0], *m_allied_narrow_sensor, allied_balance_ratio_selector_blue);
        }
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_white_balance_ratio_allied_wide_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_BALANCE_RATIO(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_white_balance_ratio_allied_wide->value()/10.0);
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_BALANCE_RATIO(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_white_balance_ratio_allied_wide->value()/10.0);
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_white_balance_ratio_allied_wide_valueChanged(int value)
{
    ui->label_white_balance_ratio_allied_wide_val->setText(QString("%1").arg((float)value / 10.0));
}

void MainWindow::on_horizontalSlider_wide_black_level_sliderReleased()
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_BLACK_LEVEL(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_wide_black_level->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_BLACK_LEVEL(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_wide_black_level->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_wide_black_level_valueChanged(int value)
{
    ui->label_wide_black_level_val->setText(QString("%1").arg(value));
}

void MainWindow::on_horizontalSlider_gamma_wide_sliderReleased()
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_GAMMA(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_gamma_wide->value()/10.0);
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_GAMMA(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_gamma_wide->value()/10.0);
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_gamma_wide_valueChanged(int value)
{
    ui->label_gamma_wide_val->setText(QString("%1").arg((float)value/10.0));
}

void MainWindow::on_horizontalSlider_saturation_wide_sliderReleased()
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_SATURATION(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_saturation_wide->value()/10.0);
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_SATURATION(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_saturation_wide->value()/10.0);
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::on_horizontalSlider_saturation_wide_valueChanged(int value)
{
    ui->label_saturation_wide_val->setText(QString("%1").arg((float)value/10.0));
}

void MainWindow::on_horizontalSlider_sharpness_wide_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_SHARPNESS(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_sharpness_wide->value()/10.0);
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_SHARPNESS(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_sharpness_wide->value()/10.0);
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_sharpness_wide_valueChanged(int value)
{
    ui->label_sharpness_wide_val->setText(QString("%1").arg((float)value/10.0));
}

void MainWindow::on_horizontalSlider_hue_wide_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_HUE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_hue_wide->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_HUE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_hue_wide->value());
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_horizontalSlider_hue_wide_valueChanged(int value)
{
    ui->label_hue_wide_val->setText(QString("%1").arg(value));
}

void MainWindow::on_comboBox_intensity_auto_precedence_currentIndexChanged(int index)
{
    int error = L3CAM_OK;
    if(index == 0){
        if(m_current_allied_camera == wide_camera){
            error = CHANGE_ALLIED_CAMERA_INTENSITY_AUTO_PRECEDENCE(m_devices[0], *m_allied_wide_sensor, allied_auto_precedence_minimize_noise);
        }else if(m_current_allied_camera == narrow_camera){
            error = CHANGE_ALLIED_CAMERA_INTENSITY_AUTO_PRECEDENCE(m_devices[0], *m_allied_narrow_sensor, allied_auto_precedence_minimize_noise);
        }
    }else if(index == 1){
        if(m_current_allied_camera == wide_camera){
            error = CHANGE_ALLIED_CAMERA_INTENSITY_AUTO_PRECEDENCE(m_devices[0], *m_allied_wide_sensor, allied_auto_precedence_minimize_blur);
        }else if(m_current_allied_camera == narrow_camera){
            error = CHANGE_ALLIED_CAMERA_INTENSITY_AUTO_PRECEDENCE(m_devices[0], *m_allied_narrow_sensor, allied_auto_precedence_minimize_blur);
        }
    }
    qDebug()<<__func__<<"Response"<<error<<getBeamErrorDescription(error);
    addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

}

void MainWindow::on_pushButton_set_narrow_protocol_clicked()
{
    streamingProtocols protocol;
    switch(ui->comboBox_narrow_protocol->currentIndex()){
    case 0:
        protocol = protocol_raw_udp;
        break;
    case 1:
        protocol = protocol_gstreamer;
        break;
    default:
        protocol = protocol_raw_udp;
        break;
    }

    if(m_allied_narrow_sensor != NULL){
        m_allied_narrow_sensor->protocol = protocol;
        CHANGE_STREAMING_PROTOCOL(m_devices[0], m_allied_narrow_sensor);
    }
}

void MainWindow::on_pushButton_set_wide_protocol_clicked()
{
    streamingProtocols protocol;
    switch(ui->comboBox_wide_protocol->currentIndex()){
    case 0:
        protocol = protocol_raw_udp;
        break;
    case 1:
        protocol = protocol_gstreamer;
        break;
    default:
        protocol = protocol_raw_udp;
        break;
    }

    if(m_allied_wide_sensor != NULL){
        m_allied_wide_sensor->protocol = protocol;
        CHANGE_STREAMING_PROTOCOL(m_devices[0], m_allied_wide_sensor);
    }
}

void MainWindow::on_pushButton_temperatures_clicked()
{
    int32_t *temperatures = (int32_t*)malloc(sizeof(int32_t)*11);

    int error = GET_DEVICE_TEMPERATURES(m_devices[0], temperatures);
    if(error != L3CAM_OK){
        qDebug()<<"Error in temperatures error:"<<error<<" - "<<getBeamErrorDescription(error);
        addMessageToLogWindow("Error in temperatures error: " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }else{

        ui->label_bcpu_val->setText(QString("%1 ºC").arg(temperatures[0]/1000.0));
        ui->label_mcpu_val->setText(QString("%1 ºC").arg(temperatures[1]/1000.0));
        ui->label_gpu_val->setText(QString("%1 ºC").arg(temperatures[2]/1000.0));
        ui->label_pll_val->setText(QString("%1 ºC").arg(temperatures[3]/1000.0));
        ui->label_board_val->setText(QString("%1 ºC").arg(temperatures[4]/1000.0));
        ui->label_diode_val->setText(QString("%1 ºC").arg(temperatures[5]/1000.0));
        ui->label_pmic_val->setText(QString("%1 ºC").arg(temperatures[6]/1000.0));
        ui->label_fan_value->setText(QString("%1 ºC").arg(temperatures[7]/1000.0));
        ui->label_inter_val->setText(QString("%1 ºC").arg(temperatures[8]/1000.0));
        ui->label_allied_w_t_value->setText(QString("%1 ºC").arg(temperatures[9]/1000.0));
        ui->label_allied_n_t_value->setText(QString("%1 ºC").arg(temperatures[10]/1000.0));

    }
    free(temperatures);
}

void MainWindow::on_pushButton_get_allied_pipeline_clicked()
{
    char *pipeline;
    if(ui->radioButton_wide_camera->isChecked()){
        GET_RTSP_PIPELINE(m_devices[0], *m_allied_wide_sensor, &pipeline);
        QString string_pipeline = QString("%1").arg(pipeline);

        ui->lineEdit_wide_rtsp->setText(string_pipeline);
    }
    if(ui->radioButton_narrow_camera->isChecked()){
        GET_RTSP_PIPELINE(m_devices[0], *m_allied_narrow_sensor, &pipeline);
        QString string_pipeline = QString("%1").arg(pipeline);

        ui->lineEdit_narrow_rtsp->setText(string_pipeline);
    }
}

void MainWindow::on_pushButton_long_range_clicked()
{
    SET_BIAS_LONG_RANGE(m_devices[0]);
}


void MainWindow::on_pushButton_short_range_clicked()
{
    SET_BIAS_SHORT_RANGE(m_devices[0]);
}

void MainWindow::updateSensorStatus(uint8_t device_status, QLabel *status_label)
{
    QString status = "";
    QString style = m_status_undefined_style;

    switch(device_status){

    case deviceStatus::undefined:
        style = m_status_undefined_style;
        break;
    case deviceStatus::no_error:
        style = m_status_connected_style;
        status = "OK";
        break;
    case deviceStatus::alarm:
        style = m_status_alarm_style;
        status = "ALARM";
        break;
    case deviceStatus::error_s:
        style = m_status_error_style;
        status = "ERROR";
        break;
    }

    status_label->setStyleSheet(style);
    status_label->setText(status);
}

void MainWindow::initializeSystemStatus()
{

    (m_thermal_sensor != NULL) ? m_thermal_status = deviceStatus::no_error : m_thermal_status = deviceStatus::error_s;
    (m_lidar_sensor != NULL) ? m_lidar_status = deviceStatus::no_error : m_lidar_status = deviceStatus::error_s;
    (m_pol_sensor != NULL) ? m_polarimetric_status = deviceStatus::no_error : m_polarimetric_status = deviceStatus::error_s;
    (m_rgb_sensor != NULL) ? m_rgb_status = deviceStatus::no_error : m_rgb_status = deviceStatus::error_s;
    (m_allied_wide_sensor != NULL) ? m_allied_wide_status = deviceStatus::no_error : m_allied_wide_status = deviceStatus::error_s;
    (m_allied_narrow_sensor != NULL) ? m_allied_narrow_status = deviceStatus::no_error : m_allied_narrow_status = deviceStatus::error_s;

    updateSensorStatus(m_thermal_status, ui->label_thermal_1_status_value);
    updateSensorStatus(m_polarimetric_status, ui->label_polarimetric_status_value);
    updateSensorStatus(m_rgb_status, ui->label_rgb_status_value);
    updateSensorStatus(m_lidar_status, ui->label_lidar_status_value);
    updateSensorStatus(m_allied_wide_status, ui->label_allied_wide_status_value);
    updateSensorStatus(m_allied_narrow_status, ui->label_allied_narrow_status_value);

}

void MainWindow::addMessageToLogWindow(QString message, uint8_t level)
{
    QColor color;

    switch(level){
    case logType::error:
        color = Qt::red;
        break;
    case logType::warning:
        color = Qt::darkYellow;
        break;
    case logType::verbose:
        color = Qt::black;
        break;
    }

    QString now = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss:zzz");
    ui->listWidget_log_window->addItem(QString("[%1] %2").arg(now).arg(message));
    int items = ui->listWidget_log_window->count();
    ui->listWidget_log_window->item(items-1)->setForeground(color);
    ui->listWidget_log_window->scrollToBottom();
}

void MainWindow::on_actionAbout_triggered()
{
    QString appVersion = "<p>L3Cam Viewer version " + qApp->applicationVersion() + ".</p>";
    QString libVersion = "<p>Library libL3Cam version " + QString(GET_VERSION()) + ".</p>";
    QString qtVersion =  "<p>Compiled against Qt open source version " + QString(qVersion()) + ".</p>";
    QString copyright =  "<p>Copyright (c) 2023 Beamagine.</p>";
    QString more =       "<p>For more info visit <a href='https://beamagine.com/'>beamagine.com</a> or contact us at <a href='mailto:info@beamagine.com'>info@beamagine.com</a>.</p>";

    QString msg = appVersion + libVersion + qtVersion + copyright + more;

    QMessageBox::about(this, "About", msg);
}


void MainWindow::on_comboBox_background_currentIndexChanged(int index)
{
    switch (index) {
    case 0: //black
        m_point_cloud_viewer->doChangeBackgroundColor(0,0,0);
        break;

    case 1: //white
        m_point_cloud_viewer->doChangeBackgroundColor(1,1,1);
        break;

    case 2: //green
        m_point_cloud_viewer->doChangeBackgroundColor(0.3,0.5,0);
        break;
    }
}

