/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Replay.cpp
 *	Package : Clickodrome
 *	Description : Récupère les informations de la supervision pour refaire le match
 *	Auteur : Damien DUVAL
 *	Version 20090309
 */


#ifndef HEADER_REPLAY
#define HEADER_REPLAY

#include <QtGui>
#include "../fonctions/fonctions.h"
//#include "../fenprincipale/FenPrincipale.h"

#define N_MAX 1000
#define REPLAY_FILE_INI		"environnement/replay_file"              //path dans le fichier INI
#define REPLAY_FILE 		readINI(REPLAY_FILE_INI).toString()    //chemin du dernier replay ouvert

typedef struct 
{
        double temps; // en quart de seconde
        QPoint pos;
        int angle;
}Instant;

class Replay : public QObject
{
        Q_OBJECT

	private:
		 Instant instants[N_MAX];
                 int nb_instants;
                 bool annuler;
                 QString nom_fichier;
                 QWidget *myParent;


                 void dialogFichier();

        private slots:
                void reinitialisation();
                void sliderDeplace(int);
                void getPositionReplay(double temps, QPoint pos, int angle);
                void chargerReplay();

	public:
                Replay(QWidget*);              
                Instant getInstant(int t);
                double getTemps(int t);
                QPoint getPos(int t);
                int getAngle(int t);
                void setInstant(Instant i, int t);
                void setInstant(double temps, QPoint pos, int angle, int t);

        signals:
                void changeMaxSlider(int new_max);
                void changeTime(double temps);
                void sendPositionRobot(QPoint, int);
                void stopReplayThread();

	

};

#endif

