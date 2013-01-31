/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenChangerNumRS232.cpp
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de changer le numéro de port sur lequel on établie la connexion RS232
 *	Auteur : Aurélie Ottavi
 *	Version 20081025
 */
 
 
 #include "FenChangerNumRS232.h"
 
 FenChangerNumRS232::FenChangerNumRS232(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Changer numéro du port de la connexion série");
    
	//Creation des widgets
	
	numPort=new QSpinBox(this);
    valider=new QPushButton("Modifier");   
   
	
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Numéro de port", numPort);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);// Ajout du layout de formulaire
    layoutPrincipal->addWidget(valider);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	// Connexion des  signaux aux slots
    connect(valider, SIGNAL(clicked()), this, SLOT(on_valider_clicked()));

}

//Slot qui envoi un signal avec comme paramètre le numéro du port choisi quand l'utilisateur à valider le changement de port
void FenChangerNumRS232::on_valider_clicked(){

//on recupere la valeur du champ numPort
int i_numPort=numPort->value();

//On envoi un signal à la fenetre qui gere la connexion rs232 (FenCAN)
emit(sendNumPort(i_numPort));
//on quitte la fenêtre	
		QCloseEvent close;
		closeEvent (&close);

}
