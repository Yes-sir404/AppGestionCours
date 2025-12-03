#include "mainwindow.h"
#include "ui_mainwindow.h" // Généré depuis le XML ci-dessus
#include "signupdialog.h"  // Pour ouvrir l'inscription
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QStackedWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentUser(nullptr)
{
    ui->setupUi(this);

    // Au démarrage, on affiche la page 0 (Login)
    ui->stackedWidget->setCurrentIndex(0);

    // On connecte les boutons de déconnexion au même slot
    connect(ui->btnLogoutProf, &QPushButton::clicked, this, &MainWindow::on_btnLogout_clicked);
    connect(ui->btnLogoutStudent, &QPushButton::clicked, this, &MainWindow::on_btnLogout_clicked);
}

MainWindow::~MainWindow()
{
    if (m_currentUser) {
        delete m_currentUser;
    }
    delete ui;
}

// --- LOGIQUE DE CONNEXION ---
void MainWindow::on_btnLogin_clicked()
{
    QString email = ui->lineEditEmail->text();
    QString pass = ui->lineEditPassword->text();

    if (email.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    // Appel à la fonction statique qu'on a codée dans User.cpp
    User* loggedInUser = User::login(email, pass);

    if (loggedInUser) {
        // Connexion Réussie !
        m_currentUser = loggedInUser;

        // Vérification du Rôle pour l'aiguillage
        if (m_currentUser->getRole() == UserRole::PROFESSEUR) {
            // Page 1 : Prof
            ui->stackedWidget->setCurrentIndex(1);
            ui->lblWelcomeProf->setText("Bienvenue Professeur " + m_currentUser->getNom());
        }
        else {
            // Page 2 : Étudiant
            ui->stackedWidget->setCurrentIndex(2);
            ui->lblWelcomeStudent->setText("Bienvenue Étudiant " + m_currentUser->getNom());
        }

        // On vide les champs de mot de passe par sécurité
        ui->lineEditPassword->clear();

    } else {
        QMessageBox::critical(this, "Erreur", "Email ou mot de passe incorrect.");
    }
}

// --- OUVERTURE INSCRIPTION ---
void MainWindow::on_btnGoToSignup_clicked()
{
    // On crée la fenêtre d'inscription
    SignupDialog signupDialog(this);

    // On l'ouvre en mode "Modal" (bloque la fenêtre principale)
    if (signupDialog.exec() == QDialog::Accepted) {
        // Si l'inscription a réussi, on peut pré-remplir l'email
        // (Optionnel, à adapter selon ce que vous voulez faire)
    }
}

// --- DECONNEXION ---
void MainWindow::on_btnLogout_clicked()
{
    // 1. On nettoie la mémoire
    if (m_currentUser) {
        delete m_currentUser;
        m_currentUser = nullptr;
    }

    // 2. On retourne à la page de login
    resetLoginForm();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::resetLoginForm()
{
    ui->lineEditEmail->clear();
    ui->lineEditPassword->clear();
}
