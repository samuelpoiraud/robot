/*
 *	Club Robot ESEO 2009 - 2010
 *	Archi-Tech' & Chomp
 *
 *	Fichier : FenReplay.cpp
 *	Package : Clickodrome
 *	Description : Fen�tre pour demander � l'utilisateur s'il veut cr�er un nouveau replay ou en charger un ancien.
 *	Auteur : Damien DUVAL
 *	Version 20100405
 */


#include "FenReplay.h"

//Constructeur: Cr�ation de la fen�tre et de ses �lements
FenReplay::FenReplay(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Menu Replay");
    
    //cr�ation des widgets composant la fenetre
    nouveau=new QPushButton("Nouveau");
    charger=new QPushButton("Charger");

    QHBoxLayout *layoutPrincipal = new QHBoxLayout;
    layoutPrincipal->addWidget(nouveau);
    layoutPrincipal->addWidget(charger);

    // Connexion des  signaux aux slots
    connect(nouveau, SIGNAL(clicked()), this, SLOT(on_nouveau_clicked()));
    connect(charger, SIGNAL(clicked()), this, SLOT(on_charger_clicked()));

    resize(300,100);
    setLayout(layoutPrincipal);

}

void FenReplay::on_nouveau_clicked(){
    //emit(startReplay());
    done(NEW);
}

void FenReplay::on_charger_clicked(){
    done(LOAD);
}
