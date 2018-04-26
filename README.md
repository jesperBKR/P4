4th Semester Project In Robotics AAU
====================================

This repository holds the RUPEE software  developed for Group 461's 4th semester project in robotics at AAU.

Prerequisites
-------------
To be able to use this package the following prerequisites are required before use:
* [Ubuntu 16.04.4 LTS](https://www.ubuntu.com/download/desktop "Ubuntu")
* [ROS Kinetic Kame](http://wiki.ros.org/kinetic "ROS Kinetic Kame")
* [Librealsense legacy release](https://github.com/IntelRealSense/librealsense/blob/v1.12.1/doc/installation.md "librealsense")
* [Some realsense SDK](https://github.com/IntelRealSense/librealsense/blob/v1.12.1/doc/installation.md "RealSense SDK")
* [OpenCV 2.4 or newer](https://opencv.org/releases.html "OpenCV")
* [Qt 4.0 or newer](https://www.qt.io/downlo "Qt")

The software package is validated using these prerequisites and cannot be guaranteed to work if using different versions. 

Installation
-------------


Basic Information
------------
The project consist of a JACO arm and an IntelRealsense camera.

How to build
------------
First clone the repository:
```
git clone https://github.com/raggyftw/P4.git
```

To build the GUI:
```
catkin_make --pkg testing
```
TODO: Add more commands and names?
