#include "../include/testing/progress.hpp"
#include "../include/testing/newstuff.hpp"
#include "ui_progress.h"

progress::progress(int argc, char** argv, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::progress)
{
  //newstuff ns(argc,argv);
  ui->setupUi(this);
  //ns = new newstuff(argc,argv);
  ui->progressBar->setValue(50);
  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
  done = false;
  //ns = new newstuff(argc,argv);

}

 progress::~progress()
{
  delete ui;
}

void progress::closeEvent(QCloseEvent *event){
  Q_EMIT windowClosed();
  event->accept();

}
void progress::on_stopButton_clicked(){
  done = true;
  //ns.show();
  this->close();
}

void progress::on_pauseButton_clicked(){

}
