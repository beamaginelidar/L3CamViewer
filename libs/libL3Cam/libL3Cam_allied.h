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
// Title:		libL3Cam_allied
// Purpose:		Interface of the library for communications with L3CAM devices allied sensors
//
// Created on:	05/07/2023 at 12:00:00 by Beamagine.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __libL3Cam_allied_H__
#define __libL3Cam_allied_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#include "beamagine.h"
#include "beamErrors.h"

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

//! @brief  Gets black level of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  black_level Pointer where black level is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_BLACK_LEVEL(l3cam device, sensor sensor_id, float *black_level);

//! @brief  Gets the Exposure time [µs] of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  exposure_time Pointer where exposure time is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_EXPOSURE_TIME_US(l3cam device, sensor sensor_id, float *exposure_time);

//! @brief  Gets the automatic exposure time of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  enabled Pointer with the boolean value, true if auto exposure time is enabled, false if not
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_AUTO_EXPOSURE_TIME(l3cam device, sensor sensor_id, bool *enabled);

//! @brief  Gets the exposure time range of the auto exposure time parameter of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  min Pointer where auto exposure time range minimum is saved
//! @param  max Pointer where auto exposure time range maximum is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_AUTO_EXPOSURE_TIME_RANGE(l3cam device, sensor sensor_id, float *min, float *max);

//! @brief  Gets the gain [dB] of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  gain Pointer where gain is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_GAIN(l3cam device, sensor sensor_id, float *gain);

//! @brief  Gets the automatic gain of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  enabled Pointer with the boolean value, true if auto gain is enabled, false if not
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_AUTO_GAIN(l3cam device, sensor sensor_id, bool *enabled);

//! @brief  Gets the gain range of the auto gain parameter of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  min Pointer where auto gain range minimum is saved
//! @param  max Pointer where auto gain range maximum is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_AUTO_GAIN_RANGE(l3cam device, sensor sensor_id, float *min, float *max);

//! @brief  Gets the gamma of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  gamma Pointer where gamma is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_GAMMA(l3cam device, sensor sensor_id, float *gamma);

//! @brief  Gets the saturation of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  saturation Pointer where saturation is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_SATURATION(l3cam device, sensor sensor_id, float *saturation);

//! @brief  Gets the saturation of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  sharpness Pointer where sharpness is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_SHARPNESS(l3cam device, sensor sensor_id, int *sharpness);

//! @brief  Gets the hue of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  hue Pointer where hue is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_HUE(l3cam device, sensor sensor_id, float *hue);

//! @brief  Gets the Precedence of Intensity Controller of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  mode Pointer where intensity auto precedence mode is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_INTENSITY_AUTO_PRECEDENCE(l3cam device, sensor sensor_id, int *mode);

//! @brief  Gets the automatic white balance of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  enabled Pointer with the boolean value, true if auto white balance is enabled, false if not
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_AUTO_WHITE_BALANCE(l3cam device, sensor sensor_id, bool *enabled);

//! @brief  Gets the Balance ratio selector of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  ratio_selector Pointer where balance artio selector is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_BALANCE_RATIO_SELECTOR(l3cam device, sensor sensor_id, int *ratio_selector);

//! @brief  Gets the ratio of the selected color component to a reference color component of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  ratio_selector Pointer where balance ratio is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_BALANCE_RATIO(l3cam device, sensor sensor_id, float *balance_ratio);

//! @brief  Gets the rate at which the auto function changes the white balance of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  balance_white_auto_rate Pointer where balance white auto rate is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_BALANCE_WHITE_AUTO_RATE(l3cam device, sensor sensor_id, float *balance_white_auto_rate);

//! @brief  Gets the tolerance in variation from the ideal white balance value in which the algorithm will not react of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  balance_white_auto_tolerance Pointer where balance white auto tolerance is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_BALANCE_WHITE_AUTO_TOLERANCE(l3cam device, sensor sensor_id, float *balance_white_auto_tolerance);

//! @brief  Gets the window used to measure values for auto functions of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  height Pointer where auto mode region height is saved
//! @param  width Pointer where auto mode region width is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_AUTO_MODE_REGION(l3cam device, sensor sensor_id, int *height, int *width);

//! @brief  Gets the subregion of the image where the intensity controller operates on of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  mode Pointer where intensity controller region mode is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_INTENSITY_CONTROLLER_REGION(l3cam device, sensor sensor_id, int *mode);

//! @brief  Gets target intensity value for auto intensity control of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  intensity_controller_target Pointer where intensity controller target is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_INTENSITY_CONTROLLER_TARGET(l3cam device, sensor sensor_id, float *intensity_controller_target);

//! @brief  Gets the maximum number of driver buffers used by the acquisition engine of allied camera
//! @param  device The device to execute the function
//! @param  sensor_id The allied camera to change the maximum driver buffers count
//! @param  max_driver_buffers_count Pointer where max driver buffers count is saved
//! @return 0 if OK otherwise Error, check error definition
int GET_ALLIED_CAMERA_MAX_DRIVER_BUFFERS_COUNT(l3cam device, sensor sensor_id, int *max_driver_buffers_count);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __libL3Cam_allied_H__ */
