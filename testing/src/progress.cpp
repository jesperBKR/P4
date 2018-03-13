#include "../include/testing/progress.hpp"
#include "ui_progress.h"

progress::progress(int argc, char** argv, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::progress)
{
  ui->setupUi(this);
}

 progress::~progress()
{
  delete ui;
}
