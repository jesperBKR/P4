#include <ros/ros.h>
#include <geometry_msgs/pointF32.h>
#include "../msg/Setup.h"

using namespace ros;

void guiCallback(const testing::Setup gui_msg){
  ROS_INFO("I heard: [%d]", gui_msg.reps);
}

int main(int argc, char ** argv)
{

  void positionCallback(geometry_msgs::pointF32& position_msg )

  init(argc, argv, "handler_node");
  NodeHandle nh;

  Subscriber position_sub nh.subscribe("object_pos", 1, positionCallback);
  ros::Subscriber gui_sub = nh.subscribe("This_topic", 1000, guiCallback);

  // Define our transformation to the camera
  void positionCallback(geometry_msgs::pointF32 & position_msg)
  {

  }


  ros::spin();

   return 0;

}
