#include "dbmanager.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

// Initialisation du Singleton (L'instance unique)
// Cela permet d'accéder à la BDD de partout sans la passer en paramètre
DbManager& DbManager::instance()
{
    static DbManager instance;
    return instance;
}

// Constructeur : C'est ici qu'on configure les paramètres
DbManager::DbManager()
{
    qDebug() << "--- INITIALISATION DBMANAGER (SQLite) ---";

    // 0. Vérification des pilotes disponibles
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        qDebug() << "❌ ERREUR FATALE : Le pilote QSQLITE est introuvable !";
        qDebug() << "Pilotes disponibles :" << QSqlDatabase::drivers();
        // On ne retourne pas ici car c'est un constructeur, mais l'erreur sera visible.
    }

    // 1. Configuration pour SQLite
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("gestioncours.db"); // Fichier local

    // 2. Connexion
    if (!m_db.open()) {
        qDebug() << "❌ ECHEC CONNEXION :" << m_db.lastError().text();
    } else {
        qDebug() << "✅ Connexion SQLite réussie !";
        
        // 3. Création des tables si nécessaire
        if (!createTables()) {
             qDebug() << "❌ Erreur lors de la création des tables.";
        }
    }
}

// Destructeur : On ferme proprement
DbManager::~DbManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DbManager::connectToDb()
{
    if (m_db.isOpen()) {
        return true;
    }

    if (m_db.open()) {
        qDebug() << "Connexion reussie a la base de donnees !";
        return true;
    } else {
        qDebug() << "Erreur de connexion BDD :" << m_db.lastError().text();
        return false;
    }
}

bool DbManager::isOpen() const
{
    return m_db.isOpen();
}

void DbManager::closeDb()
{
    m_db.close();
}

bool DbManager::createTables()
{
    QSqlQuery query;
    
    // 1. Table Utilisateurs
    // On stocke le rôle en TEXT ('PROF' ou 'ETUDIANT')
    QString createUsers = "CREATE TABLE IF NOT EXISTS utilisateurs ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "nom TEXT NOT NULL, "
                          "prenom TEXT NOT NULL, "
                          "email TEXT NOT NULL UNIQUE, "
                          "password TEXT NOT NULL, "
                          "role TEXT NOT NULL"
                          ");";
                          
    if (!query.exec(createUsers)) {
        qDebug() << "Erreur création table utilisateurs:" << query.lastError().text();
        return false;
    }

    // 2. Table Modules
    QString createModules = "CREATE TABLE IF NOT EXISTS modules ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "nom TEXT NOT NULL, "
                            "description TEXT, "
                            "code_acces TEXT NOT NULL UNIQUE, "
                            "id_prof INTEGER NOT NULL, "
                            "FOREIGN KEY(id_prof) REFERENCES utilisateurs(id)"
                            ");";

    if (!query.exec(createModules)) {
        qDebug() << "Erreur création table modules:" << query.lastError().text();
        return false;
    }

    // 3. Table Inscriptions (Liaison Etudiant <-> Module)
    QString createInscriptions = "CREATE TABLE IF NOT EXISTS inscriptions ("
                                 "id_etudiant INTEGER NOT NULL, "
                                 "id_module INTEGER NOT NULL, "
                                 "date_inscription TEXT, "
                                 "PRIMARY KEY (id_etudiant, id_module), "
                                 "FOREIGN KEY(id_etudiant) REFERENCES utilisateurs(id), "
                                 "FOREIGN KEY(id_module) REFERENCES modules(id)"
                                 ");";

    if (!query.exec(createInscriptions)) {
        qDebug() << "Erreur création table inscriptions:" << query.lastError().text();
        return false;
    }

    // 4. Table Publications
    QString createPublications = "CREATE TABLE IF NOT EXISTS publications ("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "titre TEXT NOT NULL, "
                                 "contenu TEXT, "
                                 "date_creation TEXT, "
                                 "id_module INTEGER NOT NULL, "
                                 "type TEXT NOT NULL, "
                                 "file_path TEXT, "
                                 "FOREIGN KEY(id_module) REFERENCES modules(id)"
                                 ");";

    if (!query.exec(createPublications)) {
        qDebug() << "Erreur création table publications:" << query.lastError().text();
        return false;
    }

    qDebug() << "✅ Tables vérifiées/créées avec succès.";
    return true;
}
