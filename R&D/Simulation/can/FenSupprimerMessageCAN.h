/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenSupprimerMessageCAN.h
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de supprimer un paramètre
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */

#ifndef HEADER_FENSUPPRIMERMESSAGECAN
#define HEADER_FENSUPPRIMERMESSAGECAN

#include <QtGui>
#include <QtXml>
#include "fenPrincipale/FenPrincipale.h"
#include "xml_chemins.h"

using namespace std;


class FenSupprimerMessageCAN : public QDialog
{
  Q_OBJECT
  
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		FenSupprimerMessageCAN (QWidget *parent);
		
	private:
		//Widgets composants la fenetre
		QComboBox *messagesCAN;
        QPushButton *supprimer;
		QLineEdit *nom;


	private slots:
		//Slot qui supprime le message CAN séléctioné  dans le fichier messagesCAN.xml 
		void on_supprimer_clicked();

};



#endif
