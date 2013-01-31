/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenEditer.h
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de modifier ou de supprimer un message CAN predefini
 *	Auteur : Aurélie Ottavi
 *	Version 20090407
 */

#ifndef HEADER_FENEDITER
#define HEADER_FENEDITER

#include <QtGui>
#include <QtXml>

#include "xml_chemins.h"
#include <iostream>
#include "FenCAN.h"

using namespace std;


class FenEditer: public QDialog
{
  Q_OBJECT
  
	public:
		//Constructeur: Creation de la fenetre et de ses elements
		FenEditer(QWidget *parent,QString text="");
		QPushButton *modifier;
		QPushButton *supprimer;
		QString *ancienAlias;
		
	private:
		//Widgets composants la fenetre
        QLineEdit *nouvelAlias;

	private slots:
		
		void on_modifier_clicked();
		void on_supprimer_clicked();

};



#endif
