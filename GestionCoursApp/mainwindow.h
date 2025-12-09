#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "user.h"
#include "classmodule.h"
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QTextEdit>

#include <QLabel>
#include <QUuid>
#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slot pour le bouton Login
    void on_btnLogin_clicked();

    // Slot pour ouvrir la fenêtre d'inscription
    void on_btnGoToSignup_clicked();
    void logoutUser();

    // Slots pour l'interface étudiant
    void handleJoinClass();
    void on_courseSelected(int index);

    // Slots pour l'interface Professeur
    void on_btnCreateClass_clicked();

    void on_btnSelectFile_clicked();
    void on_publicationItemClicked(QListWidgetItem *item);



private:
    Ui::MainWindow *ui;
    User* m_currentUser;

    QString saveFileLocally(QString cheminSource); // Helper pour sauvegarder le fichier
    void downloadPublication(int pubId); // Helper pour télécharger
    void resetLoginForm();

    
    // Méthode pour construire l'interface étudiant dynamiquement
    void setupStudentUI();
    void loadStudentCourses();

    // Widgets pour l'interface étudiant (créés dynamiquement)
    QLineEdit* m_leCodeClass;
    QComboBox* m_comboCourses;
    QListWidget* m_listPublications;
    
    // Filtres
    PubType m_currentFilterType;
    bool m_filterActive;

    // Méthode pour construire l'interface Professeur
    void setupProfUI();
    void loadProfCourses();

    // Widgets Professeur
    QLineEdit* m_leCourseName;
    QLineEdit* m_leCourseDesc;
    QListWidget* m_listProfCourses;
    
    QLineEdit* m_lePubTitle;
    QTextEdit* m_tePubContent;
    QComboBox* m_comboPubType;
    QLabel* m_lblSelectedFile;
    QString m_selectedFilePath;
    void loadStyleSheet();
};

#endif // MAINWINDOW_H
