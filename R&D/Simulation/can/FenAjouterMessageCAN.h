/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenAjouterMessageCAN.h
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet d'ajouter un message CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */

#ifndef HEADER_FENAJOUTERMESSAGECAN
#define HEADER_FENAJOUTERMESSAGECAN


#include <QtGui>
#include <QtXml>

#include "xml_chemins.h"
#include "fenprincipale/FenPrincipale.h"

using namespace std;


class FenAjouterMessageCAN : public QDialog
{
  Q_OBJECT
  
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		FenAjouterMessageCAN(QWidget *parent);
		
	private:
		//Widgets composants la fenetre
        QLineEdit *nom;
		QLineEdit *code;
        vector<QCheckBox*> parametres;
        QPushButton *ajouter;


	private slots:
		//Slot qui ajoute le messageCAN crée dans le fichier messagesCAN.xml 
		void on_ajouter_clicked();

};



#endif
