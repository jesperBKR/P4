#ifndef Q_MOC_RUN
#endif
#ifndef PROGRESS_H
#define PROGRESS_H

#include <QMainWindow>

namespace Ui {
class progress;
}

class progress : public QMainWindow
{
  Q_OBJECT

public:
  explicit progress(int argc, char** argv, QWidget *parent = 0);
   ~progress();

private:
  Ui::progress *ui;
};

#endif // PROGRESS_H
