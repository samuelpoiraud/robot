/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : fen2d.cpp
 *	Package : Clickodrome
 *	Description : Fonctions du clickodrome 
 *  (classe fen2d, cf fen2d.h)
 *	Auteur : Gwennaël Arbona
 *	Version 20080925
 */

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <math.h>

#include "fen2d.h"


/*---------------------------------------------------
Constructeur: Initialisation du widget
Règle la zone d'affichage, connecte les éléments
---------------------------------------------------*/
fen2d::fen2d (QWidget *parent) : QWidget(parent)
{
    
        // Valeurs numériques (angles en degrés)
	r_pos = QPoint(T_WIDTH-(INIT_POS_Y*T_WIDTH/T_REEL_WIDTH),INIT_POS_X*T_HEIGHT/T_REEL_HEIGHT);
	m_pos = QPoint();
        walle_pos = QPoint();
	r_angle = INIT_POS_A;
	a_pointsNumber = 0;
	a_pointsNumberMulti = 0;
	a_pointFictif = QPoint();
	a_angle = 0;
	angleAenvoyer=false;
	// Boutons
	QPushButton *launch = new QPushButton("Lancer", this);
	QPushButton *reset = new QPushButton("Effacer", this);

	// Labels pour afficher X ; Y ; Angle
	QLabel *label_x_txt = new QLabel("X=",this);
	label_x_int = new QLabel(this);
	QLabel *label_y_txt = new QLabel("Y=",this);
	label_y_int = new QLabel(this);
	QLabel *label_a_txt = new QLabel("A=",this);
	label_a_int = new QLabel(this);

	setFixedSize(T_WIDTH + 10, T_HEIGHT + 30);
	launch->setGeometry(215, T_HEIGHT + 8, 100, 20);
	reset->setGeometry(320, T_HEIGHT + 8, 100, 20);
	label_x_txt->setGeometry(0, T_HEIGHT + 8, 20, 10); // "X="
	label_x_int->setGeometry(20, T_HEIGHT + 8, 40, 10); // x
	label_y_txt->setGeometry(70, T_HEIGHT + 8, 20, 10); // "Y="
	label_y_int->setGeometry(90, T_HEIGHT + 8, 40, 10); // y
	label_a_txt->setGeometry(140, T_HEIGHT + 8, 20, 10); // "A="
	label_a_int->setGeometry(160, T_HEIGHT + 8, 40, 10); // angle
	
	// Connexions
	
	QObject::connect(launch, SIGNAL(clicked()), this, SLOT(launchPoints()));
        QObject::connect(reset, SIGNAL(clicked()), this, SLOT(deletePoints()));


	setMouseTracking(true);
	rightbutton_pressed = false;
        leftbutton_pressed = false;
        ctrl_pressed = false;
        show_walle = false;
    
    
    
    // Initialisation des images
    iTerrain=QImage::QImage(IMAGE_TERRAIN);        
    iElement=QImage::QImage(IMAGE_ELEMENT);
    iRobot=QImage::QImage(IMAGE_ROBOT);
    iWalle=QImage::QImage(IMAGE_WALLE);

    
    
    
}

/*---------------------------------------------------
Evènement: Touche de clavier enfoncée
CTRL : Placement du robot adverse ON
---------------------------------------------------*/
void fen2d::keyPressEvent(QKeyEvent *event)
 {
     if (event->key() == Qt::Key_Control) {
        ctrl_pressed = true;
     }
 }

/*---------------------------------------------------
Evènement: Touche de clavier relachée
CTRL : Placement du robot adverse OFF
---------------------------------------------------*/
void fen2d::keyReleaseEvent(QKeyEvent *event)
 {
     if (event->key() == Qt::Key_Control) {
        ctrl_pressed = false;
     }
 }

/*---------------------------------------------------
Evènement: Double clic de souris
Clic gauche: lancer
Clic droit: non defini
---------------------------------------------------*/
void fen2d::mouseDoubleClickEvent(QMouseEvent *click)
{
	
	//Envoi des données
	launchPoints();
}

/*---------------------------------------------------
Evènement: Clic de souris
Clic gauche: ajout d'un point si position correcte
Clic droit: rotation pour faire face au point de clic
---------------------------------------------------*/
void fen2d::mousePressEvent(QMouseEvent *click)
{
    setFocus(Qt::MouseFocusReason);
    //Touche CONTROL enfoncée : placement du robot adverse et activation du signal balise
    if(ctrl_pressed){
        if(click->button() == Qt::RightButton){
            show_walle = false;
            emit(basculeBaliseOff());
        }
        else{
            show_walle = true ;
            walle_pos = click->pos();
            emit(basculeBaliseOn(walle_pos));
        }       
        update();
    }
    else{ // Sinon , placement du robot normal

        int a = click->pos().x();
        int b = click->pos().y();
        int la = R_SIZE_LARGEUR / 2;
        int lo = R_SIZE_LONGUEUR / 2;

	//int securite = sqrt(lo*lo + la*la); // détermine tous les endroits où le robot peut tourner sur lui même sans toucher les bords du terrain.
	
	// Clic droit: rotation vers la cible
	if (click->button() == Qt::RightButton){
			rightbutton_pressed = true;
			a_angle = (int)(90+(atan2 (b - a_points[a_pointsNumber-1].y(), 
				a - a_points[a_pointsNumber-1].x()) * 180/ PI ));
			update();
			return;	
	}

	leftbutton_pressed = true;
	// Interdit de toucher les bords ou de sortir
	if (a < 5 + la) a = 5 + la;
	if (a > 5 + T_WIDTH - la) a = 5 + T_WIDTH - la;
	if (b < 5 + lo) b = 5 + lo;
	if (b > 5 + T_HEIGHT) return;
	if (b > 5 + T_HEIGHT - lo) b = 5 + T_HEIGHT - lo;

	// Limite de points par trajectoire
	if (a_pointsNumber >= MAX_POINTS){
		QMessageBox::critical(this, "Erreur", 
			"La trajectoire est limitée à " + QString::number(MAX_POINTS) + " points");
		deletePoints();
	}

	// Mise à jour après correction
	a_points[a_pointsNumber] = QPoint(a, b);
	update();

    }//end if(ctrl_key) else
}

/*---------------------------------------------------
Evènement: Bouton de souris relaché
Clic gauche: ajout d'un point si position correcte
Clic droit: rotation pour faire face au point de clic
---------------------------------------------------*/
void fen2d::mouseReleaseEvent(QMouseEvent *click)
{
   
    if(!rightbutton_pressed && !leftbutton_pressed)
    {
        return;
    }
	if (click->button() == Qt::RightButton){
		rightbutton_pressed = false;
		angleAenvoyer=true;
	}
	else if(click->button() == Qt::LeftButton){
		leftbutton_pressed = false;
		a_pointsNumber ++; // On passe au point suivant
	}
	update();
}


/*---------------------------------------------------
Evènement: Mouvement de souris
Mise à jour des coordonnées du pointeur
---------------------------------------------------*/
void fen2d::mouseMoveEvent(QMouseEvent *click)
{
	int a = click->pos().x();
	int b = click->pos().y();
	int la = R_SIZE_LARGEUR / 2;
	int lo = R_SIZE_LONGUEUR / 2;
	QPoint p = qt_to_robot(click->pos());
	QString string;

	label_x_int->setText(string.setNum(p.x()));
	label_y_int->setText(string.setNum(p.y()));

	if(rightbutton_pressed){
		a_angle = 90+(int)(atan2 (b - a_points[a_pointsNumber-1].y(),
			a - a_points[a_pointsNumber-1].x()) * 180/ PI );
		label_a_int->setText(string.setNum(270-a_angle));
		update();
		//return;
	}
	if(leftbutton_pressed){

		// Interdit de toucher les bords ou de sortir
		if (a < 5 + la) a = 5 + la;
		if (a > 5 + T_WIDTH - la) a = 5 + T_WIDTH - la;
		if (b < 5 + lo) b = 5 + lo;
		if (b > 5 + T_HEIGHT) return;
		if (b > 5 + T_HEIGHT - lo) b = 5 + T_HEIGHT - lo;

		// Mise à jour après correction
		a_points[a_pointsNumber] = QPoint(a, b);

		update();
	}

}


/*---------------------------------------------------
Evènement: Actualisation du widget
Cette fonction réalise toute la partie dessin:
terrain, robot, pointeur, trajectoires...
---------------------------------------------------*/
void fen2d::paintEvent(QPaintEvent *paint)
{		
	// Initialisation du terrain
	QPainter painter(this);
	int i;
    //traitement de l'image du terrain
    QRectF target(0,0 , T_WIDTH, T_HEIGHT);
    QRectF source(T_IMAGE_W_OFF, T_IMAGE_H_OFF, T_IMAGE_WIDTH, T_IMAGE_HEIGHT);
    
    painter.drawImage(target, iTerrain, source);
    
    //traitement du masque des éléments de jeu
    painter.drawImage(target, iElement, source);
        
	
	// Affichage des trajectoires
	for (i = 0; i <= a_pointsNumber; i++){
		if(a_points[i].x() > 0 && a_points[i].y()){
                    if(i<a_pointsNumber)
                    {
                        painter.setPen(Qt::red);
                    }
                    else
                    {
                        painter.setPen(Qt::cyan);
                    }
                    painter.drawText(QPoint(a_points[i].x() - 4, a_points[i].y() + 3), "+");
                    if(i<a_pointsNumber)
                    {
                        painter.setPen(Qt::lightGray);
                    }
                    else
                    {
                        painter.setPen(Qt::darkMagenta);
                    }

                    if(i>0)
                    {
                        painter.drawLine(a_points[i-1], a_points[i]);
                    }
                    else
                    {
                        painter.drawLine(QPoint(r_pos.x(), r_pos.y()), a_points[i]);
                    }
		}
	}

	/*debug:Affichage du multipoint
		for (i = 0; i < a_pointsNumberMulti; i++){
		painter.setPen(Qt::blue);
		
		painter.drawText(QPoint(a_pointsMulti[i].x() - 4, a_pointsMulti[i].y() + 3), "+");
		cout<<"a_pointsMulti["<<i<<"].x():"<<a_pointsMulti[i].x()<< "a_pointsMulti["<<i<<"].y():"<<a_pointsMulti[i].y();
	}*/
	
	// Affichage du pointeur robot
	if (a_pointsNumber > 0){
		painter.setOpacity(RP_OPACITE);
		painter.setBrush(Qt::NoBrush);

		painter.translate(a_points[a_pointsNumber-1].x(), a_points[a_pointsNumber-1].y());
		painter.rotate(a_angle);
		drawRobot(&painter);
		painter.rotate(-a_angle);
		painter.translate(-a_points[a_pointsNumber-1].x(), -a_points[a_pointsNumber-1].y());
	}
	
	// Affichage du vrai robot
	painter.setOpacity(RR_OPACITE);
	painter.setBrush(Qt::lightGray);
	
	painter.translate(r_pos.x(), r_pos.y());
	painter.rotate(r_angle);
	drawRobot(&painter);
        painter.rotate(-r_angle);
        painter.translate(-r_pos.x(), -r_pos.y());


        // Affichage du robot adverse (Wall-E)
        if(show_walle){
            drawWalle(&painter);
        }

	// Affichage du point fictif lors de la réception d'un message CAN
	painter.setPen(Qt::cyan);
	painter.drawText(QPoint(a_pointFictif.x() - 4, a_pointFictif.y() + 3), "+");
}



/*---------------------------------------------------
Dessine un mignon robot
---------------------------------------------------*/
void fen2d::drawRobot(QPainter *painter)
{
    
    QRectF target(-R_SIZE_X_CENTRE,-R_SIZE_Y_CENTRE, R_SIZE_LARGEUR, R_SIZE_LONGUEUR);
    QRectF source(R_IMAGE_LAR_OFF, R_IMAGE_LONG_OFF, R_IMAGE_LARGEUR, R_IMAGE_LONGUEUR);
    painter->drawImage(target, iRobot, source);
}

/*---------------------------------------------------
Dessine un wall-e représentant le robot adverse
---------------------------------------------------*/
void fen2d::drawWalle(QPainter *painter)
{
    QRectF target(-RAYON_ADVERSAIRE/2 + walle_pos.x(), -RAYON_ADVERSAIRE/2 + walle_pos.y(), RAYON_ADVERSAIRE, RAYON_ADVERSAIRE);
    painter->drawImage(target, iWalle);
}


/*---------------------------------------------------
Slot: Efface tous les points de trajectoire
---------------------------------------------------*/
void fen2d::deletePoints()
{
        for(int i = 0; i<a_pointsNumber ; i++)
            a_points[i] = QPoint();
	a_pointsNumber = 0;
        a_angle = 0;
	angleAenvoyer=false;
	update();
}


/*---------------------------------------------------
Slot: Envoie la trajectoire au widget CAN
---------------------------------------------------*/
void fen2d::launchPoints()
{
	int i;
	for (i = 0; i < a_pointsNumber; i++)
		a_points[i] = qt_to_robot(a_points[i]);
	
	emit(sendMessage(a_points, a_pointsNumber, -a_angle+270,angleAenvoyer));
	deletePoints();
        
}


/*---------------------------------------------------
Slot: Met à jour les coordonnées du robot
---------------------------------------------------*/
void fen2d::setPosition(QPoint new_pos, int new_angle)
{
	r_pos = robot_to_qt(new_pos);
	r_angle = -new_angle+270;
	update();
}

void fen2d::setPointMulti(QPoint new_pos)
{	
	m_pos = robot_to_qt(new_pos);
		// Limite de points par trajectoire
	if (a_pointsNumberMulti >= MAX_POINTS){
		QMessageBox::critical(this, "Erreur", 
			"La trajectoire est limitée à " + QString::number(MAX_POINTS) + " points");
		deletePoints();
	}
	//cout<<"setPointMulti:X:"<<new_pos.x()<<"Y:"<<new_pos.y()<<endl;
	// Mise à jour après correction
	a_pointsMulti[a_pointsNumberMulti] = m_pos;
	a_pointsNumberMulti ++;
	
	update();
}

/*---------------------------------------------------
Conversion référentiel Qt ==> Robot
---------------------------------------------------*/
QPoint fen2d::qt_to_robot(QPoint p)
{
	return QPoint (
		5 * (T_WIDTH - p.x()), 
		5 * (p.y())
	);
}

/*---------------------------------------------------
Conversion référentiel Robot ==> Qt  
---------------------------------------------------*/
QPoint fen2d::robot_to_qt(QPoint p)
{
	return QPoint (
		(-p.x()) / 5 + T_WIDTH, 
		p.y() / 5
	);
}

/*---------------------------------------------------
Slot: Met à jour les coordonnées du point Fictif lors de la réception d'un message CAN
---------------------------------------------------*/
void fen2d::setPointFictif(QPoint new_pos)
{
	a_pointFictif = robot_to_qt(new_pos);
	update();
}

/*---------------------------------------------------
Slot : Lors de la reception du message can 0x748, affichage du robot adverse sur le terrain
---------------------------------------------------*/
void fen2d::robotAdverseDetection(QPoint coord)
{
    show_walle = true ;
    walle_pos = coord;
    emit(basculeBaliseOff());
}

