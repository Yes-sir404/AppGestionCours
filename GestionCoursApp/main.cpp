#include "mainwindow.h"
#include "signupdialog.h"
#include "dbmanager.h"
#include "stylehelper.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Appliquer le style global
    a.setStyleSheet(StyleHelper::getStyle());

    // Initialisation de la base de données au démarrage
    if (!DbManager::instance().isOpen()) {
        qDebug() << "⚠️ Attention: La base de données n'est pas ouverte au démarrage.";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
