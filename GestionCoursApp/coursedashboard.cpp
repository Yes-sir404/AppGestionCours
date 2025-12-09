
#include "coursedashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDate>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QUuid>
#include <QMenu>
#include <QToolButton>
#include <QDebug>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QResizeEvent>
#include <QTimer>

// =========================================================
// WIDGET PUBLICATION (La "Carte" style Facebook)
// =========================================================
PublicationWidget::PublicationWidget(int pubId, QString title, QString content, QString date, QString author, QString type, QString filePath, bool isProf, QWidget* parent)
    : QWidget(parent), m_pubId(pubId), m_filePath(filePath)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20); // Plus d'espace interne
    mainLayout->setSpacing(15);
    // IMPORTANT: Permet au layout de contraindre la taille du widget
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize); 
    
    // Style "Carte" pour ce widget
    this->setStyleSheet("background-color: white; border-radius: 10px; border: 1px solid #E0E0E0;");

    // --- HEADER: Avatar/Type + Date + Menu ---
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(10);
    
    // Icone
    QLabel* lblIcon = new QLabel();
    lblIcon->setFixedSize(45, 45);
    lblIcon->setAlignment(Qt::AlignCenter);
    lblIcon->setStyleSheet("background-color: #E8F0FE; color: #1a73e8; border-radius: 22px; font-weight: bold; font-size: 20px; border: none;");
    
    QString iconText = "📢";
    if (type == "COURS") iconText = "📘";
    else if (type == "TD_TP") iconText = "📝";
    lblIcon->setText(iconText);
    
    // Métadonnées
    QVBoxLayout* metaLayout = new QVBoxLayout();
    metaLayout->setSpacing(2);
    
    QLabel* lblAuthor = new QLabel(author.isEmpty() ? "Cours" : author);
    lblAuthor->setStyleSheet("font-weight: bold; font-size: 14px; color: #202124; border: none;");
    
    QLabel* lblDate = new QLabel(date);
    lblDate->setStyleSheet("color: #5F6368; font-size: 12px; border: none;");
    
    metaLayout->addWidget(lblAuthor);
    metaLayout->addWidget(lblDate);
    
    headerLayout->addWidget(lblIcon);
    headerLayout->addLayout(metaLayout);
    headerLayout->addStretch(); // Pousse tout vers la gauche
    
    // --- BOUTON TELECHARGEMENT (Header Right) ---
    if (!filePath.isEmpty()) {
        QPushButton* btnDownload = new QPushButton("📎 Télécharger");
        btnDownload->setCursor(Qt::PointingHandCursor);
        // Style ajusté : Plus grand, plus visible
        btnDownload->setStyleSheet(
            "QPushButton { "
            "  background-color: #E8F0FE; "
            "  color: #1a73e8; "
            "  border: none; "
            "  padding: 8px 16px; "        // Plus de padding
            "  border-radius: 18px; "      // Plus rond
            "  font-weight: bold; "
            "  font-size: 13px; "          // Plus gros
            "} "
            "QPushButton:hover { background-color: #D2E3FC; }"
        );
        connect(btnDownload, &QPushButton::clicked, [this, title](){ emit downloadRequested(m_pubId, title); });
        headerLayout->addWidget(btnDownload);
    }

    // Menu "..." pour le prof
    if (isProf) {
        QToolButton* btnMenu = new QToolButton();
        btnMenu->setText("•••"); // 3 points plus esthétiques
        // Plus grand et plus facile à cliquer
        btnMenu->setStyleSheet(
            "QToolButton { "
            "  background: transparent; "
            "  border: none; "
            "  font-size: 24px; "         // Font plus grande
            "  color: #5F6368; "
            "  padding: 4px; "
            "} "
            "QToolButton::menu-indicator { image: none; }"
            "QToolButton:hover { background-color: #F1F3F4; border-radius: 20px; }" // Feedback visuel
        );
        btnMenu->setFixedSize(40, 40); // Zone de clic fixe
        btnMenu->setPopupMode(QToolButton::InstantPopup);
        btnMenu->setCursor(Qt::PointingHandCursor);
        
        QMenu* menu = new QMenu(this);
        QAction* actDelete = menu->addAction("Supprimer");
        connect(actDelete, &QAction::triggered, [this](){ emit deleteRequested(m_pubId); });
        
        btnMenu->setMenu(menu);
        headerLayout->addWidget(btnMenu);
    }
    
    mainLayout->addLayout(headerLayout);
    
    // --- BODY: Titre + Contenu ---
    QLabel* lblTitle = new QLabel(title);
    lblTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-top: 5px; color: #202124; border: none; background: transparent;");
    lblTitle->setWordWrap(true);
    lblTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);
    mainLayout->addWidget(lblTitle);
    
    if (!content.isEmpty()) {
        QLabel* lblContent = new QLabel(content);
        lblContent->setWordWrap(true);
        lblContent->setStyleSheet("color: #3C4043; font-size: 14px; margin-bottom: 5px; border: none; background: transparent;");
        lblContent->setTextInteractionFlags(Qt::TextSelectableByMouse);
        mainLayout->addWidget(lblContent);
    }
    
    // Footer supprimé car on a tout remonté
}

// =========================================================
// CLASSE PRINCIPALE COURSE DASHBOARD
// =========================================================

CourseDashboard::CourseDashboard(User* user, QWidget *parent)
    : QDialog(parent), m_currentUser(user)
{
    this->setWindowTitle("EduLink - Fil d'Actualité");
    this->setMinimumSize(600, 700); // Format vertical style mobile/feed
    // IMPORTANT: On active les boutons Min/Max pour le redimensionnement
    this->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    this->setStyleSheet("background-color: #F0F2F5;"); // Gris très clair style FB
    
    setupUI();
    loadCourses();
    refreshFeed();
}

CourseDashboard::~CourseDashboard() {}

void CourseDashboard::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 1. ZONE DE PUBLICATION (Visible uniquement si PROF)
    if (m_currentUser->getRole() == UserRole::PROFESSEUR) {
        QGroupBox* groupPub = new QGroupBox("Créer une publication");
        groupPub->setStyleSheet("QGroupBox { background-color: white; border: 1px solid #DADCE0; border-radius: 8px; margin-top: 10px; padding: 15px; } QGroupBox::title { color: #1a73e8; font-weight: bold; }");
        
        QVBoxLayout* pubLayout = new QVBoxLayout(groupPub);
        
        // Ligne 1: Choix du cours
        QHBoxLayout* row1 = new QHBoxLayout();
        QLabel* lblClass = new QLabel("Publier dans :");
        lblClass->setStyleSheet("font-weight: bold; color: #5F6368;");
        row1->addWidget(lblClass);
        
        m_comboCourses = new QComboBox();
        m_comboCourses->setStyleSheet("padding: 5px; border: 1px solid #ccc; border-radius: 4px;");
        row1->addWidget(m_comboCourses, 1);
        pubLayout->addLayout(row1);
        
        // Ligne 2: Titre
        m_leTitle = new QLineEdit();
        m_leTitle->setPlaceholderText("Titre de votre publication...");
        m_leTitle->setStyleSheet("QLineEdit { border: none; border-bottom: 2px solid #E0E0E0; padding: 8px; font-size: 14px; background: transparent; } QLineEdit:focus { border-bottom: 2px solid #1a73e8; }");
        pubLayout->addWidget(m_leTitle);
        
        // Ligne 3: Contenu
        m_teContent = new QTextEdit();
        m_teContent->setPlaceholderText("Exprimez-vous (Infos, Cours, Consignes...)...");
        m_teContent->setMaximumHeight(100);
        m_teContent->setStyleSheet("QTextEdit { border: 1px solid #E0E0E0; background: #F8F9FA; padding: 8px; border-radius: 5px; } QTextEdit:focus { border: 1px solid #1a73e8; background: white; }");
        pubLayout->addWidget(m_teContent);
        
        // Ligne 4: Options (Type, Fichier, Bouton)
        QHBoxLayout* rowActions = new QHBoxLayout();
        rowActions->setSpacing(10);
        
        m_comboType = new QComboBox();
        m_comboType->addItem("📢 Annonce", "ANNONCE");
        m_comboType->addItem("📘 Cours", "COURS");
        m_comboType->addItem("📝 TD/TP", "TD_TP");
        m_comboType->setStyleSheet("padding: 5px;");
        
        QPushButton* btnFile = new QPushButton("📎 Joindre");
        btnFile->setToolTip("Joindre un fichier");
        btnFile->setCursor(Qt::PointingHandCursor);
        btnFile->setStyleSheet("QPushButton { background-color: #F1F3F4; color: #5F6368; border: none; padding: 6px 12px; border-radius: 15px; font-weight: 500; } QPushButton:hover { background-color: #E8EAED; color: #202124; }");
        connect(btnFile, &QPushButton::clicked, this, &CourseDashboard::on_btnSelectFile_clicked);
        
        m_lblFile = new QLabel("");
        m_lblFile->setStyleSheet("color: #1a73e8; font-style: italic;");
        
        QPushButton* btnPub = new QPushButton("Publier");
        btnPub->setCursor(Qt::PointingHandCursor);
        btnPub->setStyleSheet("QPushButton { background-color: #1a73e8; color: white; padding: 8px 24px; border-radius: 18px; font-weight: bold; border: none; } QPushButton:hover { background-color: #1669bb; }");
        connect(btnPub, &QPushButton::clicked, this, &CourseDashboard::on_btnPublish_clicked);
        
        rowActions->addWidget(m_comboType);
        rowActions->addWidget(btnFile);
        rowActions->addWidget(m_lblFile);
        rowActions->addStretch();
        rowActions->addWidget(btnPub);
        
        pubLayout->addLayout(rowActions);
        
        mainLayout->addWidget(groupPub);
    } 
    else {
        // Pour l'étudiant, on peut mettre un filtre
        QHBoxLayout* filterLayout = new QHBoxLayout();
        filterLayout->addWidget(new QLabel("Filtrer par cours :"));
        m_comboCourses = new QComboBox();
        m_comboCourses->addItem("Tous mes cours", -1);
        connect(m_comboCourses, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CourseDashboard::refreshFeed);
        filterLayout->addWidget(m_comboCourses, 1);
        mainLayout->addLayout(filterLayout);
    }
    
    // 2. FIL D'ACTUALITÉ
    m_listFeed = new QListWidget();
    m_listFeed->setSpacing(15); // Espace entre les cards
    m_listFeed->setStyleSheet("background: transparent; border: none;");
    m_listFeed->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    
    mainLayout->addWidget(m_listFeed);
}

void CourseDashboard::loadCourses() {
    m_comboCourses->clear();
    
    if (m_currentUser->getRole() == UserRole::PROFESSEUR) {
        QList<ClassModule> modules = ClassModule::getModulesByProf(m_currentUser->getId());
        for (const auto& mod : modules) {
             m_comboCourses->addItem(mod.getNom(), mod.getId());
        }
    } else {
        // Etudiant
        m_comboCourses->addItem("Tous mes cours", -1); // Option par défaut
        QList<ClassModule> modules = ClassModule::getModulesByStudent(m_currentUser->getId());
        for (const auto& mod : modules) {
             m_comboCourses->addItem(mod.getNom(), mod.getId());
        }
    }
}

void CourseDashboard::on_btnSelectFile_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Choisir un fichier");
    if (!path.isEmpty()) {
        m_selectedFilePath = path;
        QFileInfo fi(path);
        m_lblFile->setText(fi.fileName());
    }
}

QString CourseDashboard::saveFileLocally(QString path) {
    if (path.isEmpty()) return "";
    QString destDir = QCoreApplication::applicationDirPath() + "/uploads";
    QFileInfo fi(path);
    QString newName = QUuid::createUuid().toString(QUuid::WithoutBraces) + "." + fi.suffix();
    QString destPath = destDir + "/" + newName;
    
    if (QFile::copy(path, destPath)) {
        return "uploads/" + newName;
    }
    return "";
}

void CourseDashboard::on_btnPublish_clicked() {
    if (m_comboCourses->count() == 0) return;
    
    QString title = m_leTitle->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Oups", "Veuillez mettre un titre !");
        return;
    }
    
    int moduleId = m_comboCourses->currentData().toInt();
    QString type = m_comboType->currentData().toString();
    QString content = m_teContent->toPlainText();
    QString localPath = saveFileLocally(m_selectedFilePath);
    QString date = QDate::currentDate().toString("dd/MM/yyyy");
    
    QSqlQuery query;
    query.prepare("INSERT INTO publications (titre, contenu, date_creation, id_module, type, file_path) "
                  "VALUES (:t, :c, :d, :m, :type, :f)");
    query.bindValue(":t", title);
    query.bindValue(":c", content);
    query.bindValue(":d", date);
    query.bindValue(":m", moduleId);
    query.bindValue(":type", type);
    query.bindValue(":f", localPath);
    
    if (query.exec()) {
        m_leTitle->clear();
        m_teContent->clear();
        m_lblFile->clear();
        m_selectedFilePath.clear();
        refreshFeed(); // Reload feed
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de publier.\n" + query.lastError().text());
    }
}

void CourseDashboard::refreshFeed() {
    m_listFeed->clear();
    
    QString sql = "SELECT p.*, m.nom as module_nom FROM publications p "
                  "JOIN modules m ON p.id_module = m.id ";
    
    QString whereClause;
    
    // Filtrage
    if (m_currentUser->getRole() == UserRole::PROFESSEUR) {
        whereClause = "WHERE m.id_prof = :uid "; // Voir tous les posts de ses cours
    } else {
        // Etudiant : Voir posts des cours inscrits
        // Sous-requête pour avoir les IDs des modules
        whereClause = "WHERE m.id IN (SELECT id_module FROM inscriptions WHERE id_etudiant = :uid) ";
    }
    
    // Filtre spécifique combobox (Etudiant ou Prof)
    int selectedModuleId = -1;
    if (m_comboCourses->currentIndex() >= 0) {
        selectedModuleId = m_comboCourses->currentData().toInt();
    }
    
    if (selectedModuleId != -1) {
        whereClause += "AND m.id = :mid ";
    }
    
    sql += whereClause + "ORDER BY p.id DESC"; // Plus récent en haut (id auto-incrémenté)
    
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":uid", m_currentUser->getId());
    if (selectedModuleId != -1) {
        query.bindValue(":mid", selectedModuleId);
    }
    
    if (query.exec()) {
        while(query.next()) {
            int id = query.value("id").toInt();
            QString t = query.value("titre").toString();
            QString c = query.value("contenu").toString();
            QString d = query.value("date_creation").toString();
            QString type = query.value("type").toString();
            QString f = query.value("file_path").toString();
            QString author = query.value("module_nom").toString(); // On affiche le nom du cours comme auteur
            
            PublicationWidget* widget = new PublicationWidget(id, t, c, d, author, type, f, (m_currentUser->getRole() == UserRole::PROFESSEUR));
            
            // Connexions signaux
            connect(widget, &PublicationWidget::deleteRequested, this, &CourseDashboard::handleDeletePub);
            connect(widget, &PublicationWidget::downloadRequested, this, &CourseDashboard::handleDownloadPub);
            
            QListWidgetItem* item = new QListWidgetItem(m_listFeed);
            
            // CORRECTION: Responsive & Auto-Height
            // 1. On donne au widget la largeur réelle de la liste (moins le scrollbar approx)
            int width = m_listFeed->viewport()->width(); 
            if (width <= 0) width = 500; // Valeur fallback si pas encore affiché
            
            widget->resize(width, widget->sizeHint().height());
            // 2. On demande au widget de recalculer sa taille préférée (polices, word-wrap, etc.)
            widget->adjustSize(); 
            
            // 3. On applique la taille calculée à l'item
            item->setSizeHint(widget->sizeHint());
            
            m_listFeed->setItemWidget(item, widget);
        }
    } else {
        qDebug() << "Feed Error:" << query.lastError().text();
    }
}

void CourseDashboard::handleDeletePub(int pubId) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Confirmer");
    msgBox.setText("Supprimer cette publication ?");
    msgBox.setIcon(QMessageBox::Question);
    
    QPushButton* btnYes = msgBox.addButton("Oui", QMessageBox::YesRole);
    QPushButton* btnNo = msgBox.addButton("Non", QMessageBox::NoRole);
    
    // Appliquer le style
    // OUI = Rouge (Danger)
    btnYes->setStyleSheet("background-color: #dc3545; color: white; padding: 6px 15px; border-radius: 5px; font-weight: bold; border: none;"); 
    // NON = Gris (Annuler)
    btnNo->setStyleSheet("background-color: #6c757d; color: white; padding: 6px 15px; border-radius: 5px; font-weight: bold; border: none;");

    msgBox.exec();

    if (msgBox.clickedButton() == btnYes) {
        QSqlQuery q;
        q.prepare("DELETE FROM publications WHERE id = :id");
        q.bindValue(":id", pubId);
        if (q.exec()) {
            refreshFeed();
        }
    }
}

void CourseDashboard::handleDownloadPub(int pubId, QString title) {
    QSqlQuery q;
    q.prepare("SELECT file_path FROM publications WHERE id = :id");
    q.bindValue(":id", pubId);
    if(q.exec() && q.next()) {
        QString relPath = q.value(0).toString();
        QString fullPath = QCoreApplication::applicationDirPath() + "/" + relPath;
        
        if (QFile::exists(fullPath)) {
            QString savePath = QFileDialog::getSaveFileName(this, "Enregistrer", title);
            if (!savePath.isEmpty()) {
                QFile::copy(fullPath, savePath); 
            }
        } else {
            QMessageBox::warning(this, "Erreur", "Fichier introuvable sur le serveur.");
        }
    }
}

void CourseDashboard::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
    
    // On lance un timer simple pour ne pas spammer le recalcul pendant le resize
    QTimer::singleShot(50, this, [this](){
        
        int width = m_listFeed->viewport()->width();
        if (width <= 0) return;
        
        for(int i=0; i < m_listFeed->count(); i++) {
            QListWidgetItem* item = m_listFeed->item(i);
            QWidget* widget = m_listFeed->itemWidget(item);
            if (widget) {
                // IMPORTANT: On force la largeur, et on reset la hauteur à 0 pour laisser le layout recalculer
                // On retire un petit padding (25px) pour éviter l'apparition de la scrollbar horizontale
                int safeWidth = width - 25; 
                widget->resize(safeWidth, 0); 
                widget->adjustSize(); // Le layout va calculer la hauteur nécessaire
                
                // On ajoute un peu de marge en bas pour éviter que ça coupe
                QSize sz = widget->sizeHint();
                sz.setHeight(sz.height() + 10);
                
                // CRUCIAL: On force la largeur calculée pour que la carte prenne toute la place
                // Sinon, elle garde sa "taille préférée" minime et les boutons restent au milieu
                sz.setWidth(safeWidth);

                item->setSizeHint(sz);
            }
        }
    });
}
