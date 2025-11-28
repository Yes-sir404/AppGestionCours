#include "user.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ==========================================
// 1. CLASSE USER
// ==========================================

// Constructeurs
User::User() : m_id(0), m_role(ETUDIANT) {}

User::User(int id, QString nom, QString prenom, QString email, QString password, UserRole role)
    : m_id(id), m_nom(nom), m_prenom(prenom), m_email(email), m_password(password), m_role(role) {}

User::~User() {}

// Getters
int User::getId() const { return m_id; }
QString User::getNom() const { return m_nom; }
QString User::getPrenom() const { return m_prenom; }
QString User::getEmail() const { return m_email; }
QString User::getPassword() const { return m_password; }
UserRole User::getRole() const { return m_role; }
QString User::getNomComplet() const { return m_nom + " " + m_prenom; }

// Setters
void User::setId(int id) { m_id = id; }
void User::setNom(const QString &nom) { m_nom = nom; }
void User::setPrenom(const QString &prenom) { m_prenom = prenom; }
void User::setEmail(const QString &email) { m_email = email; }
void User::setPassword(const QString &password) { m_password = password; }


// --- [LOGIN] : Authentification ---
User* User::login(const QString &email, const QString &password) {
    QSqlQuery query;
    // On prépare la requête (Sécurité anti-injection SQL)
    query.prepare("SELECT id, nom, prenom, role FROM utilisateurs WHERE email = :email AND password = :pass");
    query.bindValue(":email", email);
    query.bindValue(":pass", password); // Dans un vrai projet, le mdp serait hashé !

    if (query.exec()) {
        if (query.next()) {
            // L'utilisateur existe ! On récupère ses infos
            int id = query.value("id").toInt();
            QString nom = query.value("nom").toString();
            QString prenom = query.value("prenom").toString();
            QString roleStr = query.value("role").toString();

            // On crée le bon objet selon le rôle (Polymorphisme)
            if (roleStr == "PROF") {
                return new Prof(id, nom, prenom, email, password);
            } else {
                return new Etudiant(id, nom, prenom, email, password);
            }
        }
    } else {
        qDebug() << "Erreur Login SQL:" << query.lastError().text();
    }

    return nullptr; // Échec de connexion
}

// --- [SIGNUP] : Inscription ---
bool User::signup() {
    QSqlQuery query;
    query.prepare("INSERT INTO utilisateurs (nom, prenom, email, password, role) "
                  "VALUES (:nom, :prenom, :email, :pass, :role)");

    query.bindValue(":nom", m_nom);
    query.bindValue(":prenom", m_prenom);
    query.bindValue(":email", m_email);
    query.bindValue(":pass", m_password);

    // Conversion Enum -> String pour la BDD
    QString roleStr = (m_role == PROFESSEUR) ? "PROF" : "ETUDIANT";
    query.bindValue(":role", roleStr);

    if (query.exec()) {
        // On récupère l'ID généré automatiquement par PostgreSQL
        m_id = query.lastInsertId().toInt();
        return true;
    } else {
        qDebug() << "Erreur Signup SQL:" << query.lastError().text();
        return false;
    }
}

// --- [VERIFICATION] : Email existe ? ---
bool User::emailExiste(const QString &email) {
    QSqlQuery query;
    query.prepare("SELECT id FROM utilisateurs WHERE email = :email");
    query.bindValue(":email", email);

    if (query.exec() && query.next()) {
        return true; // Trouvé
    }
    return false;
}

// ==========================================
// 2. CLASSE PROF
// ==========================================
Prof::Prof(int id, QString nom, QString prenom, QString email, QString password)
    : User(id, nom, prenom, email, password, PROFESSEUR) {}


// ==========================================
// 3. CLASSE ETUDIANT
// ==========================================
Etudiant::Etudiant(int id, QString nom, QString prenom, QString email, QString password)
    : User(id, nom, prenom, email, password, ETUDIANT) {}

// --- [REJOINDRE CLASSE] : Code Unique ---
bool Etudiant::rejoindreClasse(const QString &codeUnique) {
    QSqlQuery query;

    // Étape 1 : Trouver l'ID du module grâce au code
    query.prepare("SELECT id FROM modules WHERE code_acces = :code");
    query.bindValue(":code", codeUnique);

    if (!query.exec()) {
        qDebug() << "Erreur recherche module:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        int idModule = query.value("id").toInt();

        // Étape 2 : Inscrire l'étudiant dans la table de liaison
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO inscriptions (id_etudiant, id_module, date_inscription) "
                            "VALUES (:idEtud, :idMod, CURRENT_DATE)");

        insertQuery.bindValue(":idEtud", this->m_id); // 'this->m_id' est l'ID de l'étudiant connecté
        insertQuery.bindValue(":idMod", idModule);

        if (insertQuery.exec()) {
            return true; // Succès !
        } else {
            qDebug() << "Erreur inscription:" << insertQuery.lastError().text();
            // Peut échouer si l'étudiant est DÉJÀ inscrit (contrainte Primary Key)
            return false;
        }
    }

    return false; // Code incorrect (module introuvable)
}
