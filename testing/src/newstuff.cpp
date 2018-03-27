#include "../include/testing/newstuff.hpp"
//#include "../include/testing/progress.hpp"
#include "ui_newstuff.h"
#include <ros/ros.h>
#include <ros/network.h>
#include <QThread>
#include <string>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <sstream>
#include <QMessageBox>
#include <QVariant>
#include <QPalette>
int reps;
int current_stage;
QString easy_exercise1,easy_exercise2,easy_exercise3,easy_exercise4;
QString normal_exercise1,normal_exercise2,normal_exercise3,normal_exercise4;
QString hard_exercise1,hard_exercise2,hard_exercise3,hard_exercise4;
newstuff::newstuff(int argc, char** argv, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::newstuff),
  init_argc(argc),
  init_argv(argv)
  //wrk(argc,argv)
  {
  itemList << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" <<"10" <<"11" << "12" << "13" << "14" << "15";
  stageList << "Easy" <<"Medium" <<"Hard";
  easy_exercise1 ="";normal_exercise1 ="";hard_exercise1 ="";easy_exercise2 ="";normal_exercise2 ="";hard_exercise2 ="";easy_exercise3 ="";normal_exercise3 ="";hard_exercise3 ="";
  ui->setupUi(this);
  this->setStyleSheet("background-color: white;");
  ui->exerciseButton_1->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->exerciseButton_2->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->exerciseButton_3->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->exerciseButton_4->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->exerciseButton_1->setText("Exercises not set");
  ui->exerciseButton_1->setEnabled(false);
  ui->exerciseButton_2->setText("Exercises not set");
  ui->exerciseButton_2->setEnabled(false);
  ui->exerciseButton_3->setText("Exercises not set");
  ui->exerciseButton_3->setEnabled(false);
  ui->exerciseButton_4->setText("Exercises not set");
  ui->exerciseButton_4->setEnabled(false);
  ui->statusLabel->setText("Difficulty");
  ui->menuBar->setStyleSheet("QMenuBar::item{background-color:white;}");
  //ui->labelLayout->setStyleSheet("border-style: solid; border-color: grey; border-width: 3px;border-radius: 10px;");
  ui->exerciseArea->setStyleSheet(" border-style: solid; border-color: grey; border-width: 3px;border-radius: 10px;");
  ui->status_Area->setStyleSheet(" border-style: solid; border-color: grey; border-width: 3px;border-radius: 10px;");
  //ui->menuFile->setStyleSheet("menuBar::item {background-color: transparent;");

  //ui->menuFile->setStyleSheet("background-color: red;");
  //ui->menuBar->setStyleSheet("background-color: white);");
  //ui->menuFile->setStyleSheet("background-color: white);");
  reps =0;
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
ros::init(init_argc,init_argv,"testing");
ros::NodeHandle n;
chatter_publisher = n.advertise<std_msgs::Int16>("This_topic", 1000);
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
  chatter_publisher = n.advertise<std_msgs::Int16>("This_topic", 1000);
  return true;
}

void Worker::run() {

	ros::Rate loop_rate(1);
	while ( ros::ok() ) {
    std_msgs::Int16 dat;
    dat.data = reps;
		chatter_publisher.publish(dat);
		ros::spinOnce();
		loop_rate.sleep();
	}
  std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
  Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}
void newstuff::unhide(){
  this->show();
  ui->exerciseButton_1->setEnabled(false);
  ui->exerciseButton_2->setEnabled(false);
  ui->exerciseButton_3->setEnabled(false);
  ui->exerciseButton_4->setEnabled(false);
  ui->statusLabel->setText("Difficulty");
  ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->exerciseButton_1->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->exerciseButton_2->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->exerciseButton_3->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->exerciseButton_4->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
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
    ui->exerciseButton_1->setText(easy_exercise1);
    ui->exerciseButton_2->setText(easy_exercise2);
    ui->exerciseButton_3->setText(easy_exercise3);
    ui->exerciseButton_4->setText(easy_exercise4);
    ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_1->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_2->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_3->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_4->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->statusLabel->setText("Difficulty \nExercise difficulty: Easy");
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
    ui->exerciseButton_1->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_2->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_3->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_4->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->statusLabel->setText("Difficulty \nExercise difficulty: Not set");
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
    ui->exerciseButton_1->setText(normal_exercise1);
    ui->exerciseButton_2->setText(normal_exercise2);
    ui->exerciseButton_3->setText(normal_exercise3);
    ui->exerciseButton_4->setText(normal_exercise4);
    ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_1->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_2->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_3->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_4->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->statusLabel->setText("Difficulty \nExercise difficulty: Normal");

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
    ui->exerciseButton_1->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_2->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_3->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_4->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->statusLabel->setText("Difficulty \nExercise difficulty: Not set");
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
    ui->exerciseButton_1->setText(hard_exercise1);
    ui->exerciseButton_2->setText(hard_exercise2);
    ui->exerciseButton_3->setText(hard_exercise3);
    ui->exerciseButton_4->setText(hard_exercise4);
    ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_1->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_2->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_3->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_4->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->statusLabel->setText("Difficulty \nExercise difficulty: Hard");
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
    ui->exerciseButton_1->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_2->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_3->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->exerciseButton_4->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->statusLabel->setText("Difficulty \nExercise difficulty: Not set");
  }
}

void ExerciseSelect::on_exercise_select_button_1_clicked(){
  Selection selec("Exercises Available");
  accept();
  selec.exec();
}

void ExerciseSelect::on_exercise_select_button_2_clicked(){
  Selection selec("Exercises Available");
  accept();
  selec.exec();
}

void ExerciseSelect::on_exercise_select_button_3_clicked(){
  Selection selec("Exercises Available");
  accept();
  selec.exec();
}

void ExerciseSelect::on_exercise_select_button_4_clicked(){
  Selection selec("Exercises Available");
  accept();
  selec.exec();
}

void Selection::on_pnp_button_clicked(){
  switch(current_stage) {
    case 1 :
      easy_exercise1 = "pnp stage 1";
      easy_exercise2 = "pnp stage 1";
      easy_exercise3 = "pnp stage 1";
      easy_exercise4 = "pnp stage 1";
      break;
    case 2 :
    normal_exercise1 = "pnp stage 2";
    normal_exercise2 = "pnp stage 2";
    normal_exercise3 = "pnp stage 2";
    normal_exercise4 = "pnp stage 2";
      break;
    case 3 :
    hard_exercise1 = "pnp stage 3";
    hard_exercise2 = "pnp stage 3";
    hard_exercise3 = "pnp stage 3";
    hard_exercise4 = "pnp stage 3";
      break;
  }
  accept();
}
void Selection::on_rng_button_clicked(){
  switch(current_stage) {
    case 1 :
    easy_exercise1 = "rng stage 1";
    easy_exercise2 = "rng stage 1";
    easy_exercise3 = "rng stage 1";
    easy_exercise4 = "rng stage 1";
      break;
    case 2 :
    normal_exercise1 = "rng stage 2";
    normal_exercise2 = "rng stage 2";
    normal_exercise3 = "rng stage 2";
    normal_exercise4 = "rng stage 2";
      break;
    case 3 :
    hard_exercise1 = "rng stage 3";
    hard_exercise2 = "rng stage 3";
    hard_exercise3 = "rng stage 3";
    hard_exercise4 = "rng stage 3";
      break;
  }
  accept();
}
void Selection::on_ass_button_clicked(){
  switch(current_stage) {
    case 1 :
    easy_exercise1 = "ass stage 1";
    easy_exercise2 = "ass stage 1";
    easy_exercise3 = "ass stage 1";
    easy_exercise4 = "ass stage 1";
      break;
    case 2 :
    normal_exercise1 = "ass stage 2";
    normal_exercise2 = "ass stage 2";
    normal_exercise3 = "ass stage 2";
    normal_exercise4 = "ass stage 2";
      break;
    case 3 :
    hard_exercise1 = "ass stage 3";
    hard_exercise2 = "ass stage 3";
    hard_exercise3 = "ass stage 3";
    hard_exercise4 = "ass stage 3";
      break;
  }
  accept();
}
void Selection::on_taunt_button_clicked(){
  switch(current_stage) {
    case 1 :
    easy_exercise1 = "taunt stage 1";
    easy_exercise2 = "taunt stage 1";
    easy_exercise3 = "taunt stage 1";
    easy_exercise4 = "taunt stage 1";
      break;
    case 2 :
    normal_exercise1 = "taunt stage 2";
    normal_exercise2 = "taunt stage 2";
    normal_exercise3 = "taunt stage 2";
    normal_exercise4 = "taunt stage 2";
      break;
    case 3 :
    hard_exercise1 = "taunt stage 3";
    hard_exercise2 = "taunt stage 3";
    hard_exercise3 = "taunt stage 3";
    hard_exercise4 = "taunt stage 3";
      break;
  }
  accept();
}
void Selection::on_catch_button_clicked(){
  switch(current_stage) {
    case 1 :
    easy_exercise1 = "catch stage 1";
    easy_exercise2 = "catch stage 1";
    easy_exercise3 = "catch stage 1";
    easy_exercise4 = "catch stage 1";
      break;
    case 2 :
    normal_exercise1 = "catch stage 2";
    normal_exercise2 = "catch stage 2";
    normal_exercise3 = "catch stage 2";
    normal_exercise4 = "catch stage 2";
      break;
    case 3 :
    hard_exercise1 = "catch stage 3";
    hard_exercise2 = "catch stage 3";
    hard_exercise3 = "catch stage 3";
    hard_exercise4 = "catch stage 3";
      break;
  }
  accept();
}
void CustomDialog::on_okButton_clicked(){
  accept();
}

void newstuff::on_exerciseButton_1_clicked(){
  CustomDialog dialog(itemList,"Select Repetitions");
  if (dialog.exec() == QDialog::Accepted){
    cbox = dialog.box->currentText();
    reps = cbox.toInt();
    prg->show();
    this->hide();
    thread->start();
  }
}
void newstuff::on_exerciseButton_2_clicked(){
  CustomDialog dialog(itemList,"Select Repetitions");
  if (dialog.exec() == QDialog::Accepted){
    cbox = dialog.box->currentText();
    reps = cbox.toInt();
    prg->show();
    this->hide();
    thread->start();
  }
}
void newstuff::on_exerciseButton_3_clicked(){
  CustomDialog dialog(itemList,"Select Repetitions");
  if (dialog.exec() == QDialog::Accepted){
    cbox = dialog.box->currentText();
    reps = cbox.toInt();
    prg->show();
    this->hide();
    thread->start();
  }
}
void newstuff::on_exerciseButton_4_clicked(){
  CustomDialog dialog(itemList,"Select Repetitions");
  if (dialog.exec() == QDialog::Accepted){
    cbox = dialog.box->currentText();
    reps = cbox.toInt();
    prg->show();
    this->hide();
    thread->start();
  }
}

void newstuff::on_action_exit_triggered(){
  this->close();
}

void newstuff::on_action_exercises_triggered(){
  CustomDialog dialog(stageList,"Select difficulty for exercises");
  int stage;
  if (dialog.exec() == QDialog::Accepted){
    cbox = dialog.box->currentText();

    if(cbox == "Easy"){
      current_stage = 1;
    }
    else if(cbox == "Medium"){
      current_stage = 2;
    }
    else if(cbox == "Hard"){
      current_stage = 3;
    }
    ExerciseSelect exerselec("Select the exercise you want to edit");
    exerselec.exec();
  }
  std::cout<<current_stage<<std::endl;
  update(current_stage);
}

void newstuff::update(int stage){
  if(stage == 1){
    ui->exerciseButton_1->setText(easy_exercise1);
    ui->exerciseButton_2->setText(easy_exercise2);
    ui->exerciseButton_3->setText(easy_exercise3);
    ui->exerciseButton_4->setText(easy_exercise4);
  }
  else if(stage == 2){
    ui->exerciseButton_1->setText(normal_exercise1);
    ui->exerciseButton_2->setText(normal_exercise2);
    ui->exerciseButton_3->setText(normal_exercise3);
    ui->exerciseButton_4->setText(normal_exercise4);
  }
  else if(stage == 3){
    ui->exerciseButton_1->setText(hard_exercise1);
    ui->exerciseButton_2->setText(hard_exercise2);
    ui->exerciseButton_3->setText(hard_exercise3);
    ui->exerciseButton_4->setText(hard_exercise4);
  }
}
