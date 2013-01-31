/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : ReadCANThread.h
 *	Package : Clickodrome
 *	Description : gere la lecture de messages CAN et temps pour le replay
 *	Auteur : Aurélie Ottavi && Damien DUVAL
 *	Version 20090322
 */


#ifndef HEADER_READCANREPLAYTHREAD
#define HEADER_READCANREPLAYTHREAD

#include <QtGui>
#include <QtXml>
#include <qthread.h>
#include <iostream>
#include "../can/RS232.h"
#include "../can/xml_chemins.h"
#include "../fonctions/fonctions.h"


using namespace std;
typedef unsigned char Uint8;

class ReadCANreplayThread : public QThread
    {
	
		Q_OBJECT
		
        public:
            void run();
                        void receiveMessageCAN(Uint8 *buffer, double temps);
			//QDomNode returnNodeMessCAN(QString nom,QString valeur);
			
			RS232 *connexion;
			
                signals:
                        void sendPositionReplay(double temps, QPoint new_pos, int new_angle);
                        void reinitialiserReplay();

		private:
			int interrompue3;
			QFile file;
			
			
		private slots:
			void interrompre();
    };
	
#endif
