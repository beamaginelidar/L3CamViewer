/*  Copyright (c) 2023, Beamagine

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
#include "beamagine.h"
#include "beamErrors.h"

//! @brief  Function to register the callback where error notifications will be received
//! @param  callback The user callback function
//! @return none
void registerErrorCallback(l3camErrorCallback callback);

//! @brief  Returns the error description for the error specified
//! @param  error_code Integer with the error received
//! @return pointer with the error description
const char* getBeamErrorDescription(int error_code);

//! @brief  Returns the library version
//! @param  none
//! @return pointer with the library version
const char* GET_VERSION();

//! @brief  Initializes the library and internal communication system
//! @param  ip_address Use this parameter to specify the local address where the device is to be found,
//!         set this parameter to NULL to find devices in all the physical ports
//! @param  device_ip_address Use this parameter to specify the L3CAM device ip address and connect directly
//!         with this device, set tis parameter to NULL when discovery is desired
//! @return 0 if OK, otherwise Error, check error definitions
int INITIALIZE(char *local_ip_address, char *device_ip_address);

//! @brief  Closes the library communications, call this function to properly relase resources
//! @param  none
//! @return 0 if OK, otherwise Error.
int TERMINATE(l3cam device);

//! @brief  Search for L3CAM devices in the network, this function is blocking call
//! @param  devices[] Array where L3CAM device information is stored
//! @param  num_devices Pointer where the number of devices found is returned
//! @return 0 if OK, otherwise Error, check error definition
int FIND_DEVICES(l3cam devices[], int *num_devices);

//! @brief  Returns the local IP address of the NIC where the L3CAM is connected
//! @param  device The device to execute the function
//! @return Pointer with the IP address
const char *GET_LOCAL_SERVER_ADDRESS(l3cam device);

//! @brief  Gets the device internal status
//! @param  device The device to execute the function
//! @param  status Pointer where the status is saved, check satus definition
//! @return 0 if OK, otherwise Error, check error definition
int GET_DEVICE_STATUS(l3cam device, int32_t *system_status);

//! @brief  Gets the available sensors in the L3CAM device
//! @param  device The device to execute the function
//! @param  sensors Array where the sensors information is stored
//! @param  num_sensors Pointer where the number of sensors is returned
//! @return 0 if OK, otherwise Error, check error definition
int GET_SENSORS_AVAILABLE(l3cam device, sensor sensors[], int *num_sensors);

//! @brief  Changes the streaming protocol of a desired sensor
//! @param  device The device to execute the function
//! @param  sensor Pointer with the sensor to change the protocol
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_STREAMING_PROTOCOL(l3cam device, sensor *sensor_id);

//! @brief  Returns the RTSP pipeline for a specific sensor
//! @param  device The device to execute the function
//! @param  sensor The sensor to retrieve the RTSP pipeline
//! @param  pipeline Pointer to store the pipeline
//! @return 0 if OK, otherwise Error, check error definition
int GET_RTSP_PIPELINE(l3cam device, sensor sensor_id, char **pipeline); 

//! @brief  Changes the RTSP pipeline for the specified sensor, the pipeline is not stored 
//! @param  device The device to execute the function
//! @param  sensor Pointer with the sensor to change the RTSP pipeline
//! @param  pipeline Pointer with the new pipeline
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RTSP_PIPELINE(l3cam device, sensor sensor_id, char *pipeline);

//! @brief  Returns the network configuration of the device
//! @param  device The device to execute the function
//! @param  ip_address pointer to store the device ip address
//! @param  netmask pointer to store the device ip address netmask
//! @param  gateway pointer to store the device gateway address
//! @return 0 if OK, otherwise Error, check error definition
int GET_NETWORK_CONFIGURATION(l3cam device, char **ip_address, char **netmask, char **gateway);

//! @brief  Changes the network configuration of the device, this change is permanent
//! @param  device The device to execute the function
//! @param  ip_address pointer with the new device ip address
//! @param  netmask pointer with the new device ip address netmask
//! @param  gateway pointer with the device gateway address
//! @param  enable_dhcp Boolean that enables or disabes the DHCP configuration, if false proper network configuration is required
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_NETWORK_CONFIGURATION(l3cam device, char *ip_address, char *netmask, char *gateway, bool enable_dhcp);

//! @brief  Power offs the device, use this function to properly shut down the device
//! @param  device The device to execute the function
//! @return none
void POWER_OFF_DEVICE(l3cam device);

//! @brief  Starts the device, initializes internal sensors
//! @param  device The device to execute the function
//! @return 0 if OK, otherwise Error
int START_DEVICE(l3cam device);

//! @brief  Stops the device, stops internal sensors and threads
//! @param  device The device to execute the function
//! @return 0 if OK, otherwise Error, check error definition
int STOP_DEVICE(l3cam device);

//! @brief  Starts the streaming threads and functionality in the L3CAM device
//! @param  device The device to execute the function
//! @return 0 if OK, otherwise Error, check error definition
int START_STREAM(l3cam device);

//! @brief  Stops the streaming threads and functionality in the L3CAM device
//! @param  device The device to execute the function
//! @return 0 if OK, otherwise Error, check error definition
int STOP_STREAM(l3cam device);

//! @brief  Changes the pointcloud color representation
//! @param  device The device to execute the function
//! @param  visualization_color the desired color visualization, check color types definition
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_POINT_CLOUD_COLOR(l3cam device, int visualization_color);

//! @brief  Changes the color ranges for the pointcloud representation
//! @param  device The device to execute the function
//! @param  max_value Max distance/intensity value for color representation
//! @param  min_value Min distance/intensity value for color representation
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_POINT_CLOUD_COLOR_RANGE(l3cam device, int min_value, int max_value);

//! @brief  Changes the range of LiDAR data
//! @param  device The device to execute the function
//! @param  min_distance The minimum distance in mm of the LiDAR data
//! @param  max_distance The maximum distance in mm of the LiDAR data
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_DISTANCE_RANGE(l3cam device, int min_distance, int max_distance);

//! @brief  Get device temperatures in degrees Celsius (C)
//! @param  device The device to execute the function
//! @param  device_temperatures Pointer to store the device temperatures
//! @return 0 if OK, otherwise Error, check error definition
int GET_DEVICE_TEMPERATURES(l3cam device, int32_t *device_temperatures);

//! @brief  Modifies the BIAS parameters to enable short range mode
//! @param  device The device to execute the function
//! @param  enabled, True to enable short range mode, false to disable it
//! @return 0 if OK, otherwise Error, check error definition
int SET_BIAS_SHORT_RANGE(l3cam device, bool enabled);

//! @brief  Allows the user to enable/disable the automatic bias algorithm of the device
//! @param  device The device to execute the function
//! @param  enabled True to enable the autobias, false to disable
//! @return none
int ENABLE_AUTO_BIAS(l3cam device, bool enabled);

//! @brief  Allows the user to manually modify the bias parameter of each module of the device when autobias is disabled
//! @param  device The device to execute the function
//! @param  index value 1 for right module, value 2 for left module.
//!         Integer that indicates if the bias is for the left or the right module.
//! @param  bias Bias value to send. The range is 600mv to 3500mv
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_BIAS_VALUE(l3cam device, uint8_t index, int32_t bias);

//! @brief  Allows the user to manually modify the autobias algorithm parameters of each module of the device
//! @param  device The device to execute the function
//! @param  index value 1 for right module, value 2 for left module.
//!         Integer that indicates if the bias is for the left or the right module.
//! @param  gain Value between 0 and 100 that indicates the amount of noise allowed in the pointcloud
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_AUTOBIAS_VALUE(l3cam device, uint8_t index, uint8_t gain);

//! @brief  Allows the user to get the autobias algorithm parameters of each module of the device
//! @param  device The device to execute the function
//! @param  index value 1 for right module, value 2 for left module.
//!         Integer that indicates if the bias is for the left or the right module.
//! @param  gain Value between 0 and 100 that indicates the amount of noise allowed in the pointcloud
//! @return 0 if OK, otherwise Error, check error definition
int GET_AUTOBIAS_VALUE(l3cam device, uint8_t index, uint8_t *gain);

//! @brief  Modify the filter that removes noise caused by high detectivity elements
//! @param  device The device to execute the function
//! @param  depth Percentage that indicates the maximum depth that the filter is effective
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_HIDET_NOISE_FILTER_PARAMETERS(l3cam device, uint8_t depth);

//! @brief  Gets the current value for the high detectivity filter
//! @param  device The device to execute the function
//! @param  depth Percentage that indicates the maximum depth that the filter is effective
//! @return 0 if OK, otherwise Error, check error definition
int GET_HIDET_NOISE_FILTER_PARAMETERS(l3cam device, uint8_t *depth);


#ifdef __cplusplus
}
#endif

#endif  /* ndef __libL3Cam_H__ */
