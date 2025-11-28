#include "signupdialog.h"
#include "ui_signupdialog.h"
#include "user.h"       // On inclut notre classe User
#include <QMessageBox>  // Pour les messages d'erreur/succès
#include <QLineEdit>

SignupDialog::SignupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignupDialog)
{
    ui->setupUi(this);

    // Astuce : Masquer les caractères du mot de passe
    ui->lineEditPass->setEchoMode(QLineEdit::Password);

    // Cocher Étudiant par défaut
    ui->radioEtudiant->setChecked(true);
}

SignupDialog::~SignupDialog()
{
    delete ui;
}

// =======================================================
// FONCTION DU BOUTON : S'INSCRIRE
// =======================================================
void SignupDialog::on_btnInscrire_clicked()
{
    // 1. Récupération des données saisies
    QString nom = ui->lineEditNom->text();
    QString prenom = ui->lineEditPrenom->text();
    QString email = ui->lineEditEmail->text();
    QString password = ui->lineEditPass->text();

    // 2. Vérification : Champs vides ?
    if(nom.isEmpty() || prenom.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return; // On arrête la fonction ici
    }

    // 3. Vérification : Email déjà pris ?
    if(User::emailExiste(email)) {
        QMessageBox::warning(this, "Erreur", "Cet email est déjà utilisé.");
        return;
    }

    // 4. Détermination du Rôle (LA PARTIE IMPORTANTE)
    UserRole roleSelectionne;

    if (ui->radioProf->isChecked()) {
        roleSelectionne = UserRole::PROFESSEUR;
    } else {
        roleSelectionne = UserRole::ETUDIANT; // Par défaut
    }

    // 5. Création de l'objet User temporaire pour l'inscription
    // (On met 0 pour l'ID car la base de données le générera)
    User nouvelUtilisateur(0, nom, prenom, email, password, roleSelectionne);

    // 6. Sauvegarde dans la Base de Données
    if(nouvelUtilisateur.signup()) {
        // SUCCÈS
        QMessageBox::information(this, "Succès", "Compte créé avec succès !");
        this->accept(); // Ferme la fenêtre et retourne "Accepté"
    } else {
        // ÉCHEC TECHNIQUE (Erreur SQL)
        QMessageBox::critical(this, "Erreur", "Impossible de créer le compte. Vérifiez la base de données.");
    }
}

// =======================================================
// FONCTION DU BOUTON : ANNULER
// =======================================================
void SignupDialog::on_btnAnnuler_clicked()
{
    this->reject(); // Ferme la fenêtre simplement
}
