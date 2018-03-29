#include "../include/testing/progress.hpp"
#include "../include/testing/newstuff.hpp"
#include "ui_progress.h"

using namespace std;
QString value;
int rep_count;
int prev_sec, prev_min;
progress::progress(int argc, char** argv, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::progress),
  init_argc(argc),
  init_argv(argv),
  st(argc,argv)
{
  ui->setupUi(this);
  this->setStyleSheet("background-color: white;");
  ui->progressBar->setValue(0);
  ui->label->setStyleSheet("background-color: rgba(153,204,255,255)");
  ui->progressBar->setStyleSheet("QProgressBar {border: 2px solid grey;border-radius: 5px;text-align: center;}");
  ui->progressBar->setStyleSheet("QProgressBar {border: 2px solid grey;border-radius: 5px;text-align: center;} QProgressBar::chunk {background-color: #f442b0;width: 20px;margin: 0.5px;}");
  ui->stopButton->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->pauseButton->setStyleSheet("background-color: rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;");
  ui->descriptionText->setStyleSheet("background-color: lightgrey");
  ui->lcdNumber->setPalette(Qt::black);
  ui->lcdNumber->setStyleSheet("border: 2px solid grey;border-radius: 5px;");
  ui->lcdNumber->setNumDigits(5);
  ui->lcdNumber->display(0);
  ui->menuBar->setStyleSheet("QMenuBar::item{background-color:white;}");

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
  subThread* sub = new subThread(argc,argv);
  sthread =  new QThread;
  QObject::connect(sub, SIGNAL(rosShutdown()), this, SLOT(close()));
  sub->moveToThread(sthread);
  connect(sthread, SIGNAL(started()), sub, SLOT(process()));
  connect(sub, SIGNAL(finished()), sthread, SLOT(quit()));
  connect(sub, SIGNAL(finished()), sub, SLOT(deleteLater()));
  connect(sthread, SIGNAL(finished()), sthread, SLOT(deleteLater()));
  connect(sub, SIGNAL(update()), this, SLOT(updateUI()));
  timer = new QTimer(this);
  etimer = new QElapsedTimer;
  mins = 0;
  secs = 0;
  connect(timer, SIGNAL(timeout()),this,SLOT(timer_tick()));

}

progress::~progress(){
  if(ros::isStarted()) {
    ros::shutdown(); // explicitly needed since we use ros::start();
    ros::waitForShutdown();
  }
  wait();
  delete ui;
}

subThread::subThread(int argc, char** argv) :
init_argc(argc),
init_argv(argv)
{
  ros::init(init_argc,init_argv,"test_sub");
}

subThread::~subThread(){

}

void subThread::process(){
  run();
}
void subThread::run(){
  ros::NodeHandle n;
  chatter_subscriber = n.subscribe("This_topic", 1000, &subThread::subCallback, this);
	ros::spin();
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}
void subThread::subCallback(const std_msgs::Int16 msg){
  int ph = msg.data;
  std::cout << ph << std::endl;
  value = "received: " + QString::number(ph) + " repetitions";
  rep_count = ph;
  Q_EMIT update();
}

void progress::timer_tick(){
  if (etimer->isValid()){
    secs = (etimer->elapsed()/ 1000)+prev_sec;
    mins = ((secs/60)%60)+prev_min;
    secs = secs % 60;
    std::stringstream ss;

    if(mins < 10 && secs < 10){
      ss<<"0"<< mins <<":" <<"0"<<secs;
    }
    else if(mins < 10){
      ss<<"0"<< mins <<":" <<secs;
    }
    else if(secs < 10){
      ss<< mins <<":" <<"0"<<secs;
    }
    else {
      ss<< mins <<":"<<secs;
    }
    secmin = ss.str();
    QString qstr =QString::fromStdString(secmin);
    ui->lcdNumber->display(qstr);
  }
}
void progress::closeEvent(QCloseEvent *event){
  Q_EMIT windowClosed();
  event->accept();
}

void progress::showEvent(QShowEvent *event){
  sthread->start();

  etimer->start();
  timer->start(1000);
}

void progress::on_stopButton_clicked(){
  etimer->invalidate();
  timer->stop();
  this->close();
}

void progress::on_pauseButton_clicked(){
  if (ui->pauseButton->text() == "Pause"){
    Q_EMIT pause();
    ui->pauseButton->setText("Play");
    prev_sec = prev_sec + etimer->elapsed() /1000;
    prev_min = prev_min + (prev_sec/60)%60;
    etimer->invalidate();
    timer->stop();
  }
  else {
    Q_EMIT play();
    ui->pauseButton->setText("Pause");
    etimer->start();
    timer->start(1000);
  }

}
void progress::updateUI(){
  secs = (etimer->elapsed()/ 1000)+prev_sec;
  mins = ((secs/60)%60)+prev_min;
  int display_prog;
  int reps_done = mins;
  if (rep_count != 0){
    display_prog = (100/rep_count)*reps_done;
  }
  if (ui->pauseButton->text() == "Pause"){
    ui->progressBar->setValue(display_prog);
    ui->descriptionText->setText(value);
  }
  else {
    ui->descriptionText->setText(value);
}
}
