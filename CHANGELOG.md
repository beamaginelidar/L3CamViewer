# Change log for L3CamViewer

All notable changes to the L3CamViewer application will be documented in this file.

## [1.1.2] - 25/09/2023

### Added

+ Support for Windows tested
+ Reorganization of udp class that reads data streaming 
+ Libraries needed to compile in Windows
+ New libL3Cam library headers (version 0.1.15R)
+ Ui elements to show the l3cam device information (ip address, serial number and software version)
+ the not configured sensors will be shown in grey color for devices with version 2.2.6 and higher

### Changed

### Fixed

### Removed

- Unused l3cam library erros
- qDebug() messages that are writen in log window

### Known bugs

### Issues

This version is not fully compatible with l3cam devices with software version older than 2.2.6. When connected with a lower version, all the sensors that are not available in the device will appear with error status.
