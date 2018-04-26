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
Some more indepth stuff here, mby scripts that install it all 

Basic Information
------------
The project consist of a JACO arm and an IntelRealsense camera.

How to build
------------
To setup the package the following steps must be followed.

First create a new catkin workspace:
```
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/
catkin_make
```
The workspace should now be setup correctly containing a build, devel and src folder. 
Now move into the src folder and clone the repository containing the RUPEE software:
```
cd ~/catkin_ws/src
git clone https://github.com/raggyftw/P4.git
```
To build the package do:
```
cd ~/catkin_ws/
catkin_make
```
The executables should now have been built. 

Running RUPEE 
------------
To run the RUPEE software start up RUPEE App:
```
roslaunch rupee_app rupee_app
```
