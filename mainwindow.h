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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QLabel>
#include <QLineEdit>

#include <libs/libL3Cam/libL3Cam.h>

#include "libL3Cam.h"
#include "libL3Cam_polarimetric.h"
#include "libL3Cam_econ.h"
#include "libL3Cam_thermal.h"
#include "libL3Cam_allied.h"
#include "beamagine.h"
#include "beamErrors.h"


#include <udpreceivercontroller.h>
#include <saveDataManager.h>
#include <imageSaveDataExecutor.h>
#include <pointCloudSaveDataExecutor.h>

#include <pclPointCloudViewerController.h>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>

typedef struct logType{
    static const uint8_t error = 0;
    static const uint8_t warning = 1;
    static const uint8_t verbose = 2;
}logType;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setMainWindowObj(MainWindow *ptr);

private:
    void deviceDetected();

    void initializeGUI();

    void initializeRgbDefault();

    void initializePolDefault();

    void initializeReceivers();

    void closeEvent(QCloseEvent *event);

    void drawDetections(cv::Mat &image_to_show, std::vector<detectionImage> detections, uint8_t threshold);

    void setPathToSaveData(QLineEdit *line_edit);

    void enableSaveConfiguration();

    void changeSaveDataSettings();

    void checkAllFramesSaved();

    void loadBlurringNetworks();

    void applyFaceBlurring(cv::Mat &image);

    void updateSensorStatus(uint8_t device_status, QLabel *status_label);

    void initializeSystemStatus();

    void addMessageToLogWindow(QString message, uint8_t level=logType::verbose);

    static void errorNotification(int const *error);

public slots:

    void updateSensorError(int32_t error);

private slots:

    void on_pushButton_getVersion_clicked();

    void on_pushButton_initialize_clicked();

    void on_pushButton_findDevices_clicked();

    void on_pushButton_status_clicked();

    void on_pushButton_sensors_clicked();

    void on_pushButton_fast_init_clicked();

    void searchTimerTimeOut();

    void on_checkBox_enable_temp_filter_clicked(bool checked);

    void on_pushButton_apply_thermal_filter_clicked();

    void on_comboBox_thermal_color_currentIndexChanged(int index);

    void on_checkBox_rgb_white_bal_clicked(bool checked);

    void on_checkBox_rgb_exp_clicked(bool checked);

    //void on_checkBox_enable_double_clicked(bool checked);

    void on_pushButton_set_rgb_protocol_clicked();

    void on_pushButton_get_rgb_pipeline_clicked();

    void on_pushButton_set_rgb_image_size_clicked();

    void on_pushButton_set_framerate_clicked();

    void on_horizontalSlider_rgb_bri_valueChanged(int value);

    void on_horizontalSlider_contrast_valueChanged(int value);

    void on_horizontalSlider_saturation_valueChanged(int value);

    void on_horizontalSlider_sharpness_valueChanged(int value);

    void on_horizontalSlider_gamma_valueChanged(int value);

    void on_horizontalSlider_gain_valueChanged(int value);

    void on_horizontalSlider_white_balance_valueChanged(int value);

    void on_horizontalSlider_exposure_valueChanged(int value);

    void on_pushButton_rgb_default_clicked();

    void on_horizontalSlider_rgb_bri_sliderReleased();

    void on_horizontalSlider_contrast_sliderReleased();

    void on_horizontalSlider_saturation_sliderReleased();

    void on_horizontalSlider_sharpness_sliderReleased();

    void on_horizontalSlider_gamma_sliderReleased();

    void on_horizontalSlider_gain_sliderReleased();

    void on_horizontalSlider_white_balance_sliderReleased();

    void on_horizontalSlider_exposure_sliderReleased();

    void on_pushButton_power_off_clicked();

    void on_pushButton_start_device_clicked();

    void on_pushButton_start_streaming_clicked();

    void on_radioButton_range3D_clicked(bool checked);

    void on_radioButton_range3DZ_clicked(bool checked);

    void on_radioButton_intensity_clicked(bool checked);

    void on_radioButton_fusion_thermal_clicked(bool checked);

    void on_radioButton_fusion_mono_clicked(bool checked);

    void pointCloudReadyToShow(int32_t* pointcloud_data, uint32_t timestamp);

    void imageRgbReadyToShow(uint8_t* image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp);

    void imageRgbPolReadyToShow(uint8_t* image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp);

    void imageThermalReadyToShow(uint8_t* image_data, uint16_t height, uint16_t width, uint8_t channels, std::vector<detectionImage> detections, uint32_t timestamp);

    void on_pushButton_set_th_protocol_clicked();

    void on_pushButton_get_thermal_pipeline_clicked();

    void on_pushButton_get_lidar_pipeline_clicked();

    void on_pushButton_depth_rtsp_clicked();

    void on_pushButton_rgb_rtsp_clicked();

    void on_pushButton_pol_rtsp_clicked();

    void on_pushButton_ther_rtsp_clicked();

    void on_pushButton_wide_rtsp_clicked();

    void on_pushButton_narrow_rtsp_clicked();

    void on_checkBox_show_axis_clicked(bool checked);

    void on_pushButton_apply_filter_clicked();

    void on_pushButton_apply_color_ranges_clicked();

    void on_pushButton_set_lidar_protocol_clicked();

    void on_pushButton_set_network_settings_clicked();

    void on_pushButton_terminate_clicked();

    void on_checkBox_allied_wide_exposure_clicked(bool checked);

    void on_radioButton_wide_camera_clicked(bool checked);

    void on_radioButton_narrow_camera_clicked(bool checked);

    void on_horizontalSlider_allied_wide_exposure_valueChanged(int value);

    void on_horizontalSlider_allied_wide_exposure_sliderReleased();

    void on_horizontalSlider_allied_wide_min_exposure_valueChanged(int value);

    void on_horizontalSlider_allied_wide_min_exposure_sliderReleased();

    void on_horizontalSlider_allied_wide_max_exposure_sliderReleased();

    void on_horizontalSlider_allied_wide_max_exposure_valueChanged(int value);

    void on_checkBox_allied_wide_gain_clicked(bool checked);

    void on_horizontalSlider_gain_wide_valueChanged(int value);

    void on_horizontalSlider_gain_wide_sliderReleased();

    void on_horizontalSlider_allied_wide_min_gain_sliderReleased();

    void on_horizontalSlider_allied_wide_min_gain_valueChanged(int value);

    void on_horizontalSlider_allied_wide_max_gain_valueChanged(int value);

    void on_horizontalSlider_allied_wide_max_gain_sliderReleased();

    void on_horizontalSlider_intensity_controller_target_wide_sliderReleased();

    void on_horizontalSlider_intensity_controller_target_wide_valueChanged(int value);

    void on_comboBox_intensity_controller_region_wide_currentIndexChanged(int index);

    void on_horizontalSlider_allied_wide_auto_mode_height_valueChanged(int value);

    void on_horizontalSlider_allied_wide_auto_mode_height_sliderReleased();

    void on_horizontalSlider_allied_wide_auto_mode_width_valueChanged(int value);

    void on_horizontalSlider_allied_wide_auto_mode_width_sliderReleased();

    void on_horizontalSlider_max_buffers_count_wide_valueChanged(int value);

    void on_horizontalSlider_max_buffers_count_wide_sliderReleased();

    void on_checkBox_allied_wide_white_bal_clicked(bool checked);

    void on_horizontalSlider_white_balance_auto_tol_allied_wide_sliderReleased();

    void on_horizontalSlider_white_balance_auto_tol_allied_wide_valueChanged(int value);

    void on_horizontalSlider_white_balance_auto_rate_allied_wide_sliderReleased();

    void on_horizontalSlider_white_balance_auto_rate_allied_wide_valueChanged(int value);

    void on_comboBox_white_balance_ratio_selector_currentIndexChanged(int index);

    void on_horizontalSlider_white_balance_ratio_allied_wide_sliderReleased();

    void on_horizontalSlider_white_balance_ratio_allied_wide_valueChanged(int value);

    void on_horizontalSlider_wide_black_level_sliderReleased();

    void on_horizontalSlider_wide_black_level_valueChanged(int value);

    void on_horizontalSlider_gamma_wide_sliderReleased();

    void on_horizontalSlider_gamma_wide_valueChanged(int value);

    void on_horizontalSlider_saturation_wide_sliderReleased();

    void on_horizontalSlider_saturation_wide_valueChanged(int value);

    void on_horizontalSlider_sharpness_wide_sliderReleased();

    void on_horizontalSlider_sharpness_wide_valueChanged(int value);

    void on_horizontalSlider_hue_wide_sliderReleased();

    void on_horizontalSlider_hue_wide_valueChanged(int value);

    void on_comboBox_intensity_auto_precedence_currentIndexChanged(int index);

    void on_pushButton_set_narrow_protocol_clicked();

    void on_pushButton_set_wide_protocol_clicked();

    void on_pushButton_temperatures_clicked();

    void on_pushButton_get_allied_pipeline_clicked();

    void on_pushButton_long_range_clicked();

    void on_pushButton_short_range_clicked();

    void on_actionAbout_triggered();

    void on_comboBox_background_currentIndexChanged(int index);

    void on_checkBox_save_pointcloud_clicked(bool checked);

    void on_checkBox_save_thermal_clicked(bool checked);

    void on_checkBox_save_rgb_clicked(bool checked);

    void on_checkBox_save_pol_clicked(bool checked);

    void on_checkBox_save_wide_clicked(bool checked);

    void on_checkBox_save_narrow_clicked(bool checked);

    void on_pushButton_save_pointcloud_clicked();

    void on_pushButton_save_thermal_clicked();

    void on_pushButton_save_rgb_clicked();

    void on_pushButton_save_pol_clicked();

    void on_pushButton_save_wide_clicked();

    void on_pushButton_save_narrow_clicked();

    void thermalSaveExecutorIsAvailable(bool available);

    void rgbSaveExecutorIsAvailable(bool available);

    void pointcloudSaveExecutorIsAvailable(bool available);

    void polSaveExecutorIsAvailable(bool available);

    void thermalImageToSaveReceived(imageData data);

    void rgbImageToSaveReceived(imageData data);

    void pointcloudToSaveReceived(pointcloudData data);

    void polImageToSaveReceived(imageData data);

    void on_checkBox_blur_faces_clicked(bool checked);

    void on_pushButton_save_clicked();

    void on_horizontalSlider_pol_bri_sliderReleased();

    void on_pushButton_pol_black_level_clicked();

    void on_horizontalSlider_pol_gain_sliderReleased();

    void on_checkBox_pol_gain_clicked(bool checked);

    void on_pushButton_set_pol_gain_minmax_clicked();

    void on_checkBox_pol_exposure_clicked(bool checked);

    void on_pushButton_pol_exposure_clicked();

    void on_pushButton_set_pol_expo_minmax_clicked();

    void on_horizontalSlider_pol_bri_valueChanged(int value);

    void on_horizontalSlider_pol_gain_valueChanged(int value);

    void on_pushButton_restore_pol_default_clicked();

    void on_pushButton_set_pol_protocol_clicked();

    void on_pushButton_get_pol_pipeline_clicked();

    void on_pushButton_get_curr_params_clicked();

    void on_checkBox_auto_bias_clicked(bool checked);

    void on_horizontalSlider_bias_valueChanged(int value);

    void on_horizontalSlider_bias_left_valueChanged(int value);

    void on_horizontalSlider_bias_sliderReleased();

    void on_horizontalSlider_bias_left_sliderReleased();

    void pointSelectedNotification(QString data);

private:
    Ui::MainWindow *ui;

    udpReceiverController *m_rgb_image_reader;
    udpReceiverController *m_thermal_image_reader;
    udpReceiverController *m_pointcloud_reader;
    udpReceiverController *m_rgb_pol_image_reader;

    saveDataManager* m_save_thermal_image_manager;
    imageSaveDataExecutor *m_save_thermal_image_executor;

    saveDataManager* m_save_rgb_image_manager;
    imageSaveDataExecutor *m_save_rgb_image_executor;

    saveDataManager* m_save_pointcloud_manager;
    pointCloudSaveDataExecutor *m_save_pointcloud_executor;

    saveDataManager *m_save_polarimetric_manager;
    imageSaveDataExecutor *m_save_polarimetric_executor;


    pclPointCloudViewerController *m_point_cloud_viewer;

    l3cam m_devices[1];
    sensor m_sensors[6];

    sensor *m_rgb_sensor;
    sensor *m_thermal_sensor;
    sensor *m_lidar_sensor;
    sensor *m_pol_sensor;
    sensor *m_allied_narrow_sensor;
    sensor *m_allied_wide_sensor;

    cv::dnn::Net m_faces_net;

    QString m_library_version;
    QString m_status_connected_style;
    QString m_status_error_style;
    QString m_status_alarm_style;
    QString m_status_searching_style;
    QString m_status_undefined_style;

    QString m_server_address;

    QString m_red_button_style;
    QString m_green_button_style;

    QString m_path_to_save_thermal;
    QString m_path_to_save_pol;
    QString m_path_to_save_pointcloud;
    QString m_path_to_save_rgb;
    QString m_path_to_save_wide;
    QString m_path_to_save_narrow;

    QTimer *m_search_timer;
    QTimer *m_rgb_value_changed;

    float m_pol_black_level;
    float m_pol_exposure;
    float m_pol_auto_exposure_min;
    float m_pol_auto_exposure_max;

    int m_rgb_contrast;
    int m_rgb_brightness;
    int m_rgb_staturation;
    int m_rgb_sharpness;
    int m_rgb_gamma;
    int m_rgb_gain;
    int m_rgb_white_balance;
    int m_rgb_exposure_time;

    int m_pol_brightness;
    int m_pol_gain;
    int m_pol_auto_gain_min;
    int m_pol_auto_gain_max;

    int m_devices_connected;
    int m_sensors_connected;

    int m_max_distance;
    int m_min_distance;
    int m_max_intensity;
    int m_min_intensity;

    uint16_t m_pcd_port;
    uint16_t m_rgb_port;
    uint16_t m_thermal_port;
    uint16_t m_rgbp_port;

    int16_t m_save_images_counter;
    int16_t m_save_images_rgb_counter;
    int16_t m_save_pointcloud_counter;
    int16_t m_save_thermal_counter;
    int16_t m_save_pol_counter;
    int16_t m_save_wide_counter;
    int16_t m_save_narrow_counter;

    uint8_t m_current_allied_camera;

    uint8_t m_thermal_status;
    uint8_t m_polarimetric_status;
    uint8_t m_lidar_status;
    uint8_t m_rgb_status;
    uint8_t m_allied_wide_status;
    uint8_t m_allied_narrow_status;

    bool m_searching_device;
    bool m_searching_status;

    bool m_rgb_auto_exp;
    bool m_rgb_auto_white;

    bool m_pol_auto_gain;
    bool m_pol_auto_exposure;

    bool m_device_started;
    bool m_device_streaming;

    bool m_save_thermal_image;
    bool m_save_pol_image;
    bool m_save_pointcloud;
    bool m_save_rgb_image;
    bool m_save_wide_image;
    bool m_save_narrow_image;

    bool m_save_data;

    bool m_save_all;

    bool m_blurring_loaded;
    bool m_apply_blurring;

    bool dev_initialized;

};

#endif // MAINWINDOW_H
