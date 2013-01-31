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
 
#include "Parametre.h"


//Constructeur VIDE
Parametre::Parametre()
{          
		this -> nom="null";
		this -> type="null";
		this -> valeur=0;
}

//Constructeur
Parametre::Parametre(QString nom,QString type,float valeur)
{          
		this -> nom=nom;
		this -> type=type;
		this -> valeur=valeur;
}