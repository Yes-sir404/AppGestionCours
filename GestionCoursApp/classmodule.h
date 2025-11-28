#ifndef CLASSMODULE_H
#define CLASSMODULE_H

#include <QString>
#include <QList>
#include <QSqlQuery>
#include <QVariant>
#include <QRandomGenerator>
#include "publication.h"

class ClassModule {
public:
    ClassModule();
    ClassModule(int id, QString nom, QString description, int profId, QString code = "");

    // Setters
    void setNom(const QString &nom);
    void setDescription(const QString &desc);
    void setProfId(int id);

    // Getters
    int getId() const;
    QString getNom() const;
    QString getDescription() const;
    QString getCodeAcces() const;
    int getProfId() const;

    // --- Fonctionnalités Clés ---

    // 1. Génère un code unique aléatoire (ex: "X9B2P")
    void generateUniqueCode();

    // 2. Sauvegarde la nouvelle classe dans PostgreSQL
    bool createInDb();

    // 3. Récupère les publications avec filtre optionnel
    // Si filterType est PubType::ANNONCE, ne retourne que les annonces, etc.
    // Passer un pointeur nul ou une enum "ALL" si on veut tout.
    QList<Publication*> getPublications(PubType filterType, bool filterActive = false);

    // Méthode statique pour charger tous les modules d'un prof
    static QList<ClassModule> getModulesByProf(int profId);

    // Méthode statique pour charger tous les modules d'un étudiant
    static QList<ClassModule> getModulesByStudent(int studentId);

private:
    int m_id;
    QString m_nom;
    QString m_description;
    QString m_codeAcces;
    int m_profId;
};

#endif // CLASSMODULE_H
