#ifndef PYTHONAPICODES_H
#define PYTHONAPICODES_H

static const int CHANGE_DATA_COLLECTION_PATH = 4020;
static const int ENABLE_SENSOR_DATA_COLLECTION = 4021;
static const int DISABLE_SENSOR_DATA_COLLECTION = 4022;
static const int START_RECORDING_REQUEST = 4023;
static const int STOP_RECORDING_REQUEST = 4024;

static const int INITIALIZE = 5000;
static const int FIND_DEVICES = 5001;
static const int GET_STATUS = 5002;
static const int GET_SENSORS = 5003;
static const int START_DEVICE = 5004;
static const int STOP_DEVICE = 5005;
static const int START_STREAM = 5006;
static const int STOP_STREAM = 5007;
static const int POWER_OFF = 5008;
static const int FAST_INIT = 5009;
static const int TERMINATE = 5010;

#endif // PYTHONAPICODES_H
