#ifndef Q_MOC_RUN
#endif
#ifndef NEWSTUFF_H
#define NEWSTUFF_H

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <QMainWindow>
#include <string>
#include <QThread>
#include <QStringListModel>

class Worker : public QObject {
  Q_OBJECT
public:
    Worker(int argc, char** argv);
    ~Worker();
    void run();
    bool init();
    int init_argc;
    char** init_argv;
    bool ros_ready;
    std::string name;

public Q_SLOTS:
  void process();
Q_SIGNALS:
    void finished();
    void error(QString err);
    void rosShutdown();
private:
   ros::Publisher chatter_publisher;


  // newstuff n;

};
namespace Ui {
class newstuff;
}

class newstuff : public QMainWindow{
  Q_OBJECT

public:
   explicit newstuff(int argc, char** argv, QWidget *parent = 0);
    ~newstuff();
    int init_argc;
    char** init_argv;
    void showNoMasterMessage();
    void showButtonTestMessage();
    QThread* thread; 

public Q_SLOTS:
  void on_stuffButton_clicked();
private:
  Ui::newstuff *ui;
  Worker wrk;
};


#endif // NEWSTUFF_H
