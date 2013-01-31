/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Parametre.h
 *	Package : Clickodrome
 *	Description : permet d'avoir un type parametre regroupant toutes les infos d'un paramètre
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */

#ifndef HEADER_PARAMETRE
#define HEADER_PARAMETRE

#include <QtGui>

using namespace std;


class Parametre
{

	public:
		//Constructeur: Creation de la fenetre et de ses elements
		Parametre(QString nom,QString type,float valeur);
		Parametre();
		QString nom;
		QString type;
		float valeur;

};



#endif
