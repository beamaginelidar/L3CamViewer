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
// Title:		libL3Cam_thermal
// Purpose:		Interface of the library for communications with L3CAM devices thermal sensor
//
// Created on:	05/07/2023 at 12:00:00 by Beamagine.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __libL3Cam_thermal_H__
#define __libL3Cam_thermal_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#include "beamagine.h"
#include "beamErrors.h"

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

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __libL3Cam_thermal_H__ */
