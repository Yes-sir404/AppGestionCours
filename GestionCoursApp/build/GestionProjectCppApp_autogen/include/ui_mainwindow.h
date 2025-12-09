/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QStackedWidget *stackedWidget;
    QWidget *pageLogin;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer_1;
    QLabel *labelTitle;
    QFormLayout *formLayout;
    QLabel *lblEmail;
    QLineEdit *lineEditEmail;
    QLabel *lblPass;
    QLineEdit *lineEditPassword;
    QPushButton *btnLogin;
    QPushButton *btnGoToSignup;
    QSpacerItem *verticalSpacer_2;
    QWidget *pageProf;
    QVBoxLayout *verticalLayout_3;
    QLabel *lblWelcomeProf;
    QPushButton *btnLogoutProf;
    QSpacerItem *verticalSpacer_3;
    QWidget *pageStudent;
    QVBoxLayout *verticalLayout_4;
    QLabel *lblWelcomeStudent;
    QPushButton *btnLogoutStudent;
    QSpacerItem *verticalSpacer_4;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        pageLogin = new QWidget();
        pageLogin->setObjectName("pageLogin");
        verticalLayout_2 = new QVBoxLayout(pageLogin);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalSpacer_1 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_1);

        labelTitle = new QLabel(pageLogin);
        labelTitle->setObjectName("labelTitle");
        QFont font;
        font.setPointSize(20);
        font.setBold(true);
        labelTitle->setFont(font);
        labelTitle->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(labelTitle);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setFormAlignment(Qt::AlignHCenter|Qt::AlignTop);
        lblEmail = new QLabel(pageLogin);
        lblEmail->setObjectName("lblEmail");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, lblEmail);

        lineEditEmail = new QLineEdit(pageLogin);
        lineEditEmail->setObjectName("lineEditEmail");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, lineEditEmail);

        lblPass = new QLabel(pageLogin);
        lblPass->setObjectName("lblPass");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, lblPass);

        lineEditPassword = new QLineEdit(pageLogin);
        lineEditPassword->setObjectName("lineEditPassword");
        lineEditPassword->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, lineEditPassword);


        verticalLayout_2->addLayout(formLayout);

        btnLogin = new QPushButton(pageLogin);
        btnLogin->setObjectName("btnLogin");
        btnLogin->setStyleSheet(QString::fromUtf8("background-color: #28a745; color: white; padding: 5px;"));

        verticalLayout_2->addWidget(btnLogin);

        btnGoToSignup = new QPushButton(pageLogin);
        btnGoToSignup->setObjectName("btnGoToSignup");
        btnGoToSignup->setFlat(true);
        btnGoToSignup->setStyleSheet(QString::fromUtf8("color: black;"));

        verticalLayout_2->addWidget(btnGoToSignup);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        stackedWidget->addWidget(pageLogin);
        pageProf = new QWidget();
        pageProf->setObjectName("pageProf");
        verticalLayout_3 = new QVBoxLayout(pageProf);
        verticalLayout_3->setObjectName("verticalLayout_3");
        lblWelcomeProf = new QLabel(pageProf);
        lblWelcomeProf->setObjectName("lblWelcomeProf");
        QFont font1;
        font1.setPointSize(16);
        lblWelcomeProf->setFont(font1);
        lblWelcomeProf->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(lblWelcomeProf);

        btnLogoutProf = new QPushButton(pageProf);
        btnLogoutProf->setObjectName("btnLogoutProf");

        verticalLayout_3->addWidget(btnLogoutProf);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_3);

        stackedWidget->addWidget(pageProf);
        pageStudent = new QWidget();
        pageStudent->setObjectName("pageStudent");
        verticalLayout_4 = new QVBoxLayout(pageStudent);
        verticalLayout_4->setObjectName("verticalLayout_4");
        lblWelcomeStudent = new QLabel(pageStudent);
        lblWelcomeStudent->setObjectName("lblWelcomeStudent");
        lblWelcomeStudent->setFont(font1);
        lblWelcomeStudent->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(lblWelcomeStudent);

        btnLogoutStudent = new QPushButton(pageStudent);
        btnLogoutStudent->setObjectName("btnLogoutStudent");

        verticalLayout_4->addWidget(btnLogoutStudent);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_4);

        stackedWidget->addWidget(pageStudent);

        verticalLayout->addWidget(stackedWidget);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "EduLink - Link the students and professors", nullptr));
        labelTitle->setText(QCoreApplication::translate("MainWindow", "Bienvenue sur la Plateforme EduLink", nullptr));
        lblEmail->setText(QCoreApplication::translate("MainWindow", "Email :", nullptr));
        lineEditEmail->setPlaceholderText(QCoreApplication::translate("MainWindow", "user@eduLink.(prof|etud).ma", nullptr));
        lblPass->setText(QCoreApplication::translate("MainWindow", "Password :", nullptr));
        btnLogin->setText(QCoreApplication::translate("MainWindow", "Se Connecter", nullptr));
        btnGoToSignup->setText(QCoreApplication::translate("MainWindow", "Pas encore de compte ? S'inscrire", nullptr));
        lblWelcomeProf->setText(QCoreApplication::translate("MainWindow", "Espace Professeur", nullptr));
        btnLogoutProf->setText(QCoreApplication::translate("MainWindow", "D\303\251connexion", nullptr));
        lblWelcomeStudent->setText(QCoreApplication::translate("MainWindow", "Espace \303\211tudiant", nullptr));
        btnLogoutStudent->setText(QCoreApplication::translate("MainWindow", "D\303\251connexion", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
