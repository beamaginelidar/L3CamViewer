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
// Title:		libL3Cam_econ
// Purpose:		Interface of the library for communications with L3CAM devices econ sensor
//
// Created on:	05/07/2023 at 12:00:00 by Beamagine.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __libL3Cam_econ_H__
#define __libL3Cam_econ_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#include "beamagine.h"
#include "beamErrors.h"

//! @brief  Restarts de configuration values of the RGB camera
//! @param  device The device to execute the function
//! @return 0 if OK, otherwise Error, check error definition
int SET_RGB_CAMERA_DEFAULT_SETTINGS(l3cam device);

//! @brief  Changes the brightness of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  brightness The value of the brightness ECON -15 to 15
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_BRIGHTNESS(l3cam device, int brightness);

//! @brief  Changes the contrast of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  constrast The value of the contrast ECON 0 to 30
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_CONTRAST(l3cam device, int contrast);

//! @brief  Changes the saturation of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  saturation The value of the saturation ECON 0 to 60
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_SATURATION(l3cam device, int saturation);

//! @brief  Changes the sharpness of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  sharpness The value of the sharpness ECON 0 to 127
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_SHARPNESS(l3cam device, int sharpness);

//! @brief  Changes the gamma of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  gamma The value of the gamam ECON 40 to 500
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_GAMMA(l3cam device, int gamma);

//! @brief  Changes the gain of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  gain The value of the gain ECON 0 to 63
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_GAIN(l3cam device, int gain);

//! @brief  Changes the white balance of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  white_balance The value of the white balance ECON 1000 to 10000
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_WHITE_BALANCE(l3cam device, int white_balance);

//! @brief  Changes the exposure time of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  exposure_time The value of the exposure time ECON 1 to 10000
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_EXPOSURE_TIME(l3cam device, int exposure_time);

//! @brief  Enables/disables the automatic white balance of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  enabled boolean to enabe/disable the camera function
//! @return 0 if OK, otherwise Error, check error definition
int ENABLE_RGB_CAMERA_AUTO_WHITE_BALANCE(l3cam device, bool enabled);

//! @brief  Enables/disables the automatic exposure time of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  enabled boolean to enabe/disable the camera function
//! @return 0 if OK, otherwise Error, check error definition
int ENABLE_RGB_CAMERA_AUTO_EXPOSURE_TIME(l3cam device, bool enabled);

//! @brief  Changes the resolution of the RGB camera sensor image
//! @param  device The device to execute the function
//! @param  resolution The desired resolution see beamagine.h
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_RESOLUTION(l3cam device, econResolutions resolution);

//! @brief  Changes the framerate of the RGB camera sensor
//! @param  device The device to execute the function
//! @param  framerate Integer that indicates the framerate 1fps - 16fps
//! @return 0 if OK, otherwise Error, check error definition
int CHANGE_RGB_CAMERA_FRAMERATE(l3cam device, int framerate);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __libL3Cam_econ_H__ */
