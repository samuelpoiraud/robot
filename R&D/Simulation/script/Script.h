/*
 *	Club Robot ESEO 2009 - 2010
 *	Archi-Tech'
 *
 *	Fichier : Script.h
 *	Package : Clickodrome
 *	Description : Utilise le fichier script.xml pour envoyer une séquence de messages CAN au robot avec attente (par évenement ou temporisation).
 *	Auteur : Damien DUVAL
 *	Version 20090811
 */


#ifndef SCRIPT_H
#define SCRIPT_H
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <QtGui>
#include <QtXml>
#include "../can/FenCAN.h"
#include "../can/ReadCANThread.h"
#include "../ui_Script.h"
#include "../fenprincipale/FenPrincipale.h"
#include "ScriptThread.h"
#include "../can/xml_chemins.h"
#include "../fonctions/fonctions.h"

#ifndef PI
#define PI 3.141502
#endif //PI

#define SCRIPT_FILE_INI		"environnement/script_file"              //path dans le fichier INI
#define SCRIPT_FILE 		readINI(SCRIPT_FILE_INI).toString()    //chemin du dernier script ouvert
#define PREMIER_ETAPE 1
#define TIME_OUT 6000

#define STATUS_UNDEFINED 	0
#define STATUS_ABORT 		1
#define STATUS_OK    		2
#define STATUS_ERROR 		3
#define STATUS_START 		4	 





using namespace std;

typedef unsigned char Uint8;
typedef unsigned short Uint16;


class Script : public QDialog, private Ui::Script
{
  Q_OBJECT
	
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		Script(QWidget *parent);
		
		ReadCANThread *read;
		public slots:
		void scriptVersFenCAN(Uint8*);		
        void lireMessage(Uint8* message);

		signals:
		void envoyerMessageCAN(Uint8*);
		void stopRct();		

	private:
		
		
		bool script_modifie;
		bool message_envoye;		
		int numero_etape;		
		Uint8* buffer;// buffer reçoit le contenu du buffer,
		QList <QDomElement> liste_etapes;
		char statusMessage;
		QTextCursor cursor;
		QString nom_fichier;
		void xmlVersEditeur();
		int nombre_etapes();
		void majListeEtapes();
		void colorMessage();
		void dialogFichier();

		vector<QAbstractSpinBox*> parametresSpin;
		vector<QComboBox*> parametresCombo;

	private slots:
		void on_boutonSauvegarder_clicked();
		void Annuler_clicked();
		void on_boutonCharger_clicked();
		void on_edition_clicked();
		void on_boutonLancer_clicked();
		void on_boutonAjouter_clicked();
		void envoyerProchainMessage(QDomElement e);
		void setMessageEnvoye(bool);
		void on_pListeMessages_itemSelectionChanged();

};





#endif // SCRIPT_H
