# Change log for L3CamViewer

All notable changes to the L3CamViewer application will be documented in this file.

## [30/05/2024] 2.0.0

### Added 

- Support for wide econ and wide thermal sensors
- Support for YUV econ wide image
- Support for new library version 0.1.18R
- New checkbox to enable/disable autobias
- Sliders to manually modify the bias parameter
- Input to modify the autobias parameter
- Python script to read raw thermal images, visualize and save them.
- binary file with thermal raw data, as saved in the data collection tab.

### Changed

- ComboBox to select the point cloud color types
- The new thermal library options are enabled/Disabled by L3Cam software version
- The python script for reading binary pointclouds now shows the RGB stored in the file

### Fixed

- The files to save in the data collection that are in the queue are not being saved
- If the INITIALIZE function for the library is called more than once the application crashes

### Removed

- Radio buttons for point cloud color

### Known bugs

### Issues

## [08/03/2024] 1.1.3

### Added

- Support for new library version 0.1.17R
- Functions to enable the udp temperatures over UDP
- Code to read YUV_422 images from allied cameras
- Support for old thermal camera commands when device SW version is older than 2.2.9
- Support for new thermal pipeline mode and new thermal colormaps
- Support for visualizing and saving raw thermal data
- Device disconnected error notificacion

### Changed

- 2D Sensors tab now is named ECON + THER + POL

### Fixed

### Removed

### Known bugs

### Issues

This version is not fully compatible with l3cam devices with software version older than 2.2.6. When connected with a lower version, all the sensors that are not available in the device will appear with error status.

## [25/09/2023] 1.1.2
### Added

- Support for Windows tested
- Reorganization of udp class that reads data streaming 
- Libraries needed to compile in Windows
- New libL3Cam library headers (version 0.1.15R)
- Ui elements to show the l3cam device information (ip address, serial number and software version)
- the not configured sensors will be shown in grey color for devices with version 2.2.6 and higher

### Changed

### Fixed

### Removed

- Unused l3cam library erros
- qDebug() messages that are writen in log window

### Known bugs

### Issues

This version is not fully compatible with l3cam devices with software version older than 2.2.6. When connected with a lower version, all the sensors that are not available in the device will appear with error status.
