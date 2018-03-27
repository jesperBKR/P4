#ifndef Q_MOC_RUN
#endif
#ifndef PROGRESS_H
#define PROGRESS_H

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <QMainWindow>
#include <iostream>
#include <QDesktopWidget>
#include <QRect>
#include <QCloseEvent>
#include <QThread>
#include <string>
#include <QString>
#include "std_msgs/String.h"
#include "std_msgs/Int16.h"
#include <QTimer>
#include <QElapsedTimer>
#include <sstream>

class subThread : public QObject {
  Q_OBJECT
public:
    subThread(int argc, char** argv);
    ~subThread();
    void run();
    bool init();
    int init_argc;
    char** init_argv;
    bool ros_ready;
    std::string name;
    QString test;
    void subCallback(const std_msgs::Int16 msg);

public Q_SLOTS:
  void process();
Q_SIGNALS:
    void finished();
    void error(QString err);
    void rosShutdown();
    void update();
private:
   ros::Subscriber chatter_subscriber;
};

namespace Ui {
class progress;
}

class progress : public QMainWindow
{
  Q_OBJECT

public:
  explicit progress(int argc, char** argv, QWidget *parent = 0);
   ~progress();
   int init_argc;
   char** init_argv;
   QThread* sthread;
   subThread st;
   QTimer* timer;
   QElapsedTimer* etimer;
Q_SIGNALS:
  void windowClosed();
public Q_SLOTS:
  void on_pauseButton_clicked();
  void on_stopButton_clicked();
  void updateUI();
  void timer_tick();
private:
  Ui::progress *ui;
  void closeEvent(QCloseEvent *event);
  int secs,mins,fake_time;
  std::string sec,min,secmin;
};

#endif // PROGRESS_H
