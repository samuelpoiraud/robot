/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenModifierMessageCAN.h
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet  de modifier  un message CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */

#ifndef HEADER_FENMODIFIERMESSAGECAN
#define HEADER_FENMODIFIERMESSAGECAN

#include <QtGui>
#include <QtXml>
#include "fenPrincipale/FenPrincipale.h"

#include "xml_chemins.h"

using namespace std;


class FenModifierMessageCAN : public QDialog
{
  Q_OBJECT
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		FenModifierMessageCAN(QWidget *parent);
		
	private:
		//Widgets composants la fenetre
		QComboBox *messagesCAN;
        QPushButton *modifier;
		QLineEdit *nom;
		QLineEdit *code;
		vector<QCheckBox*> parametres;


	public slots:
		//Slot qui permet d'afficher les informations du message CAN séléctionné pour pouvoir les modifier ensuite
		void on_messagesCAN_currentIndexChanged(int machin);
		
		//Slot qui modifie les information du  message CAN séléctioné  dans le fichier messagesCAN.xml 
		void on_modifier_clicked();

};



#endif
