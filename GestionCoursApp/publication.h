// ************************************FOR HAJAR****************************************************
#ifndef PUBLICATION_H
#define PUBLICATION_H

#include <QString>
#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>

// Enumération pour les types de publication
enum class PubType {
    COURS,
    TD_TP,
    ANNONCE
};

// --- Classe de base ---
class Publication {
public:
    Publication();
    virtual ~Publication();

    // Setters
    void setId(int id);
    void setTitre(const QString &titre);
    void setContenu(const QString &contenu);
    void setDateCreation(const QDateTime &date);
    void setModuleId(int moduleId);

    // Getters
    int getId() const;
    QString getTitre() const;
    QString getContenu() const;
    QDateTime getDateCreation() const;
    int getModuleId() const;

    // Méthode virtuelle pure pour récupérer le type sous forme de string (pour la BDD)
    virtual QString getTypeString() const = 0;
    virtual PubType getType() const = 0;

    // Opérations BDD
    bool publier(); // Sauvegarder dans la BDD
    static bool supprimer(int idPub);

protected:
    int m_id;
    QString m_titre;
    QString m_contenu;
    QDateTime m_dateCreation;
    int m_moduleId;
};

// --- Classe Cours ---
class CoursePub : public Publication {
public:
    CoursePub();
    void setFilePath(const QString &path); // Pour stocker le chemin du PDF
    QString getFilePath() const;

    QString getTypeString() const override { return "COURS"; }
    PubType getType() const override { return PubType::COURS; }

private:
    QString m_filePath;
};

// --- Classe TD/TP ---
class TdTpPub : public Publication {
public:
    TdTpPub();

    QString getTypeString() const override { return "TD_TP"; }
    PubType getType() const override { return PubType::TD_TP; }
};

// --- Classe Annonce ---
class AnnouncementPub : public Publication {
public:
    AnnouncementPub();

    QString getTypeString() const override { return "ANNONCE"; }
    PubType getType() const override { return PubType::ANNONCE; }
};

#endif // PUBLICATION_H
