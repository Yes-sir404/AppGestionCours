#include "mainwindow.h"
#include "ui_mainwindow.h" // Généré depuis le XML ci-dessus
#include "signupdialog.h"  // Pour ouvrir l'inscription
#include "styles.h"
#include "coursedashboard.h" // NOUVEAU
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QStackedWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QComboBox>
#include <QFileDialog>
#include <QTextEdit>
#include <QFileInfo>
#include <QFile>
#include <QClipboard> // Ajout pour le presse-papier
#include <QApplication>
#include <QPixmap>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentUser(nullptr)
{
    ui->setupUi(this);

    // --- AJOUT DU LOGO ---
    QLabel* logoLabel = new QLabel(this);
    // On charge depuis les ressources
    QPixmap logoPixmap(":/logo/EduLink_Logo.svg"); 
    if (!logoPixmap.isNull()) {
        // Redimensionnement propre
        logoLabel->setPixmap(logoPixmap.scaled(250, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setAlignment(Qt::AlignCenter);
        
        // On l'ajoute au layout de la page de login (pageLogin -> verticalLayout_2)
        // On l'insère à l'index 1 (après le spacer du haut) pour qu'il soit au dessus du titre
        ui->verticalLayout_2->insertWidget(1, logoLabel);
    } else {
        qDebug() << "Erreur: Impossible de charger le logo :/logo/EduLink_Logo.svg";
    }
    // ---------------------

    // AJOUTEZ CETTE LIGNE ↓
    loadStyleSheet();
    
    // Configuration de la fenêtre
    setMinimumSize(900, 650);  // ← AJOUTEZ AUSSI CETTE LIGNE
    
    // Au démarrage, on affiche la page 0 (Login)
    ui->stackedWidget->setCurrentIndex(0);

    // On efface le style manuel du bouton Login pour laisser le CSS global agir
    ui->btnLogin->setStyleSheet("");

    // On connecte les boutons de déconnexion au même slot
    connect(ui->btnLogoutProf, &QPushButton::clicked, this, &MainWindow::logoutUser);
    connect(ui->btnLogoutStudent, &QPushButton::clicked, this, &MainWindow::logoutUser);

    // Initialisation des pointeurs et variables
    m_listPublications = nullptr;
    m_currentFilterType = PubType::ANNONCE; // Valeur par défaut
    m_filterActive = false;
    
    m_listProfCourses = nullptr; // Important pour le check d'init
    m_comboCourses = nullptr;
    m_leCodeClass = nullptr;
    m_leCourseName = nullptr;
    m_leCourseDesc = nullptr;
}

MainWindow::~MainWindow()
{
    if (m_currentUser) {
        delete m_currentUser;
    }
    delete ui;
}
//Design
void MainWindow::loadStyleSheet()
{
    this->setStyleSheet(Styles::getGlobalStyleSheet());
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

    // --- VALIDATION AJOUTÉE ---
    if (!User::isValidEmailStructure(email)) {
        QMessageBox::warning(this, "Erreur", "L'email doit finir par @edulink.prof.ma ou @edulink.etud.ma");
        return;
    }

    // Optionnel : on peut aussi pré-valider le mot de passe pour éviter des requêtes inutiles
    if (!User::isValidPasswordStructure(pass)) {
        QMessageBox::warning(this, "Erreur", "Le format du mot de passe est invalide (min 6 caractères).");
        return;
    }
    // -------------------------

    // Appel à la fonction statique qu'on a codée dans User.cpp
    qDebug() << "Tentative de connexion pour :" << email;
    User* loggedInUser = User::login(email, pass);

    if (loggedInUser) {
        qDebug() << "Connexion réussie. Rôle :" << loggedInUser->getRole();
        // Connexion Réussie !
        m_currentUser = loggedInUser;

        // Vérification du Rôle pour l'aiguillage
        if (m_currentUser->getRole() == UserRole::PROFESSEUR) {
            // Page 1 : Prof
            qDebug() << "Setup Prof UI...";
            ui->stackedWidget->setCurrentIndex(1);
            setupProfUI();
            qDebug() << "Setup Prof UI OK";
        }
        else {
            // Page 2 : Étudiant
            qDebug() << "Setup Student UI...";
            ui->stackedWidget->setCurrentIndex(2);
            setupStudentUI(); // Construction dynamique de l'interface
            qDebug() << "Setup Student UI OK";
        }

        // On vide les champs de mot de passe par sécurité
        ui->lineEditPassword->clear();

    } else {
        qDebug() << "Échec connexion";
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
// --- DECONNEXION ---
void MainWindow::logoutUser()
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

// ==========================================
// INTERFACE ETUDIANT
// ==========================================

void MainWindow::setupStudentUI()
{
    // Si l'interface est déjà construite, on recharge juste les données
    if (m_listPublications != nullptr) {
        loadStudentCourses();
        return;
    }

    // Récupération du layout existant (celui du .ui)
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->pageStudent->layout());
    
    if (mainLayout) {
        // IMPORTANT: On retire les widgets du .ui qu'on veut garder AVANT de vider le layout
        // Sinon delete item->widget() les supprimerait, causant un crash car 'ui' pointe dessus
        mainLayout->removeWidget(ui->lblWelcomeStudent);
        mainLayout->removeWidget(ui->btnLogoutStudent);
        
        // On vide le reste (spacers, etc.)
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    } else {
        mainLayout = new QVBoxLayout(ui->pageStudent);
    }

    // 1. En-tête (Bienvenue + Déconnexion)
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    // On réintègre les widgets du .ui
    ui->lblWelcomeStudent->setParent(ui->pageStudent); 
    // Mise à jour du texte de bienvenue
    ui->lblWelcomeStudent->setText(QString("Bienvenue, %1 %2").arg(m_currentUser->getPrenom(), m_currentUser->getNom()));
    headerLayout->addWidget(ui->lblWelcomeStudent);
    
    // Le bouton déconnexion du .ui
    ui->btnLogoutStudent->setParent(ui->pageStudent);
    headerLayout->addWidget(ui->btnLogoutStudent);
    
    mainLayout->addLayout(headerLayout);

    // 2. Rejoindre un cours
    QGroupBox* groupJoin = new QGroupBox("Rejoindre un cours");
    QHBoxLayout* joinLayout = new QHBoxLayout(groupJoin);
    
    m_leCodeClass = new QLineEdit();
    m_leCodeClass->setPlaceholderText("Entrez le code du cours (ex: X9B2P)");
    QPushButton* btnJoin = new QPushButton("Rejoindre");
    connect(btnJoin, &QPushButton::clicked, this, &MainWindow::handleJoinClass);
    
    joinLayout->addWidget(m_leCodeClass);
    joinLayout->addWidget(btnJoin);
    
    mainLayout->addWidget(groupJoin);

    // 3. ACCUEIL / FIL D'ACTUALITÉ
    QPushButton* btnDashboard = new QPushButton("Voir le Fil d'Actualité ➜");
    btnDashboard->setStyleSheet("background-color: #1a73e8; font-size: 16px; padding: 15px; margin-top: 20px; color: white;");
    connect(btnDashboard, &QPushButton::clicked, [this](){
        CourseDashboard* dash = new CourseDashboard(m_currentUser, this);
        dash->exec();
        delete dash;
    });
    mainLayout->addWidget(btnDashboard);

    // On retire l'ancienne liste ici pour ne pas faire doublon
    // m_listPublications = new QListWidget(); ... (supprimé)
        
    // Chargement initial
    loadStudentCourses();
}

void MainWindow::handleJoinClass()
{
    QString code = m_leCodeClass->text().trimmed();
    if (code.isEmpty()) return;

    if (ClassModule::joinModule(m_currentUser->getId(), code)) {
        QMessageBox::information(this, "Succès", "Vous avez rejoint le cours !");
        m_leCodeClass->clear();
        loadStudentCourses(); // Rafraîchir la liste
    } else {
        QMessageBox::warning(this, "Erreur", "Code invalide ou vous êtes déjà inscrit.");
    }
}

void MainWindow::loadStudentCourses()
{
    if (!m_comboCourses) return;
    m_comboCourses->clear();
    QList<ClassModule> modules = ClassModule::getModulesByStudent(m_currentUser->getId());
    
    for (const ClassModule& module : std::as_const(modules)) {
        m_comboCourses->addItem(module.getNom(), module.getId()); // On stocke l'ID en UserData
    }
    
    if (!modules.isEmpty()) {
        on_courseSelected(0); // Charger le premier
    } else {
        m_listPublications->clear();
    }
}

void MainWindow::on_courseSelected(int index)
{
    if (index < 0) return;
    int moduleId = m_comboCourses->itemData(index).toInt();
    
    // On reconstruit un objet temporaire juste pour appeler la méthode
    ClassModule currentModule(moduleId, "", "", 0); 
    
    m_listPublications->clear();
    QList<Publication*> pubs = currentModule.getPublications(m_currentFilterType, m_filterActive);
    
    for (Publication* pub : std::as_const(pubs)) {
        QString icon = "";
        bool canDownload = false;
        
        if (pub->getType() == PubType::COURS) {
            icon = "📘 [COURS] ";
            // On vérifie s'il y a un fichier associé (simple check sur le champ filePath)
            // Note: Idéalement on devrait vérifier ce champ dans la classe Publication
            CoursePub* cp = dynamic_cast<CoursePub*>(pub);
            if (cp) {
                 qDebug() << "Publication ID:" << pub->getId() << " Path:" << cp->getFilePath();
                 if (!cp->getFilePath().isEmpty()) {
                    canDownload = true;
                 }
            }
        }
        else if (pub->getType() == PubType::TD_TP) {
            icon = "📝 [TD/TP] ";
            // On pourrait activer le téléchargement pour TD/TP si on ajoute le champ
        }
        else {
            icon = "📢 [ANNONCE] ";
        }
        
        // Création de l'item (widget container)
        QListWidgetItem* item = new QListWidgetItem(m_listPublications);
        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->setContentsMargins(5, 5, 5, 5);
        
        QLabel* lblText = new QLabel(icon + pub->getTitre() + "\n" + pub->getContenu());
        lblText->setWordWrap(true);
        layout->addWidget(lblText, 1); // Stretch factor 1 pour prendre la place
        
        if (canDownload) {
            QPushButton* btnDl = new QPushButton("Télécharger");
            // On connecte le bouton directement à la méthode de téléchargement
            // On utilise une lambda qui capture l'ID
            int id = pub->getId();
            connect(btnDl, &QPushButton::clicked, [this, id]() {
                qDebug() << "Bouton télécharger cliqué pour l'ID:" << id;
                this->downloadPublication(id);
            });
            layout->addWidget(btnDl, 0); // Pas de stretch
        }
        
        widget->setLayout(layout);
        item->setSizeHint(widget->sizeHint()); // Important pour la taille
        
        m_listPublications->setItemWidget(item, widget);
        item->setData(Qt::UserRole, pub->getId()); // On garde l'ID au cas où
        
        // Nettoyage mémoire (car getPublications renvoie des new)
        delete pub; 
    }
}

// ==========================================
// INTERFACE PROFESSEUR
// ==========================================

void MainWindow::setupProfUI()
{
    // Si déjà initialisé (check simple sur un pointeur)
    if (m_listProfCourses != nullptr) {
        loadProfCourses();
        return;
    }

    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->pageProf->layout());
    if (!mainLayout) {
        if (ui->pageProf->layout()) delete ui->pageProf->layout();
        mainLayout = new QVBoxLayout(ui->pageProf);
    } else {
        // Nettoyage safe
        mainLayout->removeWidget(ui->lblWelcomeProf);
        mainLayout->removeWidget(ui->btnLogoutProf);
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
    }

    // 1. Header
    QHBoxLayout* headerLayout = new QHBoxLayout();
    ui->lblWelcomeProf->setParent(ui->pageProf);
    // Mise à jour du texte de bienvenue
    ui->lblWelcomeProf->setText(QString("Bienvenue, Pr. %1 %2").arg(m_currentUser->getPrenom(), m_currentUser->getNom()));
    headerLayout->addWidget(ui->lblWelcomeProf);
    ui->btnLogoutProf->setParent(ui->pageProf);
    headerLayout->addWidget(ui->btnLogoutProf);
    mainLayout->addLayout(headerLayout);

    // 2. Créer un Cours
    QGroupBox* groupCreate = new QGroupBox("Créer un nouveau cours");
    QFormLayout* formCreate = new QFormLayout(groupCreate);
    
    m_leCourseName = new QLineEdit();
    m_leCourseDesc = new QLineEdit(); // Ou QTextEdit si on veut plus long
    QPushButton* btnCreate = new QPushButton("Créer le cours");
    connect(btnCreate, &QPushButton::clicked, this, &MainWindow::on_btnCreateClass_clicked);
    
    formCreate->addRow("Nom du cours:", m_leCourseName);
    formCreate->addRow("Description:", m_leCourseDesc);
    formCreate->addRow(btnCreate);
    
    mainLayout->addWidget(groupCreate);

    // 3. Liste des cours créés (NOUVEAU)
    QGroupBox* groupList = new QGroupBox("Mes Cours Actifs");
    QVBoxLayout* layoutList = new QVBoxLayout(groupList);
    
    m_listProfCourses = new QListWidget();
    // SCROLL FIX: On laisse le widget s'étendre, mais on le bride en hauteur pour forcer le scroll
    m_listProfCourses->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listProfCourses->setMinimumHeight(200); 
    m_listProfCourses->setMaximumHeight(450); // Empêche de pousser les boutons hors écran
    layoutList->addWidget(m_listProfCourses);
    
    mainLayout->addWidget(groupList);

    // 4. GROS BOUTON "GÉRER MES COURS / FIL D'ACTUALITÉ"
    QPushButton* btnDashboard = new QPushButton("Gérer mes Cours & Publications ➜");
    btnDashboard->setStyleSheet("background-color: #28a745; font-size: 16px; padding: 15px; margin-top: 20px;");
    connect(btnDashboard, &QPushButton::clicked, [this](){
        CourseDashboard* dash = new CourseDashboard(m_currentUser, this);
        dash->exec(); // Ouverture modale
        delete dash;
    });
    mainLayout->addWidget(btnDashboard);

    // Initialisation
    loadProfCourses();
}
// Note: J'ai retiré l'ancienne UI de publication ici pour clarifier, car tout est dans le dashboard maintenant.

void MainWindow::loadProfCourses()
{
    if (!m_listProfCourses) return;
    m_listProfCourses->clear();
    
    // Debug: Check user ID
    if (!m_currentUser) return;
    qDebug() << "Loading courses for prof ID:" << m_currentUser->getId();

    QList<ClassModule> modules = ClassModule::getModulesByProf(m_currentUser->getId());
    qDebug() << "Found" << modules.size() << "modules.";
    
    for (const ClassModule& module : modules) {
        qDebug() << "Adding module:" << module.getNom() << module.getCodeAcces();
        
        // Widget item personnalisé
        QListWidgetItem* item = new QListWidgetItem(m_listProfCourses);
        
        QWidget* widget = new QWidget();
        // Important pour éviter les soucis de style hérité
        widget->setObjectName("courseItemWidget");
        widget->setStyleSheet("#courseItemWidget { background-color: #f8f9fa; border: 1px solid #ddd; border-radius: 5px; }");

        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->setContentsMargins(15, 10, 15, 10); // Marges latérales un peu plus grandes
        
        QLabel* lblName = new QLabel("<b>" + module.getNom() + "</b>");
        lblName->setStyleSheet("font-size: 16px; color: #333;"); // Police un peu plus grande
        lblName->setAlignment(Qt::AlignVCenter | Qt::AlignLeft); // Alignement explicite
        
        QLabel* lblCode = new QLabel("Code: <span style='color:blue; font-weight:bold;'>" + module.getCodeAcces() + "</span>");
        lblCode->setStyleSheet("font-size: 14px;");

        QPushButton* btnCopy = new QPushButton("Copier");
        btnCopy->setCursor(Qt::PointingHandCursor);
        btnCopy->setMinimumWidth(80);
        btnCopy->setMinimumHeight(30); // Un peu plus compact
        btnCopy->setStyleSheet("QPushButton { background-color: #007bff; color: white; border-radius: 4px; padding: 5px; font-weight: bold; } QPushButton:hover { background-color: #0056b3; }");
        
        QString code = module.getCodeAcces();
        connect(btnCopy, &QPushButton::clicked, [this, code]() {
            QApplication::clipboard()->setText(code);
            QMessageBox::information(this, "Copié", "Code copié : " + code);
        });

        // --- BOUTON SUPPRIMER (NOUVEAU) ---
        QPushButton* btnDelete = new QPushButton("Supprimer");
        btnDelete->setCursor(Qt::PointingHandCursor);
        btnDelete->setMinimumWidth(80);
        btnDelete->setMinimumHeight(30);
        btnDelete->setStyleSheet("QPushButton { background-color: #dc3545; color: white; border-radius: 4px; padding: 5px; font-weight: bold; } QPushButton:hover { background-color: #c82333; }");
        
        int modId = module.getId();
        connect(btnDelete, &QPushButton::clicked, [this, modId]() {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Supprimer le cours");
            msgBox.setText("Voulez-vous vraiment supprimer ce cours et tout son contenu ?");
            msgBox.setIcon(QMessageBox::Warning);
            QPushButton* yes = msgBox.addButton("Oui, supprimer", QMessageBox::YesRole);
            QPushButton* no = msgBox.addButton("Annuler", QMessageBox::NoRole);
            // CORRECTION: On donne une taille min et du padding pour ne pas couper le texte
            yes->setStyleSheet("background-color: #dc3545; color: white; min-width: 120px; padding: 8px; border-radius: 4px;");
            no->setStyleSheet("background-color: #6c757d; color: white; min-width: 100px; padding: 8px; border-radius: 4px;");
            
            msgBox.exec();
            
            if (msgBox.clickedButton() == yes) {
                if (ClassModule::deleteModule(modId)) {
                    loadProfCourses(); // Rafraîchir la liste
                } else {
                    QMessageBox::critical(this, "Erreur", "Impossible de supprimer le cours.");
                }
            }
        });
        
        layout->addWidget(lblName, 1);
        layout->addWidget(lblCode, 0);
        layout->addWidget(btnCopy, 0);
        layout->addWidget(btnDelete, 0); // Ajout au layout
        
        // Centrage vertical des éléments dans le layout
        layout->setAlignment(Qt::AlignVCenter);

        widget->setLayout(layout);
        
        // CORRECTION: On augmente la hauteur pour être sûr que "g" "j" etc ne soient pas coupés
        item->setSizeHint(QSize(0, 100)); 
        
        m_listProfCourses->setItemWidget(item, widget);
    }
}

void MainWindow::on_btnCreateClass_clicked()
{
    QString name = m_leCourseName->text().trimmed();
    QString desc = m_leCourseDesc->text().trimmed();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom du cours est obligatoire.");
        return;
    }
    
    ClassModule newModule;
    newModule.setNom(name);
    newModule.setDescription(desc);
    newModule.setProfId(m_currentUser->getId());
    newModule.generateUniqueCode();
    
    if (newModule.createInDb()) {
        QMessageBox::information(this, "Succès", "Cours créé ! Code d'accès : " + newModule.getCodeAcces());
        m_leCourseName->clear();
        m_leCourseDesc->clear();
        loadProfCourses();
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le cours.");
    }
}

/*
void MainWindow::on_profCourseSelected(int index)
{
    // Pour l'instant on ne fait rien de spécial quand on change de cours,
    // mais on pourrait charger l'historique des publications de ce cours.
    Q_UNUSED(index);
}
*/

void MainWindow::on_btnSelectFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Sélectionner un fichier", "", "Tous les fichiers (*.*)");
    if (!filePath.isEmpty()) {
        m_selectedFilePath = filePath;
        QFileInfo fi(filePath);
        m_lblSelectedFile->setText(fi.fileName());
    }
}

/*
void MainWindow::on_btnPublish_clicked()
{
    // DEPRECATED : Logic moved to CourseDashboard
}
*/



QString MainWindow::saveFileLocally(QString cheminSource) {
    if (cheminSource.isEmpty()) return "";

    // 1. Définir le dossier de destination
    QString dossierDestination = QCoreApplication::applicationDirPath() + "/uploads";
    
    // 2. Générer un nom unique pour éviter les conflits
    QFileInfo fileInfo(cheminSource);
    QString extension = fileInfo.suffix();
    QString nouveauNom = QUuid::createUuid().toString(QUuid::WithoutBraces) + "." + extension;
    
    QString cheminFinal = dossierDestination + "/" + nouveauNom;

    // 3. Copier le fichier
    if (QFile::copy(cheminSource, cheminFinal)) {
        // On retourne le chemin RELATIF pour la base de données
        return "uploads/" + nouveauNom; 
    } else {
        qDebug() << "Erreur copie fichier:" << cheminSource << "vers" << cheminFinal;
        return ""; 
    }
}

void MainWindow::on_publicationItemClicked(QListWidgetItem *item)
{
    int pubId = item->data(Qt::UserRole).toInt();
    downloadPublication(pubId);
}


void MainWindow::downloadPublication(int pubId)
{
    qDebug() << "Début downloadPublication pour ID:" << pubId;
    // Récupérer le chemin depuis la BDD
    QSqlQuery query;
    query.prepare("SELECT titre, type, chemin_fichier FROM publications WHERE id = :id");
    query.bindValue(":id", pubId);
    
    if (query.exec() && query.next()) {
        QString type = query.value("type").toString();
        QString relativePath = query.value("chemin_fichier").toString();
        QString titre = query.value("titre").toString();
        
        if (type == "COURS" && !relativePath.isEmpty()) {
            // Reconstruire le chemin absolu
            QString fullPath = QCoreApplication::applicationDirPath() + "/" + relativePath;
             QFileInfo checkFile(fullPath);
             
             if (checkFile.exists() && checkFile.isFile()) {
                 int rep = QMessageBox::question(this, "Téléchargement", 
                                       "Voulez-vous télécharger le fichier joint à ce cours ?",
                                       QMessageBox::Yes | QMessageBox::No);
                 
                 if (rep == QMessageBox::Yes) {
                     QString savePath = QFileDialog::getSaveFileName(this, "Enregistrer sous", titre + "." + checkFile.suffix());
                     if (!savePath.isEmpty()) {
                         if (QFile::copy(fullPath, savePath)) {
                             QMessageBox::information(this, "Succès", "Fichier téléchargé : " + savePath);
                         } else {
                             QMessageBox::critical(this, "Erreur", "Impossible de sauvegarder le fichier.");
                         }
                     }
                 }
             } else {
                 // Fichier introuvable
                 QMessageBox::warning(this, "Info", "Aucun fichier associé ou fichier introuvable sur le serveur.");
             }
        } else {
             QMessageBox::information(this, "Info", "Cette publication ne contient pas de fichier téléchargeable.");
        }
    }
}

