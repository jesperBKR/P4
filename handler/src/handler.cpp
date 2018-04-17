//Include Libraries
#include <ros/ros.h>
#include<iostream>

//Include Messages
#include <geometry_msgs/Point32.h>
#include<std_msgs/Int32.h>
#include<std_msgs/Bool.h>
#include "rupee_msgs/Setup.h"
#include "rupee_msgs/object_pos.h"

using namespace ros;

//Global Variables
Rate loop_rate(10);

//Class
class Handler{
public:
  Handler(){
    //Subscribers
    Subscriber position_sub = nh.subscribe("object_pos", 1, &Handler::positionCallback,this);
    Subscriber gui_sub = nh.subscribe("This_topic", 1, &Handler::guiCallback,this);
    Subscriber process_sub = nh.subscribe("process_JACO",1,&Handler::processCallback,this);

    //Publishers
    ros::Publisher move_pub = nh.advertise<rupee_msgs::object_pos>("move_to", 1);
    ros::Publisher feedback_pub = nh.advertise<std_msgs::Int32>("feedback", 1);
  }
  ~Handler();

  //Input values from GUI: reps, diff, and type
  void guiCallback(const rupee_msgs::Setup gui_msg){
    ROS_INFO("I heard: [%d], [%d], [%d]", gui_msg.reps,gui_msg.diff,gui_msg.type);
    if(gui_msg.reps == 0){
      start = false;
    }
    else{
      start = true;
    }

  }
  //Object position, xyz
  void positionCallback(const geometry_msgs::Point32& position_msg){
    ROS_INFO("I heard: [%f], [%f], [%f]", position_msg.x,position_msg.y,position_msg.z);
  }
  //JACO number of repetitions completed
  void processCallback(const std_msgs::Bool& process_msg){
    ROS_INFO("I head: [%d]",process_msg.data);
    if(start){
      moveit.move.data = true;
      std::cout << "x-position: " << std::endl;
      std::cin >> moveit.location.x;
      std::cout << "y-position: " << std::endl;
      std::cin >> moveit.location.y;
      std::cout << "z-position: " << std::endl;
      std::cin >> moveit.location.z;
      move_pub.publish(moveit);
    }
  }

private:
  //ROS
  NodeHandle nh;
  Subscriber position_sub;
  Subscriber gui_sub;
  Subscriber process_sub;
  Publisher move_pub;
  Publisher feedback_pub;

  //Variables and objects
  rupee_msgs::object_pos moveit;
  bool start;
};


int main(int argc, char ** argv){

  init(argc, argv, "handler_node");
  Handler handler;

  while (true||ok()){

  }

  spinOnce();
  loop_rate.sleep();

  return 0;

}
