/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenPrincipale.cpp
 *	Package : Clickodrome
 *	Description : La fenêtre principale, le principal est fait avec Qt Designer, les finitions se font ici.
 *	Auteur :  D. Duval,A.Ottavi, G.Arbona
 *	Version 20100405
 */
#include "FenPrincipale.h"


/*---------------------------------------------------
Constructeur
Creation des differentes parties du clikodrome (envoi messages CAN, 2D)
---------------------------------------------------*/
FenPrincipale::FenPrincipale(QWidget *parent) : QMainWindow(parent)
{	
	setupUi(this); // Utilise le code de ui_fenetrePrincipale.h (Qt Designer)
	setWindowIcon(QIcon("textures/eseoGroupeIcone.gif"));
	creerActions();

    //fenetreGL = new FenGL(fenOpenGL) ; // fenOpenGL : nom du widget correspondant
	fen2d *fenetre2D = new fen2d(fen2D);  // fen2D : nom du widget correspondant	
	fenetreCAN= new FenCAN(fenCAN);
	//fenListe= new FenListe(gauche);
        replay = new Replay(this);
		
	
	//connections entre signaux et slots
	connect(fenetre2D, SIGNAL(sendMessage(QPoint*, int, int,bool)),fenetreCAN, SLOT(envoiDeplacement(QPoint*, int, int,bool))); 
	connect(fenetreCAN, SIGNAL(sendPositionRobot(QPoint, int)),fenetre2D, SLOT(setPosition(QPoint, int))); 
	connect(fenetreCAN, SIGNAL(sendPointMulti(QPoint)),fenetre2D, SLOT(setPointMulti(QPoint)));
	connect(fenetreCAN, SIGNAL(sendPointMulti(QPoint)),fenetre2D, SLOT(setPointFictif(QPoint)));
        connect(fenetreCAN, SIGNAL(sendPosWalle(QPoint)),fenetre2D, SLOT(robotAdverseDetection(QPoint)));
	
	connect(fenetreCAN->readCANreplayThread, SIGNAL(sendPositionReplay(double,QPoint,int)),replay,SLOT(getPositionReplay(double,QPoint,int)));
        connect(fenetreCAN->readCANreplayThread, SIGNAL(reinitialiserReplay()),replay,SLOT(reinitialisation()));

        connect(replay,SIGNAL(changeMaxSlider(int)),this,SLOT(modifierMaxSlider(int)));
	connect(replay,SIGNAL(changeTime(double)),replayTime,SLOT(setValue(double)));
	connect(replaySlider,SIGNAL(valueChanged(int)),replay,SLOT(sliderDeplace(int)));
	connect(replay, SIGNAL(sendPositionRobot(QPoint, int)),fenetre2D, SLOT(setPosition(QPoint, int)));	
        connect(actionCharger_Replay, SIGNAL(triggered()), replay, SLOT(chargerReplay()));
        connect(actionNouveau_Replay, SIGNAL(triggered()), this, SLOT(nouveauReplay()));


	connect(this, SIGNAL(basculeReadCAN()), fenetreCAN, SLOT(basculeThreadReadCAN()));
        connect(this, SIGNAL(basculeReplay()), fenetreCAN, SLOT(basculeThreadReplay()));
        connect(fenetre2D, SIGNAL(basculeBaliseOn(QPoint)),fenetreCAN, SLOT(basculeBaliseOn(QPoint)));
        connect(fenetre2D, SIGNAL(basculeBaliseOff()),fenetreCAN, SLOT(basculeBaliseOff()));

        connect(replay, SIGNAL(stopReplayThread()), fenetreCAN->readCANreplayThread, SLOT(interrompre()));
        connect(this, SIGNAL(newReplay()), fenetreCAN->readCANreplayThread, SLOT(start()));
        connect(this, SIGNAL(stopReplayThread()), fenetreCAN->readCANreplayThread, SLOT(interrompre()));
        connect(this, SIGNAL(chargerReplay()), replay, SLOT(chargerReplay()));
	
}


/*---------------------------------------------------
Connect les signaux envoyés par le menu  a leurs slots
---------------------------------------------------*/
void FenPrincipale::creerActions()
{
	actionQuitter->setShortcut(tr("Ctrl+Q"));
	connect(actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));

        connect(actionAide, SIGNAL(triggered()), this, SLOT(aide()));
        actionAide->setShortcut(QKeySequence::HelpContents);

        connect(actionAProposQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	connect(actionScript, SIGNAL(triggered()), this, SLOT(editeurScript()));

	connect(actionAjouterMessageCAN, SIGNAL(triggered()), this, SLOT(ajouterMessageCAN()));
	
        connect(actionAjouterParametre, SIGNAL(triggered()), this, SLOT(ajouterParametre()));
	
	connect(actionModifierMessageCAN, SIGNAL(triggered()), this, SLOT(modifierMessageCAN()));
	
        connect(actionModifierParametre, SIGNAL(triggered()), this, SLOT(modifierParametre()));
	
	connect(actionSupprimerMessageCAN, SIGNAL(triggered()), this, SLOT(supprimerMessageCAN()));
	
        connect(actionSupprimerParametre, SIGNAL(triggered()), this, SLOT(supprimerParametre()));
	
	actionReplayActive->setCheckable(true);
	connect(actionReplayActive, SIGNAL(triggered(bool)), this, SLOT(basculeReadCANReplay(bool)));
        connect(actionReplayActive, SIGNAL(triggered(bool)), actionCharger_Replay, SLOT(setEnabled(bool)));
        connect(actionReplayActive, SIGNAL(triggered(bool)), actionNouveau_Replay, SLOT(setEnabled(bool)));
        connect(actionReplayActive, SIGNAL(triggered(bool)), this, SLOT(menuReplay(bool)));

}

/**
@Version :  0.01a
@Dernière modification :  02/09/08
@Auteur : D. Duval
@Description : SLOT Affiche un message a titre informatif.  Onglet "Aide" -> " ? "
Lit le fichier Aide.txt
**/
void FenPrincipale::aide()
{
	QFile file( "txt/aide.txt" );	
    
	if( file.open( QIODevice::ReadOnly ) )
	{
		QTextStream ts( &file );
		QString text = ts.readAll(); // Lit tout le fichier 	
		QMessageBox::information(this, "Aide", text);
	}	
		
	file.close();
}


/*---------------------------------------------------
Slot qui reinitialise la vue 3D
---------------------------------------------------*/
/* void FenPrincipale::resetVue3d()
{
	fenetreGL->resetVue();
} */

/*---------------------------------------------------
Slot qui crée une fenetre pour l'ajout d'un message CAN
---------------------------------------------------*/
void FenPrincipale::ajouterMessageCAN(){
     
     FenAjouterMessageCAN *fenetreMessageCAN = new FenAjouterMessageCAN(this);
     fenetreMessageCAN->exec();

}

/*---------------------------------------------------
Slot qui crée une fenetre pour l'ajout d'un paramètre
---------------------------------------------------*/
void FenPrincipale::ajouterParametre(){
     
     FenAjouterParam *fenetreParametre = new FenAjouterParam(this);
     fenetreParametre->exec();

}

/*---------------------------------------------------
Slot qui crée une fenetre pour la suppression d'un message CAN
---------------------------------------------------*/
void FenPrincipale::supprimerMessageCAN(){
     
     FenSupprimerMessageCAN *fenetreMessageCAN = new FenSupprimerMessageCAN(this);
     fenetreMessageCAN->exec();

}

/*---------------------------------------------------
Slot qui crée une fenetre pour la suppression d'un paramètre
---------------------------------------------------*/
void FenPrincipale::supprimerParametre(){
     
     FenSupprimerParam *fenetreParametre = new FenSupprimerParam(this);
     fenetreParametre->exec();

}

/*---------------------------------------------------
Slot qui crée une fenetre pour la modification d'un message CAN
---------------------------------------------------*/
void FenPrincipale::modifierMessageCAN(){
     
     FenModifierMessageCAN *fenetreMessageCAN = new FenModifierMessageCAN(this);
     fenetreMessageCAN->exec();

}

/*---------------------------------------------------
Slot qui crée une fenetre pour la modification d'un paramètre
---------------------------------------------------*/
void FenPrincipale::modifierParametre(){
     
     FenModifierParam *fenetreParametre = new FenModifierParam(this);
     fenetreParametre->exec();

}

/*---------------------------------------------------
Slot : Fenêtre pour demander à l'utilisateur s'il veut créer un nouveau replay ou en charger un ancien.
---------------------------------------------------*/
void FenPrincipale::menuReplay(bool ok){
    if(ok)
    {
        FenReplay *fenetreReplay = new FenReplay(this);

        //connect(fenetreReplay, SIGNAL(startReplay()), fenetreCAN->readCANreplayThread, SLOT(start()));

        if(fenetreReplay->exec()) // Charger
        {
            emit(chargerReplay());
        }
        else // Nouveau
        {
            fenetreCAN->readCANreplayThread->start();
        }
    }
}



/*---------------------------------------------------
Slot qui crée une fenetre pour l'edition et l'envoi de séquences scriptées
---------------------------------------------------*/
void FenPrincipale::editeurScript(){
	 Script *fenetreScript = new Script(this);
	 connect(fenetreScript,SIGNAL(envoyerMessageCAN(Uint8*)),fenetreCAN, SLOT(ecritureMessageCAN(Uint8*)));
	 
	connect(fenetreCAN->readCANThread,SIGNAL(messageCANRecu(Uint8*)),this, SLOT(basculeReadScript(Uint8*)),Qt::DirectConnection);
	 connect(this, SIGNAL(basculeScript(Uint8*)), fenetreScript, SLOT(lireMessage(Uint8*)),Qt::DirectConnection);	
	fenetreScript->exec();
	disconnect(fenetreScript,0,0,0);
	disconnect(fenetreCAN->readCANThread,SIGNAL(messageCANRecu(Uint8*)),this, SLOT(basculeReadScript(Uint8*)));
}


/*---------------------------------------------------
Slot qui bascule entre la réception de message CAN (par défaut activé), et la réception des données du dernier match (mode Replay).
---------------------------------------------------*/
void FenPrincipale::basculeReadCANReplay(bool replay){
	
	//Si on était en mode Replay
	if(!replay){
		actionReplayActive->setChecked(false);
                replayTime->setEnabled(false);
                replaySlider->setEnabled(false);
		emit(basculeReadCAN());
		
	}
	else{ //Si on était en mode normal
		actionReplayActive->setChecked(true);
                replayTime->setEnabled(true);
                replaySlider->setEnabled(true);
                replaySlider->setValue(0);
		emit(basculeReplay());
	}
}

/*---------------------------------------------------
Slot qui bascule vers le slot de reception de messages du script.
---------------------------------------------------*/
void FenPrincipale::basculeReadScript(Uint8* buffer){
	emit(basculeScript(buffer));
}

/*---------------------------------------------------
Slot qui modifie la portée maximale du ReplaySlider.
---------------------------------------------------*/
void FenPrincipale::modifierMaxSlider(int new_max){
    replaySlider->setMaximum(new_max);
}

/*---------------------------------------------------
Slot qui désactive puis réactive le Thread de replay.
---------------------------------------------------*/
void FenPrincipale::nouveauReplay(){
    emit(stopReplayThread());
    // Attendre que le thread soit interrompu pour le redémarrer
    //qSleep(200);  Qt 4.5 Only :(
    for(int i=0;i<1000000;i++);
    emit(newReplay());
}




