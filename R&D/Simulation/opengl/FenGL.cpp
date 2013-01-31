/**
@Version :  0.01a
@Dernière modification :  04/09/08
@Auteur : D. Duval
@Description : Gère la sous-fenêtre "FenGL" avec Qt et OpenGL 
**/
#include <QtGui>
#include <QtOpenGL>
#include <math.h>
#include "FenGL.h"

/**
@Version :  0.01a
@Dernière modification :  03/09/08
@Auteur : D. Duval
@Description : Constructeur 
**/
 FenGL::FenGL(QWidget *parent) 
     : QGLWidget(parent)
 {
	//Initialisation des variables
     object = 0;
     xRot = 0;
     yRot = 0;
     zRot = 0;
	 robotRot = 0;
	 
	 scale = 1.0 ;
	 xTrans = 0.0 ;
	 yTrans = 0.0 ;
	 zTrans = -7.0 ;
	 robotTrans = 0.0 ;

	 //Initialisation des couleurs
     bleu = Qt::blue;
     rouge = Qt::red;
	 vert = Qt::green;
	 blanc = Qt::white;
 }

/**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : Destructeur 
**/
 FenGL::~FenGL() 
 {
     makeCurrent();
     glDeleteLists(object, 1);
 }
/**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : Défini la taille de la fenêtre OpenGL 
**/
 QSize FenGL::minimumSizeHint() const
 {
     return QSize(50, 50);
 }

 QSize FenGL::sizeHint() const
 {
     return QSize(450, 450);
 }

/**
@Version :  0.01a
@Dernière modification :  04/09/08
@Auteur : D. Duval
@Description : Réinitialise la vue 3D.
**/
 void FenGL::resetVue()
 {
     xRot = 0;
     yRot = 0;
     zRot = 0;
	 scale = 1.0 ;
	 xTrans = 0.0 ;
	 yTrans = 0.0 ;
	 zTrans = -7.0 ;
	 updateGL();
 }
 
 
void FenGL::setAngle(int angle){
	rotateRobot(angle);
	updateGL();
}

/**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : Fonctions de rotations de la camera selon les 3 axes.
**/
 void FenGL::setXRotation(int angle)
 {
     normalizeAngle(&angle);
     if (angle != xRot) {
         xRot = angle;
         emit xRotationChanged(angle);
         updateGL();
     }
 }

 void FenGL::setYRotation(int angle)
 {
     normalizeAngle(&angle);
     if (angle != yRot) {
         yRot = angle;
         emit yRotationChanged(angle);
         updateGL();
     }
 }

 void FenGL::setZRotation(int angle)
 {
     normalizeAngle(&angle);
     if (angle != zRot) {
         zRot = angle;
         emit zRotationChanged(angle);
         updateGL();
     }
 }
/**
@Version :  0.01a
@Dernière modification :  19/09/08
@Auteur : D. Duval
@Description : Fonction d'initialisation de la fenêtre OpenGL (obligatoire)
**/
 void FenGL::initializeGL()
 {
	 chargerTerrain();
     qglClearColor(vert.dark());
     object = makeObject();
     glShadeModel(GL_SMOOTH);
	 glClearDepth(1.0f);
     glEnable(GL_DEPTH_TEST);
	 glDepthFunc(GL_LEQUAL);	
     glEnable(GL_CULL_FACE);
	 glEnable(GL_TEXTURE_2D);
	 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	 glEnable(GL_NORMALIZE) ;
	 glEnable(GL_COLOR_MATERIAL);
	 
	 //Lumière
	 glEnable(GL_LIGHTING);
	 GLfloat lightPosition[] = {0.5, 0.0, 0.0, 1.0}; // 4e param !=0 : (x, y, z, 1) position.		4e param = 0 : (x, y, z, 0) : vecteur
	 GLfloat lightDiffuse[]= { 1.0, 1.0, 1.0, 0.5 };
	 GLfloat lightAmbient[]= { 0.0, 0.0, 0.0, 1.0 };
	 glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	 glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	 glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	 glEnable(GL_LIGHT0);
 }

 /**
@Version :  0.01a
@Dernière modification :  25/09/08
@Auteur : D. Duval
@Description : Affiche les objets
**/
 void FenGL::paintGL()
 {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glLoadIdentity(); // Terrain
	 gluLookAt(0.0, 0.0, 0.0,  0.0, 0.0, -0.60,  0.0, 1.0, 0.0);
     glTranslated(xTrans, yTrans, zTrans);
	 glScalef(scale, scale, scale); // zoom
     glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
     glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
     glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
	 afficherTerrain();				// Création du terrain
	 
	 glLoadIdentity(); // Robot
	 gluLookAt(0.0, 0.0, 0.0,  0.0, 0.0, -0.60,  0.0, 1.0, 0.0);
	 glTranslated(xTrans + robotTrans, yTrans, zTrans);
	 glScalef(scale, scale, scale); // zoom
     glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
     glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
     glRotated(zRot / 16.0 + robotRot, 0.0, 0.0, 1.0);
     glCallList(object); 			// Création du robot
	 

 }
/**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description :  Ajustement de la camera en fonction de la fenêtre.
**/
 void FenGL::resizeGL(int width, int height)
 {
     int side = qMin(width, height);
     glViewport((width - side) / 2, (height - side) / 2, side, side);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
     glMatrixMode(GL_MODELVIEW);
 }
/**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : Fonction utilisée dans FenGL::mouseMoveEvent pour déterminer la distance parcourue par le curseur entre le clic et le relachement du clic.
**/
 void FenGL::mousePressEvent(QMouseEvent *event)
 {
     lastPos = event->pos();
 }

/**
@Version :  0.01a
@Dernière modification :  03/09/08
@Auteur : D. Duval
@Description :  
Bouton GAUCHE : Déplacement du terrain selon X et Y (en fonction de la caméra)
Bouton DROIT : Rotation de la caméra selon X et Z.
**/ 
 void FenGL::mouseMoveEvent(QMouseEvent *event)
 {
     int dx = event->x() - lastPos.x();
     int dy = event->y() - lastPos.y();

     if (event->buttons() & Qt::LeftButton) {
         //setXRotation(xRot + 8 * dy);
         //setYRotation(yRot + 8 * dx);
		 xTrans += dx*0.002 ;
		 yTrans += dy*0.002 ;
		 updateGL();
     } else if (event->buttons() & Qt::RightButton) {
         setXRotation(xRot + 8 * dy);
         setZRotation(zRot + 8 * dx);
     }
     lastPos = event->pos();
 }

/**
@Version :  0.01a
@Dernière modification :  03/09/08
@Auteur : D. Duval
@Description : Zoom avec la molette de la souris.
**/ 
 void FenGL::wheelEvent(QWheelEvent *event)
 {
    event->delta() > 0 ? scale += scale*0.1 : scale -= scale*0.1;
	if(scale<0.2) scale = 0.2 ;
	if(scale>4) scale = 4;
    updateGL();

 }

 /**
@Version :  0.01a
@Dernière modification :  24/09/08
@Auteur : D. Duval
@Description : Gestion du clavier
**/
void FenGL::keyPressEvent( QKeyEvent *event )
{
	switch( event->key() )
	{
	case Qt::Key_Up:		
		QMessageBox::information(this, "Key UP", "Vous venez d'appuyer sur KEy UP");
		moveRobot(0.01);
		break;
      
	case Qt::Key_Down:
		moveRobot(-0.01);
		break;
      
	case Qt::Key_Left:
		rotateRobot(-1);
		break;
		
    case Qt::Key_Right:
		rotateRobot(1);
		break;
	 
	default:
		keyPressEvent( event );
	}
}
 
 /**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : Fonction utilisée pour creer des objets en 3D et les texturer.
@Return :  GLuint La liste des objets a créer
**/
 GLuint FenGL::makeObject()
 {
     GLuint list = glGenLists(1);
     glNewList(list, GL_COMPILE);
	 glColorMaterial(GL_FRONT, GL_DIFFUSE); // Couleur pour éclairage
	 // le "Robot"
	glBegin(GL_TRIANGLES);
	 // liste des sommets (x et y seulement , z étant "fixe" )
		GLdouble x1 = 0.4 ;
		GLdouble y1 = 0 ;
		GLdouble x2 = 0.15 ;
		GLdouble y2 = 0.3 ;
		GLdouble x3 = -0.15 ;
		GLdouble y3 = 0.3 ;
		GLdouble x4 = -0.3 ;
		GLdouble y4 = 0.15 ;
		GLdouble x5 = -0.3 ;
		GLdouble y5 = -0.15 ;
		GLdouble x6 = -0.15 ;
		GLdouble y6 = -0.3 ;
		GLdouble x7 = 0.15 ;
		GLdouble y7 = -0.3 ;

		polyg(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, x7, y7);
		

     glEnd();
		texturer(0.16, 0.16, -0.16, 0.16, -0.16, -0.16, 0.16, -0.16);

     glEndList();
     return list;
 
 }

/**
@Version :  0.01a
@Dernière modification :  19/09/08
@Auteur : D. Duval
@Description : Modélisation du "Robot"
@Paramètres : Coordonnées des sommets du robot (x et y)
**/
  void FenGL::polyg(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
               GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4,
               GLdouble x5, GLdouble y5, GLdouble x6, GLdouble y6,
               GLdouble x7, GLdouble y7)
 {

	 //Dessous :
	 qglColor(bleu.dark());
	 glColor3d(0.0, 0.0, 1.0); // Couleur pour éclairage
	 glNormal3d(0.0,0.0,-1.0); // Assigne la normale de la surface à tous les sommets de la face du dessous.	Pour l'écalrage.
     
	 glVertex3d(x1, y1, -0.99);
     glVertex3d(x2, y2, -0.99);
     glVertex3d(x7, y7, -0.99);

     glVertex3d(x2, y2, -0.99);
     glVertex3d(x3, y3, -0.99);
     glVertex3d(x7, y7, -0.99);

	 glVertex3d(x3, y3, -0.99);
     glVertex3d(x6, y6, -0.99);
     glVertex3d(x7, y7, -0.99);

     glVertex3d(x3, y3, -0.99);
     glVertex3d(x5, y5, -0.99);
     glVertex3d(x6, y6, -0.99);

     glVertex3d(x3, y3, -0.99);
     glVertex3d(x4, y4, -0.99);
     glVertex3d(x5, y5, -0.99);

	 // Côtés :
	 qglColor(rouge.dark(240));
	 glColor3d(1.0, 0.0, 0.0); // Couleur pour éclairage
	 cote(x1, y1, x2, y2);
	 //qglColor(rouge.dark(215));
	 cote(x2, y2, x3, y3);
	 //qglColor(rouge.dark(185));
	 cote(x3, y3, x4, y4);
	 //qglColor(rouge.dark(170));
	 cote(x4, y4, x5, y5);
	 cote(x5, y5, x6, y6);
	 //qglColor(rouge.dark(185));
	 cote(x6, y6, x7, y7);
	 //qglColor(rouge.dark(215));
	 cote(x7, y7, x1, y1);

	 //Dessus :
	 qglColor(blanc.dark());
	 glNormal3d(0.0,0.0,1.0); // Assigne la normale de la surface à tous les sommets de la face du dessus.	Pour l'écalrage.
	 glColor3d(1.0, 1.0, 1.0); // Couleur pour éclairage
	 
     glVertex3d(x1, y1, -0.60);
     glVertex3d(x7, y7, -0.60);
     glVertex3d(x2, y2, -0.60);

     glVertex3d(x2, y2, -0.60);
     glVertex3d(x7, y7, -0.60);
     glVertex3d(x3, y3, -0.60);

	 glVertex3d(x3, y3, -0.60);
     glVertex3d(x7, y7, -0.60);
     glVertex3d(x6, y6, -0.60);

     glVertex3d(x3, y3, -0.60);
     glVertex3d(x6, y6, -0.60);
     glVertex3d(x5, y5, -0.60);

     glVertex3d(x3, y3, -0.60);
     glVertex3d(x5, y5, -0.60);
     glVertex3d(x4, y4, -0.60);


 }
/**
@Version :  0.01a
@Dernière modification :  19/09/08
@Auteur : D. Duval
@Description : Création des côtés du "Robot" (simplification du code de la fonction ci-dessus)
@Paramètres : les coordonnées des 2 côtés (x et y)
**/
  void FenGL::cote(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
	 Vecteur v1 = {x2-x1,y2-y1,0.0};
	 Vecteur v2 = {0.0,0.0,-0.39};
	 Vecteur vNormal = vectNormal( v2,v1 );
	 glNormal3d( vNormal.x, vNormal.y, vNormal.z);	//Normale du côté pour l'éclairage.
	 
     glVertex3d(x1, y1, -0.60);
     glVertex3d(x2, y2, -0.60);
     glVertex3d(x1, y1, -0.99);

     glVertex3d(x2, y2, -0.99);
     glVertex3d(x1, y1, -0.99);
     glVertex3d(x2, y2, -0.60);
}

 void FenGL::extrude(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
 {
     qglColor(rouge.dark(250 + int(100 * x1)));

     glVertex3d(x1, y1, +0.05);
     glVertex3d(x2, y2, +0.05);
     glVertex3d(x2, y2, -0.05);
     glVertex3d(x1, y1, -0.05);
 }
/**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : Modulo 360° du paramètre *angle.
@Paramètre : angle en degré entier
**/
 void FenGL::normalizeAngle(int *angle)
 {
     while (*angle < 0)
         *angle += 360 * 16;
     while (*angle > 360 * 16)
         *angle -= 360 * 16;
 }

 /**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : Place le logo Eseo sur le dessus du "Robot"
@Paramètres : Les coordonnées des 4 sommets du carré texturé.
**/
 void FenGL::texturer(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
					GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4)
 {
  QImage final;
  QImage aux;
    
  if ( !aux.load( "textures/eseoGroupe256.bmp" ) )
  {
	  QMessageBox::critical(this,"Erreur","Texture manquante");
  }
    
  final = QGLWidget::convertToGLFormat( aux );
  glGenTextures( 1, &texture[0] );
  glBindTexture( GL_TEXTURE_2D, texture[0] );
  glTexImage2D( GL_TEXTURE_2D, 0, 3, final.width(), final.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, final.bits() );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  glBegin(GL_QUADS);

	glTexCoord2f(1.0f, 1.0f); glVertex3d(x1, y1, -0.5999);	
	glTexCoord2f(0.0f, 1.0f); glVertex3d(x4, y4, -0.5999);	
	glTexCoord2f(0.0f, 0.0f); glVertex3d(x3, y3, -0.5999);	
	glTexCoord2f(1.0f, 0.0f); glVertex3d(x2, y2, -0.5999);	

  glEnd();
 }
 
/**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : Lis le fichier "terrain.txt" pour le mettre en mémoire.
terrain.txt se présente comme suit :

Red Green Blue
x1   y1    z1
x2   y2   z2
x3   y3   z3

Chaque paragraphe représente un Triangle qui sera colorié selon la couleur précisée (en rgb).
**/
 void FenGL::chargerTerrain()
{
	QFile fichier( "txt/terrain.txt" );
	    
	if( fichier.open( QIODevice::ReadOnly ) )
	{
		QTextStream ts( &fichier );
		  
		Sommet som[3];
		int scount = 0;
		Triangle t;
		bool couleur = true, allok, ok;
		  
		while( !ts.atEnd() )
		{
			QStringList line = (ts.readLine().simplified()).split( " ", QString::SkipEmptyParts );
		  
			if( couleur && line.count() == 3 )
			{
				t.r = line[0].toFloat();
				t.g = line[1].toFloat();
				t.b = line[2].toFloat();
				couleur = false;
			}
			
			else if( line.count() == 3 )
			{
				allok = true;
				som[scount].x = line[0].toFloat( &ok );
				allok &= ok;
				som[scount].y = line[1].toFloat( &ok );
				allok &= ok;
				som[scount].z = line[2].toFloat( &ok );
				allok &= ok;
				  
				if( allok )
					scount++;
				  
				if( scount == 3 )
				{
					scount = 0;
					  
					t.sommet[0] = som[0];
					t.sommet[1] = som[1];
					t.sommet[2] = som[2];
						
					terrain.append( t );
					couleur = true;
				}
			}
		}
		  
		fichier.close();
	}
}

/**
@Version :  0.01a
@Dernière modification :  19/09/08
@Auteur : D. Duval
@Description : Modéliser ce qu'on a lu précédemment dans "terrain.txt".
**/
 void FenGL::afficherTerrain()
 {
	GLfloat x_m, y_m, z_m;

	for( QList<Triangle>::const_iterator i=terrain.begin(); i!=terrain.end(); ++i )
		{
			glColorMaterial(GL_FRONT, GL_DIFFUSE); // Couleur pour éclairage
			QColor couleurTriangle((*i).r, (*i).g, (*i).b);
			
			glBegin(GL_TRIANGLES);

				qglColor(couleurTriangle);
				glColor3d((*i).r, (*i).g, (*i).b); // Couleur pour éclairage
				
				Vecteur v1 = { (*i).sommet[1].x- (*i).sommet[0].x, (*i).sommet[1].y- (*i).sommet[0].y, (*i).sommet[1].z- (*i).sommet[0].z};
				Vecteur v2 = { (*i).sommet[2].x- (*i).sommet[0].x, (*i).sommet[2].y- (*i).sommet[0].y, (*i).sommet[2].z- (*i).sommet[0].z};
				Vecteur vNormal = vectNormal( v2, v1);
				glNormal3d(vNormal.x, vNormal.y, vNormal.z); // Vecteur normal utilisé pour l'éclairage
				
				x_m = (*i).sommet[0].x;
				y_m = (*i).sommet[0].y;
				z_m = (*i).sommet[0].z;
				glVertex3d(x_m,y_m,z_m);

				x_m = (*i).sommet[1].x;
				y_m = (*i).sommet[1].y;
				z_m = (*i).sommet[1].z;
				glVertex3d(x_m,y_m,z_m);

				x_m = (*i).sommet[2].x;
				y_m = (*i).sommet[2].y;
				z_m = (*i).sommet[2].z;
				glVertex3d(x_m,y_m,z_m);
			glEnd();
		}
 }

 
/**
@Version :  0.01a
@Dernière modification :  19/09/08
@Auteur : D. Duval
@Description : Pour trouver le vecteur normal sur une surface, utilisé pour l'éclairage.
@Param : 2 vecteurs du plan
@Return : Vecteur normal 
**/ 
Vecteur FenGL::vectNormal(Vecteur v1, Vecteur v2)
{
   Vecteur vNormal;
   vNormal.x = ((v1.y * v2.z) - (v1.z * v2.y));
   vNormal.y = ((v1.z * v2.x) - (v1.x * v2.z));
   vNormal.z = ((v1.x * v2.y) - (v1.y * v2.x));
   return vNormal;
} 

void FenGL::moveRobot(GLdouble x)
{
	robotTrans += x ;
}

void FenGL::rotateRobot(int angle)
{
	robotRot += angle;
}
