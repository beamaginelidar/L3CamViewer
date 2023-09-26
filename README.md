# L3CamViewer

This package is a Qt5 application to visualize the L3Cam device sensors manufactured by [Beamagine](https://beamagine.com/) and test the libL3Cam library, provided by Beamagine as part of the [L3Cam SDK](https://github.com/beamaginelidar/libl3cam.git). For more info on the L3Cam check the [L3Cam User Manual](https://github.com/beamaginelidar/libl3cam/blob/main/L3CAM%20User%20Manual.pdf).

# Installation

## Dependencies

### libL3Cam

First, you can download and install the L3Cam SDK. It's not necessary since it is already in `/libs`.

To do so, download the required package from Beamagine's [L3Cam SDK](https://github.com/beamaginelidar/libl3cam.git) repository depending on your hardware architecture and install:

```
sudo dpkg -i <PACKAGE>
```

### OpenCV & PCL

### OpenCV

The OpenCV version used in Linux Ubuntu 20.04 is 4.5.5

The OpenCV version used in Windows 10 is 4.4.0

### PCL

The PCL version used in Linux Ubuntu 20.04 is 1.9.0

The PCL version used in Windows 10 is 1.9.1

You might have to change some parts of the `L3CamViewer.pro` file and put the routes to where the PCL library is located on your PC instead of `$(HOME)/(...)`.

You might also have to change the `LD_LIBRARY_PATH` environment variable in Qt Creator. To do so:

1. Open the project in Qt Creator.

2. Go to Projects &rarr; Build & Run &rarr; `<YOUR_KIT>` &rarr; Run.

3. Scroll to Environment and find the `LD_LIBRARY_PATH` variable.

4. Modify it so it points to: `<YOUR_WORKSPACE>/L3CamViewer/libs/opencv4` and `<YOUR_WORKSPACE>/L3CamViewer/libs/pcl-1.9.0`

### Other dependencies

Install needed dependencies executing the script **install.sh** or manually execute in a terminal:

```
sudo apt update
sudo apt install build-essential -y
sudo apt install gcc g++ ffmpeg libavcodec-dev libavformat-dev libswscale-dev libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev libgtk-3-dev libpng-dev libjpeg-dev libopenexr-dev libtiff-dev libwebp-dev -y
sudo apt install libvtk7-dev -y
sudo apt install libboost1.71-all-dev -y
sudo apt install libtinyxml2.6.2v5
sudo apt install libqt5core5a libqt5gui5 libqt5network5 libqt5widgets5 libqt5printsupport5 -y 
```

## L3Cam Viewer

Clone this repository in your workspace (e.g. workspace):

```
cd ~/workspace && git clone https://github.com/beamaginelidar/libL3CamTest
```

## Operational Advice

### MTU Size

You will need to increase the MTU (Maximum Transmission Unit) on the network interface attached to the camera.

You can check what your current MTU setting is by running the following command:

```
ip a | grep mtu
```

You should increase the MTU to `9000` to allow jumbo frames. If you use Network Manager, this can be done by opening the network interface settings and editing the "MTU" box under the "Identity" tab.

See the "Linux host configuration" section of the [L3Cam User Manual](https://github.com/beamaginelidar/libl3cam/blob/main/L3CAM%20User%20Manual.pdf) for full details.

### Receive Buffer Size

It is also recommended to increase your network default and maximum receive buffer size.

You can check what your current buffer size is:

```
sudo sysctl 'net.core.rmem_max'
sudo sysctl 'net.core.rmem_default'
```

Update the buffer size with the following commands:

```
sudo sh -c "echo 'net.core.rmem_default=268435456' >> /etc/sysctl.conf"
sudo sh -c "echo 'net.core.rmem_max=268435456' >> /etc/sysctl.conf"
sudo sysctl -p
```

### Port usage

`libL3Cam` uses the following ports for streaming and communications:

| PROTOCOL | PORT                             | CONFIGURATION  |
| -------- | -------------------------------- | -------------- |
| TCP      | 6000 (L3CAM)                     | Fixed          |
| UDP      | 6050 (LiDAR)                     | Fixed          |
| UDP      | 6060 (Allied Wide, Polarimetric) | Fixed          |
| UDP      | 6020 (Allied Narrow, RGB)        | Fixed          |
| UDP      | 6030 (LWIR)                      | Fixed          |
| RTSP     | 5040 (LiDAR)                     | Reconfigurable |
| RTSP     | 5030 (Allied Wide, Polarimetric) | Reconfigurable |
| RTSP     | 5010 (Allied Narrow, RGB)        | Reconfigurable |
| RTSP     | 5020 (LWIR)                      | Reconfigurable |

TCP is used internally by `libL3Cam` and is transparent to the user. However, the system host must have the required port available.

# User guide

Start the application and make sure the L3Cam is connected.

## Initialization

To initialize the L3Cam, click the `FAST INIT` button or follow these steps:

1. Click `INITIALIZE`: This button will initialize the library and the internal communication system.

2. Click `FIND DEVICES`: This button will search for L3CAM devices in the network.

3. Click `GET STATUS`: This button gets the device internal status.

4. Click `GET SENSORS`: This button gets the available sensors in the L3CAM device, which will be shown in the System tab.

5. Once the L3Cam is initialized start the device by clicking `START DEVICE`.

6. When the `START STREAMING` button is available, click it to start streaming and the available sensors streamings will be shown on the right.

The response of every button call is shown in the logger screen below the app.

## Power off L3Cam

To power off the device safely you should follow these steps:

1. (Optional) Click `STOP STREAM` and `STOP DEVICE`.

2. Click `Power OFF`: this will shut down the L3Cam properly.

3. Click `TERMINATE`: this will close the library communications and the app safely, properly releasing resources.

You can also `TERMINATE` the app without `Power OFF`. 

Both `Power OFF` and `TERMINATE` will stop stream and device, that's why it's optional.

## Tabs

### System
In this tab the user is able to:
+ Change the network configuration of L3CAM device, such as the fixed ip address, or enable/disable dhcp.
+ Get temperatures information of the device and sensors. Using the `GET Temperatures` button, the different temperature sensors information will be shown.
+ The available sensors, and the status of each sensor will be shown when connected to a device. 
+ Device information such as the IP address, the serial number and the software version.
+ Change the RTSP streaming pipelines for each sensor.

### 2D Sensors
This tab includes the configuration parameters for the Econ RGB camera, the Polarimetric camera (mono or RGB-P) and the thermal camera.

+ Change the available parameters of the econ RGB Camera
+ Change the available parameters of the allied polarimetric mono or RGB-P camera.
+ Change the available parameters of the thermal camera.
+ Change the cameras streaming protocol (UDP, RTSP over gstreamer).
+ Get the current camera streaming pipeline.

### Allied Cameras
This tab is dedicated to the allied camera parameters. If the wide camera and the narrow camera are connected, using the round button selector `Wide Camera` and `Narrow Camera` will determinate the camera which is being modified.
+ Change the available allied camera parameters
+ Change streaming protocol (UDP, RTSP over gstreamer)
+ Get current camera streaming pipeline.
### LiDAR Settings
This tab includes the parameters related to the LiDAR and the 3D visualization settings.\
For the visualization settings:
+ Change the visualization point cloud color.
+ Change the background color of the 3D window.
+ Change color range.
+ Show/hide the 3D axis.

For the LiDAR settings:
+ Remote points filter, allows to select a minumim and maximum distance for the pointcloud.
+ Bias, behaves as the gain of a camera, high BIAS value means more detecetivity but also more noise, low BIAS means less detectivity and less noise.
+ Change the streaming protocol (UDP, RTSP over gstreamer). The RTSP is only by custom request, if you need it please contact us.
### Data Collection
This tab is to save the images and the pointclouds. The pointclouds are saved in a propieraty binary format and the images are saved in png format.\
The name of the files work as a timestamp with the format **hhmmsszzz.ext**.
For example an image captured at 14:25:42:501 will have the name 142542501.png

**Use different folders for each sensor, especially if they are 2D sensors**

Use the checkbox to enable the saving of the specific sensor.\
Use the `...` button to select a folder for the selected sensor.\
The `Frames to save` parameter, the user can select a number of frames to save or use **-1** to save all the frames.\
Click on the green button to start the data collection, it will turn red until clicked again or until the number of frames has been reached.
If needed, the option to blur the faces of people detected can be done by enabling the `Blur Faces` check box.
### 

Feel free to test all the Sensors and AlliedCameras parameters, but note that some parameters can only be changed when the L3Cam is not streaming and some when it is streaming.
