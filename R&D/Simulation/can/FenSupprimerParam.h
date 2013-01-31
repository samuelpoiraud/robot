/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenSupprimerParam.h
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de msuprimer un paramètre
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */

#ifndef HEADER_FENSUPPRIMERPARAM
#define HEADER_FENSUPPRIMERPARAM

#include <QtGui>
#include <QtXml>

#include "xml_chemins.h"

using namespace std;


class FenSupprimerParam : public QDialog
{
  Q_OBJECT
  
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		FenSupprimerParam(QWidget *parent);
		
	private:
		//Widgets composants la fenetre
		QComboBox *parametres;
        QPushButton *supprimer;
		QLineEdit *nom;


	private slots:
		//Slot qui supprime le  paramètre séléctioné  dans le fichier parametres.xml 
		void on_supprimer_clicked();

};



#endif
