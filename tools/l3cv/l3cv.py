"""
Copyright (c) 2024, Beamagine

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
"""

import socket
import time

class l3cvSensors():
    LIDAR = 1
    ECON_RGB = 2
    POLARIMETRIC = 3
    THERMAL = 4
    ALLIED_NARROW = 5
    ALLIED_WIDE = 6
    ECON_WIDE = 7
    THERMAL_BIN = 99
    
class l3cvCodes():

    CHANGE_DATA_COLLECTION_PATH = 4020

    ENABLE_SENSOR_DATA_COLLECTION = 4021
    DISABLE_SENSOR_DATA_COLLECTION = 4022

    START_DATA_COLLECTION = 4023
    STOP_DATA_COLLECTION = 4024

    INITIALIZE = 5000
    FIND_DEVICES = 5001
    GET_STATUS = 5002
    GET_SENSORS = 5003
    START_DEVICE = 5004
    STOP_DEVICE = 5005
    START_STREAM = 5006
    STOP_STREAM = 5007
    POWER_OFF = 5008
    FAST_INIT = 5009
    TERMINATE = 5010


class l3cv():
    def __init__(self):
        self._viewer_address = "127.0.0.1"
        
    def setViewerAddress(self, viewer_address):
        self._viewer_address = viewer_address

    def connect(self):
        try:
        #connect to L3CamViewer
            self._tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self._tcp_socket.connect((self._viewer_address, 54548))
        except:
            print("Error connecting with L3CamViewer")

    def close(self):
        self._tcp_socket.close()

    def enableSensorToSaveData(self, sensor):
        #assert sensor >= l3cvSensors.LIDAR and sensor <= l3cvSensors.ECON_WIDE, f"Error in sensor code, got: {sensor}"
        message = str(l3cvCodes.ENABLE_SENSOR_DATA_COLLECTION) + "_" + str(sensor)
        self.__sendMessage(message)

    def disableSensorDataCollection(self, sensor):
        #assert sensor >= l3cvSensors.LIDAR and sensor <= l3cvSensors.ECON_WIDE, f"Error in sensor code, got: {sensor}"
        message = str(l3cvCodes.DISABLE_SENSOR_DATA_COLLECTION) + "_" + str(sensor)
        self.__sendMessage(message)
    
    def setSensorPathDataCollection(self, sensor, path):
        #assert sensor >= l3cvSensors.LIDAR and sensor <= l3cvSensors.ECON_WIDE, f"Error in sensor code, got: {sensor}"
        message = str(l3cvCodes.CHANGE_DATA_COLLECTION_PATH) + "_" + str(sensor) + "_"  + path
        self.__sendMessage(message)

    def startDataCollection(self, frames_to_save):
        #assert frames_to_save < -1, f"Error in frames to save counter, got: {frames_to_save}"
        message = str(l3cvCodes.START_DATA_COLLECTION) + "_" + str(frames_to_save)
        self.__sendMessage(message)
  
    def stopDataCollection(self):
        message = str(l3cvCodes.STOP_DATA_COLLECTION)
        self.__sendMessage(message)

    def fastInit(self):
        message = str(l3cvCodes.FAST_INIT)
        self.__sendMessage(message)
        return self.__readResponse()
    
    def startDevice(self):
        message = str(l3cvCodes.START_DEVICE)
        self.__sendMessage(message)
    
    def stopDevice(self):
        message = str(l3cvCodes.STOP_DEVICE)
        self.__sendMessage(message)

    def startStream(self):
        message = str(l3cvCodes.START_STREAM)
        self.__sendMessage(message)
    
    def stopStream(self):
        message = str(l3cvCodes.STOP_STREAM)
        self.__sendMessage(message)

    def __sendMessage(self, message):
        try:
            self._tcp_socket.sendall(bytes(message, 'utf-8'))
            time.sleep(0.3) #added time to avoid mixing tcp messages
        except:
            print("Error sending request")

    def __readResponse(self):
        buffer_size = 512
        response = ""
        while(response == ""):
            response = self._tcp_socket.recv(buffer_size).decode("utf-8")
            time.sleep(0.5)
            print(response)
        return int(response)
    
    def __processResponse(self):
        print("processing response")

