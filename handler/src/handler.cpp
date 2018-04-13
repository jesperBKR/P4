#include <ros/ros.h>
#include <geometry_msgs/pointF32.h>

using namespace ros;

int main(int argc, char ** argv[])
{
  void positionCallback(geometry_msgs::pointF32& position_msg )

  init(argc, argv, "handler_node");
  NodeHandle nh;

  Subscriber position_sub nh.subscribe("object_pos", 1, positionCallback);

  // Define our transformation to the camera
  void positionCallback(geometry_msgs::pointF32 & position_msg)
  {

  }
  return -1;
}
