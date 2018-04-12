#include <ros/ros.h>
#include "../msg/Setup.h"


//using namespace ros;

void guiCallback(const testing::Setup gui_msg){
  ROS_INFO("I heard: [%d]", gui_msg.reps);
}

int main(int argc, char ** argv)
{
  ros::init(argc,argv,"handler_node");
  ros::NodeHandle nh;



  ros::Subscriber gui_sub = nh.subscribe("This_topic", 1000, guiCallback);

  ros::spin();

   return 0;
}
