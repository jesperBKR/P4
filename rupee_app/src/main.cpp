/**
 * @file /src/main.cpp
 *
 * @brief Qt based gui.
 *
 * @date November 2010
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QApplication>
#include "../include/testing/newstuff.hpp"
#include "../include/testing/progress.hpp"
#include <QDesktopWidget>


/*****************************************************************************
** Main
*****************************************************************************/

int main(int argc, char **argv) {

    /*********************
    ** Qt
    **********************/
    QApplication app(argc, argv);
  //  Worker w;
    QDesktopWidget dw;
    int x=dw.width()*0.5;
    int y=dw.height()*0.5;
    Rupee rp(argc,argv);
    progress p(argc, argv);
    rp.setFixedSize(x,y);
    p.setFixedSize(x,y);
    rp.show();
    app.setQuitOnLastWindowClosed(false);
      app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    int result = app.exec();

	return result;
}
