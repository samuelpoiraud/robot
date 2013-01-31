/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenEnregistrer.h
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet d'enregistrer un message CAN avec des parametres predefinis sous un alias
 *	Auteur : Aurélie Ottavi
 *	Version 20090407
 */

#ifndef HEADER_FENENREGISTRER
#define HEADER_FENENREGISTRER

#include <QtGui>
#include <QtXml>

#include "xml_chemins.h"
#include "Parametre.h"
#include <iostream>
#include "FenCAN.h"
using namespace std;


class FenEnregistrer: public QDialog
{
  Q_OBJECT
  
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		FenEnregistrer(QWidget *parent);
		QPushButton *enregistrer;
		QString *code;
		QString *nom;
		Parametre *parametres[9];
		int nbParametre;
		
	private:
		//Widgets composants la fenetre
        QLineEdit *alias;
        


	private slots:
		//Slot qui ajoute le messageCAN crée dans le fichier messagesCAN.xml 
		void on_enregistrer_clicked();

};



#endif
