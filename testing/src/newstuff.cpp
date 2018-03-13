#include "../include/testing/newstuff.hpp"
#include "ui_newstuff.h"
#include <ros/ros.h>
#include <ros/network.h>
#include <QThread>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include <QMessageBox>


newstuff::newstuff(int argc, char** argv, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::newstuff),
  init_argc(argc),
  init_argv(argv),
  wrk(argc,argv)
  {
  ui->setupUi(this);

  std::cout<<"Init"<<std::endl;
  QObject::connect(ui->stuffButton, SIGNAL(clicked()), this, SLOT(on_stuffButton_clicked()));
  Worker* worker = new Worker(argc,argv);
  thread =  new QThread;
  QObject::connect(worker, SIGNAL(rosShutdown()), this, SLOT(close()));
  worker->moveToThread(thread);
  //connect(worker, SIGNAL(error(QString)), worker, SLOT(errorString(QString)));
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  std::cout<<"start"<<std::endl;

}

 newstuff::~newstuff()
{
  if(ros::isStarted()) {
    ros::shutdown(); // explicitly needed since we use ros::start();
    ros::waitForShutdown();
  }
  wait();
  delete ui;
}
Worker::Worker(int argc, char** argv):
init_argc(argc),
init_argv(argv)
{
ros_ready = false;
name = "I'm publishing yay! ";
ros::init(init_argc,init_argv,"testing");
ros::NodeHandle n;
chatter_publisher = n.advertise<std_msgs::String>("This_topic", 1000);
}
Worker::~Worker(){

}

bool Worker::init(){
  ros::init(init_argc,init_argv,"testing");
  if ( !ros::master::check() ) {
    return false;
  }
  ros::start(); // explicitly needed since our nodehandle is going out of scope.
  ros::NodeHandle n;
  // Add your ros communications here.
  chatter_publisher = n.advertise<std_msgs::String>("This_topic", 1000);
  return true;
}

void Worker::run() {

	ros::Rate loop_rate(1);
	int count = 0;
	while ( ros::ok() ) {
		std_msgs::String msg;
		std::stringstream ss;
		ss << name << count;
		msg.data = ss.str();
		chatter_publisher.publish(msg);
    std::cout<< "Test"<<std:: endl;
		ros::spinOnce();
		loop_rate.sleep();
		++count;
	}
  std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
  Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}

void newstuff::showNoMasterMessage() {
		QMessageBox msgBox;
		msgBox.setText("Found the ros master.");
		msgBox.exec();
		close();
}
void newstuff::showButtonTestMessage() {
		QMessageBox msgBox;
		msgBox.setText("Button tester ...");
		msgBox.exec();

}

void newstuff::on_stuffButton_clicked(){
  thread->start();
  showButtonTestMessage();
}

void Worker::process(){
  std::cout << "did i get here?" << std::endl;
  //while(ros::ok()){
    run();
    std::cout << "did i get past it?" << std::endl;

  //}
  Q_EMIT finished();
}
