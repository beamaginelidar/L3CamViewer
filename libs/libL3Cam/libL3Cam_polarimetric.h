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
// Title:		libL3Cam_polarimetric
// Purpose:		Interface of the library for communications with L3CAM devices polarimetric sensor
//
// Created on:	05/07/2023 at 12:00:00 by Beamagine.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __libL3Cam_polarimetric_H__
#define __libL3Cam_polarimetric_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#include "beamagine.h"
#include "beamErrors.h"

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

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __libL3Cam_polarimetric_H__ */
