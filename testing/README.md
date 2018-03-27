Graphical User Interface (GUI) Setup 
====================================
Please go through the following steps in order to run the GUI Package.
Setup
-------------
First clone the repository into the src folder of your Catkin Workspace
```
git clone https://github.com/raggyftw/P4.git
```
Then cd into the root of your workspace and source your workspace
```
devel/setup.bash
```
To build the GUI package run the command:
```
catkin_make --pkg testing
```
Once it compiles the GUI can be run using the command 
```
rosrun testing testing
```
