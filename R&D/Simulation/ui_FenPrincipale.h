/********************************************************************************
** Form generated from reading ui file 'FenPrincipale.ui'
**
** Created: Sat 1. May 12:14:51 2010
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_FENPRINCIPALE_H
#define UI_FENPRINCIPALE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FenPrincipale
{
public:
    QAction *actionQuitter;
    QAction *actionAProposQt;
    QAction *actionAide;
    QAction *actionReset3d;
    QAction *actionAjouterMessageCAN;
    QAction *actionAjouterParametre;
    QAction *actionSupprimerMessageCAN;
    QAction *actionModifierMessageCAN;
    QAction *actionSupprimerParametre;
    QAction *actionModifierParametre;
    QAction *actionReplayActive;
    QAction *actionScript;
    QAction *actionCharger_Replay;
    QAction *actionNouveau_Replay;
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_3;
    QWidget *fenCAN;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayoutRight;
    QWidget *fen2D;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_4;
    QSlider *replaySlider;
    QLabel *label;
    QDoubleSpinBox *replayTime;
    QMenuBar *menubar;
    QMenu *menuTest;
    QMenu *menu;
    QMenu *menuEdition;
    QMenu *menuScript;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *FenPrincipale)
    {
    if (FenPrincipale->objectName().isEmpty())
        FenPrincipale->setObjectName(QString::fromUtf8("FenPrincipale"));
    FenPrincipale->resize(978, 720);
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8("../../../../../../../Pictures/eseoGroupeIcone.gif")), QIcon::Normal, QIcon::Off);
    FenPrincipale->setWindowIcon(icon);
    actionQuitter = new QAction(FenPrincipale);
    actionQuitter->setObjectName(QString::fromUtf8("actionQuitter"));
    actionAProposQt = new QAction(FenPrincipale);
    actionAProposQt->setObjectName(QString::fromUtf8("actionAProposQt"));
    actionAide = new QAction(FenPrincipale);
    actionAide->setObjectName(QString::fromUtf8("actionAide"));
    actionReset3d = new QAction(FenPrincipale);
    actionReset3d->setObjectName(QString::fromUtf8("actionReset3d"));
    actionAjouterMessageCAN = new QAction(FenPrincipale);
    actionAjouterMessageCAN->setObjectName(QString::fromUtf8("actionAjouterMessageCAN"));
    actionAjouterParametre = new QAction(FenPrincipale);
    actionAjouterParametre->setObjectName(QString::fromUtf8("actionAjouterParametre"));
    actionSupprimerMessageCAN = new QAction(FenPrincipale);
    actionSupprimerMessageCAN->setObjectName(QString::fromUtf8("actionSupprimerMessageCAN"));
    actionModifierMessageCAN = new QAction(FenPrincipale);
    actionModifierMessageCAN->setObjectName(QString::fromUtf8("actionModifierMessageCAN"));
    actionSupprimerParametre = new QAction(FenPrincipale);
    actionSupprimerParametre->setObjectName(QString::fromUtf8("actionSupprimerParametre"));
    actionModifierParametre = new QAction(FenPrincipale);
    actionModifierParametre->setObjectName(QString::fromUtf8("actionModifierParametre"));
    actionReplayActive = new QAction(FenPrincipale);
    actionReplayActive->setObjectName(QString::fromUtf8("actionReplayActive"));
    actionScript = new QAction(FenPrincipale);
    actionScript->setObjectName(QString::fromUtf8("actionScript"));
    actionCharger_Replay = new QAction(FenPrincipale);
    actionCharger_Replay->setObjectName(QString::fromUtf8("actionCharger_Replay"));
    actionCharger_Replay->setEnabled(false);
    actionNouveau_Replay = new QAction(FenPrincipale);
    actionNouveau_Replay->setObjectName(QString::fromUtf8("actionNouveau_Replay"));
    actionNouveau_Replay->setEnabled(false);
    centralwidget = new QWidget(FenPrincipale);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    layoutWidget = new QWidget(centralwidget);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(9, 9, 531, 641));
    verticalLayout_3 = new QVBoxLayout(layoutWidget);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    verticalLayout_3->setContentsMargins(0, 0, 0, 0);
    fenCAN = new QWidget(layoutWidget);
    fenCAN->setObjectName(QString::fromUtf8("fenCAN"));

    verticalLayout_3->addWidget(fenCAN);

    verticalLayoutWidget = new QWidget(centralwidget);
    verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
    verticalLayoutWidget->setGeometry(QRect(540, 10, 432, 667));
    verticalLayoutRight = new QVBoxLayout(verticalLayoutWidget);
    verticalLayoutRight->setObjectName(QString::fromUtf8("verticalLayoutRight"));
    verticalLayoutRight->setContentsMargins(0, 0, 0, 0);
    fen2D = new QWidget(verticalLayoutWidget);
    fen2D->setObjectName(QString::fromUtf8("fen2D"));
    fen2D->setMinimumSize(QSize(430, 640));
    layoutWidget1 = new QWidget(fen2D);
    layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
    layoutWidget1->setGeometry(QRect(0, 0, 432, 641));
    verticalLayout_4 = new QVBoxLayout(layoutWidget1);
    verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
    verticalLayout_4->setContentsMargins(0, 0, 0, 0);

    verticalLayoutRight->addWidget(fen2D);

    replaySlider = new QSlider(verticalLayoutWidget);
    replaySlider->setObjectName(QString::fromUtf8("replaySlider"));
    replaySlider->setEnabled(false);
    replaySlider->setOrientation(Qt::Horizontal);

    verticalLayoutRight->addWidget(replaySlider);

    label = new QLabel(centralwidget);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(400, 650, 81, 16));
    replayTime = new QDoubleSpinBox(centralwidget);
    replayTime->setObjectName(QString::fromUtf8("replayTime"));
    replayTime->setEnabled(false);
    replayTime->setGeometry(QRect(480, 650, 52, 22));
    replayTime->setFrame(true);
    replayTime->setReadOnly(true);
    replayTime->setButtonSymbols(QAbstractSpinBox::NoButtons);
    replayTime->setMaximum(999.99);
    replayTime->setValue(0);
    FenPrincipale->setCentralWidget(centralwidget);
    menubar = new QMenuBar(FenPrincipale);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 978, 22));
    menuTest = new QMenu(menubar);
    menuTest->setObjectName(QString::fromUtf8("menuTest"));
    menu = new QMenu(menubar);
    menu->setObjectName(QString::fromUtf8("menu"));
    menuEdition = new QMenu(menubar);
    menuEdition->setObjectName(QString::fromUtf8("menuEdition"));
    menuScript = new QMenu(menubar);
    menuScript->setObjectName(QString::fromUtf8("menuScript"));
    FenPrincipale->setMenuBar(menubar);
    statusbar = new QStatusBar(FenPrincipale);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    FenPrincipale->setStatusBar(statusbar);

    menubar->addAction(menuTest->menuAction());
    menubar->addAction(menuEdition->menuAction());
    menubar->addAction(menuScript->menuAction());
    menubar->addAction(menu->menuAction());
    menuTest->addAction(actionAjouterMessageCAN);
    menuTest->addAction(actionModifierMessageCAN);
    menuTest->addAction(actionSupprimerMessageCAN);
    menuTest->addSeparator();
    menuTest->addAction(actionAjouterParametre);
    menuTest->addAction(actionModifierParametre);
    menuTest->addAction(actionSupprimerParametre);
    menuTest->addSeparator();
    menuTest->addAction(actionQuitter);
    menu->addAction(actionAProposQt);
    menu->addAction(actionAide);
    menuEdition->addSeparator();
    menuEdition->addAction(actionReplayActive);
    menuEdition->addSeparator();
    menuEdition->addAction(actionCharger_Replay);
    menuEdition->addAction(actionNouveau_Replay);
    menuScript->addAction(actionScript);

    retranslateUi(FenPrincipale);

    QMetaObject::connectSlotsByName(FenPrincipale);
    } // setupUi

    void retranslateUi(QMainWindow *FenPrincipale)
    {
    FenPrincipale->setWindowTitle(QApplication::translate("FenPrincipale", "Trace-Bot Robot Eseo 1.0", 0, QApplication::UnicodeUTF8));
    actionQuitter->setText(QApplication::translate("FenPrincipale", "Quitter", 0, QApplication::UnicodeUTF8));
    actionAProposQt->setText(QApplication::translate("FenPrincipale", "A propos de Qt", 0, QApplication::UnicodeUTF8));
    actionAide->setText(QApplication::translate("FenPrincipale", "Aide", 0, QApplication::UnicodeUTF8));
    actionReset3d->setText(QApplication::translate("FenPrincipale", "R\303\251initialiser la vue 3D", 0, QApplication::UnicodeUTF8));
    actionAjouterMessageCAN->setText(QApplication::translate("FenPrincipale", "Ajouter message CAN", 0, QApplication::UnicodeUTF8));
    actionAjouterParametre->setText(QApplication::translate("FenPrincipale", "Ajouter param\303\250tre", 0, QApplication::UnicodeUTF8));
    actionSupprimerMessageCAN->setText(QApplication::translate("FenPrincipale", "Supprimer message CAN", 0, QApplication::UnicodeUTF8));
    actionModifierMessageCAN->setText(QApplication::translate("FenPrincipale", "Modifier message CAN", 0, QApplication::UnicodeUTF8));
    actionSupprimerParametre->setText(QApplication::translate("FenPrincipale", "Supprimer param\303\250tre", 0, QApplication::UnicodeUTF8));
    actionModifierParametre->setText(QApplication::translate("FenPrincipale", "Modifier param\303\250tre", 0, QApplication::UnicodeUTF8));
    actionReplayActive->setText(QApplication::translate("FenPrincipale", "Mode r\303\251ception du replay", 0, QApplication::UnicodeUTF8));
    actionScript->setText(QApplication::translate("FenPrincipale", "Editeur de Script", 0, QApplication::UnicodeUTF8));
    actionCharger_Replay->setText(QApplication::translate("FenPrincipale", "Charger Replay", 0, QApplication::UnicodeUTF8));
    actionNouveau_Replay->setText(QApplication::translate("FenPrincipale", "Nouveau Replay", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("FenPrincipale", "Replay : (temps)", 0, QApplication::UnicodeUTF8));
    replayTime->setPrefix(QApplication::translate("FenPrincipale", "[", 0, QApplication::UnicodeUTF8));
    replayTime->setSuffix(QApplication::translate("FenPrincipale", "]", 0, QApplication::UnicodeUTF8));
    menuTest->setTitle(QApplication::translate("FenPrincipale", "Fichier", 0, QApplication::UnicodeUTF8));
    menu->setTitle(QApplication::translate("FenPrincipale", "?", 0, QApplication::UnicodeUTF8));
    menuEdition->setTitle(QApplication::translate("FenPrincipale", "Replay", 0, QApplication::UnicodeUTF8));
    menuScript->setTitle(QApplication::translate("FenPrincipale", "Script", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FenPrincipale: public Ui_FenPrincipale {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FENPRINCIPALE_H
