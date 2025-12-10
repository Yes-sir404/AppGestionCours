
#ifndef COURSEDASHBOARD_H
#define COURSEDASHBOARD_H

#include <QDialog>
#include <QListWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include "user.h"
#include "classmodule.h"

// Un widget personnalisé pour afficher une publication style "Post Facebook"
class PublicationWidget : public QWidget {
    Q_OBJECT
public:
    PublicationWidget(int pubId, QString title, QString content, QString date, QString author, QString type, QString filePath, bool isProf, QWidget* parent = nullptr);

signals:
    void downloadRequested(int pubId, QString title);
    void deleteRequested(int pubId);

private:
    int m_pubId;
    QString m_filePath;
};

class CourseDashboard : public QDialog
{
    Q_OBJECT

public:
    explicit CourseDashboard(User* user, QWidget *parent = nullptr);
    ~CourseDashboard();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void on_btnPublish_clicked();
    void on_btnSelectFile_clicked();
    void refreshFeed();
    
    // Actions sur les posts
    void handleDeletePub(int pubId);
    void handleDownloadPub(int pubId, QString title);

private:
    User* m_currentUser;
    
    // UI Elements
    QComboBox* m_comboCourses;
    QLineEdit* m_leTitle;
    QTextEdit* m_teContent;
    QComboBox* m_comboType;
    QLabel* m_lblFile;
    QString m_selectedFilePath;
    
    QListWidget* m_listFeed; // La liste déroulante des posts

    void setupUI();
    void loadCourses();
    QString saveFileLocally(QString path);
};

#endif // COURSEDASHBOARD_H
