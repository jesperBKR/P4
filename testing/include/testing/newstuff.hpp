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

class ExerciseSelect: public QDialog{
  Q_OBJECT
public:
  ExerciseSelect(QString title){
    //setLayout(new QHBoxLayout());
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QHBoxLayout* hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);

    //setLayout(vLayout);
    setWindowTitle(title);
    QPushButton* exercise_select_button_1 = new QPushButton("Exercise 1");
    QPushButton* exercise_select_button_2 = new QPushButton("Exercise 2");
    QPushButton* exercise_select_button_3 = new QPushButton("Exercise 3");
    QPushButton* exercise_select_button_4 = new QPushButton("Exercise 4");
    hLayout->addWidget(exercise_select_button_1);
    hLayout->addWidget(exercise_select_button_2);
    hLayout->addWidget(exercise_select_button_3);
    hLayout->addWidget(exercise_select_button_4);
    QDesktopWidget dw;
    int x=dw.width()*0.4;
    int y=dw.height()*0.4;
    setFixedSize(x,y);
    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
    connect(exercise_select_button_1, SIGNAL(clicked()), this,SLOT (on_exercise_select_button_1_clicked()));
    connect(exercise_select_button_2, SIGNAL(clicked()), this,SLOT (on_exercise_select_button_2_clicked()));
    connect(exercise_select_button_3, SIGNAL(clicked()), this,SLOT (on_exercise_select_button_3_clicked()));
    connect(exercise_select_button_4, SIGNAL(clicked()), this,SLOT (on_exercise_select_button_4_clicked()));
    setStyleSheet("QDialog {background-color: white;} QPushButton {background-color:rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;}  ");
  }
private:
public Q_SLOTS:
  void on_exercise_select_button_1_clicked();
  void on_exercise_select_button_2_clicked();
  void on_exercise_select_button_3_clicked();
  void on_exercise_select_button_4_clicked();
};

class Selection: public QDialog{
  Q_OBJECT
public:
  Selection(QString title){

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QHBoxLayout* hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);

    setWindowTitle(title);
    QPushButton* pnp_button = new QPushButton("Pick & Place");
    QPushButton* rng_button = new QPushButton("Reach & Grap");
    QPushButton* ass_button = new QPushButton("Assistive");
    QPushButton* taunt_button = new QPushButton("Taunt");
    QPushButton* catch_button = new QPushButton("Catch");
    hLayout->addWidget(pnp_button);
    hLayout->addWidget(rng_button);
    hLayout->addWidget(ass_button);
    hLayout->addWidget(taunt_button);
    hLayout->addWidget(catch_button);
    QDesktopWidget dw;
    int x=dw.width()*0.4;
    int y=dw.height()*0.4;
    setFixedSize(x,y);
    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
    connect(pnp_button, SIGNAL(clicked()), this,SLOT (on_pnp_button_clicked()));
    connect(rng_button, SIGNAL(clicked()), this,SLOT (on_rng_button_clicked()));
    connect(ass_button, SIGNAL(clicked()), this,SLOT (on_ass_button_clicked()));
    connect(taunt_button, SIGNAL(clicked()), this,SLOT (on_taunt_button_clicked()));
    connect(catch_button, SIGNAL(clicked()), this,SLOT (on_catch_button_clicked()));
    setStyleSheet("QDialog {background-color: white;} QPushButton {background-color:rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;}  ");

  }
private:
public Q_SLOTS:
  void on_pnp_button_clicked();
  void on_rng_button_clicked();
  void on_ass_button_clicked();
  void on_taunt_button_clicked();
  void on_catch_button_clicked();
};

class CustomDialog : public QDialog{
  Q_OBJECT
public:
  CustomDialog(const QStringList items, QString title)
  {
    setLayout(new QHBoxLayout());
    setWindowTitle(title);
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
    box->setStyleSheet( "selection-background-color: rgba(153,204,255,255);selection-color: white; color: black; background-color: rgba(153,204,255,255);border-style: solid;border: 1px solid grey;border-radius: 5;");
    setStyleSheet("QDialog {background-color: white;} QPushButton {background-color:rgba(153,204,255,255); border-style: solid;border-color: grey;border-width: 3px;border-radius: 10px;}  ");

  }

  QComboBox* comboBox() { return box;}
  QComboBox* box;
  public Q_SLOTS:
  void on_okButton_clicked();
private:
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
    void update(int stage);
    QThread* thread;
    progress* prg;
    QStringList itemList;
    QStringList stageList;
    QString cbox;
Q_SIGNALS:
  void startUp();
public Q_SLOTS:
  void on_fugl0Button_clicked();
  void on_fugl1Button_clicked();
  void on_fugl2Button_clicked();
  void on_exerciseButton_1_clicked();
  void on_exerciseButton_2_clicked();
  void on_exerciseButton_3_clicked();
  void on_exerciseButton_4_clicked();
  void unhide();
  void pausePub();
  void playPub();

private Q_SLOTS:
void on_action_exercises_triggered();
void on_action_exit_triggered();
private:
  Ui::newstuff *ui;
  void setExercise(int exercise_type);
  void setJacoFunction(double workspace[5],int reps, int diff );
};


#endif // NEWSTUFF_H
