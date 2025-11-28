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
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("localhost");
    m_db.setDatabaseName("gestion_cours_db"); // Le nom exact de votre base
    m_db.setUserName("postgres");              // Votre login Postgres (souvent 'postgres')
    m_db.setPassword("votre_mot_de_passe");    // <--- REMPLACEZ CECI PAR VOTRE VRAI MDP
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
