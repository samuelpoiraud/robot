/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : fen2d.h
 *	Package : Clickodrome
 *	Description : Classe du widget clickodrome
 *  Signaux:
 *  	Entrée: setPosition (point, angle) pour récupérer la position du robot
 *  	Sortie: sendMessage (points[], nombre, angle) pour lancer un déplacement
 * 	Réglages utiles: taille du robot
 *	Auteur : Gwennaël ARBONA
 *  Modification : Adrien GIRARD ; Damien DUVAL
 *	Version 20080925
 */

#ifndef HEADER_FEN2D
#define HEADER_FEN2D

#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <iostream>
#include "../fonctions/fonctions.h"
#define MAX_POINTS 1337

/*---------------------------------------------------
! Réglages dimensionnels: terrain (L*l), taille du robot
Toutes les valeurs sont en pixels avec l'unité 5mm
---------------------------------------------------*/



#define T_WIDTH             readINI("terrain/x_logiciel").toInt()                      //largeur du terrain sur le logiciel
#define T_HEIGHT            readINI("terrain/y_logiciel").toInt()                      //longueur du terrain sur le logiciel
#define T_IMAGE_WIDTH       readINI("terrain/x_image").toInt()                         //largeur du terrain sur l'image
#define T_IMAGE_HEIGHT      readINI("terrain/y_image").toInt()                         //longueur du terrain sur l'image
#define T_REEL_WIDTH        readINI("terrain/x_reel").toInt()                          //largeur du terrain réel
#define T_REEL_HEIGHT       readINI("terrain/y_reel").toInt()                          //longueur du terrain réel
#define T_IMAGE_W_OFF       readINI("terrain/x_offset").toInt()                        //offset en largeur à donner à l'image
#define T_IMAGE_H_OFF       readINI("terrain/y_offset").toInt()                        //offset en longueur à donner à l'image
#define R_SIZE_LARGEUR      readINI("robot/x_logiciel").toInt()                        //longueur typique du robot sur la répresentation
#define R_SIZE_LONGUEUR     readINI("robot/y_logiciel").toInt()                        //longueur typique du robot sur la représentation
#define R_SIZE_X_CENTRE     readINI("robot/x_centre").toInt()                          //centre typique du robot sur la répresentation
#define R_SIZE_Y_CENTRE     readINI("robot/y_centre").toInt()                          //centre typique du robot sur la répresentation
#define R_IMAGE_LARGEUR     readINI("robot/x_image").toInt()                           //longueur typique du robot sur l'image
#define R_IMAGE_LONGUEUR    readINI("robot/y_image").toInt()                           //longueur typique du robot sur l'image
#define R_IMAGE_LAR_OFF     readINI("robot/x_offset").toInt()                          //offset sur la largeur typique du robot
#define R_IMAGE_LONG_OFF    readINI("robot/y_offset").toInt()                          //offset sur la longueur typique du robot
#define IMAGE_TERRAIN       readINI("terrain/image").toString().toStdString().c_str()  //chemin de l'image du terrain    
#define IMAGE_ELEMENT       readINI("elements/image").toString().toStdString().c_str() //chemin de l'image des éléments
#define IMAGE_ROBOT         readINI("robot/image").toString().toStdString().c_str()    //chemin de l'image du robot
#define IMAGE_WALLE         readINI("adversaire/image").toString().toStdString().c_str()//chemin de l'image du robot adverse
#define INIT_POS_X          readINI("positions/x1").toInt()                            //x initial            
#define INIT_POS_Y          readINI("positions/y1").toInt()                            //y initial
#define INIT_POS_A          readINI("positions/a1").toInt()                            //angle initial
#define RP_OPACITE          readINI("robot/opacité_pointeur").toInt()/10.0             //opacité du pointeur de robot
#define RR_OPACITE          readINI("robot/opacité").toInt()/10.0                      //opacité du robot réel
#define RAYON_ADVERSAIRE    readINI("adversaire/rayon").toInt()                        //rayon du robot adverse


//#define RAYON_ROND_CENTRAL 30
#ifndef PI
#define PI 3.141593
#endif //PI

using namespace std;
/*---------------------------------------------------
Visualisation 2D du terrain (clickodrome)
---------------------------------------------------*/
class fen2d : public QWidget
{
	Q_OBJECT
	
	public:
		fen2d(QWidget *parent = 0);
	
	// Envoi d'une trajectoire: tableau, longueur, angle
	signals:
		void sendMessage(QPoint *s_points, int s_pointsNumber, int s_angle, bool angleAenvoyer);
                void basculeBaliseOn(QPoint position); // signal envoyé lorqu'on place le robot adverse sur le terrain
                void basculeBaliseOff(); // signal envoyé lorqu'on enlève le robot adverse sur le terrain

	// Evènements
	protected:
		void paintEvent(QPaintEvent *paint);
		void mousePressEvent(QMouseEvent *click);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *click);
		void mouseDoubleClickEvent(QMouseEvent *event);
                void keyPressEvent(QKeyEvent *event);
                void keyReleaseEvent(QKeyEvent *event);
	
	// Coordonnées et fonctions internes
	private:
                QPoint r_pos,m_pos,walle_pos;
                QImage iTerrain;
                QImage iElement;
                QImage iRobot;
                QImage iWalle;
		int r_angle;

		
		//Afficheurs de position et d'angle
		QLabel *label_x_int;
		QLabel *label_y_int;
		QLabel *label_a_int;

		QPoint a_points[MAX_POINTS];
		QPoint a_pointsMulti[MAX_POINTS];
		QPoint a_pointFictif;
		signed int a_angle;
		bool angleAenvoyer;
		bool rightbutton_pressed;
		bool leftbutton_pressed;
                bool ctrl_pressed;
                bool show_walle;
		int a_pointsNumber;
		int a_pointsNumberMulti;
		
		// Fonctions
		QPoint qt_to_robot(QPoint p);
		QPoint robot_to_qt(QPoint p);
		void drawRobot(QPainter *painter);
		void drawWalle(QPainter *painter);
	
	// Réception d'information
	private slots:
		void deletePoints();
		void launchPoints();
		void setPosition(QPoint new_pos, int new_angle);
		void setPointMulti(QPoint);
		void setPointFictif(QPoint);
		void robotAdverseDetection(QPoint);

};

#endif
