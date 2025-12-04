#include "publication.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

// --- Publication ---
Publication::Publication() : m_id(0), m_moduleId(0) {}
Publication::~Publication() {}

void Publication::setId(int id) { m_id = id; }
void Publication::setTitre(const QString &titre) { m_titre = titre; }
void Publication::setContenu(const QString &contenu) { m_contenu = contenu; }
void Publication::setDateCreation(const QDateTime &date) { m_dateCreation = date; }
void Publication::setModuleId(int moduleId) { m_moduleId = moduleId; }

int Publication::getId() const { return m_id; }
QString Publication::getTitre() const { return m_titre; }
QString Publication::getContenu() const { return m_contenu; }
QDateTime Publication::getDateCreation() const { return m_dateCreation; }
int Publication::getModuleId() const { return m_moduleId; }

bool Publication::publier() {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO publications (titre, contenu, date_creation, id_module, type, file_path) "
                  "VALUES (:titre, :contenu, :date, :module, :type, :file)");
    
    query.bindValue(":titre", m_titre);
    query.bindValue(":contenu", m_contenu);
    query.bindValue(":date", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":module", m_moduleId);
    query.bindValue(":type", getTypeString());
    
    QString filePath = "";
    if (getType() == PubType::COURS) {
        // Cast sécurisé car on a vérifié le type
        const CoursePub* cp = dynamic_cast<const CoursePub*>(this);
        if (cp) filePath = cp->getFilePath();
    }
    query.bindValue(":file", filePath);

    if (query.exec()) {
        m_id = query.lastInsertId().toInt();
        return true;
    } else {
        qDebug() << "Erreur publication:" << query.lastError().text();
        return false;
    }
}

bool Publication::supprimer(int idPub) {
    // TODO: Implémenter la suppression
    return false;
}

// --- CoursePub ---
CoursePub::CoursePub() {}
void CoursePub::setFilePath(const QString &path) { m_filePath = path; }
QString CoursePub::getFilePath() const { return m_filePath; }

// --- TdTpPub ---
TdTpPub::TdTpPub() {}

// --- AnnouncementPub ---
AnnouncementPub::AnnouncementPub() {}
