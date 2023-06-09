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

#ifndef BEAMAGINE_H
#define BEAMAGINE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum sensorTypes {
    sensor_lidar = 1,
    sensor_econ_rgb,
    sensor_pol,
    sensor_thermal,
    sensor_allied_narrow,
    sensor_allied_wide
}sensorTypes;

enum imageTypes {
    image_mono = 1,
    image_rgb,
    //!mono polarimetric modes
    pol_co,
    pol_cross,
    pol_docp,
    pol_grad,
    //!rgb polarimetric modes
    pol_raw,
    pol_aolp,
    pol_dolp,
    pol_hsv,
};

typedef enum thermalTypes{
    //!thermal image modes
    thermal_WHITE = 1,
    thermal_BLACK = 17,
    thermal_IRON = 20,
    thermal_COOL = 2,
    thermal_AMBER = 9,
    thermal_INDIGO = 10,
    thermal_TYRIAN = 16,
    thermal_GLORY = 8,
    thermal_ENVY = 16,
    thermal_WHITE_NEW = 100,
    thermal_BLACK_NEW,
    thermal_SPECTRA,
    thermal_PRISM,
    thermal_TYRIAN_NEW,
    thermal_AMBER_NEW,
    thermal_IRON_NEW,
    thermal_HI,
    thermal_HILO,
}thermalTypes;

enum pointCloudColor{
    RAINBOW = 0,
    RAINBOW_Z,
    INTENSITY,
    RGB_FUSION,
    POLARIMETRIC_FUSION,
    POL_PROCESSED_FUSION,
    THERMAL_FUSION,
    RGBT_FUSION,
    ALLIED_NARROW_FUSION = 12,
    ALLIED_WIDE_FUSION
};

enum alliedCamerasIds{
   wide_camera = 0,
   narrow_camera = 1
};

#define allied_auto_precedence_minimize_noise 0
#define allied_auto_precedence_minimize_blur 1

#define allied_balance_ratio_selector_red 0
#define allied_balance_ratio_selector_blue 1

#define allied_controller_region_auto_mode_region 0
#define allied_controller_region_full_image 4

typedef enum streamingProtocols {
    protocol_raw_udp = 1,
    protocol_gstreamer,
    protocol_rtsp,
}streamingProtocols;

typedef enum econResolutions{
    reso_640_480 = 1,
    reso_1280_720,
    reso_1920_1080
}econResolutions;

typedef struct sensor{
    streamingProtocols protocol;
    sensorTypes sensor_type;
    uint8_t sensor_status;
    uint8_t image_type;
    bool perception_enabled;
}sensor;

typedef struct l3cam{
    char ip_address[16];
    char serial_number[10];
    uint8_t model;
}l3cam;

#endif // BEAMAGINE_H

