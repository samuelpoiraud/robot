/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenChangerNumRS232.h
 *	Package : Clickodrome
 *	Description : cr�e une fenetre qui permet de changer le num�ro de port sur lequel on �tablie la connexion RS232
 *	Auteur : Aur�lie Ottavi
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
		//Slot qui ferme la connexion pr�existante pour en ouvrir une nouvelle sur le port qui � �t� choisit
		void on_valider_clicked();
		
	

};



#endif
