#include "classmodule.h"
#include "dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// Constructeur par défaut
ClassModule::ClassModule() : m_id(-1), m_profId(-1) {}

// Constructeur avec paramètres
ClassModule::ClassModule(int id, QString nom, QString description, int profId, QString code)
    : m_id(id), m_nom(nom), m_description(description), m_codeAcces(code), m_profId(profId)
{
}

// --- Setters ---
void ClassModule::setNom(const QString &nom) { m_nom = nom; }
void ClassModule::setDescription(const QString &desc) { m_description = desc; }
void ClassModule::setProfId(int id) { m_profId = id; }

// --- Getters ---
int ClassModule::getId() const { return m_id; }
QString ClassModule::getNom() const { return m_nom; }
QString ClassModule::getDescription() const { return m_description; }
QString ClassModule::getCodeAcces() const { return m_codeAcces; }
int ClassModule::getProfId() const { return m_profId; }

// --- Fonctionnalités Clés ---

// 1. Génère un code unique aléatoire (ex: "X9B2P")
void ClassModule::generateUniqueCode() {
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    const int randomStringLength = 5; // Longueur du code

    QString randomString;
    // On boucle pour générer chaque caractère
    for(int i = 0; i < randomStringLength; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        randomString.append(possibleCharacters.at(index));
    }
    
    // Note: Idéalement, on devrait vérifier dans la BDD si ce code existe déjà
    // pour éviter les collisions, mais pour ce projet scolaire, c'est suffisant.
    m_codeAcces = randomString;
}

// 2. Sauvegarde la nouvelle classe dans PostgreSQL
bool ClassModule::createInDb() {
    // On récupère la connexion à la BDD (suppose que DbManager a ouvert la connexion par défaut)
    QSqlDatabase db = QSqlDatabase::database(); 
    
    if (!db.isOpen()) {
        qDebug() << "Erreur: Base de données non connectée.";
        return false;
    }

    // Préparation de la requête INSERT
    QSqlQuery query;
    // On utilise prepare() pour la sécurité (protection contre injections SQL)
    query.prepare("INSERT INTO modules (nom, description, code_acces, id_prof) "
                  "VALUES (:nom, :desc, :code, :prof)");
    
    // On lie les valeurs (bindValue)
    query.bindValue(":nom", m_nom);
    query.bindValue(":desc", m_description);
    query.bindValue(":code", m_codeAcces);
    query.bindValue(":prof", m_profId);

    // Exécution
    if (query.exec()) {
        // Si succès, on récupère l'ID généré automatiquement (SERIAL)
        m_id = query.lastInsertId().toInt();
        qDebug() << "Module créé avec succès, ID:" << m_id;
        return true;
    } else {
        qDebug() << "Erreur création module:" << query.lastError().text();
        return false;
    }
}

// 3. Récupère les publications
QList<Publication*> ClassModule::getPublications(PubType filterType, bool filterActive) {
    QList<Publication*> list;
    QSqlQuery query;
    
    QString sql = "SELECT id, titre, contenu, date_creation, type, file_path FROM publications WHERE id_module = :moduleId";
    
    // Ajout du filtre si actif
    if (filterActive) {
        QString typeStr;
        switch (filterType) {
            case PubType::COURS: typeStr = "COURS"; break;
            case PubType::TD_TP: typeStr = "TD_TP"; break;
            case PubType::ANNONCE: typeStr = "ANNONCE"; break;
        }
        sql += " AND type = :type";
        query.prepare(sql);
        query.bindValue(":type", typeStr);
    } else {
        query.prepare(sql);
    }
    
    query.bindValue(":moduleId", m_id);
    
    if (query.exec()) {
        while (query.next()) {
            int id = query.value("id").toInt();
            QString titre = query.value("titre").toString();
            QString contenu = query.value("contenu").toString();
            QDateTime date = QDateTime::fromString(query.value("date_creation").toString(), Qt::ISODate);
            QString type = query.value("type").toString();
            QString filePath = query.value("file_path").toString();
            
            Publication* pub = nullptr;
            
            if (type == "COURS") {
                CoursePub* p = new CoursePub();
                p->setFilePath(filePath);
                pub = p;
            } else if (type == "TD_TP") {
                pub = new TdTpPub();
            } else if (type == "ANNONCE") {
                pub = new AnnouncementPub();
            }
            
            if (pub) {
                pub->setId(id);
                pub->setTitre(titre);
                pub->setContenu(contenu);
                pub->setDateCreation(date);
                pub->setModuleId(m_id);
                list.append(pub);
            }
        }
    } else {
        qDebug() << "Erreur chargement publications:" << query.lastError().text();
    }
    
    return list;
}

// Méthode statique pour charger tous les modules d'un prof
QList<ClassModule> ClassModule::getModulesByProf(int profId) {
    QList<ClassModule> modules;
    QSqlQuery query;
    
    // Sélectionne tous les modules créés par ce prof
    query.prepare("SELECT id, nom, description, code_acces FROM modules WHERE id_prof = :profId");
    query.bindValue(":profId", profId);
    
    if (query.exec()) {
        while (query.next()) {
            // On reconstruit l'objet ClassModule
            int id = query.value("id").toInt();
            QString nom = query.value("nom").toString();
            QString desc = query.value("description").toString();
            QString code = query.value("code_acces").toString();
            
            ClassModule module(id, nom, desc, profId, code);
            modules.append(module);
        }
    } else {
        qDebug() << "Erreur chargement modules prof:" << query.lastError().text();
    }
    
    return modules;
}

// Méthode statique pour charger tous les modules d'un étudiant
QList<ClassModule> ClassModule::getModulesByStudent(int studentId) {
    QList<ClassModule> modules;
    QSqlQuery query;
    
    // Jointure pour trouver les modules où l'étudiant est inscrit
    // Tables: modules (m), inscriptions (i)
    query.prepare("SELECT m.id, m.nom, m.description, m.code_acces, m.id_prof "
                  "FROM modules m "
                  "JOIN inscriptions i ON m.id = i.id_module "
                  "WHERE i.id_etudiant = :studentId");
    
    query.bindValue(":studentId", studentId);
    
    if (query.exec()) {
        while (query.next()) {
            int id = query.value("id").toInt();
            QString nom = query.value("nom").toString();
            QString desc = query.value("description").toString();
            QString code = query.value("code_acces").toString();
            int profId = query.value("id_prof").toInt();
            
            ClassModule module(id, nom, desc, profId, code);
            modules.append(module);
        }
    } else {
        qDebug() << "Erreur chargement modules étudiant:" << query.lastError().text();
    }
    
    return modules;
}

// 4. Inscription d'un étudiant via le code
bool ClassModule::joinModule(int studentId, const QString &code) {
    QSqlQuery query;
    
    // Étape 1 : Trouver l'ID du module correspondant au code
    query.prepare("SELECT id FROM modules WHERE code_acces = :code");
    query.bindValue(":code", code);
    
    if (!query.exec()) {
        qDebug() << "Erreur recherche module:" << query.lastError().text();
        return false;
    }
    
    if (query.next()) {
        int moduleId = query.value("id").toInt();
        
        // Étape 2 : Inscrire l'étudiant (INSERT dans inscriptions)
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO inscriptions (id_etudiant, id_module) VALUES (:student, :module)");
        insertQuery.bindValue(":student", studentId);
        insertQuery.bindValue(":module", moduleId);
        
        if (insertQuery.exec()) {
            qDebug() << "Inscription réussie pour l'étudiant" << studentId << "au module" << moduleId;
            return true;
        } else {
            // Peut échouer si déjà inscrit (clé primaire composite)
            qDebug() << "Erreur inscription (peut-être déjà inscrit):" << insertQuery.lastError().text();
            return false;
        }
    } else {
        qDebug() << "Aucun module trouvé avec le code:" << code;
        return false;
    }
}

// 5. Supprimer un module par ID
bool ClassModule::deleteModule(int id) {
    QSqlQuery query;
    // Note: Si la base de données a des contraintes ON DELETE CASCADE bloquées, 
    // il faudrait d'abord supprimer les publications/inscriptions.
    // On suppose ici que la FK est configurée en CASCADE ou qu'on supprime bêtement.
    
    // On nettoie d'abord les dépendances manuelles si besoin (exemple simple)
    QSqlQuery qDep;
    qDep.prepare("DELETE FROM publications WHERE id_module = :id");
    qDep.bindValue(":id", id);
    qDep.exec(); // On tente le nettoyage
    
    qDep.prepare("DELETE FROM inscriptions WHERE id_module = :id");
    qDep.bindValue(":id", id);
    qDep.exec();

    // Suppression du module
    query.prepare("DELETE FROM modules WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Erreur suppression module:" << query.lastError().text();
        return false;
    }
}
