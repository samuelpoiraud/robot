/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenChangerNumRS232.cpp
 *	Package : Clickodrome
 *	Description : cr�e une fenetre qui permet de changer le num�ro de port sur lequel on �tablie la connexion RS232
 *	Auteur : Aur�lie Ottavi
 *	Version 20081025
 */
 
 
 #include "FenChangerNumRS232.h"
 
 FenChangerNumRS232::FenChangerNumRS232(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Changer num�ro du port de la connexion s�rie");
    
	//Creation des widgets
	
	numPort=new QSpinBox(this);
    valider=new QPushButton("Modifier");   
   
	
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Num�ro de port", numPort);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);// Ajout du layout de formulaire
    layoutPrincipal->addWidget(valider);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	// Connexion des  signaux aux slots
    connect(valider, SIGNAL(clicked()), this, SLOT(on_valider_clicked()));

}

//Slot qui envoi un signal avec comme param�tre le num�ro du port choisi quand l'utilisateur � valider le changement de port
void FenChangerNumRS232::on_valider_clicked(){

//on recupere la valeur du champ numPort
int i_numPort=numPort->value();

//On envoi un signal � la fenetre qui gere la connexion rs232 (FenCAN)
emit(sendNumPort(i_numPort));
//on quitte la fen�tre	
		QCloseEvent close;
		closeEvent (&close);

}
