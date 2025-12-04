#include "mainwindow.h"
#include "ui_mainwindow.h" // Généré depuis le XML ci-dessus
#include "signupdialog.h"  // Pour ouvrir l'inscription
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentUser(nullptr)
{
    ui->setupUi(this);

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
    
    m_comboProfCourses = nullptr; // Important pour le check d'init
}

MainWindow::~MainWindow()
{
    if (m_currentUser) {
        delete m_currentUser;
    }
    delete ui;
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

    // Appel à la fonction statique qu'on a codée dans User.cpp
    User* loggedInUser = User::login(email, pass);

    if (loggedInUser) {
        // Connexion Réussie !
        m_currentUser = loggedInUser;

        // Vérification du Rôle pour l'aiguillage
        if (m_currentUser->getRole() == UserRole::PROFESSEUR) {
            // Page 1 : Prof
            ui->stackedWidget->setCurrentIndex(1);
            setupProfUI();
            // ui->lblWelcomeProf->setText("Bienvenue Professeur " + m_currentUser->getNom());
        }
        else {
            // Page 2 : Étudiant
            ui->stackedWidget->setCurrentIndex(2);
            setupStudentUI(); // Construction dynamique de l'interface
            // ui->lblWelcomeStudent->setText("Bienvenue Étudiant " + m_currentUser->getNom()); // Déplacé dans setupStudentUI
        }

        // On vide les champs de mot de passe par sécurité
        ui->lineEditPassword->clear();

    } else {
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

    // 3. Mes Cours (Liste déroulante)
    mainLayout->addWidget(new QLabel("Mes Cours :"));
    m_comboCourses = new QComboBox();
    connect(m_comboCourses, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_courseSelected);
    mainLayout->addWidget(m_comboCourses);

    // 4. Filtres
    QHBoxLayout* filterLayout = new QHBoxLayout();
    QPushButton* btnAll = new QPushButton("Tout");
    QPushButton* btnCours = new QPushButton("Cours");
    QPushButton* btnTD = new QPushButton("TD/TP");
    QPushButton* btnAnnonce = new QPushButton("Annonces");
    
    // On utilise des propriétés dynamiques ou des lambdas pour identifier le filtre
    connect(btnAll, &QPushButton::clicked, [this](){ m_filterActive = false; on_courseSelected(m_comboCourses->currentIndex()); });
    connect(btnCours, &QPushButton::clicked, [this](){ m_currentFilterType = PubType::COURS; m_filterActive = true; on_courseSelected(m_comboCourses->currentIndex()); });
    connect(btnTD, &QPushButton::clicked, [this](){ m_currentFilterType = PubType::TD_TP; m_filterActive = true; on_courseSelected(m_comboCourses->currentIndex()); });
    connect(btnAnnonce, &QPushButton::clicked, [this](){ m_currentFilterType = PubType::ANNONCE; m_filterActive = true; on_courseSelected(m_comboCourses->currentIndex()); });

    filterLayout->addWidget(btnAll);
    filterLayout->addWidget(btnCours);
    filterLayout->addWidget(btnTD);
    filterLayout->addWidget(btnAnnonce);
    
    mainLayout->addLayout(filterLayout);

    // 5. Liste des Publications
    m_listPublications = new QListWidget();
    mainLayout->addWidget(m_listPublications);
    
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
        if (pub->getType() == PubType::COURS) icon = "📘 [COURS] ";
        else if (pub->getType() == PubType::TD_TP) icon = "📝 [TD/TP] ";
        else icon = "📢 [ANNONCE] ";
        
        QListWidgetItem* item = new QListWidgetItem(icon + pub->getTitre() + "\n" + pub->getContenu());
        m_listPublications->addItem(item);
        
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
    if (m_comboProfCourses != nullptr) {
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

    // 3. Sélection du Cours
    mainLayout->addWidget(new QLabel("Gérer le cours :"));
    m_comboProfCourses = new QComboBox();
    connect(m_comboProfCourses, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_profCourseSelected);
    mainLayout->addWidget(m_comboProfCourses);

    // 4. Publier
    QGroupBox* groupPub = new QGroupBox("Nouvelle Publication");
    QVBoxLayout* pubLayout = new QVBoxLayout(groupPub);
    
    m_lePubTitle = new QLineEdit();
    m_lePubTitle->setPlaceholderText("Titre de la publication");
    
    m_tePubContent = new QTextEdit();
    m_tePubContent->setPlaceholderText("Contenu / Message...");
    m_tePubContent->setMaximumHeight(100);
    
    QHBoxLayout* typeLayout = new QHBoxLayout();
    m_comboPubType = new QComboBox();
    m_comboPubType->addItem("Annonce", QVariant::fromValue(PubType::ANNONCE));
    m_comboPubType->addItem("Cours", QVariant::fromValue(PubType::COURS));
    m_comboPubType->addItem("TD/TP", QVariant::fromValue(PubType::TD_TP));
    
    QPushButton* btnFile = new QPushButton("Joindre un fichier");
    connect(btnFile, &QPushButton::clicked, this, &MainWindow::on_btnSelectFile_clicked);
    m_lblSelectedFile = new QLabel("Aucun fichier sélectionné");
    
    typeLayout->addWidget(new QLabel("Type:"));
    typeLayout->addWidget(m_comboPubType);
    typeLayout->addWidget(btnFile);
    typeLayout->addWidget(m_lblSelectedFile);
    
    QPushButton* btnPublish = new QPushButton("Publier");
    connect(btnPublish, &QPushButton::clicked, this, &MainWindow::on_btnPublish_clicked);
    
    pubLayout->addWidget(m_lePubTitle);
    pubLayout->addWidget(m_tePubContent);
    pubLayout->addLayout(typeLayout);
    pubLayout->addWidget(btnPublish);
    
    mainLayout->addWidget(groupPub);
    
    // Initialisation
    loadProfCourses();
}

void MainWindow::loadProfCourses()
{
    m_comboProfCourses->clear();
    QList<ClassModule> modules = ClassModule::getModulesByProf(m_currentUser->getId());
    
    for (const ClassModule& module : modules) {
        // On affiche Nom + Code pour que le prof puisse le donner
        m_comboProfCourses->addItem(module.getNom() + " (Code: " + module.getCodeAcces() + ")", module.getId());
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

void MainWindow::on_profCourseSelected(int index)
{
    // Pour l'instant on ne fait rien de spécial quand on change de cours,
    // mais on pourrait charger l'historique des publications de ce cours.
    Q_UNUSED(index);
}

void MainWindow::on_btnSelectFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Sélectionner un fichier", "", "Tous les fichiers (*.*)");
    if (!filePath.isEmpty()) {
        m_selectedFilePath = filePath;
        QFileInfo fi(filePath);
        m_lblSelectedFile->setText(fi.fileName());
    }
}

void MainWindow::on_btnPublish_clicked()
{
    if (m_comboProfCourses->currentIndex() < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un cours.");
        return;
    }
    
    QString title = m_lePubTitle->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le titre est obligatoire.");
        return;
    }
    
    int moduleId = m_comboProfCourses->currentData().toInt();
    PubType type = m_comboPubType->currentData().value<PubType>();
    
    Publication* pub = nullptr;
    
    if (type == PubType::COURS) {
        CoursePub* cp = new CoursePub();
        cp->setFilePath(m_selectedFilePath);
        pub = cp;
    } else if (type == PubType::TD_TP) {
        pub = new TdTpPub();
    } else {
        pub = new AnnouncementPub();
    }
    
    pub->setTitre(title);
    pub->setContenu(m_tePubContent->toPlainText());
    pub->setModuleId(moduleId);
    
    if (pub->publier()) {
        QMessageBox::information(this, "Succès", "Publication ajoutée !");
        m_lePubTitle->clear();
        m_tePubContent->clear();
        m_lblSelectedFile->setText("Aucun fichier sélectionné");
        m_selectedFilePath.clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la publication.");
    }
    
    delete pub;
}

