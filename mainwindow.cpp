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
#include <QFileDialog>
#include <QTemporaryDir>
#include <QStringList>

Q_DECLARE_METATYPE(uint16_t)
Q_DECLARE_METATYPE(uint8_t)
Q_DECLARE_METATYPE(int32_t)
Q_DECLARE_METATYPE(std::vector<detectionImage>)
Q_DECLARE_METATYPE(pointcloudData)
Q_DECLARE_METATYPE(imageData)
Q_DECLARE_METATYPE(uint32_t)
Q_DECLARE_METATYPE(binaryFloatData)

#include <QSpinBox>

static MainWindow *mainWindowObj = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    QString title = "L3Cam Viewer - " + qApp->applicationVersion();
    this->setWindowTitle(title);

    m_status_connected_style  = "border-radius: 5px; background-color: rgb(114,255,158); color:rgb(0,0,0);";
    m_status_error_style = "border-radius: 5px; background-color: rgb(255,114,114); color:rgb(0,0,0);";
    m_status_alarm_style = "border-radius: 5px; background-color: rgb(255,255,158); color:rgb(0,0,0);";
    m_status_searching_style = "border-radius: 5px; background-color: rgb(114,158,255); color:rgb(0,0,0);";
    m_status_undefined_style = "border-radius: 5px; background-color: rgb(190,190,190); color:rgb(0,0,0);";

    m_red_button_style = "border-radius: 50px; border: 2px solid #555; background-color: rgb(250, 47, 23); outline: 0px;";
    m_green_button_style = "border-radius: 50px; border: 2px solid #555; background-color: rgb(105, 224, 106); outline: 0px;";

    ui->label_image_rgb->setBackgroundRole(QPalette::Base);
    ui->label_image_rgb->setScaledContents(true);

    ui->label_image_thermal->setBackgroundRole(QPalette::Base);
    ui->label_image_thermal->setScaledContents(true);

    ui->label_image_rgb_2->setBackgroundRole(QPalette::Base);
    ui->label_image_rgb_2->setScaledContents(true);

    ui->label_l3cam_status->setStyleSheet(m_status_undefined_style);

    ui->pushButton_save->setStyleSheet(m_green_button_style);

    m_searching_device = false;
    m_searching_status = false;
    m_search_timer = new QTimer();
    connect(m_search_timer, SIGNAL(timeout()), this, SLOT(searchTimerTimeOut()));

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
    m_temperatures_reader = new udpReceiverController();

    m_rgb_port = 6020;
    m_thermal_port = 6030;
    m_pcd_port = 6050;
    m_rgbp_port = 6060;

    m_max_distance = 180000;
    m_min_distance = 1500;
    m_max_intensity = 3000;
    m_min_intensity = 0;

    m_save_pointcloud = false;
    m_save_pol_image = false;
    m_save_thermal_image = false;
    m_save_rgb_image = false;
    m_save_wide_image = false;
    m_save_narrow_image = false;
    m_save_thermal_data_image = false;

    m_save_thermal_image_executor = new imageSaveDataExecutor();
    m_save_thermal_image_manager = new saveDataManager();

    m_save_thermal_data_manager = new saveDataManager();
    m_save_thermal_data_executor = new imageSaveDataExecutor();

    m_save_rgb_image_manager = new saveDataManager();
    m_save_rgb_image_executor = new imageSaveDataExecutor();

    m_save_pointcloud_manager = new saveDataManager();
    m_save_pointcloud_executor = new pointCloudSaveDataExecutor();

    m_save_polarimetric_manager = new saveDataManager();
    m_save_polarimetric_executor = new imageSaveDataExecutor();

    m_temperatures_viewer = new imageViewerForm();

    m_temperatures_viewer->setWindowTitle("Thermal data");

    m_save_thermal_image_manager->setDataTypeToSave(images);
    m_save_rgb_image_manager->setDataTypeToSave(images);
    m_save_pointcloud_manager->setDataTypeToSave(pointcloud);
    m_save_polarimetric_manager->setDataTypeToSave(images);

    m_save_thermal_data_manager->setDataTypeToSave(binaryFloat);


    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<int32_t>("int32_t");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<std::vector<detectionImage> >("std::vector<detectionImage>");
    qRegisterMetaType<pointcloudData>("pointcloudData");
    qRegisterMetaType<imageData>("imageData");
    qRegisterMetaType<binaryFloatData>("binaryFloatData");

    connect(m_pointcloud_reader, SIGNAL(pointcloudReadyToShow(int32_t*,uint32_t)), this, SLOT(pointCloudReadyToShow(int32_t*,uint32_t)));

    connect(m_rgb_image_reader, SIGNAL(imageRgbReadyToShow(uint8_t*,uint16_t,uint16_t,uint8_t,std::vector<detectionImage>,uint32_t)),
            this, SLOT(imageRgbReadyToShow(uint8_t*,uint16_t,uint16_t,uint8_t,std::vector<detectionImage>,uint32_t)));

    connect(m_thermal_image_reader, SIGNAL(imageRgbReadyToShow(uint8_t*,uint16_t,uint16_t,uint8_t,std::vector<detectionImage>,uint32_t)),
            this, SLOT(imageThermalReadyToShow(uint8_t*,uint16_t,uint16_t,uint8_t,std::vector<detectionImage>,uint32_t)));

    connect(m_temperatures_reader, SIGNAL(temperatureDataReceived(float*,uint16_t,uint16_t,uint32_t)),
            this, SLOT(temperatureDataReady(float*,uint16_t,uint16_t,uint32_t)));

    connect(m_rgb_pol_image_reader, SIGNAL(imageRgbReadyToShow(uint8_t*,uint16_t,uint16_t,uint8_t,std::vector<detectionImage>,uint32_t)),
            this, SLOT(imageRgbPolReadyToShow(uint8_t*,uint16_t,uint16_t,uint8_t,std::vector<detectionImage>,uint32_t)));

    connect(m_save_thermal_image_executor, SIGNAL(executorIsAvailable(bool)), this, SLOT(thermalSaveExecutorIsAvailable(bool)));
    connect(m_save_thermal_image_manager, SIGNAL(sendImageToSave(imageData)), this, SLOT(thermalImageToSaveReceived(imageData)));

    connect(m_save_thermal_data_executor, SIGNAL(executorIsAvailable(bool)), this, SLOT(thermalDataSaveExecutorAvailable(bool)));
    connect(m_save_thermal_data_manager, SIGNAL(sendBufferToSave(binaryFloatData)), this, SLOT(thermalDataBufferToSaveReceived(binaryFloatData)));

    connect(m_save_rgb_image_executor, SIGNAL(executorIsAvailable(bool)), this, SLOT(rgbSaveExecutorIsAvailable(bool)));
    connect(m_save_rgb_image_manager, SIGNAL(sendImageToSave(imageData)), this, SLOT(rgbImageToSaveReceived(imageData)));

    connect(m_save_pointcloud_executor, SIGNAL(executorIsAvailable(bool)), this, SLOT(pointcloudSaveExecutorIsAvailable(bool)));
    connect(m_save_pointcloud_manager, SIGNAL(sendPointCloudToSave(pointcloudData)), this, SLOT(pointcloudToSaveReceived(pointcloudData)));

    connect(m_save_polarimetric_executor, SIGNAL(executorIsAvailable(bool)), this, SLOT(polSaveExecutorIsAvailable(bool)));
    connect(m_save_polarimetric_manager, SIGNAL(sendImageToSave(imageData)), this, SLOT(polImageToSaveReceived(imageData)));

    m_temperatures_viewer->hide();

    m_save_data = false;
    m_save_images_counter = 0;
    m_save_images_rgb_counter = 0;
    m_save_pointcloud_counter = 0;
    m_save_thermal_counter = 0;
    m_save_thermal_data_counter = 0;
    m_save_pol_counter = 0;
    m_save_wide_counter = 0;
    m_save_narrow_counter = 0;

    m_save_all = false;

    m_blurring_loaded = false;
    m_apply_blurring = false;


    m_path_to_save_thermal = QDir::homePath() + "/";
    m_path_to_save_thermal_bin = QDir::homePath() + "/";
    m_path_to_save_pol = QDir::homePath() + "/";
    m_path_to_save_pointcloud = QDir::homePath() + "/";
    m_path_to_save_rgb = QDir::homePath() + "/";
    m_path_to_save_wide = QDir::homePath() + "/";
    m_path_to_save_narrow = QDir::homePath() + "/";

    enableSaveConfiguration();


    m_dev_initialized = false;
    m_new_thermal_library = false;

    //!load face blurring
    loadBlurringNetworks();

    initializeGUI();

    m_library_version = QString("%1").arg(GET_VERSION());

    //!Always go to first tab
    ui->tabWidget->setCurrentIndex(0);

    m_initializing_thermal_settings = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMainWindowObj(MainWindow *ptr)
{
    mainWindowObj = ptr;
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
        addMessageToLogWindow(QString("Device status error %1 - %2").arg(error).arg(getBeamErrorDescription(error)) , logType::error);
        return;
    }

    uint8_t statusType = logType::verbose;
    if(status == 1 || status == 5 || status == 10)
        statusType = logType::warning;
    if(status == 2 || status == 3 || status == 6 || status == 7)
        statusType = logType::error;
    addMessageToLogWindow("Device status " + QString::number(status), statusType);

    error = GET_SENSORS_AVAILABLE(m_devices[0], m_sensors, &m_sensors_connected);

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
    initializePolDefault();

    ui->pushButton_start_device->setEnabled(false);
    ui->pushButton_start_streaming->setEnabled(false);

    m_point_cloud_viewer = new pclPointCloudViewerController();

    connect(m_point_cloud_viewer, SIGNAL(sendPointSelectedData(QString)), this, SLOT(pointSelectedNotification(QString)));

    m_point_cloud_viewer->startController();
    m_point_cloud_viewer->setAxisEnabled(true);

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

void MainWindow::initializePolDefault()
{
    m_pol_brightness = 127;
    m_pol_black_level = 6.0;
    m_pol_gain = 24;
    m_pol_auto_gain_min = 0;
    m_pol_auto_gain_max = 48;
    m_pol_exposure = 500000.0;
    m_pol_auto_exposure_min = 33.456;
    m_pol_auto_exposure_max = 1000000000.0;

    m_pol_auto_gain = true;
    m_pol_auto_exposure = true;

    ui->horizontalSlider_pol_bri->setValue(m_pol_brightness);
    ui->doubleSpinBox_black_level->setValue(m_pol_black_level);
    ui->horizontalSlider_pol_gain->setValue(m_pol_gain);
    ui->checkBox_pol_gain->setChecked(m_pol_auto_gain);
    ui->doubleSpinBox_pol_gain_min->setValue(m_pol_auto_gain_min);
    ui->doubleSpinBox_pol_gain_max->setValue(m_pol_auto_gain_max);
    ui->checkBox_pol_exposure->setChecked(m_pol_auto_exposure);
    ui->doubleSpinBox_pol_expo->setValue(m_pol_exposure);
    ui->doubleSpinBox_pol_expo_min->setValue(m_pol_auto_exposure_min);
    ui->doubleSpinBox_pol_expo_max->setValue(m_pol_auto_exposure_max);

    ui->horizontalSlider_pol_gain->setDisabled(m_pol_auto_gain);
    ui->doubleSpinBox_pol_gain_min->setEnabled(m_pol_auto_gain);
    ui->doubleSpinBox_pol_gain_max->setEnabled(m_pol_auto_gain);
    ui->pushButton_set_pol_gain_minmax->setEnabled(m_pol_auto_gain);
    ui->doubleSpinBox_pol_expo->setDisabled(m_pol_auto_exposure);
    ui->pushButton_pol_exposure->setDisabled(m_pol_auto_exposure);
    ui->doubleSpinBox_pol_expo_min->setEnabled(m_pol_auto_exposure);
    ui->doubleSpinBox_pol_expo_max->setEnabled(m_pol_auto_exposure);
    ui->pushButton_set_pol_expo_minmax->setEnabled(m_pol_auto_exposure);
}

void MainWindow::initializeReceivers()
{
    if(m_lidar_sensor != NULL){

        m_pointcloud_reader->setIpAddress(m_server_address);
        m_pointcloud_reader->setPort(m_pcd_port);
        m_pointcloud_reader->doReadPointcloud(true);
        m_pointcloud_reader->startController();

        m_save_pointcloud_executor->setPathToSavePcd(ui->lineEdit_save_pointcloud_path->text());
        m_save_pointcloud_executor->startController();
        m_save_pointcloud_manager->startController();
    }

    if(m_rgb_sensor != NULL || m_allied_narrow_sensor != NULL){

        ui->radioButton_fusion_mono->setEnabled(true);

        m_rgb_image_reader->setIpAddress(m_server_address);
        m_rgb_image_reader->setPort(m_rgb_port);
        m_rgb_image_reader->doReadImageRgb(true);
        m_rgb_image_reader->startController();

        m_save_rgb_image_executor->setPathToSaveImages((m_rgb_sensor != NULL) ? ui->lineEdit_save_narrow_path->text() : ui->lineEdit_save_rgb_path->text());
        m_save_rgb_image_manager->startController();
        m_save_rgb_image_executor->startController();
    }

    if(m_allied_wide_sensor!= NULL || m_pol_sensor != NULL){

        m_rgb_pol_image_reader->setIpAddress(m_server_address);
        m_rgb_pol_image_reader->setPort(m_rgbp_port);
        m_rgb_pol_image_reader->doReadImageRgb(true);
        m_rgb_pol_image_reader->startController();

        m_save_polarimetric_executor->setPathToSaveImages((m_pol_sensor != NULL) ? ui->lineEdit_save_wide_path->text() : ui->lineEdit_save_pol_path->text());
        m_save_polarimetric_manager->startController();
        m_save_polarimetric_executor->startController();
    }

    if(m_thermal_sensor != NULL){
        ui->radioButton_fusion_thermal->setEnabled(true);

        m_thermal_image_reader->setIpAddress(m_server_address);
        m_thermal_image_reader->setPort(m_thermal_port);
        m_thermal_image_reader->doReadImageRgb(true);
        m_thermal_image_reader->startController();

        m_save_thermal_image_executor->setPathToSaveImages(ui->lineEdit_save_thermal_path->text());
        m_save_thermal_image_manager->startController();
        m_save_thermal_image_executor->startController();

        m_save_thermal_data_executor->setPathToSaveImages(ui->lineEdit_save_thermal_path->text());
        m_save_thermal_data_manager->startController();
        m_save_thermal_data_executor->startController();

        m_temperatures_reader->setIpAddress(m_server_address);
        m_temperatures_reader->setPort(6031);
        m_temperatures_reader->doReadTemperatures(true);
        m_temperatures_reader->startController();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_dev_initialized){
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
                    addMessageToLogWindow("Stop device response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
                    return;
                }
            }

            TERMINATE(m_devices[0]);

            ui->label_l3cam_status->setText("DISCONNECTED");
            ui->label_l3cam_status->setStyleSheet(m_status_undefined_style);

            event->accept();

            if(m_temperatures_viewer->isVisible()){
                m_temperatures_viewer->close();
            }
        }
        else {
            event->ignore();
        }
    }
    else{
        event->accept();

        if(m_temperatures_viewer->isVisible()){
            m_temperatures_viewer->close();
        }
    }


}

void MainWindow::on_pushButton_getVersion_clicked()
{
    const char *version = GET_VERSION();
    addMessageToLogWindow("libL3Cam Version " + QString(version));
}

void MainWindow::on_pushButton_initialize_clicked()
{
    registerErrorCallback(errorNotification);
    int error = L3CAM_OK;

    if(ui->lineEdit_local_address->text().isEmpty() || ui->lineEdit_device_address->text().isEmpty()){
        error = INITIALIZE(NULL, NULL);
    }
    else{
        error = INITIALIZE((char*)ui->lineEdit_local_address->text().toStdString().c_str(), (char*)ui->lineEdit_device_address->text().toStdString().c_str() );
    }

    addMessageToLogWindow("initializing response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

    if(!error){
        m_dev_initialized = true;
    }
}

void MainWindow::on_pushButton_findDevices_clicked()
{

    m_devices_connected = 0;
    int error = FIND_DEVICES(m_devices, &m_devices_connected);

    addMessageToLogWindow("Finding devices response - " + QString::number(error) + " - num devices found " + QString::number(m_devices_connected), (error) ? logType::error : logType::verbose);

    if(m_devices_connected != 0){
        addMessageToLogWindow("Device address " + QString(m_devices[0].ip_address));

        ui->label_device_ip_address->setText(QString("IP ADDRESS: %1").arg(m_devices[0].ip_address));
        ui->label_device_sn->setText(QString("S/N: %1").arg(m_devices[0].serial_number));
        ui->label_device_sw_version->setText(QString("VERSION: %1").arg(m_devices[0].app_version));

        m_initializing_thermal_settings = true;
        enableThermalSettingsForVersion(QString("%1").arg(m_devices[0].app_version));
        m_initializing_thermal_settings = false;

        ui->pushButton_start_device->setEnabled(true);

        ui->label_l3cam_status->setText("Connected");
        ui->label_l3cam_status->setStyleSheet(m_status_connected_style);

    }
}

void MainWindow::on_pushButton_status_clicked()
{
    int32_t status = 0;
    int error = GET_DEVICE_STATUS(m_devices[0], &status);

    if(error != L3CAM_OK){
        addMessageToLogWindow("Error getting status - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
        return;
    }

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

    addMessageToLogWindow("Get sensors available response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

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
    registerErrorCallback(errorNotification);
    error = INITIALIZE(NULL, NULL);
    addMessageToLogWindow("INITIALIZE - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);

    if(!error){
        m_dev_initialized = true;
    }

    if(error != L3CAM_OK){
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

            ui->label_device_ip_address->setText(QString("IP ADDRESS: %1").arg(m_devices[0].ip_address));
            ui->label_device_sn->setText(QString("S/N: %1").arg(m_devices[0].serial_number));
            ui->label_device_sw_version->setText(QString("VERSION: %1").arg(m_devices[0].app_version));

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
                    addMessageToLogWindow("Start stream response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
                    m_device_streaming = false;
                }
            }else{
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
        addMessageToLogWindow("Error enabling thermal filter " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_apply_thermal_filter_clicked()
{
    int error = CHANGE_THERMAL_CAMERA_TEMPERATURE_FILTER(m_devices[0], ui->spinBox_min_temperature->value(), ui->spinBox_max_temperature->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow("Error changing thermal filter values " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_comboBox_thermal_color_currentIndexChanged(int index)
{
    if(m_initializing_thermal_settings){
        return;
    }

    int error = L3CAM_OK;
    int32_t value = 0;


    if(m_new_thermal_library){
        value = index;
    }else{
        value = getValueForOldLibrary(index);
    }

    error = CHANGE_THERMAL_CAMERA_COLORMAP(m_devices[0], value);

    if(error != L3CAM_OK){
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
    econResolutions cur_reso = reso_1920_1080;

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
        addMessageToLogWindow(QString(getBeamErrorDescription(result)), logType::error);
    }
}


void MainWindow::on_horizontalSlider_rgb_bri_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_BRIGHTNESS(m_devices[0], ui->horizontalSlider_rgb_bri->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_contrast_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_CONTRAST(m_devices[0], ui->horizontalSlider_contrast->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_saturation_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_SATURATION(m_devices[0], ui->horizontalSlider_saturation->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_sharpness_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_SHARPNESS(m_devices[0], ui->horizontalSlider_sharpness->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_gamma_sliderReleased()
{
    int error = CHANGE_RGB_CAMERA_GAMMA(m_devices[0], ui->horizontalSlider_gamma->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_gain_sliderReleased()
{
    int error =  CHANGE_RGB_CAMERA_GAIN(m_devices[0], ui->horizontalSlider_gain->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_horizontalSlider_white_balance_sliderReleased()
{
    if(m_rgb_auto_white == false){
        int error = CHANGE_RGB_CAMERA_WHITE_BALANCE(m_devices[0], ui->horizontalSlider_white_balance->value());
        if(error != L3CAM_OK){
            addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
        }
    }
}

void MainWindow::on_horizontalSlider_exposure_sliderReleased()
{
    if(m_rgb_auto_exp == false){
        int error = CHANGE_RGB_CAMERA_EXPOSURE_TIME(m_devices[0], ui->horizontalSlider_exposure->value());
        if(error != L3CAM_OK){
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
            addMessageToLogWindow("Stop device response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
        }
    }else{
        error = START_DEVICE(m_devices[0]);
        if(error == L3CAM_OK){
            ui->pushButton_start_device->setText("STOP DEVICE");
            m_device_started = true;
            ui->pushButton_start_streaming->setEnabled(true);
        }else{
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
            addMessageToLogWindow("Stop stream response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
        }
    }else{
        error = START_STREAM(m_devices[0]);
        if(error == L3CAM_OK){
            ui->pushButton_start_device->setEnabled(false);
            ui->pushButton_start_streaming->setText("STOP STREAMING");
            m_device_streaming = true;
        }else{
            addMessageToLogWindow("Start stream response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
            m_device_streaming = false;
        }
    }
    ui->comboBox_intensity_controller_region_wide->setDisabled(m_device_streaming);
    ui->horizontalSlider_allied_wide_auto_mode_height->setDisabled(m_device_streaming);
    ui->horizontalSlider_allied_wide_auto_mode_width->setDisabled(m_device_streaming);
    ui->horizontalSlider_max_buffers_count_wide->setDisabled(m_device_streaming);
    ui->horizontalSlider_wide_black_level->setDisabled(m_device_streaming);
    ui->horizontalSlider_sharpness_wide->setDisabled(m_device_streaming);
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
    addMessageToLogWindow("change pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
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
    addMessageToLogWindow("change pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
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
    addMessageToLogWindow("change pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::on_radioButton_fusion_thermal_clicked(bool checked)
{
    int error = 0;
    if(checked){
        error = CHANGE_POINT_CLOUD_COLOR(m_devices[0], THERMAL_FUSION);
    }
    addMessageToLogWindow("change fusion pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::on_radioButton_fusion_mono_clicked(bool checked)
{
    int error = 0;
    if(checked){
        error = CHANGE_POINT_CLOUD_COLOR(m_devices[0], RGB_FUSION);
    }
    addMessageToLogWindow("change fusion pointcloud color response - " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), (error) ? logType::error : logType::verbose);
}

void MainWindow::pointCloudReadyToShow(int32_t *pointcloud_data, uint32_t timestamp)
{
    Q_UNUSED(timestamp)

    if(m_save_data && m_save_pointcloud){

        if(m_save_pointcloud_counter > 0 || m_save_all){
            int buff_size = ((pointcloud_data[0] * 5) + 1) * sizeof(int32_t);
            int32_t *temp_buff = (int32_t*) malloc(buff_size);
            memcpy(temp_buff, pointcloud_data, buff_size);

            m_save_pointcloud_manager->doSavePointCloudToBin(temp_buff, pointcloud_data[0], timestamp);
            free(temp_buff);

            if(!m_save_all){
                m_save_pointcloud_counter--;
                checkAllFramesSaved();

            }
        }
    }

    if(m_device_started){
        m_point_cloud_viewer->doShowPointCloud(pointcloud_data);
    }

    free(pointcloud_data);
}

void MainWindow::imageRgbReadyToShow(uint8_t *image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp)
{
    cv::Mat image_to_show;

    if(m_device_started){

        if(channels == 1){
            image_to_show = cv::Mat(height, width, CV_8UC1, image_data);
            if(m_blurring_loaded && m_apply_blurring){
                applyFaceBlurring(image_to_show);
            }
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_GRAY2BGR);
        }else if(channels == 2){
            image_to_show = cv::Mat(height, width, CV_8UC2, image_data);
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_YUV2BGR_Y422);
            if(m_blurring_loaded && m_apply_blurring){
                applyFaceBlurring(image_to_show);
            }
        }
        else if(channels == 3){
            image_to_show = cv::Mat(height, width, CV_8UC3, image_data);
            if(m_blurring_loaded && m_apply_blurring){
                applyFaceBlurring(image_to_show);
            }
        }
        cv::cvtColor(image_to_show, image_to_show, cv::COLOR_BGR2RGB);

        if(!detections.empty()){
            drawDetections(image_to_show, detections, 30);
        }

        if(m_save_data && (m_save_rgb_image || m_save_narrow_image)){

            if(m_save_images_rgb_counter > 0 || m_save_narrow_counter > 0 || m_save_all){

                int buff_size = sizeof(uint8_t)*height*width*channels;
                uint8_t *temp_buf = (uint8_t*)malloc(buff_size);

                memcpy(temp_buf, (uint8_t*)image_to_show.data, buff_size*sizeof(uint8_t));

                m_save_rgb_image_manager->doSavePointerToPng(temp_buf, width, height, channels, timestamp);

                free(temp_buf);

                if(!m_save_all){
                    if(m_save_rgb_image) m_save_images_rgb_counter--;
                    if(m_save_narrow_image) m_save_narrow_counter--;
                    checkAllFramesSaved();
                }
            }
        }

        cv::resize(image_to_show, image_to_show, cv::Size(600,400));

        QImage image = QImage((uchar*) image_to_show.data, image_to_show.cols, image_to_show.rows, image_to_show.step,  QImage::Format_RGB888);

        ui->label_image_rgb->clear();
        QPixmap image_pix_map = QPixmap::fromImage(image);
        ui->label_image_rgb->setPixmap(image_pix_map);
        ui->label_image_rgb->adjustSize();
    }
    image_to_show.release();
}

void MainWindow::imageRgbPolReadyToShow(uint8_t *image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp)
{
    cv::Mat image_to_show;

    if(m_device_started){

        if(channels == 1){
            image_to_show = cv::Mat(height, width, CV_8UC1, image_data);
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_GRAY2RGB);
        }
        else if(channels == 2){
            image_to_show = cv::Mat(height, width, CV_8UC2, image_data);
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_YUV2BGR_Y422);
            if(m_blurring_loaded && m_apply_blurring){
                applyFaceBlurring(image_to_show);
            }
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_BGR2RGB);
        }
        else if(channels == 3){
            image_to_show = cv::Mat(height, width, CV_8UC3, image_data);
            cv::cvtColor(image_to_show, image_to_show, cv::COLOR_BGR2RGB);
        }

        if(!detections.empty()){
            drawDetections(image_to_show, detections, 30);
        }
        
        if(m_save_data && (m_save_pol_image || m_save_wide_image)){

            if(m_save_pol_counter > 0 || m_save_wide_counter > 0 || m_save_all){
                
                int buff_size = sizeof(uint8_t)*height*width*channels;
                uint8_t *temp_buf = (uint8_t*)malloc(buff_size);
                memcpy(temp_buf, image_data, buff_size*sizeof(uint8_t));

                m_save_polarimetric_manager->doSavePointerToPng(temp_buf, width, height, channels, timestamp);

                free(temp_buf);

                if(!m_save_all){
                    if(m_save_pol_image) m_save_pol_counter--;
                    if(m_save_wide_image) m_save_wide_counter--;
                    checkAllFramesSaved();
                }
            }

        }

        cv::resize(image_to_show, image_to_show, cv::Size(600,400));

        QImage image = QImage((uchar*) image_to_show.data, image_to_show.cols, image_to_show.rows, image_to_show.step,  QImage::Format_RGB888);

        ui->label_image_rgb_2->clear();
        QPixmap image_pix_map = QPixmap::fromImage(image);
        ui->label_image_rgb_2->setPixmap(image_pix_map);
        ui->label_image_rgb_2->adjustSize();
    }
    image_to_show.release();
}

void MainWindow::imageThermalReadyToShow(uint8_t *image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp)
{
    cv::Mat image_to_show;

    if(m_device_started){

        image_to_show = cv::Mat(height, width, CV_8UC3, image_data);
        cv::cvtColor(image_to_show, image_to_show, cv::COLOR_BGR2RGB);

        if(m_save_data && m_save_thermal_image ){

            if(m_save_thermal_counter > 0 || m_save_all){
                int buff_size = sizeof(uint8_t)*height*width*channels;
                uint8_t *temp_buf = (uint8_t*)malloc(buff_size);
                memcpy(temp_buf, image_to_show.data, buff_size*sizeof(uint8_t));

                m_save_thermal_image_manager->doSavePointerToPng(temp_buf, width, height, channels, timestamp);

                free(temp_buf);

                if(!m_save_all){
                    m_save_thermal_counter--;
                    checkAllFramesSaved();
                }
            }

        }

        if(detections.size() > 0){
            drawDetections(image_to_show, detections, 30);
        }
        cv::resize(image_to_show, image_to_show, cv::Size(600,400));

        QImage image = QImage((uchar*) image_to_show.data, image_to_show.cols, image_to_show.rows, image_to_show.step,  QImage::Format_RGB888);

        ui->label_image_thermal->clear();
        QPixmap image_pix_map = QPixmap::fromImage(image);
        ui->label_image_thermal->setPixmap(image_pix_map);
        ui->label_image_thermal->adjustSize();
    }

    image_to_show.release();
}

void MainWindow::temperatureDataReady(float *temperature_data, uint16_t height, uint16_t width, uint32_t timestamp)
{
    if(m_device_started){

        if(m_save_data && m_save_thermal_data_image){

            if(m_save_thermal_data_counter > 0 || m_save_all){

                int buff_size = height * width * sizeof(float);
                float *temp_buff = (float*)malloc(buff_size);
                memcpy(temp_buff, temperature_data, buff_size);

                m_save_thermal_data_manager->doSaveFloatDataToBin(temp_buff, buff_size, timestamp);

                free(temp_buff);

                if(!m_save_all){
                    m_save_thermal_data_counter--;
                    checkAllFramesSaved();
                }
            }
        }

        cv::Mat float_image = cv::Mat(height, width, CV_32FC1, temperature_data);
        cv::Mat uchar_image;
        float_image.convertTo(uchar_image, CV_8U);
        cv::cvtColor(uchar_image, uchar_image, cv::COLOR_GRAY2RGB);

        cv::resize(uchar_image, uchar_image, cv::Size(620,460));

        QImage image = QImage((uchar*) uchar_image.data, uchar_image.cols, uchar_image.rows, uchar_image.step,  QImage::Format_RGB888);

        m_temperatures_viewer->showImage(image);
    }
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
        addMessageToLogWindow("Error setting depth pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_rgb_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_rgb_sensor, (char*)ui->lineEdit_rgb_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        addMessageToLogWindow("Error setting rgb pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_pol_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_pol_sensor, (char*)ui->lineEdit_pol_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        addMessageToLogWindow("Error setting polarimetric camera pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_ther_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_thermal_sensor, (char*)ui->lineEdit_ther_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        addMessageToLogWindow("Error setting thermal pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_wide_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_allied_wide_sensor, (char*)ui->lineEdit_wide_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
        addMessageToLogWindow("Error setting allied wide pipeline " + QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_pushButton_narrow_rtsp_clicked()
{
    int error = L3CAM_OK;
    error = CHANGE_RTSP_PIPELINE(m_devices[0], *m_allied_narrow_sensor, (char*)ui->lineEdit_narrow_rtsp->text().toStdString().c_str());
    if(error != L3CAM_OK){
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


void MainWindow::setPathToSaveData(QLineEdit *line_edit)
{
    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(QDir(QDir::homePath()));
    if(dialog.exec()){
        line_edit->setText(dialog.directory().absolutePath() + "/" );
    }

}

void MainWindow::enableSaveConfiguration()
{
    ui->lineEdit_save_pointcloud_path->setText(m_path_to_save_pointcloud);
    ui->lineEdit_save_pol_path->setText(m_path_to_save_pol);
    ui->lineEdit_save_thermal_path->setText(m_path_to_save_thermal);
    ui->lineEdit_save_rgb_path->setText(m_path_to_save_rgb);
    ui->lineEdit_save_wide_path->setText(m_path_to_save_wide);
    ui->lineEdit_save_narrow_path->setText(m_path_to_save_narrow);
    ui->lineEdit_save_thermal_bin_path->setText(m_path_to_save_thermal_bin);

    ui->lineEdit_save_pointcloud_path->setEnabled(m_save_data);
    ui->lineEdit_save_pol_path->setEnabled(m_save_data);
    ui->lineEdit_save_thermal_path->setEnabled(m_save_data);
    ui->lineEdit_save_rgb_path->setEnabled(m_save_data);
    ui->lineEdit_save_wide_path->setEnabled(m_save_data);
    ui->lineEdit_save_narrow_path->setEnabled(m_save_data);
    ui->lineEdit_save_thermal_bin_path->setEnabled(m_save_data);

    ui->pushButton_save_thermal->setEnabled(m_save_data);
    ui->pushButton_save_pol->setEnabled(m_save_data);
    ui->pushButton_save_pointcloud->setEnabled(m_save_data);
    ui->pushButton_save_rgb->setEnabled(m_save_data);
    ui->pushButton_save_wide->setEnabled(m_save_data);
    ui->pushButton_save_narrow->setEnabled(m_save_data);
    ui->pushButton_save_thermal_bin->setEnabled(m_save_data);

}

void MainWindow::changeSaveDataSettings()
{
    if(m_save_thermal_image){
        m_path_to_save_thermal = ui->lineEdit_save_thermal_path->text();
        ui->lineEdit_save_thermal_path->setDisabled(m_save_data);
        ui->pushButton_save_thermal->setDisabled(m_save_data);
    }

    if(m_save_thermal_data_image){
        ui->checkBox_save_thermal_data->setDisabled(m_save_data);
        ui->lineEdit_save_thermal_bin_path->setDisabled(m_save_data);
        ui->pushButton_save_thermal_bin->setDisabled(m_save_data);
    }

    if(m_save_pol_image){
        m_path_to_save_pol = ui->lineEdit_save_pol_path->text();
        ui->lineEdit_save_pol_path->setDisabled(m_save_data);
        ui->pushButton_save_pol->setDisabled(m_save_data);
    }
    if(m_save_pointcloud){
        m_path_to_save_pointcloud = ui->lineEdit_save_pointcloud_path->text();
        ui->lineEdit_save_pointcloud_path->setDisabled(m_save_data);
        ui->pushButton_save_pointcloud->setDisabled(m_save_data);
    }
    if(m_save_rgb_image){
        m_path_to_save_rgb = ui->lineEdit_save_rgb_path->text();
        ui->lineEdit_save_rgb_path->setDisabled(m_save_data);
        ui->pushButton_save_rgb->setDisabled(m_save_data);
    }
    if(m_save_wide_image){
        m_path_to_save_wide = ui->lineEdit_save_wide_path->text();
        ui->lineEdit_save_wide_path->setDisabled(m_save_data);
        ui->pushButton_save_wide->setDisabled(m_save_data);
    }
    if(m_save_narrow_image){
        m_path_to_save_narrow = ui->lineEdit_save_narrow_path->text();
        ui->lineEdit_save_narrow_path->setDisabled(m_save_data);
        ui->pushButton_save_narrow->setDisabled(m_save_data);
    }

    ui->checkBox_save_thermal->setEnabled(m_thermal_sensor != NULL && !m_save_data);
    ui->checkBox_save_pointcloud->setEnabled(m_lidar_sensor != NULL && !m_save_data);
    ui->checkBox_save_pol->setEnabled(m_pol_sensor != NULL && !m_save_data);
    ui->checkBox_save_rgb->setEnabled(m_rgb_sensor != NULL && !m_save_data);
    ui->checkBox_save_wide->setEnabled(m_allied_wide_sensor != NULL && !m_save_data);
    ui->checkBox_save_narrow->setEnabled(m_allied_narrow_sensor != NULL && !m_save_data);

    ui->spinBox_save_counter->setDisabled(m_save_data);

}

void MainWindow::checkAllFramesSaved()
{
    if((m_save_images_rgb_counter == 0) && (m_save_pointcloud_counter == 0) && (m_save_thermal_counter == 0) &&
            (m_save_pol_counter == 0) && (m_save_narrow_counter == 0) && (m_save_wide_counter == 0) && (m_save_thermal_data_counter == 0)){
        on_pushButton_save_clicked();
    }
}

void MainWindow::loadBlurringNetworks()
{
    try{
        // resources are not exposed to the filesytem, thus they cannot be converted to a standard path
        // QFile can access them so we create a temporary file to access its path
        QTemporaryDir tempDirConfig, tempDirWeight;
        if(tempDirConfig.isValid() && tempDirWeight.isValid()) {
            const QString tempFileConfig = tempDirConfig.path() + "/deploy.prototxt";
            const QString tempFileWeight = tempDirWeight.path() + "/res10_300x300_ssd_iter_140000_fp16.caffemodel";
            if(QFile::copy(":/resources/deploy.prototxt", tempFileConfig) && QFile::copy(":/resources/res10_300x300_ssd_iter_140000_fp16.caffemodel", tempFileWeight)) {
                m_faces_net = cv::dnn::readNetFromCaffe(tempFileConfig.toStdString(), tempFileWeight.toStdString());
            }
        }

        m_faces_net.setPreferableBackend(cv::dnn::DNN_TARGET_CPU);

        cv::Mat image = cv::Mat(300, 300, CV_8UC3, cv::Scalar(0,0,0));
        cv::Mat inputBlob;
        std::vector<cv::Mat> netOutput;

        inputBlob = cv::dnn::blobFromImage(image, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0), false, false);

        m_faces_net.setInput(inputBlob);
        m_faces_net.forward(netOutput, "detection_out");

        m_blurring_loaded = true;
        ui->checkBox_blur_faces->setEnabled(true);
    }catch(...){
        addMessageToLogWindow("Error loading blurring networks", logType::warning);
        m_blurring_loaded = false;
    }
}

void MainWindow::applyFaceBlurring(cv::Mat &image)
{
    cv::Mat inputBlob;
    cv::Scalar meanVal(104.0, 177.0, 123.0);
    cv::Mat netOutput;

    int kernelWidth = 105;
    int kernelHeight = 105;
    try{
        inputBlob = cv::dnn::blobFromImage(image, 1.0, cv::Size(300, 300), meanVal, false, false);

        m_faces_net.setInput(inputBlob);

        netOutput = m_faces_net.forward("detection_out");

        cv::Mat detectionMat(netOutput.size[2], netOutput.size[3], CV_32F, netOutput.ptr<float>());

        for (int i = 0; i < detectionMat.rows; i++)
        {
            float confidence = detectionMat.at<float>(i, 2);

            if (confidence > 0.2)
            {
                int x1 = abs(static_cast<int>(detectionMat.at<float>(i, 3) * image.cols));
                int y1 = abs(static_cast<int>(detectionMat.at<float>(i, 4) * image.rows));
                int x2 = abs(static_cast<int>(detectionMat.at<float>(i, 5) * image.cols));
                int y2 = abs(static_cast<int>(detectionMat.at<float>(i, 6) * image.rows));

                //cv::rectangle(frameOpenCVDNN, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2, 4);

                // Setup a rectangle to define your region of interest
                cv::Rect myROI = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));

                //(asubirana) sometimes ROI is outside image (???) and has to be discarded
                if(myROI.x >= image.cols || myROI.y >= image.rows){
                    continue;
                }
                //(asubirana) sometimes ROI has part outside the image and has to be cropped
                if(myROI.x + myROI.width >= image.cols){
                    myROI.width = myROI.width - (myROI.x + myROI.width - image.cols);
                }
                if(myROI.y + myROI.height >= image.rows){
                    myROI.height = myROI.height - (myROI.y + myROI.height - image.rows);
                }
                if(myROI.x < 0){
                    myROI.width = myROI.width + myROI.x;
                    myROI.x = 0;
                }
                if(myROI.y < 0){
                    myROI.height = myROI.height + myROI.y;
                    myROI.y = 0;
                }

                // Blur the image contained by the rectangle myROI
                cv::GaussianBlur(image(myROI), image(myROI), cv::Size(kernelWidth, kernelHeight), 0);
            }
        }
    }catch(...){
        addMessageToLogWindow("Unhandled error at MainWindow::applyFaceBlurring", logType::error);
    }

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

    if(m_lidar_sensor != NULL){
        (m_lidar_sensor->sensor_available) ? m_lidar_status = deviceStatus::no_error : m_lidar_status = deviceStatus::error_s;
    }

    if(m_thermal_sensor != NULL){
        (m_thermal_sensor->sensor_available) ? m_thermal_status = deviceStatus::no_error : m_thermal_status = deviceStatus::error_s;
    }

    if(m_pol_sensor != NULL){
        (m_pol_sensor->sensor_available) ? m_polarimetric_status = deviceStatus::no_error : m_polarimetric_status = deviceStatus::error_s;
    }

    if(m_rgb_sensor != NULL){
        (m_rgb_sensor->sensor_available) ? m_rgb_status = deviceStatus::no_error : m_rgb_status = deviceStatus::error_s;
    }

    if(m_allied_wide_sensor != NULL){
        (m_allied_wide_sensor->sensor_available) ? m_allied_wide_status = deviceStatus::no_error : m_allied_wide_status = deviceStatus::error_s;
    }

    if(m_allied_narrow_sensor != NULL){
        (m_allied_narrow_sensor->sensor_available) ? m_allied_narrow_status = deviceStatus::no_error : m_allied_narrow_status = deviceStatus::error_s;
    }

    updateSensorStatus(m_thermal_status, ui->label_thermal_1_status_value);
    updateSensorStatus(m_polarimetric_status, ui->label_polarimetric_status_value);
    updateSensorStatus(m_rgb_status, ui->label_rgb_status_value);
    updateSensorStatus(m_lidar_status, ui->label_lidar_status_value);
    updateSensorStatus(m_allied_wide_status, ui->label_allied_wide_status_value);
    updateSensorStatus(m_allied_narrow_status, ui->label_allied_narrow_status_value);

    ui->radioButton_wide_camera->setEnabled(m_allied_wide_sensor != NULL);{}
    ui->radioButton_narrow_camera->setEnabled(m_allied_narrow_sensor != NULL);
    if(m_allied_wide_sensor == NULL && m_allied_narrow_sensor != NULL){
        ui->radioButton_narrow_camera->click();
    }

    ui->checkBox_save_thermal->setEnabled(m_thermal_sensor != NULL);
    ui->checkBox_save_pointcloud->setEnabled(m_lidar_sensor != NULL);
    ui->checkBox_save_pol->setEnabled(m_pol_sensor != NULL);
    ui->checkBox_save_rgb->setEnabled(m_rgb_sensor != NULL);
    ui->checkBox_save_wide->setEnabled(m_allied_wide_sensor != NULL);
    ui->checkBox_save_narrow->setEnabled(m_allied_narrow_sensor != NULL);

    if(m_allied_wide_sensor == NULL && m_allied_narrow_sensor == NULL){
        ui->tab_Allied->setDisabled(true);
    }
    if(m_rgb_sensor == NULL){
        ui->groupBoxEconRGBSettings->setDisabled(true);
    }
    if(m_thermal_sensor == NULL){
        ui->groupBoxThermalSettings->setDisabled(true);
    }
    if(m_pol_sensor == NULL){
        ui->groupBox_pol_settings->setDisabled(true);
    }
    if(m_lidar_sensor == NULL){
        ui->tab_LiDAR->setDisabled(true);
    }
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
    ui->listWidget_log_window->addItem(QString("[%1] %2").arg(now, message));
    int items = ui->listWidget_log_window->count();
    ui->listWidget_log_window->item(items-1)->setForeground(color);
    ui->listWidget_log_window->scrollToBottom();
}

void MainWindow::errorNotification(const int *error)
{
    qDebug() << "Error notification received:" << getBeamErrorDescription(*error) << '(' << *error << ')';

    if(mainWindowObj == nullptr) return;

    QMetaObject::invokeMethod(mainWindowObj, "updateSensorError", Qt::QueuedConnection, Q_ARG(int32_t, *error));
}

void MainWindow::enableThermalSettingsForVersion(QString l3cam_version)
{
    //!version is master_a.b.c
    QString ver_num = l3cam_version.split("_")[1];
    QStringList numbers_ver = ver_num.split("."); // a.b.c
    int version = (numbers_ver.at(0).toInt() * 100) + (numbers_ver.at(1).toInt()*10) + (numbers_ver.at(2).toInt());
    ui->comboBox_thermal_color->clear();

    if(version < 229){
        //!old thermal camera library
        m_new_thermal_library = false;
        ui->comboBox_thermal_pipeline->setDisabled(true);
        ui->comboBox_thermal_color->addItem("WHITE");
        ui->comboBox_thermal_color->addItem("BLACK");
        ui->comboBox_thermal_color->addItem("IRON");
        ui->comboBox_thermal_color->addItem("COOL");
        ui->comboBox_thermal_color->addItem("AMBER");
        ui->comboBox_thermal_color->addItem("INDIGO");
        ui->comboBox_thermal_color->addItem("TYRIAN");
        ui->comboBox_thermal_color->addItem("GLORY");
        ui->comboBox_thermal_color->addItem("ENVY");
        ui->comboBox_thermal_color->addItem("WHITE_NEW");
        ui->comboBox_thermal_color->addItem("BLACK_NEW");
        ui->comboBox_thermal_color->addItem("SPECTRA");
        ui->comboBox_thermal_color->addItem("PRISM");
        ui->comboBox_thermal_color->addItem("TYRIAN_NEW");
        ui->comboBox_thermal_color->addItem("AMBER_NEW");
        ui->comboBox_thermal_color->addItem("IRON_NEW");
        ui->comboBox_thermal_color->addItem("HI");
        ui->comboBox_thermal_color->addItem("LO");
    }else{
        //!new thermal camera library
        m_new_thermal_library = true;
        ui->comboBox_thermal_pipeline->setEnabled(true);
        ui->comboBox_thermal_color->addItem("WHITE");
        ui->comboBox_thermal_color->addItem("BLACK");
        ui->comboBox_thermal_color->addItem("SPECTRA");
        ui->comboBox_thermal_color->addItem("PRISM");
        ui->comboBox_thermal_color->addItem("TYRIAN");
        ui->comboBox_thermal_color->addItem("IRON");
        ui->comboBox_thermal_color->addItem("AMBER");
        ui->comboBox_thermal_color->addItem("HI");
        ui->comboBox_thermal_color->addItem("GREEN");
    }

}

int32_t MainWindow::getValueForOldLibrary(int index)
{
    int32_t value = 0;
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
    return value;
}

void MainWindow::updateSensorError(int32_t error)
{
    QString msg = "Error notification received: " + QString(getBeamErrorDescription(error)) + " (" + QString().number(error) + ")";
    addMessageToLogWindow(msg, logType::error);

    switch(error)
    {
    case ERROR_LIDAR_TIMED_OUT:
        m_lidar_status = deviceStatus::error_s;
        updateSensorStatus(m_lidar_status, ui->label_lidar_status_value);
        break;
    case ERROR_THERMAL_CAMERA_TIMEOUT:
        m_thermal_status = deviceStatus::error_s;
        updateSensorStatus(m_thermal_status, ui->label_thermal_1_status_value);
        break;
    case L3CAM_ERROR_DEVICE_DISCONNECTED:
        break;
    }
}

void MainWindow::on_checkBox_show_axis_clicked(bool checked)
{
    m_point_cloud_viewer->setAxisEnabled(checked);
}

void MainWindow::on_pushButton_apply_filter_clicked()
{
    int error = CHANGE_DISTANCE_RANGE(m_devices[0], ui->min_distance_box->value(), ui->max_distance_box->value());
    if(error != L3CAM_OK){
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

    int error = CHANGE_POINT_CLOUD_COLOR_RANGE(m_devices[0], min_value, max_value);

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)),logType::error);
    }
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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

}

void MainWindow::on_horizontalSlider_allied_wide_max_exposure_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_allied_wide_min_exposure->value(), (float)ui->horizontalSlider_allied_wide_max_exposure->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_allied_wide_min_exposure->value(), (float)ui->horizontalSlider_allied_wide_max_exposure->value());
    }

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error );
    }
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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

}

void MainWindow::on_horizontalSlider_allied_wide_min_gain_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_GAIN_RANGE(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_allied_wide_min_gain->value(), (float)ui->horizontalSlider_allied_wide_max_gain->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_AUTO_GAIN_RANGE(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_allied_wide_min_gain->value(), (float)ui->horizontalSlider_allied_wide_max_gain->value());
    }

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }
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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

}

void MainWindow::on_horizontalSlider_intensity_controller_target_wide_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_TARGET(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_intensity_controller_target_wide->value());
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_TARGET(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_intensity_controller_target_wide->value());
    }

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)),logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)),logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

}

void MainWindow::on_checkBox_allied_wide_white_bal_clicked(bool checked)
{
    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = ENABLE_ALLIED_CAMERA_AUTO_WHITE_BALANCE(m_devices[0], *m_allied_wide_sensor, checked);
    }else if(m_current_allied_camera == narrow_camera){
        error = ENABLE_ALLIED_CAMERA_AUTO_WHITE_BALANCE(m_devices[0], *m_allied_narrow_sensor, checked);
    }

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error );
    }

}

void MainWindow::on_horizontalSlider_white_balance_ratio_allied_wide_sliderReleased()
{

    int error = L3CAM_OK;
    if(m_current_allied_camera == wide_camera){
        error = CHANGE_ALLIED_CAMERA_BALANCE_RATIO(m_devices[0], *m_allied_wide_sensor, (float)ui->horizontalSlider_white_balance_ratio_allied_wide->value()/10.0);
    }else if(m_current_allied_camera == narrow_camera){
        error = CHANGE_ALLIED_CAMERA_BALANCE_RATIO(m_devices[0], *m_allied_narrow_sensor, (float)ui->horizontalSlider_white_balance_ratio_allied_wide->value()/10.0);
    }

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error );
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }
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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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

    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(__func__) + " Response " + QString::number(error) + " - " + QString(getBeamErrorDescription(error)), logType::error);
    }

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


void MainWindow::on_checkBox_save_pointcloud_clicked(bool checked)
{
    m_save_pointcloud = checked;
    ui->lineEdit_save_pointcloud_path->setEnabled(m_save_pointcloud);
    ui->pushButton_save_pointcloud->setEnabled(m_save_pointcloud);
}


void MainWindow::on_checkBox_save_thermal_clicked(bool checked)
{
    m_save_thermal_image = checked;
    ui->lineEdit_save_thermal_path->setEnabled(m_save_thermal_image);
    ui->pushButton_save_thermal->setEnabled(m_save_thermal_image);
}


void MainWindow::on_checkBox_save_rgb_clicked(bool checked)
{
    m_save_rgb_image = checked;
    ui->lineEdit_save_rgb_path->setEnabled(checked);
    ui->pushButton_save_rgb->setEnabled(checked);
}


void MainWindow::on_checkBox_save_pol_clicked(bool checked)
{
    m_save_pol_image = checked;
    ui->lineEdit_save_pol_path->setEnabled(m_save_pol_image);
    ui->pushButton_save_pol->setEnabled(m_save_pol_image);
}


void MainWindow::on_checkBox_save_wide_clicked(bool checked)
{
    m_save_wide_image = checked;
    ui->lineEdit_save_wide_path->setEnabled(m_save_wide_image);
    ui->pushButton_save_wide->setEnabled(m_save_wide_image);
}


void MainWindow::on_checkBox_save_narrow_clicked(bool checked)
{
    m_save_narrow_image = checked;
    ui->lineEdit_save_narrow_path->setEnabled(m_save_narrow_image);
    ui->pushButton_save_narrow->setEnabled(m_save_narrow_image);
}


void MainWindow::on_pushButton_save_pointcloud_clicked()
{
    setPathToSaveData(ui->lineEdit_save_pointcloud_path);
    m_save_pointcloud_executor->setPathToSavePcd(ui->lineEdit_save_pointcloud_path->text());
}


void MainWindow::on_pushButton_save_thermal_clicked()
{
    setPathToSaveData(ui->lineEdit_save_thermal_path);
    m_save_thermal_image_executor->setPathToSaveImages(ui->lineEdit_save_thermal_path->text());
}


void MainWindow::on_pushButton_save_rgb_clicked()
{
    setPathToSaveData(ui->lineEdit_save_rgb_path);
    m_save_rgb_image_executor->setPathToSaveImages(ui->lineEdit_save_rgb_path->text());
}


void MainWindow::on_pushButton_save_pol_clicked()
{
    setPathToSaveData(ui->lineEdit_save_pol_path);
    m_save_polarimetric_executor->setPathToSaveImages(ui->lineEdit_save_pol_path->text());
}


void MainWindow::on_pushButton_save_wide_clicked()
{
    setPathToSaveData(ui->lineEdit_save_wide_path);
    m_save_polarimetric_executor->setPathToSaveImages(ui->lineEdit_save_wide_path->text());
}


void MainWindow::on_pushButton_save_narrow_clicked()
{
    setPathToSaveData(ui->lineEdit_save_narrow_path);
    m_save_rgb_image_executor->setPathToSaveImages(ui->lineEdit_save_narrow_path->text());
}


void MainWindow::thermalSaveExecutorIsAvailable(bool available)
{
    if(m_save_data && m_save_thermal_image){
        m_save_thermal_image_manager->setExecutorAvailable(available);
    }
}

void MainWindow::thermalDataSaveExecutorAvailable(bool available)
{
    if(m_save_data && m_save_thermal_data_image){
        m_save_thermal_data_manager->setExecutorAvailable(available);
    }
}

void MainWindow::rgbSaveExecutorIsAvailable(bool available)
{
    if(m_save_data && (m_save_rgb_image || m_save_narrow_image)){
        m_save_rgb_image_manager->setExecutorAvailable(available);
    }
}

void MainWindow::pointcloudSaveExecutorIsAvailable(bool available)
{
    if(m_save_data && m_save_pointcloud){
        m_save_pointcloud_manager->setExecutorAvailable(available);
    }
}

void MainWindow::polSaveExecutorIsAvailable(bool available)
{
    if(m_save_data && (m_save_pol_image || m_save_wide_image)) {
        m_save_polarimetric_manager->setExecutorAvailable(available);
    }
}


void MainWindow::thermalImageToSaveReceived(imageData data)
{
    QString file_name = QString("%1").arg(data.timestamp);
    m_save_thermal_image_executor->doSavePointerToPng(data.image_buffer, data.image_width, data.image_height, data.image_channels, file_name);
    free(data.image_buffer);
}

void MainWindow::thermalDataBufferToSaveReceived(binaryFloatData data)
{
    QString file_name = QString("%1").arg(data.timestamp);

    m_save_thermal_data_executor->doSaveFloatData(data.data_buffer, data.data_size, file_name);
    free(data.data_buffer);
}

void MainWindow::rgbImageToSaveReceived(imageData data)
{
    QString file_name = QString("%1").arg(data.timestamp);

    cv::Mat image = cv::Mat(data.image_width, data.image_height, CV_8UC3, data.image_buffer );

    m_save_rgb_image_executor->doSavePointerToPng(image.data, data.image_width, data.image_height, data.image_channels, file_name);

    free(data.image_buffer);
}

void MainWindow::pointcloudToSaveReceived(pointcloudData data)
{
    QString file_name = QString("%1").arg(data.timestamp);
    m_save_pointcloud_executor->doSaveBinaryData(data.pointcloud_buffer, file_name);

    free(data.pointcloud_buffer);
}

void MainWindow::polImageToSaveReceived(imageData data)
{
    QString file_name = QString("%1").arg(data.timestamp);
    m_save_polarimetric_executor->doSavePointerToPng(data.image_buffer, data.image_width, data.image_height, data.image_channels, file_name);
    free(data.image_buffer);
}

void MainWindow::on_checkBox_blur_faces_clicked(bool checked)
{
    m_apply_blurring = checked;
}


void MainWindow::on_pushButton_save_clicked()
{
    if(!m_save_rgb_image && !m_save_pointcloud && !m_save_thermal_image &&
            !m_save_pol_image && !m_save_wide_image && !m_save_narrow_image && !m_save_thermal_data_image)
    {
        m_save_data = false;
        return;
    }

    m_save_data = !m_save_data;

    if(m_save_data){

        if(ui->checkBox_save_pointcloud->isChecked()){
            m_save_pointcloud_executor->setPathToSavePcd(ui->lineEdit_save_pointcloud_path->text());
        }

        if(ui->checkBox_save_thermal->isChecked()){
            m_save_thermal_image_executor->setPathToSaveImages(ui->lineEdit_save_thermal_path->text());
        }

        if(ui->checkBox_save_thermal_data->isChecked()){
            m_save_thermal_data_executor->setPathToSaveImages(ui->lineEdit_save_thermal_bin_path->text());
        }

        if(ui->checkBox_save_rgb->isChecked()){
            m_save_rgb_image_executor->setPathToSaveImages(ui->lineEdit_save_rgb_path->text());
        }

        if(ui->checkBox_save_pol->isChecked()){
            m_save_polarimetric_executor->setPathToSaveImages(ui->lineEdit_save_pol_path->text());
        }

        if(ui->checkBox_save_wide->isChecked()){
            m_save_polarimetric_executor->setPathToSaveImages(ui->lineEdit_save_wide_path->text());
        }

        if(ui->checkBox_save_narrow->isChecked()){
            m_save_rgb_image_executor->setPathToSaveImages(ui->lineEdit_save_narrow_path->text());
        }


        m_save_images_counter = ui->spinBox_save_counter->value();

        if(m_save_images_counter < 0){
            m_save_all = true;

        }else{

            m_save_images_rgb_counter = (m_save_rgb_image ? m_save_images_counter : 0);
            m_save_pointcloud_counter = (m_save_pointcloud ? m_save_images_counter : 0);
            m_save_thermal_counter = (m_save_thermal_image ? m_save_images_counter : 0);
            m_save_thermal_data_counter = (m_save_thermal_data_image ? m_save_images_counter : 0);
            m_save_pol_counter = (m_save_pol_image ? m_save_images_counter : 0);
            m_save_wide_counter = (m_save_wide_image ? m_save_images_counter : 0);
            m_save_narrow_counter = (m_save_narrow_image ? m_save_images_counter : 0);
        }


    }

    changeSaveDataSettings();

    m_save_data ? (ui->pushButton_save->setStyleSheet(m_red_button_style)) : (ui->pushButton_save->setStyleSheet(m_green_button_style));

    if(!m_save_data){
        m_save_all = false;
    }
}


void MainWindow::on_horizontalSlider_pol_bri_sliderReleased()
{
    int error = CHANGE_POLARIMETRIC_CAMERA_BRIGHTNESS(m_devices[0], (uint32_t)ui->horizontalSlider_pol_bri->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}


void MainWindow::on_pushButton_pol_black_level_clicked()
{
    int error = CHANGE_POLARIMETRIC_CAMERA_BLACK_LEVEL(m_devices[0], ui->doubleSpinBox_black_level->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}


void MainWindow::on_horizontalSlider_pol_gain_sliderReleased()
{
    int error = CHANGE_POLARIMETRIC_CAMERA_GAIN(m_devices[0], ui->horizontalSlider_pol_gain->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}


void MainWindow::on_checkBox_pol_gain_clicked(bool checked)
{
    int error = ENABLE_POLARIMETRIC_CAMERA_AUTO_GAIN(m_devices[0], checked);
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
        ui->checkBox_pol_gain->setChecked(!checked);
    }
    else {
        ui->horizontalSlider_pol_gain->setDisabled(checked);
        ui->doubleSpinBox_pol_gain_min->setEnabled(checked);
        ui->doubleSpinBox_pol_gain_max->setEnabled(checked);
        ui->pushButton_set_pol_gain_minmax->setEnabled(checked);
    }
}


void MainWindow::on_pushButton_set_pol_gain_minmax_clicked()
{
    int error = CHANGE_POLARIMETRIC_CAMERA_AUTO_GAIN_RANGE(m_devices[0], ui->doubleSpinBox_pol_gain_min->value(), ui->doubleSpinBox_pol_gain_max->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}


void MainWindow::on_checkBox_pol_exposure_clicked(bool checked)
{
    int error = ENABLE_POLARIMETRIC_CAMERA_AUTO_EXPOSURE_TIME(m_devices[0], checked);
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
        ui->checkBox_pol_exposure->setChecked(!checked);
    }
    else {
        ui->doubleSpinBox_pol_expo->setDisabled(checked);
        ui->pushButton_pol_exposure->setDisabled(checked);
        ui->doubleSpinBox_pol_expo_min->setEnabled(checked);
        ui->doubleSpinBox_pol_expo_max->setEnabled(checked);
        ui->pushButton_set_pol_expo_minmax->setEnabled(checked);
    }
}


void MainWindow::on_pushButton_pol_exposure_clicked()
{
    int error = CHANGE_POLARIMETRIC_CAMERA_EXPOSURE_TIME(m_devices[0], ui->doubleSpinBox_pol_expo->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}


void MainWindow::on_pushButton_set_pol_expo_minmax_clicked()
{
    int error = CHANGE_POLARIMETRIC_CAMERA_AUTO_EXPOSURE_TIME_RANGE(m_devices[0], ui->doubleSpinBox_pol_expo_min->value(), ui->doubleSpinBox_pol_expo_max->value());
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}


void MainWindow::on_horizontalSlider_pol_bri_valueChanged(int value)
{
    ui->label_pol_bri_val->setText(QString("%1").arg(value));
}


void MainWindow::on_horizontalSlider_pol_gain_valueChanged(int value)
{
    ui->label_pol_gain_value->setText(QString("%1").arg(value));
}


void MainWindow::on_pushButton_restore_pol_default_clicked()
{
    initializePolDefault();

    int error = SET_POLARIMETRIC_CAMERA_DEFAULT_SETTINGS(m_devices[0]);
    if(error) {
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}


void MainWindow::on_pushButton_set_pol_protocol_clicked()
{
    streamingProtocols protocol;
    switch(ui->comboBox_pol_protocol->currentIndex()){
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

    if(m_pol_sensor != NULL){
        m_pol_sensor->protocol = protocol;
        CHANGE_STREAMING_PROTOCOL(m_devices[0], m_pol_sensor);
    }
}


void MainWindow::on_pushButton_get_pol_pipeline_clicked()
{
    char *pipeline;
    GET_RTSP_PIPELINE(m_devices[0], *m_pol_sensor, &pipeline);
    QString string_pipeline = QString("%1").arg(pipeline);

    ui->lineEdit_pol_rtsp->setText(string_pipeline);
}


void MainWindow::on_pushButton_get_curr_params_clicked()
{
    //! Deshabilitar la pestaña de las allied mientras se reciben los parametros
    ui->tab_Allied->setEnabled(false);

    int error = L3CAM_OK;
    sensor *allied_id = ui->radioButton_wide_camera->isChecked() ? m_allied_wide_sensor : m_allied_narrow_sensor;

    float black_level = -1.0;
    error = GET_ALLIED_CAMERA_BLACK_LEVEL(m_devices[0], *allied_id, &black_level);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_wide_black_level->setValue(black_level);
    }

    bool auto_exposure_enabled = false;
    error = GET_ALLIED_CAMERA_AUTO_EXPOSURE_TIME(m_devices[0], *allied_id, &auto_exposure_enabled);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->checkBox_allied_wide_exposure->setChecked(auto_exposure_enabled);

        ui->horizontalSlider_allied_wide_exposure->setDisabled(auto_exposure_enabled);

        ui->horizontalSlider_allied_wide_max_exposure->setEnabled(auto_exposure_enabled);
        ui->horizontalSlider_allied_wide_min_exposure->setEnabled(auto_exposure_enabled);
    }

    float exposure_time = -1.0;
    error = GET_ALLIED_CAMERA_EXPOSURE_TIME_US(m_devices[0], *allied_id, &exposure_time);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);

    }else{
        ui->horizontalSlider_allied_wide_exposure->setValue(exposure_time);
    }

    float exposure_min = -1.0;
    float exposure_max = -1.0;
    error = GET_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(m_devices[0], *allied_id, &exposure_min, &exposure_max);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);

    }else{
        ui->horizontalSlider_allied_wide_min_exposure->setValue(exposure_min);
        ui->horizontalSlider_allied_wide_max_exposure->setValue(exposure_max);
    }

    float gain = -1.0;
    error = GET_ALLIED_CAMERA_GAIN(m_devices[0], *allied_id, &gain);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);

    }else{
        ui->horizontalSlider_gain_wide->setValue(gain);
    }

    bool auto_gain_enabled = false;
    error = GET_ALLIED_CAMERA_AUTO_GAIN(m_devices[0], *allied_id, &auto_gain_enabled);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);

    }else{
        ui->checkBox_allied_wide_gain->setChecked(auto_gain_enabled);

        ui->horizontalSlider_gain_wide->setDisabled(auto_gain_enabled);

        //!habilitar el rango del autoexposure time
        ui->horizontalSlider_allied_wide_max_gain->setEnabled(auto_gain_enabled);
        ui->horizontalSlider_allied_wide_min_gain->setEnabled(auto_gain_enabled);
    }

    float gain_min = -1.0;
    float gain_max = -1.0;
    error = GET_ALLIED_CAMERA_AUTO_GAIN_RANGE(m_devices[0], *allied_id, &gain_min, &gain_max);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_allied_wide_min_gain->setValue(gain_min);
        ui->horizontalSlider_allied_wide_max_gain->setValue(gain_max);
    }

    float gamma = -1.0;
    error = GET_ALLIED_CAMERA_GAMMA(m_devices[0], *allied_id, &gamma);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_gamma_wide->setValue(gamma);
    }

    float saturation = -1.0;
    error = GET_ALLIED_CAMERA_SATURATION(m_devices[0], *allied_id, &saturation);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_saturation_wide->setValue(saturation);
    }

    int sharpness = -1;
    error = GET_ALLIED_CAMERA_SHARPNESS(m_devices[0], *allied_id, &sharpness);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_sharpness_wide->setValue(sharpness);
    }

    float hue = -1.0;
    error = GET_ALLIED_CAMERA_HUE(m_devices[0], *allied_id, &hue);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_hue_wide->setValue(hue);
    }

    int iap_mode = -1;
    error = GET_ALLIED_CAMERA_INTENSITY_AUTO_PRECEDENCE(m_devices[0], *allied_id, &iap_mode);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->comboBox_intensity_auto_precedence->setCurrentIndex(iap_mode);
    }

    int ratio_selector = -1;
    error = GET_ALLIED_CAMERA_BALANCE_RATIO_SELECTOR(m_devices[0], *allied_id, &ratio_selector);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->comboBox_white_balance_ratio_selector->setCurrentIndex(ratio_selector);
    }

    float balance_ratio = -1.0;
    error = GET_ALLIED_CAMERA_BALANCE_RATIO(m_devices[0], *allied_id, &balance_ratio);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_white_balance_ratio_allied_wide->setValue(balance_ratio);
    }

    bool auto_white_balance_enabled = false;
    error = GET_ALLIED_CAMERA_AUTO_WHITE_BALANCE(m_devices[0], *allied_id, &auto_white_balance_enabled);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->checkBox_allied_wide_white_bal->setChecked(auto_white_balance_enabled);

        ui->horizontalSlider_white_balance_auto_tol_allied_wide->setEnabled(auto_white_balance_enabled);
        ui->horizontalSlider_white_balance_auto_rate_allied_wide->setEnabled(auto_white_balance_enabled);

        ui->horizontalSlider_white_balance_ratio_allied_wide->setDisabled(auto_white_balance_enabled);
        ui->comboBox_white_balance_ratio_selector->setDisabled(auto_white_balance_enabled);
    }

    float balance_white_auto_rate = -1.0;
    error = GET_ALLIED_CAMERA_BALANCE_WHITE_AUTO_RATE(m_devices[0], *allied_id, &balance_white_auto_rate);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_white_balance_auto_rate_allied_wide->setValue(balance_white_auto_rate);
    }

    float balance_white_auto_tolerance = -1.0;
    error = GET_ALLIED_CAMERA_BALANCE_WHITE_AUTO_TOLERANCE(m_devices[0], *allied_id, &balance_white_auto_tolerance);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_white_balance_auto_tol_allied_wide->setValue(balance_white_auto_tolerance);
    }

    int height = -1;
    int width = -1;
    error = GET_ALLIED_CAMERA_AUTO_MODE_REGION(m_devices[0], *allied_id, &height, &width);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_allied_wide_auto_mode_height->setValue(height);
        ui->horizontalSlider_allied_wide_auto_mode_width->setValue(width);
    }

    int icr_mode = -1;
    error = GET_ALLIED_CAMERA_INTENSITY_CONTROLLER_REGION(m_devices[0], *allied_id, &icr_mode);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->comboBox_intensity_controller_region_wide->setCurrentIndex(icr_mode);
    }

    float intensity_controller_target = -1.0;
    error = GET_ALLIED_CAMERA_INTENSITY_CONTROLLER_TARGET(m_devices[0], *allied_id, &intensity_controller_target);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_intensity_controller_target_wide->setValue(intensity_controller_target);
    }

    int max_driver_buffers_count = -1;
    error = GET_ALLIED_CAMERA_MAX_DRIVER_BUFFERS_COUNT(m_devices[0], *allied_id, &max_driver_buffers_count);
    if(error){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }else{
        ui->horizontalSlider_max_buffers_count_wide->setValue(max_driver_buffers_count);
    }

    //! Enable allied settings tab when finished
    ui->tab_Allied->setEnabled(true);

}

void MainWindow::on_checkBox_auto_bias_clicked(bool checked)
{
    ENABLE_AUTO_BIAS(m_devices[0], checked);
}

void MainWindow::on_horizontalSlider_bias_valueChanged(int value)
{
    ui->label_bias_value->setText(QString("%1 mV").arg(value));

}

void MainWindow::on_horizontalSlider_bias_left_valueChanged(int value)
{
    ui->label_bias_value_left->setText(QString("%1 mV").arg(value));
}

void MainWindow::on_horizontalSlider_bias_sliderReleased()
{
    int bias = ui->horizontalSlider_bias->value();
    CHANGE_BIAS_VALUE(m_devices[0], indexTdc::right, bias);

}

void MainWindow::on_horizontalSlider_bias_left_sliderReleased()
{

    int bias = ui->horizontalSlider_bias_left->value();
    CHANGE_BIAS_VALUE(m_devices[0], indexTdc::left, bias);

}

void MainWindow::pointSelectedNotification(QString data)
{
    addMessageToLogWindow(data);
}

void MainWindow::on_comboBox_thermal_pipeline_currentIndexChanged(int index)
{
    if(m_initializing_thermal_settings){
        return;
    }

    int error = CHANGE_THERMAL_CAMERA_PROCESSING_PIPELINE(m_devices[0], (int32_t) index);
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }
}

void MainWindow::on_checkBox_enable_udp_temperatures_clicked(bool checked)
{
    int error = ENABLE_THERMAL_CAMERA_TEMPERATURE_DATA_UDP(m_devices[0], checked);
    if(error != L3CAM_OK){
        addMessageToLogWindow(QString(getBeamErrorDescription(error)), logType::error);
    }

    ui->checkBox_save_thermal_data->setEnabled(checked);

    if(checked){
        m_temperatures_viewer->show();
    }else{
        m_temperatures_viewer->hide();
    }
}

void MainWindow::on_checkBox_save_thermal_data_clicked(bool checked)
{
    m_save_thermal_data_image = checked;

    ui->lineEdit_save_thermal_bin_path->setEnabled(m_save_thermal_data_image);
    ui->pushButton_save_thermal_bin->setEnabled(m_save_thermal_data_image);
}

void MainWindow::on_pushButton_save_thermal_bin_clicked()
{
    setPathToSaveData(ui->lineEdit_save_thermal_bin_path);
    m_save_thermal_data_executor->setPathToSaveImages(ui->lineEdit_save_thermal_bin_path->text());
}
