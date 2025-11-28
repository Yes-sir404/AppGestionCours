#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

class DbManager {
public:
    // Méthode statique pour accéder à l'instance unique (Singleton)
    static DbManager& instance();

    // Connexion à la base de données
    bool connectToDb();

    // Fermeture propre
    void closeDb();

    // Vérifier si la connexion est ouverte
    bool isOpen() const;

private:
    // Constructeur privé pour le Singleton
    DbManager();
    ~DbManager();

    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
