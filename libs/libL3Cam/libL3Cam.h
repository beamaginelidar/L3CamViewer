/*  Copyright (c) 2023, Beamagine

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The Software may only be used in combination with Beamagine products.

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

//==============================================================================
//
// Title:		libL3Cam
// Purpose:		Interface of the library for communications with L3CAM devices
//
// Created on:	22/11/2021 at 12:01:11 by Beamagine.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __libL3Cam_H__
#define __libL3Cam_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#ifdef _WIN32
#include "cvidef.h"
#endif
#include "beamagine.h"
#include "beamErrors.h"

//! @brief  Returns the error description for the error specified
//! @param  error_code Integer with the error received
//! @return pointer with the error description
const char* getBeamErrorDescription(int error_code);

//! @brief  Returns the library version
//! @param  none
//! @return pointer with the library version
const char* GET_VERSION();

//! @brief  Initializes the library and internal communication system
//! @param  none
//! @return 0 if OK otherwise Error, check error definitions
int INITIALIZE();

//! @brief  Closes the library communications, call this function to properly relase resources
//! @param  none
//! @return 0 if OK otherwise Error.
int TERMINATE(l3cam device);

//! @brief  Search for L3CAM devices in the network, this function is blocking call
//! @param  devices[] Array where L3CAM device information is stored
//! @param  num_devices Pointer where the number of devices found is returned
//! @return 0 if OK otherwise Error, check error definition
int FIND_DEVICES(l3cam devices[], int *num_devices);

//! @brief  Returns the local IP address of the NIC where the L3CAM is connected
//! @param  device The device to execute the function
//! @return Pointer with the IP address
const char *GET_LOCAL_SERVER_ADDRESS(l3cam device);

//! @brief  Gets the device internal status
//! @param  device The device to execute the function
//! @param  status Pointer where the satatus is saved, check satus definition
//! @return 0 if OK otherwise Error, check error definition
int GET_DEVICE_STATUS(l3cam device, int32_t *system_status);

//! @brief  Gets the available sensors in the L3CAM device
//! @param  device The device to execute the function
//! @param  sensors Array where the sensors information is stored
//! @param  num_sensors Pointer where the number of sensors is returned
//! @return 0 if OK otherwise Error, check error definition
int GET_SENSORS_AVAILABLE(l3cam device, sensor sensors[], int *num_sensors);

//! @brief  Changes the streaming protocol of a desired sensor
//! @param  device The device to execute the function
//! @param  sensor Pointer with the sensor to change the protocol
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_STREAMING_PROTOCOL(l3cam device, sensor *sensor_id);

//! @brief  Returns the RTSP pipeline for a specific sensor
//! @param  device The device to execute the function
//! @param  sensor The sensor to retrieve the RTSP pipeline
//! @param  pipeline Pointer to store the pipeline
//! @return 0 if OK otherwise Error, check error definition
int GET_RTSP_PIPELINE(l3cam device, sensor sensor_id, char **pipeline); 

//! @brief  Changes the RTSP pipeline for the specified sensor, the pipeline is not stored 
//! @param  device The device to execute the function
//! @param  sensor Pointer with the sensor to change the RTSP pipeline
//! @param  pipeline Pointer with the new pipeline
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RTSP_PIPELINE(l3cam device, sensor sensor_id, char *pipeline);

//! @brief  Returns the network configuration of the device
//! @param  device The device to execute the function
//! @param  ip_address pointer to store the device ip address
//! @param  netmask pointer to store the device ip address netmask
//! @param  gateway pointer to store the device gateway address
//! @return 0 if OK otherwise Error, check error definition
int GET_NETWORK_CONFIGURATION(l3cam device, char **ip_address, char **netmask, char **gateway);

//! @brief  Changes the network configuration of the device, this change is permanent
//! @param  device The device to execute the function
//! @param  ip_address pointer with the new device ip address
//! @param  netmask pointer with the new device ip address netmask
//! @param  gateway pointer with the device gateway address
//! @param  enable_dhcp Boolean that enables or disabes the DHCP configuration, if false proper network configuration is required
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_NETWORK_CONFIGURATION(l3cam device, char *ip_address, char *netmask, char *gateway, bool enable_dhcp);

//! @brief  Power offs the device, use this function to properly shut down the device
//! @param  device The device to execute the function
//! @return none
void POWER_OFF_DEVICE(l3cam device);

//! @brief  Starts the device, initializes internal sensors
//! @param  device The device to execute the function
//! @return 0 if OK otherwise Error
int START_DEVICE(l3cam device);

//! @brief  Stops the device, stops internal sensors and threads
//! @param  device The device to execute the function
//! @return 0 if OK otherwise Error, check error definition
int STOP_DEVICE(l3cam device);

//! @brief  Starts the streaming threads and functionality in the L3CAM device
//! @param  device The device to execute the function
//! @return 0 if OK otherwise Error, check error definition
int START_STREAM(l3cam device);

//! @brief  Stops the streaming threads and functionality in the L3CAM device
//! @param  device The device to execute the function
//! @return 0 if OK otherwise Error, check error definition
int STOP_STREAM(l3cam device);

//! @brief  Changes the pointcloud color representation
//! @param  device The device to execute the function
//! @param  visualization_color the desired color visualization, check color types definition
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_POINT_CLOUD_COLOR(l3cam device, int visualization_color);

//! @brief  Changes the color ranges for the pointcloud representation
//! @param  device The device to execute the function
//! @param  max_value Max distance/intensity value for color representation
//! @param  min_value Min distance/intensity value for color representation
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_POINT_CLOUD_COLOR_RANGE(l3cam device, int min_value, int max_value);

//! @brief  Changes the range of LiDAR data
//! @param  device The device to execute the function
//! @param  min_distance The minimum distance of the LiDAR data
//! @param  max_distance The maximum distance of the LiDAR data
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_DISTANCE_RANGE(l3cam device, int min_distance, int max_distance);

//! @brief  Get device temperatures in degrees Celsius (C)
//! @param  device The device to execute the function
//! @param  device_temperatures Pointer to store the device temperatures
//! @return 0 if OK otherwise Error, check error definition
int GET_DEVICE_TEMPERATURES(l3cam device, int32_t *device_temperatures);

//! @brief  Modifies the BIAS parameters to enable long range detectivity of device
//! @param  device The device to execute the function
//! @return 0 if OK otherwise Error, check error definition
int SET_BIAS_LONG_RANGE(l3cam device);

//! @brief  Modifies the BIAS parameters to enable short range detectivity of device
//! @param  device The device to execute the function
//! @return 0 if OK otherwise Error, check error definition
int SET_BIAS_SHORT_RANGE(l3cam device);

//! @brief  Restarts de configuration values of the polarimetric cameras
//! @param  device The device to execute the function
//! @return 0 if OK otherwise Error, check error definition
int SET_POLARIMETRIC_CAMERA_DEFAULT_SETTINGS(l3cam device);

//! @brief  Changes the brightness of the polarimetric camera
//! @param  device The device to execute the function
//! @param  brightness Value to set in the camera 0 to 255
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_POLARIMETRIC_CAMERA_BRIGHTNESS(l3cam device, int brightness);

//! @brief  Changes the black level of the polarimetric camera
//! @param  device The device to execute the function
//! @param  black_level Value to set in the camera to 0 to 12.5
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_POLARIMETRIC_CAMERA_BLACK_LEVEL(l3cam device, float black_level);

//! @brief  Changes the gain of the polarimetric camera
//! @param  device The device to execute the function
//! @param  gain_value Gain to set in the camera 0-48.0 db
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_POLARIMETRIC_CAMERA_GAIN(l3cam device, float gain_value);

//! @brief  Enables/disables the automatic gain of the polarimetric camera
//! @param  device The device to execute the function
//! @param  enabled boolean to enable/disable the function
//! @return 0 if OK otherwise Error, check error definition
int ENABLE_POLARIMETRIC_CAMERA_AUTO_GAIN(l3cam device, bool enabled);

//! @brief  Changes the gain range of the auto gain parameter of the polarimetric camera
//! @param  device The device to execute the function
//! @param  min_gain Value to set as the minimum gain value 0 to 48.0
//! @param  max_gain Value to set as the maximum gain value 0 to 48.0
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_POLARIMETRIC_CAMERA_AUTO_GAIN_RANGE(l3cam device, float min_gain, float max_gain);

//! @brief  Changes the exposure time of the polarimetric camera
//! @param  device The device to execute the function
//! @param  exposure_time Value in usecs to set the exposure time 33.456us to 1000000.0us
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_POLARIMETRIC_CAMERA_EXPOSURE_TIME(l3cam device, float exposure_time);

//! @brief  Enables/disables the automatic exposure time of the polarimetric camera
//! @param  device The device to execute the function
//! @param  enabled boolean to enable/disable the function
//! @return 0 if OK otherwise Error, check error definition
int ENABLE_POLARIMETRIC_CAMERA_AUTO_EXPOSURE_TIME(l3cam device, bool enabled);

//! @brief  Changes the exposure time range of the auto exposure time parameter of the polarimetric camera
//! @param  device The device to execute the function
//! @param  min_exposure Value to set as the minimum exposure time value 33.456us to 1000000.0us
//! @param  max_exposure Value to set as the maximum exposure time value 33.456us to 1000000.0us
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_POLARIMETRIC_CAMERA_AUTO_EXPOSURE_TIME_RANGE(l3cam device, float min_exposure, float max_exposure);

//! @brief  Restarts de configuration values of the RGB camera
//! @param  device The device to execute the function
//! @return 0 if OK otherwise Error, check error definition
int SET_RGB_CAMERA_DEFAULT_SETTINGS(l3cam device);

//! @brief  Changes the brightness of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  brightness The value of the brightness ECON -15 to 15
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_BRIGHTNESS(l3cam device, int brightness);

//! @brief  Changes the contrast of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  constrast The value of the contrast ECON 0 to 30
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_CONTRAST(l3cam device, int contrast);

//! @brief  Changes the saturation of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  saturation The value of the saturation ECON 0 to 60
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_SATURATION(l3cam device, int saturation);

//! @brief  Changes the sharpness of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  sharpness The value of the sharpness ECON 0 to 127
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_SHARPNESS(l3cam device, int sharpness);

//! @brief  Changes the gamma of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  gamma The value of the gamam ECON 40 to 500
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_GAMMA(l3cam device, int gamma);

//! @brief  Changes the gain of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  gain The value of the gain ECON 0 to 63
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_GAIN(l3cam device, int gain);

//! @brief  Changes the white balance of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  white_balance The value of the white balance ECON 1000 to 10000
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_WHITE_BALANCE(l3cam device, int white_balance);

//! @brief  Changes the exposure time of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  exposure_time The value of the exposure time ECON 1 to 10000
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_EXPOSURE_TIME(l3cam device, int exposure_time);

//! @brief  Enables/disables the automatic white balance of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  enabled boolean to enabe/disable the camera function
//! @return 0 if OK otherwise Error, check error definition
int ENABLE_RGB_CAMERA_AUTO_WHITE_BALANCE(l3cam device, bool enabled);

//! @brief  Enables/disables the automatic exposure time of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  enabled boolean to enabe/disable the camera function
//! @return 0 if OK otherwise Error, check error definition
int ENABLE_RGB_CAMERA_AUTO_EXPOSURE_TIME(l3cam device, bool enabled);

//! @brief  Changes the resolution of the RGB camera sensor image
//! @param  device The device to execute the function
//! @param  resolution The desired resolution see beamagine.h
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_RESOLUTION(l3cam device, econResolutions resolution);

//! @brief  Changes the framerate of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  framerate Integer that indicates the framerate 1fps - 16fps
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_RGB_CAMERA_FRAMERATE(l3cam device, int framerate);

//! @brief  Changes the colormap of the thermal image
//! @param  device The device to execute the function
//! @param  colormap The desired colormap
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_THERMAL_CAMERA_COLORMAP(l3cam device, thermalTypes colormap);

//! @brief  Enables/disables the temperature filter of the thermal image
//! @param  device The device to execute the function
//! @param  enabled Boolean to enable or disable the filter
//! @return 0 if OK otherwise Error, check error definition
int ENABLE_THERMAL_CAMERA_TEMPERATURE_FILTER(l3cam device, bool enabled);

//! @brief  Changes the thermal temperature filter values
//! @param  device The device to execute the function
//! @param  min_temperature Min value in ºC to show -40ºC to 200ºC
//! @param  max_temperature Max value in ºC to show -40ºC to 200ºC
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_THERMAL_CAMERA_TEMPERATURE_FILTER(l3cam device, float min_temperature, float max_temperature);

//! @brief  Changes black level of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the black level
//! @param  black_level The value to configure 0 - 4096
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_BLACK_LEVEL(l3cam device, sensor sensor_id, float black_level);

//! @brief  Changes the Exposure time [µs] of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the exposure time
//! @param  exposure_time The value to configure 63.03 - 1e+07
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_EXPOSURE_TIME_US(l3cam device, sensor sensor_id, float exposure_time);

//! @brief  Enables/disables the automatic exposure time of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to enable/disable the auto exposure time
//! @param  enable boolean to enabe/disable the camera function
//! @return 0 if OK otherwise Error, check error definition
int ENABLE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME(l3cam device, sensor sensor_id, bool enable);

//! @brief  Changes the exposure time range of the auto exposure time parameter of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the auto exposure time range
//! @param  min Value to set as the minimum exposure time value 63.03us to 8.99999e+06us
//! @param  max Value to set as the maximum exposure time value 87.596us to 1e+07us
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(l3cam device, sensor sensor_id, float min, float max);

//! @brief  Changes the gain [dB] of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the gain
//! @param  gain The value to configure 0 - 48
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_GAIN(l3cam device, sensor sensor_id, float gain);

//! @brief  Enables/disables the automatic gain of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to enable/disable the auto gain
//! @param  enable boolean to enabe/disable the camera function
//! @return 0 if OK otherwise Error, check error definition
int ENABLE_ALLIED_CAMERA_AUTO_GAIN(l3cam device, sensor sensor_id, bool enable);

//! @brief  Changes the gain range of the auto gain parameter of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the auto gain range
//! @param  min Value to set as the minimum exposure time value 0 - 48
//! @param  max Value to set as the maximum exposure time value 0 - 48
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_AUTO_GAIN_RANGE(l3cam device, sensor sensor_id, float min, float max);

//! @brief  Changes the gamma of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the gamma
//! @param  gamma The value to configure 0.4 - 2.4
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_GAMMA(l3cam device, sensor sensor_id, float gamma);

//! @brief  Changes the saturation of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the saturation
//! @param  saturation The value to configure 0 - 1
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_SATURATION(l3cam device, sensor sensor_id, float saturation);

//! @brief  Changes the saturation of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the sharpness
//! @param  sharpness The value to configure -12 - 12
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_SHARPNESS(l3cam device, sensor sensor_id, int sharpness);

//! @brief  Changes the hue of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the hue
//! @param  hue The value to configure -40 - 40
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_HUE(l3cam device, sensor sensor_id, float hue);

//! @brief  Changes the Precedence of Intensity Controller of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the intensity auto precedence
//! @param  mode The value to configure, check allied_auto_precedence definition
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_INTENSITY_AUTO_PRECEDENCE(l3cam device, sensor sensor_id, int mode);

//! @brief  Enables/disables the automatic white balance of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to enable/disable the auto white balance
//! @param  enable boolean to enabe/disable the camera function
//! @return 0 if OK otherwise Error, check error definition
int ENABLE_ALLIED_CAMERA_AUTO_WHITE_BALANCE(l3cam device, sensor sensor_id, bool enable);

//! @brief  Changes the Balance ratio to control of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the white balance ratio selector
//! @param  ratio_selector The value to configure, check allied_ratio_selector definition
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_BALANCE_RATIO_SELECTOR(l3cam device, sensor sensor_id, int ratio_selector);

//! @brief  Changes the ratio of the selected color component to a reference color component of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the white balance ratio
//! @param  balance_ratio The value to configure 0 - 8
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_BALANCE_RATIO(l3cam device, sensor sensor_id, float balance_ratio);

//! @brief  Changes the rate at which the auto function changes the white balance of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the white balance auto rate
//! @param  balance_white_auto_rate The value to configure 1 - 100
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_BALANCE_WHITE_AUTO_RATE(l3cam device, sensor sensor_id, float balance_white_auto_rate);

//! @brief  Changes the tolerance in variation from the ideal white balance value in which the algorithm will not react of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the white balance auto tolerance
//! @param  balance_white_auto_tolerance The value to configure 0 - 50
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_BALANCE_WHITE_AUTO_TOLERANCE(l3cam device, sensor sensor_id, float balance_white_auto_tolerance);

//! @brief  Changes the window used to measure values for auto functions of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the auto mode region
//! @param  height The height value of the window used. [Allied 508c 0 - 1028] [Allied 319c 0 - 1554]
//! @param  width The width value of the window used.   [Allied 508c 0 - 1232] [Allied 319c 0 - 2064]
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_AUTO_MODE_REGION(l3cam device, sensor sensor_id, int height, int width);

//! @brief  Changes the subregion of the image where the intensity controller operates on of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the white balance intensity controller region
//! @param  mode The value to configure, check allied_controller_region definition
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_REGION(l3cam device, sensor sensor_id, int mode);

//! @brief  Changes target intensity value for auto intensity control of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the white balance intensity controller target
//! @param  intensity_controller_target The value to configure 10 - 90
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_INTENSITY_CONTROLLER_TARGET(l3cam device, sensor sensor_id, float intensity_controller_target);

//! @brief  Changes the maximum number of driver buffers used by the acquisition engine of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  max_driver_buffers_count The value to configure 1 - 4096
//! @return 0 if OK otherwise Error, check error definition
int CHANGE_ALLIED_CAMERA_MAX_DRIVER_BUFFERS_COUNT(l3cam device, sensor sensor_id, int max_driver_buffers_count);


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __libL3Cam_H__ */
