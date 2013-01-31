/**
@Version :  0.01a
@Dernière modification :  07/09/08
@Auteur : A. OTTAVI
@Description : La fenêtre permettant d'ajouter un paramètre de message CAN,  necessite la librairie Xml de QT pour compiler.
**/

#ifndef HEADER_FENAJOUTERPARAM
#define HEADER_FENAJOUTERPARAM

#include <QtGui>
#include <QtXml>
#include <iostream>
#include <qfile.h>
#include "fenprincipale/FenPrincipale.h"
#include "xml_chemins.h"

using namespace std;


class FenAjouterParam : public QDialog
{
  Q_OBJECT
  
	public:
		FenAjouterParam(QWidget *parent);
		
	private:
        QLineEdit *nom;
        QComboBox *type;
		QComboBox *taille;
        QPushButton *ajouter;
		QLineEdit  *choix;

	private slots:
		void on_type_currentIndexChanged(int machin);
		void on_ajouter_clicked();

};



#endif
