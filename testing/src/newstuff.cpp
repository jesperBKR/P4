#include "../include/testing/newstuff.hpp"
//#include "../include/testing/progress.hpp"
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
  itemList << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" <<"10" <<"11" << "12" << "13" << "14" << "15";
  ui->setupUi(this);
  ui->exerciseArea->setTitle("Exercise difficulty not set");
  ui->exerciseButton_1->setText("Exercises not set");
  ui->exerciseButton_1->setEnabled(false);
  ui->exerciseButton_2->setText("Exercises not set");
  ui->exerciseButton_2->setEnabled(false);
  ui->exerciseButton_3->setText("Exercises not set");
  ui->exerciseButton_3->setEnabled(false);
  ui->exerciseButton_4->setText("Exercises not set");
  ui->exerciseButton_4->setEnabled(false);
  prg = new progress(argc,argv);
  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
  //QObject::connect(ui->stuffButton, SIGNAL(clicked()), this, SLOT(on_stuffButton_clicked()));
  Worker* worker = new Worker(argc,argv);
  thread =  new QThread;
  QObject::connect(worker, SIGNAL(rosShutdown()), this, SLOT(close()));
  worker->moveToThread(thread);
  //connect(worker, SIGNAL(error(QString)), worker, SLOT(errorString(QString)));
  connect(prg, SIGNAL(windowClosed()),this, SLOT(unhide()));
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
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
void newstuff::unhide(){
  this->show();
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
void Worker::process(){
  std::cout << "did i get here?" << std::endl;
  //while(ros::ok()){
  run();
  std::cout << "did i get past it?" << std::endl;

  //}
  Q_EMIT finished();
}


void newstuff::on_fugl0Button_clicked(){
  ui->fugl0Button->setDown(true);
  if (ui->fugl0Button->isDown()){
    ui->fugl1Button->setDown(false);
    ui->fugl2Button->setDown(false);
    ui->exerciseButton_1->setEnabled(true);
    ui->exerciseButton_2->setEnabled(true);
    ui->exerciseButton_3->setEnabled(true);
    ui->exerciseButton_4->setEnabled(true);
    ui->exerciseButton_1->setText("Exercise 0.1 Placeholder");
    ui->exerciseButton_2->setText("Exercise 0.2 Placeholder");
    ui->exerciseButton_3->setText("Exercise 0.3 Placeholder");
    ui->exerciseButton_4->setText("Exercise 0.4 Placeholder");
    ui->exerciseArea->setTitle("Exercise difficulty = 0");
  }
  else {
    ui->exerciseButton_1->setEnabled(false);
    ui->exerciseButton_2->setEnabled(false);
    ui->exerciseButton_3->setEnabled(false);
    ui->exerciseButton_4->setEnabled(false);
    ui->exerciseButton_1->setText("Exercises not set");
    ui->exerciseButton_2->setText("Exercises not set");
    ui->exerciseButton_3->setText("Exercises not set");
    ui->exerciseButton_4->setText("Exercises not set");
    ui->exerciseArea->setTitle("Exercise difficulty not set");
  }
}
void newstuff::on_fugl1Button_clicked(){
  ui->fugl1Button->setDown(true);
  if (ui->fugl1Button->isDown()){
    ui->fugl0Button->setDown(false);
    ui->fugl2Button->setDown(false);
    ui->exerciseButton_1->setEnabled(true);
    ui->exerciseButton_2->setEnabled(true);
    ui->exerciseButton_3->setEnabled(true);
    ui->exerciseButton_4->setEnabled(true);
    ui->exerciseButton_1->setText("Exercise 1.1 Placeholder");
    ui->exerciseButton_2->setText("Exercise 1.2 Placeholder");
    ui->exerciseButton_3->setText("Exercise 1.3 Placeholder");
    ui->exerciseButton_4->setText("Exercise 1.4 Placeholder");
    ui->exerciseArea->setTitle("Exercise difficulty = 1 ");

  }
  else {
    ui->exerciseButton_1->setEnabled(false);
    ui->exerciseButton_2->setEnabled(false);
    ui->exerciseButton_3->setEnabled(false);
    ui->exerciseButton_4->setEnabled(false);
    ui->exerciseButton_1->setText("Exercises not set");
    ui->exerciseButton_2->setText("Exercises not set");
    ui->exerciseButton_3->setText("Exercises not set");
    ui->exerciseButton_4->setText("Exercises not set");
    ui->exerciseArea->setTitle("Exercise difficulty not set");
  }
}
void newstuff::on_fugl2Button_clicked(){
  ui->fugl2Button->setDown(true);
  if (ui->fugl2Button->isDown()){
    ui->fugl0Button->setDown(false);
    ui->fugl1Button->setDown(false);
    ui->exerciseButton_1->setEnabled(true);
    ui->exerciseButton_2->setEnabled(true);
    ui->exerciseButton_3->setEnabled(true);
    ui->exerciseButton_4->setEnabled(true);
    ui->exerciseButton_1->setText("Exercise 2.1 Placeholder");
    ui->exerciseButton_2->setText("Exercise 2.2 Placeholder");
    ui->exerciseButton_3->setText("Exercise 2.3 Placeholder");
    ui->exerciseButton_4->setText("Exercise 2.4 Placeholder");
    ui->exerciseArea->setTitle("Exercise difficulty = 2");

  }
  else {
    ui->exerciseButton_1->setEnabled(false);
    ui->exerciseButton_2->setEnabled(false);
    ui->exerciseButton_3->setEnabled(false);
    ui->exerciseButton_4->setEnabled(false);
    ui->exerciseButton_1->setText("Exercises not set");
    ui->exerciseButton_2->setText("Exercises not set");
    ui->exerciseButton_3->setText("Exercises not set");
    ui->exerciseButton_4->setText("Exercises not set");
    ui->exerciseArea->setTitle("Exercise difficulty not set");
  }
}
void CustomDialog::on_okButton_clicked(){
  accept();
}

void newstuff::on_exerciseButton_1_clicked(){
  CustomDialog dialog(itemList);
  if (dialog.exec() == QDialog::Accepted){
    prg->show();
    this->hide();
    thread->start();
  }
}
void newstuff::on_exerciseButton_2_clicked(){
  CustomDialog dialog(itemList);
  if (dialog.exec() == QDialog::Accepted){
    prg->show();
    this->hide();
    thread->start();
  }
}
void newstuff::on_exerciseButton_3_clicked(){
  CustomDialog dialog(itemList);
  if (dialog.exec() == QDialog::Accepted){
    prg->show();
    this->hide();
    thread->start();
  }
}

void newstuff::on_exerciseButton_4_clicked(){
  CustomDialog dialog(itemList);
  if (dialog.exec() == QDialog::Accepted){
    prg->show();
    this->setVisible(false);
    thread->start();
  }
}

void newstuff::on_action_exit_triggered(){
  this->close();
}
