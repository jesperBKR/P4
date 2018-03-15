#ifndef Q_MOC_RUN
#endif
#ifndef NEWSTUFF_H
#define NEWSTUFF_H

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <QMainWindow>
#include <QDialog>
#include <QComboBox>
#include <QHBoxLayout>
#include <QAbstractButton>
#include <QPushButton>
#include <string>
#include <QThread>
#include <QStringList>
#include <QStringListModel>
#include "progress.hpp"
#include <QDesktopWidget>
#include <QRect>


class CustomDialog : public QDialog{
  Q_OBJECT
public:
  CustomDialog(const QStringList items)
  {
    setLayout(new QHBoxLayout());
    //setTitle("Select repetitions")
    setWindowTitle("Select repetitions");
    box = new QComboBox;
    box->addItems(items);
    layout()->addWidget(box);
    QDesktopWidget dw;
    int x=dw.width()*0.2;
    int y=dw.height()*0.2;
    setFixedSize(x,y);
    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
    QPushButton* okButton = new QPushButton("Ok");
    layout()->addWidget(okButton);
    connect(okButton, SIGNAL(clicked()), this,SLOT (on_okButton_clicked()));
  }

  QComboBox* comboBox() { return box; }
  public Q_SLOTS:
  void on_okButton_clicked();
private:
  QComboBox* box;
};

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
    progress* prg;
    Worker wrk;
    QStringList itemList;
    //CustomDialog dialog;

public Q_SLOTS:
  void on_fugl0Button_clicked();
  void on_fugl1Button_clicked();
  void on_fugl2Button_clicked();
  void on_exerciseButton_1_clicked();
  void on_exerciseButton_2_clicked();
  void on_exerciseButton_3_clicked();
  void on_exerciseButton_4_clicked();
  void unhide();

private Q_SLOTS:
void on_action_exit_triggered();
private:
  Ui::newstuff *ui;
};


#endif // NEWSTUFF_H
