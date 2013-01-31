/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenModifierParam.h
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de modifier un paramètre
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */

#ifndef HEADER_FENMODIFIERPARAM
#define HEADER_FENMODIFIERPARAM

#include <QtGui>
#include <QtXml>

#include "xml_chemins.h"


using namespace std;


class FenModifierParam : public QDialog
{
  Q_OBJECT
  
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		FenModifierParam(QWidget *parent);
		
	private:
		//Widgets composants la fenetre
		QComboBox *parametres;
        QPushButton *modifier;
		QLineEdit *nom;
		QComboBox *type;
		QComboBox *taille;
		QLineEdit *choix;

	private slots:
		//Slot qui permet d'afficher les informations duparamètre séléctionné pour pouvoir les modifier ensuite
		void on_parametres_currentIndexChanged(int machin);
		
		//Slot qui permet d'activer  les widgets string1, string2 si le type choisit est String
		void on_type_currentIndexChanged(int machin);
		
		//Slot qui modifie les information du paramètre séléctioné  dans le fichier parametres.xml 
		void on_modifier_clicked();

};



#endif
