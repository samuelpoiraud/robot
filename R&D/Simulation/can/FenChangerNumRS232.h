/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenChangerNumRS232.h
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de changer le numéro de port sur lequel on établie la connexion RS232
 *	Auteur : Aurélie Ottavi
 *	Version 20081025
 */

#ifndef HEADER_FENCHANGERNUMRS232
#define HEADER_FENCHANGERNUMRS232

#include <QtGui>

class FenChangerNumRS232 : public QDialog
{
  Q_OBJECT
  
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		FenChangerNumRS232(QWidget *parent);
		
		signals:
		void sendNumPort(int s_NumPort);
		
	private:
		//Widgets composants la fenetre
		QSpinBox *numPort;
        QPushButton *valider;


	private slots:
		//Slot qui ferme la connexion préexistante pour en ouvrir une nouvelle sur le port qui à été choisit
		void on_valider_clicked();
		
	

};



#endif
