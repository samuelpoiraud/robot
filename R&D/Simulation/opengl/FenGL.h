/**
@Version :  0.01a
@Dernière modification :  24/09/08
@Auteur : D. Duval
@Description : Gère la sous-fenêtre "FenGL" avec Qt et OpenGL 
**/

#ifndef HEADER_FENGL
#define HEADER_FENGL

#include <QGLWidget>
#include <QtOpenGL>
#include <QList>
#include <qfile.h>

typedef struct
{
	GLfloat x, y, z;
} Sommet;

typedef struct
{
	float r, g, b;
	Sommet sommet[3];
} Triangle;

typedef struct
{
   float x, y, z; 
} Vecteur;


class FenGL : public QGLWidget
{
     Q_OBJECT

 public:
     FenGL(QWidget *parent = 0);
     ~FenGL();
	 
     QSize minimumSizeHint() const;
     QSize sizeHint() const;
	 void resetVue();
	 Vecteur vectNormal(Vecteur v1, Vecteur v2);
	 void moveRobot(GLdouble x);
	 void rotateRobot(int angle);

 public slots:
	 void setAngle(int angle);
     void setXRotation(int angle);
     void setYRotation(int angle);
     void setZRotation(int angle);

 signals:
     void xRotationChanged(int angle);
     void yRotationChanged(int angle);
     void zRotationChanged(int angle);

 protected:
     void initializeGL();
     void paintGL();
     void resizeGL(int width, int height);
     void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
	 void wheelEvent(QWheelEvent *event);
	 void keyPressEvent( QKeyEvent *event);

 private:
     GLuint makeObject();
	 void texturer(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
               GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4);
	 void polyg(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
               GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4,
               GLdouble x5, GLdouble y5, GLdouble x6, GLdouble y6,
               GLdouble x7, GLdouble y7);
	 void cote(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
     void extrude(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
     void normalizeAngle(int *angle);
	 void chargerTerrain();
	 void afficherTerrain();

     GLuint object;
	 float scale;
     int xRot;
     int yRot;
     int zRot;
	 int robotRot;
	 
	 GLdouble xTrans;
	 GLdouble yTrans;
	 GLdouble zTrans;
	 GLdouble robotTrans;
	 
     QPoint lastPos;
     QColor bleu;
	 QColor blanc;
	 QColor rouge;
     QColor vert;

	 GLuint	texture[1];
	 QList<Triangle> terrain;
};

#endif
