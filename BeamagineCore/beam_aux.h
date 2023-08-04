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

#ifndef BEAM_AUX_H
#define BEAM_AUX_H

#include "inttypes.h"
#include <vector>
#include <QString>

#include <beamagine.h>

struct boxImage{
    int16_t x;
    int16_t y;
    int16_t height;
    int16_t width;
};

struct detectionImage{
    boxImage box;
    uint16_t confidence;
    uint16_t label;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t sensor_ori;
};

struct deviceStatus{
    static const uint8_t no_error = 0;
    static const uint8_t undefined = 1;
    static const uint8_t alarm = 2;
    static const uint8_t error_s = 3;
};

struct visualizationTypes{
    static const int RAINBOW = 0;
    static const int RAINBOW_Z = 1;
    static const int INTENSITY = 2;
    static const int FUSION_RGB = 3;
    static const int FUSION_POL = 4;
    static const int FUSION_THER = 6;
    static const int DEPTH_TOT = 10;
    static const int DEPTH_Z = 11;
    static const int FUSION_ALLIED_NARROW = 12;
    static const int FUSION_ALLIED_WIDE = 13;
};

typedef struct tPointCloudUdp{
    int32_t size;
    int32_t *data_buffer;
}tPointCloudUdp;

typedef struct tPointPcd {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t intensity;
    int32_t RGB;
}tPointPcd;

typedef struct indexTdc{
    static const uint8_t right = 1;
    static const uint8_t left = 2;
    static const uint8_t both = 3;
}indexTdc;

#endif // BEAM_AUX_H

