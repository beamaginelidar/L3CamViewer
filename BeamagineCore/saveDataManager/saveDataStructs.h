#pragma once
#include <inttypes.h>
#include <time.h>

typedef enum saveDataTypes{
     images = 0,
     pointcloud
}saveDataTypes;

typedef struct imageData{
    uint32_t timestamp;
    uint16_t image_height;
    uint16_t image_width;
    uint8_t image_channels;
    uint8_t *image_buffer;
}imageData;


typedef struct pointcloudData{
    uint32_t timestamp;
    uint16_t pointcloud_size;
    int32_t *pointcloud_buffer;
}pointcloudData;

