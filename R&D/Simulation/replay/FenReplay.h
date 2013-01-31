/*
 *	Club Robot ESEO 2009 - 2010
 *	Archi-Tech' & Chomp
 *
 *	Fichier : FenReplay.h
 *	Package : Clickodrome
 *	Description : Fenêtre pour demander à l'utilisateur s'il veut créer un nouveau replay ou en charger un ancien.
 *	Auteur : Damien DUVAL
 *	Version 20100405
 */

#ifndef HEADER_FENREPLAY
#define HEADER_FENREPLAY

#include <QtGui>
#include "../fenPrincipale/FenPrincipale.h"

#define NEW 0
#define LOAD 1

using namespace std;


class FenReplay : public QDialog
{
  Q_OBJECT
  
	public:
		//Constructeur: Creation de la fenetre et de ses elements
                FenReplay (QWidget *parent);
		
	private:
                //Widgets composants la fenetre
                QPushButton *nouveau;
                QPushButton *charger;

        signals:
                //Signal pour lancer le ReadCanReplayThread
                void startReplay();

        private slots:

                void on_nouveau_clicked();
                void on_charger_clicked();

};



#endif
