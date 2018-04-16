//Include Libraries
#include <ros/ros.h>
#include<iostream>

//Include Messages
#include <geometry_msgs/Point32.h>
#include<std_msgs/Int32.h>
#include<std_msgs/Bool.h>
#include "../msg/Setup.h"
#include "../msg/object_pos.h"

using namespace ros;

//Input values from GUI: reps, diff, and type
void guiCallback(const testing::Setup gui_msg){
  ROS_INFO("I heard: [%d], [%d], [%d]", gui_msg.reps,gui_msg.diff,gui_msg.type);
}
//Object position, xyz
void positionCallback(const geometry_msgs::Point32& position_msg){
  ROS_INFO("I heard: [%f], [%f], [%f]", position_msg.x,position_msg.y,position_msg.z);
}
//JACO completed number of repetitions
void processCallback(const std_msgs::Bool& process_msg){
  ROS_INFO("I head: [%d]",process_msg.data);


}

int main(int argc, char ** argv){

  init(argc, argv, "handler_node");
  NodeHandle nh;
  ros::Publisher move_pub = nh.advertise<handler::object_pos>("move_to", 1);
  //Subscribers
  Subscriber position_sub = nh.subscribe("object_pos", 1, positionCallback);
  Subscriber gui_sub = nh.subscribe("This_topic", 1, guiCallback);
  Subscriber process_sub = nh.subscribe("process_JACO",1,processCallback);

  //Publishers
  ros::Publisher feedback_pub = nh.advertise<std_msgs::Int32>("feedback", 1);

  handler::object_pos moveit;
  moveit.move.data = true;
  std::cout << "x-position: " << std::endl;
  std::cin >> moveit.location.x;
  std::cout << "y-position: " << std::endl;
  std::cin >> moveit.location.y;
  std::cout << "z-position: " << std::endl;
  std::cin >> moveit.location.z;
  while (true||ros::ok()){
  move_pub.publish(moveit);
  }


  spin();

  return 0;

}
