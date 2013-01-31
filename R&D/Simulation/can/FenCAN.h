/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenCAN.cpp
 *	Package : Clickodrome
 *	Description : gere lenvoi de messages CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20081126
 */


#ifndef HEADER_FENCAN
#define HEADER_FENCAN

#define MAX_COORDONNEE 65535
#define ROBOT_INIT_X1 readINI("positions/x1").toInt()
#define ROBOT_INIT_X2 readINI("positions/x2").toInt()
#define ROBOT_INIT_Y1 readINI("positions/y1").toInt()
#define ROBOT_INIT_Y2 readINI("positions/y2").toInt()
#define ROBOT_INIT_A1 readINI("positions/a1").toInt()
#define ROBOT_INIT_A2 readINI("positions/a2").toInt()


#ifndef PI
#define PI 3.141593
#endif //PI
#include <QtGui>
#include <QtXml> 
#include <stdlib.h>
#include "ReadCANThread.h"
#include "FenEnregistrer.h"
#include "FenEditer.h"
#include "../replay/ReadCANreplayThread.h"
#include "EnvoieDeplacementThread.h"
#include "xml_chemins.h"
#include "CAN.h"




using namespace std;

typedef short Sint8;



class FenCAN : public QWidget
{
	Q_OBJECT
	
	
	public:
		//Constructeur	
		FenCAN(QWidget *parent = 0);
		ReadCANThread *readCANThread;	
		RS232 *connexion;	
		
        ReadCANreplayThread *readCANreplayThread;
		//Fonction pour afficher un message dans la boite de texte
		void afficherMessage(QString messageAAfficher);
		
		//fonction pour stopper toutes lectures
		void stopRead();
		


		QTextEdit *messagesRecus;
		QListWidget *messagesCAN;
		
		QListWidget *listeMessagePredefinis;
			
        signals:
            void sendNumPort(int s_NumPort);
            void sendAngle(int angle); 
            void stopThreadReadCAN();
            void stopThreadReplay();
            void trajectoireFinieSignal();
            void sendPositionRobot(QPoint positionRobot, int angle);
            void sendPointMulti(QPoint pointMulti);
            void sendPosWalle(QPoint coord);

	public slots:
		//Slot permettant de basculer entre la lecture de message CAN et la réception du dernier match (replay)
			void basculeThreadReplay();
			void basculeThreadReadCAN();

	private:
		//Initialisation des widget et connection entre signaux et slots
		void initialisation();
		
		QTimer *tellTimer;
                QTimer *baliseTimer;
                QPoint walle_pos; // Coordonnées du robot adverse
		//Elements composant le widget

		vector<QAbstractSpinBox*> parametresSpin;
		vector<QComboBox*> parametresCombo;
		QPushButton *envoyer;
		QPushButton *effacer;		
		

        QPushButton *activeTP;
        
		QPushButton *port;		
		QPushButton *refresh;
		QComboBox *numPort;

		QFormLayout *formLayout;

		EnvoieDeplacementThread *envoieDeplacementThread;
		QPushButton *envoyer2;
		QPushButton *enregistrer;
		QPushButton *editer;

		
		
	private slots:
		//Slot qui permet de modifier ou non les valeurs des perametres en fonction du message CAN séléctionné
		void on_messagesCAN_currentIndexChanged(int machin);
		
		//Slot qui permet d'afficher le message envoyé au robot ainsi que ses paramètres lorsque l'on clique sur le bouton Envoyer
		void on_envoyer_clicked();		
		
		void autoTellPosition();
		
		void on_editer_clicked();
		
		void on_envoyer2_clicked();
		
		void on_effacer_clicked();
		
		void on_modifierRS232_clicked();
        
                void switchAutoTellPosition();
                void basculeBaliseOn(QPoint);
                void basculeBaliseOff();
                void autoBalisePosition();
		
		void on_enregistrer_clicked();
		void receiveNumPort(int numPort);		
		void detecterPorts();
		
		void receiveMessage(Uint8 *buffer,int sizeBuffer);
		
		void receiveMessageCAN(Uint8 *buffer);
		
		void trajectoireFinieTrue();
		
		void ecritureMessageCAN(Uint8 *message);
		
		void afficherMessageCANString(QString nom,QString parametres);
		
		void envoiDeplacement(QPoint *points, int pointsNumber, int angle, bool angleAenvoyer);
};

#endif
