#include "mainwindow.h"
#include "signupdialog.h""

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SignupDialog w;
    w.show();
    return a.exec();
}
