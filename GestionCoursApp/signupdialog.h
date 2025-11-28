#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>

// Déclaration anticipée de la classe générée par l'interface UI
namespace Ui {
class SignupDialog;
}

class SignupDialog : public QDialog
{
    Q_OBJECT // Macro indispensable pour les signaux/slots

public:
    explicit SignupDialog(QWidget *parent = nullptr);
    ~SignupDialog();

private slots:
    // Vos fonctions de boutons
    void on_btnInscrire_clicked();
    void on_btnAnnuler_clicked();

private:
    Ui::SignupDialog *ui; // Le pointeur vers l'interface graphique
};

#endif // SIGNUPDIALOG_H
