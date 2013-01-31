/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier :FenPrincipale.h
 *	Package : Clickodrome
 *	Description : La fen�tre principale, le principal est fait avec Qt Designer.
 *	Auteur : D. Duval,A.Ottavi, G.Arbona
 *	Version 20090407
 */

#ifndef HEADER_FENPRINCIPALE
#define HEADER_FENPRINCIPALE


#include <QtGui>
//#include <QTest> // pour les qSleep(ms) Qt4.5 only :(
#include "../ui_FenPrincipale.h"
//#include "../opengl/FenGL.h"
#include "../fen2d/fen2d.h"
#include "../can/FenCAN.h"
#include "../can/FenAjouterMessageCAN.h"
#include "../can/FenModifierMessageCAN.h"
#include "../can/FenSupprimerMessageCAN.h"
#include "../can/FenAjouterParam.h"
#include "../can/FenSupprimerParam.h"
#include "../can/FenModifierParam.h"
#include "../replay/Replay.h"
#include "../replay/FenReplay.h"
#include "../script/Script.h"



class FenPrincipale : public QMainWindow, private Ui::FenPrincipale
{
		Q_OBJECT
		 
		
		
	public:
		FenCAN *fenetreCAN;
                Replay *replay;
		FenPrincipale(QWidget *parent = 0);
	
	signals:
		// A chaque r�ception de position �mettre ce signal
		void givePosition(QPoint, int);               

	private:
		// Connect les signaux envoy� par le menu  a leurs slots
		void creerActions();
	
	signals:
		// Signaux � envoyer � la fenetre CAN pour basculer entre les diff�rents modes.
		void basculeReadCAN();
		void basculeReplay();
                void basculeScript(Uint8*);
                void chargerReplay();
                void newReplay();
                void stopReplayThread();
	
	private slots:
	
		//slots associ�s aux differentes actions du menu
		void aide();
		//void resetVue3d();
		void ajouterMessageCAN();
                void ajouterParametre();
		void supprimerParametre();
		void supprimerMessageCAN();
		void modifierParametre();
		void modifierMessageCAN();
		void editeurScript();
                void menuReplay(bool);
                void nouveauReplay();
		
		
		//Slot qui bascule entre la r�ception de message CAN (par d�faut activ�), et la r�ception des donn�es du dernier match (mode Replay).
		void basculeReadCANReplay(bool);
		
		void basculeReadScript(Uint8*);
                void modifierMaxSlider(int);    //Change la port�e maximale du Slider de Replay
};

#endif

