/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : EnvoieDeplacementThread.h
 *	Package : Clickodrome
 *	Description : gere l'envoie d'une trajectoire point par point a partir du clickodrome
 *	Auteur : Aurélie Ottavi
 *	Version 20090217
 */


#ifndef HEADER_ENVOIEDEPLACEMENTTHREAD
#define HEADER_ENVOIEDEPLACEMENTTHREAD
#ifndef PI
#define PI 3.141593
#endif //PI
#include <QtGui>
#include <qthread.h>
 #include <iostream>
 #include "xml_chemins.h"
#include <QtXml>
#include "CAN.h"
 
using namespace std;
typedef unsigned char Uint8;

class EnvoieDeplacementThread : public QThread
{
	Q_OBJECT
	
	public:
		void run();
		QPoint *points;
		int pointsNumber;
		int angle;
		int trajectoireFinie;
		bool trajFinie;
		bool angleAenvoyer;
		QDomNode returnNodeMessCAN(QString nom,QString valeur);
		void CANtoString(Uint8 *buffer);
		
	signals: 
		void ecrireMessage(Uint8 *message);
		void envoieMessageCANString(QString nom,QString parametres); 
};


#endif
