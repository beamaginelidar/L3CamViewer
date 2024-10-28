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

from l3cv import *
import time


"""
This is a sample script on how to use the l3cv python class to communicate 
with L3CamViewer application. In this example the script connects to the GUI,
enable sensors to save data, configures a path in the PC that the L3CamViewer 
is running, and saves data for 10 seconds
"""

#before launch this script, ensure the L3CamViewer app is running and streaming data
mviewer_api = l3cv()

#modify the IP address if the script and the L3CamViewer are running in different machines
#mviewer_api.setViewerAddress("192.168.1.124")

#connect with the L3CamViewer app
mviewer_api.connect()

error = mviewer_api.fastInit()

print("Fast init response " + str(error))

if(error != 0):
    print("Error initializing the L3Cam")
    exit(error)


time.sleep(1)
#Enable the sensors
mviewer_api.enableSensorToSaveData(l3cvSensors.LIDAR)
mviewer_api.enableSensorToSaveData(l3cvSensors.THERMAL)

time.sleep(1)

#Set paths for each sensor, this is the path in the L3CamViewer host PC, check that the folders exist
mviewer_api.setSensorPathDataCollection(l3cvSensors.LIDAR, "python_test_datacol/LiDAR/")
mviewer_api.setSensorPathDataCollection(l3cvSensors.THERMAL, "python_test_datacol/Thermal/")

time.sleep(1)
#save all the frames until stop command is sent
mviewer_api.startDataCollection(-1)

time.sleep(10)
#stop the data collection process
mviewer_api.stopDataCollection()

#close the Connection
time.sleep(1)
mviewer_api.close()