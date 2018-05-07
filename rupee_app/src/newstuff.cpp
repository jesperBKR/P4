//Include QT libraries
#include <QMessageBox>
#include <QVariant>
#include <QPalette>
#include <QButtonGroup>
#include <QThread>
#include <QString>


//Include cpp libraries

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <sstream>

//Include header and form
#include "../include/testing/newstuff.hpp"
#include "../include/testing/other.hpp"
#include "ui_newstuff.h"

//Include ros msgs
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include "../msg/Setup.h"
#include "rupee_msgs/Setup.h"

using namespace std;
/*********Global variables**********/
int reps,temp_reps,exbut;
int current_stage,current_exercise,exercise_type;
QStringList exerciseList;
bool pub_gui = true;
QButtonGroup stage_buttons, exercise_buttons;

/***********''''''''''''**********/
/********** Rupee_app  **********/
/**********''''''''''''**********/
Rupee::Rupee(int argc, char** argv, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::Rupee),
  init_argc(argc),
  init_argv(argv)
  {
  ui->setupUi(this);
  reps =0;
  Other other;
  other.somevar = 1;
  itemList << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" <<"10" <<"11" << "12" << "13" << "14" << "15";
  stageList << "Easy" <<"Medium" <<"Hard";
  exerciseList << "exercise 1 not set" << "exercise 2 not set" << "exercise 3 not set" << "exercise 4 not set"
               << "exercise 1 not set" << "exercise 2 not set" << "exercise 3 not set" << "exercise 4 not set"
               << "exercise 1 not set" << "exercise 2 not set" << "exercise 3 not set" << "exercise 4 not set";
  this->setStyleSheet("background-color: white;");
  exercise_buttons.addButton(ui->exerciseButton_1,0);
  exercise_buttons.addButton(ui->exerciseButton_2,1);
  exercise_buttons.addButton(ui->exerciseButton_3,2);
  exercise_buttons.addButton(ui->exerciseButton_4,3);
  stage_buttons.addButton(ui->fugl0Button,0);
  stage_buttons.addButton(ui->fugl1Button,1);
  stage_buttons.addButton(ui->fugl2Button,2);
  ui->statusLabel->setStyleSheet("font-weight: bold");
  ui->statusLabel->setText("Difficulty");
  ui->menuBar->setStyleSheet("QMenuBar::item{padding: 1px 5px;background-color:white;border-style: solid;border-color: grey;border-width: 3px;} QMenuBar::item::selected{background-color: rgba(153,204,255,255);border-style: solid;border-color: grey;border-width: 3px;}");
  ui->menuFile->setStyleSheet("background-color: rgba(153,204,255,255);border-style: solid;border-color: grey;border-width: 3px;");
  ui->menuExercises->setStyleSheet("background-color: rgba(153,204,255,255);border-style: solid;border-color: grey;border-width: 3px;");
  ui->exerciseArea->setStyleSheet(" border-style: solid; border-color: grey; border-width: 3px;border-radius: 10px;");
  ui->status_Area->setStyleSheet(" border-style: solid; border-color: grey; border-width: 3px;border-radius: 10px;");
  ui->addButton->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  // There was an error: class Ui::Rupee has no member 'startButton'
  // ui->startButton->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  for(int i = 0; i < 4; i++){
    exercise_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    exercise_buttons.button(i)->setText("No exercises selected");
    exercise_buttons.button(i)->setEnabled(false);
  }
  for(int i = 0; i < 3; i++){
    stage_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");

  }
  prg = new progress(argc,argv);
  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
  Worker* worker = new Worker(argc,argv);
  thread =  new QThread;
  QObject::connect(worker, SIGNAL(rosShutdown()), this, SLOT(close()));
  worker->moveToThread(thread);
  connect(prg, SIGNAL(windowClosed()),this, SLOT(unhide()));
  connect(prg, SIGNAL(pause()),this,SLOT(pausePub()));
  connect(prg, SIGNAL(play()),this,SLOT(playPub()));
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

}

 Rupee::~Rupee()
{
  if(ros::isStarted()) {
    ros::shutdown(); // explicitly needed since we use ros::start();
    ros::waitForShutdown();
  }
  wait();
  delete ui;
}

void Rupee::unhide(){
  cout<<"\ntest"<<endl;
  this->show();
  reps = 0;
  switch (current_stage) {
    case 1:
      ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
      ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
      ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
      ui->statusLabel->setText("Exercise Difficulty: \n Easy");
      break;
    case 2:
      ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
      ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
      ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
      ui->statusLabel->setText("Exercise Difficulty: \n Medium");
      break;
    case 3:
      ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
      ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
      ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
      ui->statusLabel->setText("Exercise Difficulty: \n Hard");
      break;
  }
  for (int i = 0; i < 4; i++) {exercise_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");}
}
void Rupee::showNoMasterMessage() {
		QMessageBox msgBox;
		msgBox.setText("Found the ros master.");
		msgBox.exec();
		close();
}
void Rupee::showButtonTestMessage() {
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

void Rupee::pausePub(){
    temp_reps = reps;
    reps = 0;
}

void Rupee::playPub(){
  reps = temp_reps;
}
/*
Jesper: Testing a few buttons to check if they work
*/

void Rupee::on_addButton_clicked(){
  exercise_buttons.button(0)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  exercise_buttons.button(0)->setText("No exercises selected");
  exercise_buttons.button(0)->setEnabled(false);
  exercise_buttons.button(0)->show();
}


void Rupee::on_action_help_triggered(){
  QMessageBox helpBox;
  helpBox.setWindowTitle("Help Assistant");
  helpBox.setText("To Select an exercise, choose a difficulty from one of the three buttons in the program.\n\nOnce a difficulty has been chosen, a window to choose the exercise will pop up.\n\n The amount of repetitions desired for the chosen exercise can now be chosen from a dropdown menu, varying between 1 to 15.\n\nHave fun! :)");
  helpBox.exec();
}

void Rupee::on_fugl0Button_clicked(){
  current_stage = 1;
  ui->fugl0Button->setDown(true);
  if (ui->fugl0Button->isDown()){
    for (int i = 0; i < 4; i++) {
      exercise_buttons.button(i)->setText(exerciseList[i]);
      exercise_buttons.button(i)->setEnabled(true);
      exercise_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    }
    for (int i = 1; i < 3; i++) {
      stage_buttons.button(i)->setDown(false);
      stage_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    }
    ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->statusLabel->setText("Difficulty \nExercise Difficulty: Easy");
  }
  else {
    for (int i = 0; i < 4; i++) {
      exercise_buttons.button(i)->setText(exerciseList[i]);
      exercise_buttons.button(i)->setEnabled(false);
      exercise_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    }
    ui->statusLabel->setText("Difficulty \nExercise Difficulty: Not set");
  }
}
void Rupee::on_fugl1Button_clicked(){
  current_stage = 2;
  ui->fugl1Button->setDown(true);
  if (ui->fugl1Button->isDown()){
    ui->fugl0Button->setDown(false);
    ui->fugl2Button->setDown(false);
    ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    for (int i = 0; i < 4; i++) {
      exercise_buttons.button(i)->setText(exerciseList[i+4]);
      exercise_buttons.button(i)->setEnabled(true);
      exercise_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    }
    ui->statusLabel->setText("Difficulty \nExercise Difficulty: Medium");

  }
  else {
    for (int i = 0; i < 4; i++) {
      exercise_buttons.button(i)->setText(exerciseList[i+4]);
      exercise_buttons.button(i)->setEnabled(false);
      exercise_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    }
    ui->statusLabel->setText("Difficulty \nExercise Difficulty: Not set");
  }
}
void Rupee::on_fugl2Button_clicked(){
  current_stage = 3;
  ui->fugl2Button->setDown(true);
  if (ui->fugl2Button->isDown()){
    ui->fugl0Button->setDown(false);
    ui->fugl1Button->setDown(false);
      for (int i = 0; i < 4; i++) {
      exercise_buttons.button(i)->setText(exerciseList[i+8]);
      exercise_buttons.button(i)->setEnabled(true);
      exercise_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    }
    ui->fugl0Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->fugl1Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    ui->fugl2Button->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: black;border-width: 3px;border-radius: 10px;");
    ui->statusLabel->setText("Difficulty \nExercise Difficulty: Hard");
    }
  else {
    for (int i = 0; i < 4; i++) {
      exercise_buttons.button(i)->setText(exerciseList[i+8]);
      exercise_buttons.button(i)->setEnabled(false);
      exercise_buttons.button(i)->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
    }
    ui->statusLabel->setText("Difficulty \nExercise Difficulty: Not set");
  }
}

void ExerciseSelect::on_exercise_select_button_1_clicked(){
  Selection selec("Exercises Available");
  current_exercise = 1;
  accept();
  selec.exec();
}

void ExerciseSelect::on_exercise_select_button_2_clicked(){
  Selection selec("Exercises Available");
  current_exercise = 2;
  accept();
  selec.exec();
}

void ExerciseSelect::on_exercise_select_button_3_clicked(){
  Selection selec("Exercises Available");
  current_exercise = 3;
  accept();
  selec.exec();
}

void ExerciseSelect::on_exercise_select_button_4_clicked(){
  Selection selec("Exercises Available");
  current_exercise = 4;
  accept();
  selec.exec();
}

void Selection::on_pnp_button_clicked(){
  switch(current_stage) {
    case 1 :
    switch(current_exercise){
      case 1 :
        exerciseList[0] = "Pick &&& Place";
        break;
      case 2 :
        exerciseList[1] = "Pick &&& Place";
        break;
      case 3 :
        exerciseList[2] = "Pick &&& Place";
        break;
      case 4 :
        exerciseList[3] = "Pick &&& Place";
        break;
    }
      break;
    case 2 :
    switch(current_exercise){
      case 1 :
        exerciseList[4] = "Pick &&& Place";
        break;
      case 2 :
        exerciseList[5] = "Pick &&& Place";
        break;
      case 3 :
        exerciseList[6] = "Pick &&& Place";
        break;
      case 4 :
        exerciseList[7] = "Pick &&& Place";
        break;
    }
      break;
    case 3 :
    switch(current_exercise){
      case 1 :
        exerciseList[8] = "Pick &&& Place";
        break;
      case 2 :
        exerciseList[9] = "Pick &&& Place";
        break;
      case 3 :
        exerciseList[10] = "Pick &&& Place";
        break;
      case 4 :
        exerciseList[11] = "Pick &&& Place";
        break;
    }
      break;
  }
  accept();
}
void Selection::on_rng_button_clicked(){
  switch(current_stage) {
    case 1 :
    switch(current_exercise){
      case 1 :
        exerciseList[0] = "Reach &&& Grasp";
        break;
      case 2 :
        exerciseList[1] = "Reach &&& Grasp";
        break;
      case 3 :
        exerciseList[2] = "Reach &&& Grasp";
        break;
      case 4 :
        exerciseList[3] = "Reach &&& Grasp";
        break;
    }
      break;
    case 2 :
    switch(current_exercise){
      case 1 :
        exerciseList[4] = "Reach &&& Grasp";
        break;
      case 2 :
        exerciseList[5] = "Reach &&& Grasp";
        break;
      case 3 :
        exerciseList[6] = "Reach &&& Grasp";
        break;
      case 4 :
        exerciseList[7] = "Reach &&& Grasp";
        break;
    }
      break;
    case 3 :
    switch(current_exercise){
      case 1 :
        exerciseList[8] = "Reach &&& Grasp";
        break;
      case 2 :
        exerciseList[9] = "Reach &&& Grasp";
        break;
      case 3 :
        exerciseList[10] = "Reach &&& Grasp";
        break;
      case 4 :
        exerciseList[11] = "Reach &&& Grasp";
        break;
    }
      break;
  }
  accept();
}

void Selection::on_ass_button_clicked(){
  switch(current_stage) {
    case 1 :
    switch(current_exercise){
      case 1 :
        exerciseList[0] = "Assisted Reach";
        break;
      case 2 :
        exerciseList[1] = "Assisted Reach";
        break;
      case 3 :
        exerciseList[2] = "Assisted Reach";
        break;
      case 4 :
        exerciseList[3] = "Assisted Reach";
        break;
    }
      break;
    case 2 :
    switch(current_exercise){
      case 1 :
        exerciseList[4] = "Assisted Reach";
        break;
      case 2 :
        exerciseList[5] = "Assisted Reach";
        break;
      case 3 :
        exerciseList[6] = "Assisted Reach";
        break;
      case 4 :
        exerciseList[7] = "Assisted Reach";
        break;
    }
      break;
    case 3 :
    switch(current_exercise){
      case 1 :
        exerciseList[8] = "Assisted Reach";
        break;
      case 2 :
        exerciseList[9] = "Assisted Reach";
        break;
      case 3 :
        exerciseList[10] = "Assisted Reach";
        break;
      case 4 :
        exerciseList[11] = "Assisted Reach";
        break;
    }
      break;
  }
  accept();
}

void Selection::on_taunt_button_clicked(){
  switch(current_stage) {
    case 1 :
    switch(current_exercise){
      case 1 :
        exerciseList[0] = "Intercept &&& Grasp";
        break;
      case 2 :
        exerciseList[1] = "Intercept &&& Grasp";
        break;
      case 3 :
        exerciseList[2] = "Intercept &&& Grasp";
        break;
      case 4 :
        exerciseList[3] = "Intercept &&& Grasp";
        break;
    }
      break;
    case 2 :
    switch(current_exercise){
      case 1 :
        exerciseList[4] = "Intercept &&& Grasp";
        break;
      case 2 :
        exerciseList[5] = "Intercept &&& Grasp";
        break;
      case 3 :
        exerciseList[6] = "Intercept &&& Grasp";
        break;
      case 4 :
        exerciseList[7] = "Intercept & Grasp";
        break;
    }
      break;
    case 3 :
    switch(current_exercise){
      case 1 :
        exerciseList[8] = "Intercept &&& Grasp";
        break;
      case 2 :
        exerciseList[9] = "Intercept &&& Grasp";
        break;
      case 3 :
        exerciseList[10] = "Intercept &&& Grasp";
        break;
      case 4 :
        exerciseList[11] = "Intercept &&& Grasp";
        break;
    }
      break;
  }
  accept();
}

void Selection::on_catch_button_clicked(){
  switch(current_stage) {
    case 1 :
    switch(current_exercise){
      case 1 :
        exerciseList[0] = "Chase &&& Touch";
        break;
      case 2 :
        exerciseList[1] = "Chase &&& Touch";
        break;
      case 3 :
        exerciseList[2] = "Chase &&& Touch";
        break;
      case 4 :
        exerciseList[3] = "Chase &&& Touch";
        break;
    }
      break;
    case 2 :
    switch(current_exercise){
      case 1 :
        exerciseList[4] = "Chase &&& Touch";
        break;
      case 2 :
        exerciseList[5] = "Chase &&& Touch";
        break;
      case 3 :
        exerciseList[6] = "Chase &&& Touch";
        break;
      case 4 :
        exerciseList[7] = "Chase &&& Touch";
        break;
    }
      break;
    case 3 :
    switch(current_exercise){
      case 1 :
        exerciseList[8] = "Chase &&& Touch";
        break;
      case 2 :
        exerciseList[9] = "Chase &&& Touch";
        break;
      case 3 :
        exerciseList[10] = "Chase &&& Touch";
        break;
      case 4 :
        exerciseList[11] = "Chase &&& Touch";
        break;
    }
      break;
  }
  accept();
}
void CustomDialog::on_okButton_clicked(){
  accept();
}

void Rupee::on_exerciseButton_1_clicked(){
  if (ui->exerciseButton_1->text() == "exercise 1 not set"){
    Selection selec("Exercises Available");
    current_exercise = 1;
    selec.exec();
      //ExerciseSelect exerselec("Select the exercise you want to edit");
      //exerselec.exec();
    }

  else{
    CustomDialog dialog(itemList,"Select Repetitions");
    if (dialog.exec() == QDialog::Accepted){
      cbox = dialog.box->currentText();
      reps = cbox.toInt();
      prg->show();
      this->hide();
      thread->start();
    }
  }
  update(current_stage);
}
void Rupee::on_exerciseButton_2_clicked(){
  if (ui->exerciseButton_2->text() == "exercise 2 not set"){
    Selection selec("Exercises Available");
    current_exercise = 2;
    selec.exec();
    }
  else{
    CustomDialog dialog(itemList,"Select Repetitions");
    if (dialog.exec() == QDialog::Accepted){
      cbox = dialog.box->currentText();
      reps = cbox.toInt();
      prg->show();
      this->hide();
      thread->start();
    }
  }
  update(current_stage);
}
void Rupee::on_exerciseButton_3_clicked(){
  if (ui->exerciseButton_3->text() == "exercise 3 not set"){
    Selection selec("Exercises Available");
    current_exercise = 3;
    selec.exec();
    }

  else{
    CustomDialog dialog(itemList,"Select Repetitions");
    if (dialog.exec() == QDialog::Accepted){
      cbox = dialog.box->currentText();
      reps = cbox.toInt();
      prg->show();
      this->hide();
      thread->start();
    }
  }
  update(current_stage);
}
void Rupee::on_exerciseButton_4_clicked(){
  if (ui->exerciseButton_4->text() == "exercise 4 not set"){
    Selection selec("Exercises Available");
    current_exercise = 4;
    selec.exec();
    }

  else{
    CustomDialog dialog(itemList,"Select Repetitions");
    if (dialog.exec() == QDialog::Accepted){
      cbox = dialog.box->currentText();
      reps = cbox.toInt();
      prg->show();
      this->hide();
      thread->start();
    }
  }
  update(current_stage);
}

void Rupee::on_action_exit_triggered(){
  this->close();
}


void Rupee::on_action_exercises_triggered(){
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
  update(current_stage);
}

void Rupee::update(int stage){
  if(stage == 1){
    on_fugl0Button_clicked();
    ui->exerciseButton_1->setText(exerciseList[0]);
    ui->exerciseButton_2->setText(exerciseList[1]);
    ui->exerciseButton_3->setText(exerciseList[2]);
    ui->exerciseButton_4->setText(exerciseList[3]);
  }
  else if(stage == 2){
    on_fugl1Button_clicked();
    ui->exerciseButton_1->setText(exerciseList[4]);
    ui->exerciseButton_2->setText(exerciseList[5]);
    ui->exerciseButton_3->setText(exerciseList[6]);
    ui->exerciseButton_4->setText(exerciseList[7]);
  }
  else if(stage == 3){
    on_fugl2Button_clicked();
    ui->exerciseButton_1->setText(exerciseList[8]);
    ui->exerciseButton_2->setText(exerciseList[9]);
    ui->exerciseButton_3->setText(exerciseList[10]);
    ui->exerciseButton_4->setText(exerciseList[11]);
  }
}
/********************************/
/********** ROS threat **********/
/********************************/
Worker::Worker(int argc, char** argv):
init_argc(argc),
init_argv(argv)
{
  ros_ready = false;
  ros::init(init_argc,init_argv,"testing");
  ros::NodeHandle n;
  chatter_publisher = n.advertise<rupee_msgs::Setup>("GUI_feed", 1000);
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
  chatter_publisher = n.advertise<rupee_msgs::Setup>("GUI_feed", 1000);
  return true;
}


void Worker::run() {
	ros::Rate loop_rate(1);
	while (ros::ok()) {
    rupee_msgs::Setup data;
    data.reps = reps;
    data.diff = current_stage;
    data.type = current_exercise;
    data.run = true;
		chatter_publisher.publish(data);
		ros::spinOnce();
		loop_rate.sleep();
  }
  //std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
  //Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}
