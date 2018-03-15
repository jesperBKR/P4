#ifndef Q_MOC_RUN
#endif
#ifndef PROGRESS_H
#define PROGRESS_H

#include <QMainWindow>
#include <iostream>
#include <QDesktopWidget>
#include <QRect>
#include <QCloseEvent>

namespace Ui {
class progress;
}

class progress : public QMainWindow
{
  Q_OBJECT

public:
  explicit progress(int argc, char** argv, QWidget *parent = 0);
   ~progress();
   bool done;
Q_SIGNALS:
  void windowClosed();
public Q_SLOTS:
  void on_pauseButton_clicked();
  void on_stopButton_clicked();
private:
  Ui::progress *ui;
  void closeEvent(QCloseEvent *event);
};

#endif // PROGRESS_H
