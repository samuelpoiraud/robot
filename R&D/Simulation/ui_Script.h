/********************************************************************************
** Form generated from reading UI file 'Script.ui'
**
** Created: Tue 15. Dec 18:36:43 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCRIPT_H
#define UI_SCRIPT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Script
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *boutonSauvegarder;
    QPushButton *boutonCharger;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLabel *labelNomFichier;
    QSpacerItem *horizontalSpacer;
    QCheckBox *edition;
    QTextEdit *editeur;
    QPushButton *boutonLancer;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout_3;
    QListWidget *pListeMessages;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QLabel *label_9;
    QFrame *line_3;
    QFormLayout *formLayout_2;
    QLabel *label_3;
    QSpinBox *pNum;
    QLabel *label_4;
    QSpinBox *pT;
    QFrame *line_2;
    QLabel *label_7;
    QFormLayout *formLayout_3;
    QLabel *label_2;
    QLineEdit *pSID;
    QLabel *label_8;
    QFormLayout *formLayout_4;
    QLabel *label_5;
    QLineEdit *pESID;
    QFrame *line;
    QFormLayout *paramLayout;
    QSpacerItem *verticalSpacer_2;
    QFrame *line_5;
    QPushButton *boutonAjouter;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QDialog *Script)
    {
        if (Script->objectName().isEmpty())
            Script->setObjectName(QString::fromUtf8("Script"));
        Script->resize(898, 579);
        verticalLayoutWidget = new QWidget(Script);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(450, 10, 441, 561));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        boutonSauvegarder = new QPushButton(verticalLayoutWidget);
        boutonSauvegarder->setObjectName(QString::fromUtf8("boutonSauvegarder"));

        horizontalLayout->addWidget(boutonSauvegarder);

        boutonCharger = new QPushButton(verticalLayoutWidget);
        boutonCharger->setObjectName(QString::fromUtf8("boutonCharger"));

        horizontalLayout->addWidget(boutonCharger);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        labelNomFichier = new QLabel(verticalLayoutWidget);
        labelNomFichier->setObjectName(QString::fromUtf8("labelNomFichier"));

        horizontalLayout_2->addWidget(labelNomFichier);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        edition = new QCheckBox(verticalLayoutWidget);
        edition->setObjectName(QString::fromUtf8("edition"));

        horizontalLayout_2->addWidget(edition);


        verticalLayout->addLayout(horizontalLayout_2);

        editeur = new QTextEdit(verticalLayoutWidget);
        editeur->setObjectName(QString::fromUtf8("editeur"));
        editeur->setEnabled(false);

        verticalLayout->addWidget(editeur);

        boutonLancer = new QPushButton(verticalLayoutWidget);
        boutonLancer->setObjectName(QString::fromUtf8("boutonLancer"));

        verticalLayout->addWidget(boutonLancer);

        horizontalLayoutWidget = new QWidget(Script);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(9, 9, 437, 564));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        pListeMessages = new QListWidget(horizontalLayoutWidget);
        pListeMessages->setObjectName(QString::fromUtf8("pListeMessages"));

        horizontalLayout_3->addWidget(pListeMessages);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        label_9 = new QLabel(horizontalLayoutWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        verticalLayout_2->addWidget(label_9);

        line_3 = new QFrame(horizontalLayoutWidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_3);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label_3 = new QLabel(horizontalLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_3);

        pNum = new QSpinBox(horizontalLayoutWidget);
        pNum->setObjectName(QString::fromUtf8("pNum"));
        pNum->setMinimum(1);
        pNum->setMaximum(999);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, pNum);

        label_4 = new QLabel(horizontalLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_4);

        pT = new QSpinBox(horizontalLayoutWidget);
        pT->setObjectName(QString::fromUtf8("pT"));
        pT->setMaximum(99999);
        pT->setSingleStep(100);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, pT);


        verticalLayout_2->addLayout(formLayout_2);

        line_2 = new QFrame(horizontalLayoutWidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_2);

        label_7 = new QLabel(horizontalLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_2->addWidget(label_7);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        label_2 = new QLabel(horizontalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_2);

        pSID = new QLineEdit(horizontalLayoutWidget);
        pSID->setObjectName(QString::fromUtf8("pSID"));
        pSID->setEnabled(false);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, pSID);


        verticalLayout_2->addLayout(formLayout_3);

        label_8 = new QLabel(horizontalLayoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        verticalLayout_2->addWidget(label_8);

        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        label_5 = new QLabel(horizontalLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_5);

        pESID = new QLineEdit(horizontalLayoutWidget);
        pESID->setObjectName(QString::fromUtf8("pESID"));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, pESID);


        verticalLayout_2->addLayout(formLayout_4);

        line = new QFrame(horizontalLayoutWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        paramLayout = new QFormLayout();
        paramLayout->setObjectName(QString::fromUtf8("paramLayout"));

        verticalLayout_2->addLayout(paramLayout);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        line_5 = new QFrame(horizontalLayoutWidget);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_5);

        boutonAjouter = new QPushButton(horizontalLayoutWidget);
        boutonAjouter->setObjectName(QString::fromUtf8("boutonAjouter"));

        verticalLayout_2->addWidget(boutonAjouter);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_3);


        horizontalLayout_3->addLayout(verticalLayout_2);


        retranslateUi(Script);
        QObject::connect(edition, SIGNAL(toggled(bool)), editeur, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(Script);
    } // setupUi

    void retranslateUi(QDialog *Script)
    {
        Script->setWindowTitle(QApplication::translate("Script", "Editeur de script", 0, QApplication::UnicodeUTF8));
        boutonSauvegarder->setText(QApplication::translate("Script", "Sauvegarder en XML", 0, QApplication::UnicodeUTF8));
        boutonCharger->setText(QApplication::translate("Script", "Charger fichier XML", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Script", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-style:italic; text-decoration: underline;\">Fichier courant :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelNomFichier->setText(QApplication::translate("Script", "script.xml", 0, QApplication::UnicodeUTF8));
        edition->setText(QApplication::translate("Script", "Activer Edition", 0, QApplication::UnicodeUTF8));
        editeur->setHtml(QApplication::translate("Script", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        boutonLancer->setText(QApplication::translate("Script", "Lancer", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Script", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-weight:600; text-decoration: underline;\">Param\303\250tres :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Script", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">n\302\260 d'\303\251tape =</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Script", "temps (ms)=", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Script", "ID du message", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Script", "SID =", 0, QApplication::UnicodeUTF8));
        pSID->setText(QApplication::translate("Script", "0", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Script", "ID du message attendu (facultatif)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Script", "eSID =", 0, QApplication::UnicodeUTF8));
        pESID->setText(QApplication::translate("Script", "0", 0, QApplication::UnicodeUTF8));
        boutonAjouter->setText(QApplication::translate("Script", "Ajouter", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Script: public Ui_Script {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCRIPT_H
