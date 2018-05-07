//Include Libraries
#include<ros/ros.h>
#include<iostream>

//Include Messages
#include<std_msgs/Int8.h>
#include<std_msgs/Bool.h>
#include"rupee_msgs/Setup.h"
#include"rupee_msgs/object_pos.h"
#include"rupee_msgs/camera.h"

using namespace ros;

//Global Variables

//Class
class Handler{
public:
  //Constructor
  Handler(){
    //Subscribers
    position_sub = nh.subscribe("RS_object", 1, &Handler::positionCallback,this);
    gui_sub = nh.subscribe("GUI_feed", 1, &Handler::guiCallback,this);
    process_sub = nh.subscribe("JACO_feedback",1,&Handler::processCallback,this);

    //Publishers
    move_pub = nh.advertise<rupee_msgs::object_pos>("JACO_goal", 1);
    feedback_pub = nh.advertise<std_msgs::Int8>("GUI_feedback", 1);
  }
  //Destructor
  ~Handler(){

  }

  //Input values from GUI: reps, diff, type, and if running (run)
  void guiCallback(const rupee_msgs::Setup& gui_msg){
    ROS_INFO("Reps: [%d], Difficulty: [%d], Exercise Type: [%d], Move: [%d]", gui_msg.reps,gui_msg.diff,gui_msg.type, gui_msg.run);
    if(gui_msg.reps == 0 || !gui_msg.run){ //If gui not paused (reps = 0) or started (run = false) then it should not start
      start = false;
      if(!gui_msg.run){
        rep = 0;
      }
    }
    else{
      start = true;
      dif = gui_msg.diff;
      ex = gui_msg.type;
      feedback.data = rep;
    }
    feedback_pub.publish(feedback); //Number of repetitions done

//    //Oliver test stuff //*****
//    moveit.move.data = true;
//    //}
//    //else{
//    //  moveit.move.data = false;
//    //}
//    moveit.difficulty.data = dif;
//    moveit.exercise.data = ex;
//    moveit.location.x = 0.1539178662002; //position_msg.location.x;
//    moveit.location.y = -0.661769986153; //position_msg.location.y;
//    moveit.location.z = 0.131685048342; //position_msg.location.z;
//    move_pub.publish(moveit);
//    //*****
  }
  //Object position, xyz... What should we do if it cannot find the object? Is it still publishing a point?
  void positionCallback(const rupee_msgs::camera& position_msg){
    ROS_INFO("Postion x:[%f], y: [%f], z: [%f]", position_msg.location.x,position_msg.location.y,position_msg.location.z);
    if(start && position_msg.detected.data && moveit.location.z <0.05){
    moveit.move.data = true;
    }
    else{
      moveit.move.data = false;
    }
    moveit.difficulty.data = dif;
    moveit.exercise.data = ex;
    moveit.location.x = position_msg.location.x;
    moveit.location.y = position_msg.location.y;
    moveit.location.z = position_msg.location.z;
    move_pub.publish(moveit);

  }
  //JACO number of repetitions completed
  void processCallback(const std_msgs::Bool& process_msg){
    ROS_INFO("Rep done: [%d]",process_msg.data);
    if(start){
      if(process_msg.data){   //Whenever JACO returns true the repetition counter goes up
        rep++;
      }
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

  //Objects/Messages
  rupee_msgs::object_pos moveit;
  std_msgs::Int8 feedback;

  //Variables
  bool start;
  int rep;
  int dif;
  int ex;
};


int main(int argc, char ** argv){
  //Initiate ROS
  init(argc, argv, "handler_node");
  //Instantiate the Handler class
  Handler handler;

  spin();

  return 0;
}
