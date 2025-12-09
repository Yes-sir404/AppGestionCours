/********************************************************************************
** Form generated from reading UI file 'signupdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGNUPDIALOG_H
#define UI_SIGNUPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SignupDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *lineEditNom;
    QLabel *label_2;
    QLineEdit *lineEditPrenom;
    QLabel *label_3;
    QLineEdit *lineEditEmail;
    QLabel *label_4;
    QLineEdit *lineEditPass;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnAnnuler;
    QPushButton *btnInscrire;

    void setupUi(QDialog *SignupDialog)
    {
        if (SignupDialog->objectName().isEmpty())
            SignupDialog->setObjectName("SignupDialog");
        SignupDialog->resize(400, 400);
        verticalLayout = new QVBoxLayout(SignupDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label = new QLabel(SignupDialog);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        lineEditNom = new QLineEdit(SignupDialog);
        lineEditNom->setObjectName("lineEditNom");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, lineEditNom);

        label_2 = new QLabel(SignupDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        lineEditPrenom = new QLineEdit(SignupDialog);
        lineEditPrenom->setObjectName("lineEditPrenom");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, lineEditPrenom);

        label_3 = new QLabel(SignupDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_3);

        lineEditEmail = new QLineEdit(SignupDialog);
        lineEditEmail->setObjectName("lineEditEmail");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, lineEditEmail);

        label_4 = new QLabel(SignupDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, label_4);

        lineEditPass = new QLineEdit(SignupDialog);
        lineEditPass->setObjectName("lineEditPass");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, lineEditPass);


        verticalLayout->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        btnAnnuler = new QPushButton(SignupDialog);
        btnAnnuler->setObjectName("btnAnnuler");

        horizontalLayout->addWidget(btnAnnuler);

        btnInscrire = new QPushButton(SignupDialog);
        btnInscrire->setObjectName("btnInscrire");

        horizontalLayout->addWidget(btnInscrire);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(SignupDialog);

        QMetaObject::connectSlotsByName(SignupDialog);
    } // setupUi

    void retranslateUi(QDialog *SignupDialog)
    {
        SignupDialog->setWindowTitle(QCoreApplication::translate("SignupDialog", "Inscription", nullptr));
        label->setText(QCoreApplication::translate("SignupDialog", "Nom :", nullptr));
        label_2->setText(QCoreApplication::translate("SignupDialog", "Pr\303\251nom :", nullptr));
        label_3->setText(QCoreApplication::translate("SignupDialog", "Email :", nullptr));
        label_4->setText(QCoreApplication::translate("SignupDialog", "Mot de passe :", nullptr));
        btnAnnuler->setText(QCoreApplication::translate("SignupDialog", "Annuler", nullptr));
        btnInscrire->setText(QCoreApplication::translate("SignupDialog", "S'inscrire", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SignupDialog: public Ui_SignupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGNUPDIALOG_H
