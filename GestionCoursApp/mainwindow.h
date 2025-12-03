#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "user.h" // Important pour connaître l'objet User

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slot pour le bouton Login
    void on_btnLogin_clicked();

    // Slot pour ouvrir la fenêtre d'inscription
    void on_btnGoToSignup_clicked();

    // Slot commun pour la déconnexion (Prof et Étudiant)
    void on_btnLogout_clicked();

private:
    Ui::MainWindow *ui;

    // Pointeur vers l'utilisateur actuellement connecté
    User* m_currentUser;

    // Fonction utilitaire pour remettre l'écran à zéro
    void resetLoginForm();
};

#endif // MAINWINDOW_H
