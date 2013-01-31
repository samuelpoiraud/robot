/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : ReadCANThread.h
 *	Package : Clickodrome
 *	Description : gere la lecture de messages CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20081126
 */


#ifndef HEADER_READCANTHREAD
#define HEADER_READCANTHREAD

#include <QtGui>
#include <qthread.h>
#include <iostream>
#include "RS232.h"
#include "CAN.h"

using namespace std;
typedef unsigned char Uint8;

class ReadCANThread : public QThread
    {
	
		Q_OBJECT
		
        public:
            void run();
			
			RS232 *connexion;
			
                signals:
			void messageRecu(Uint8 *buffer, int sizeBuffer); 
			void messageCANRecu(Uint8 *buffer); 

		private:
			volatile int interrompue;
			
			
		private slots:
			void interrompre();
    };
	
#endif
