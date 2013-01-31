/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenCAN.cpp
 *	Package : Clickodrome
 *	Description : gere lenvoi et la reception de messages CAN
 *	Auteur : Aurélie Ottavi && Damien Duval
 *	Version 20090731
 */
 
#include "FenCAN.h"

/*---------------------------------------------------
Constructeur: Creation et Initialisation du widget
---------------------------------------------------*/
FenCAN::FenCAN (QWidget *parent) : QWidget(parent)
{

	//initialisation du timer ASSER_TELL_POSITION
	 tellTimer = new QTimer(this);
        tellTimer->stop();

        //initialisation du timer BEACON_POS
        baliseTimer = new QTimer(this);
        baliseTimer->stop();
	
	//------------
	envoieDeplacementThread= new EnvoieDeplacementThread();
	
	connexion=new RS232();
	
	readCANThread=new ReadCANThread();
	readCANThread->connexion =connexion ;
	
	readCANreplayThread=new ReadCANreplayThread();
	readCANreplayThread->connexion =connexion ;
	
	/*creation des widgets composant la fenetre*/
	//liste de message CAN
	messagesCAN=new QListWidget();
        messagesCAN -> setMinimumSize ( 190, 265 );
	
	//boite de texte pour l'affichage du dialogue entre le logiciel et le robot
	messagesRecus= new QTextEdit();
	messagesRecus->setReadOnly(true);

	//boutons pour modifier le port RS232
	port = new QPushButton("Modifier");
	refresh = new QPushButton("Rafraichir");
	
	//bouton d'envoi d'un message CAN
	envoyer = new QPushButton("Envoyer");

	//bouton pour effacer la fenetre de messages
	effacer = new QPushButton("Effacer");
	
	// ajout dans la combo box des differents messages CAN par lecture du fichier messagesCAN.xml

	QDomDocument doc( "messages CAN" );
    QFile file( XML_MESSAGES_CAN );
	
    if( file.open(QIODevice::ReadOnly) )
    {  
        if( doc.setContent( &file ) )
        {           
            file.close();
                        
            QDomElement root = doc.documentElement();
            
            if( root.tagName() == "listeMessageCAN" )
			{
                        
                QDomNode n = root.firstChild();  
                while( !n.isNull() )
                {
					QDomElement e = n.toElement();
					if( !e.isNull() )
					{
						if( e.tagName() == "messageCAN" )
						{   
							messagesCAN -> addItem(e.attribute( "nom", "" ));
						}
					}
					   
					  n = n.nextSibling();
                }
            }
        }
    }

	// ajout des differents parametres sous forme de spinBox ou de comboBox par lecture du fichier parametres.xml
    QDomDocument doc2( "parametres" );
    QFile file2( XML_PARAM_CAN );
 

	formLayout = new QFormLayout;


    int i=0,j=0; 
    if( file2.open(QIODevice::ReadOnly) )
    {  
        if( doc2.setContent( &file2 ) )
        {           
            file2.close();
                        
            QDomElement root = doc2.documentElement();
            
            if( root.tagName() == "listeParametres" )
			{
                        
                QDomNode n = root.firstChild();
      
                while( !n.isNull() )
                {
					QDomElement e = n.toElement();
					if( !e.isNull() )
					{
						if( e.tagName() == "parametre" )
						{   
							// ajout des widgets correspondant aux parametres en fonction du type de ce dernier
							QString nomParam=e.attribute( "nom", "" );
                            QDoubleSpinBox *dspinBox;
                            QSpinBox *spinBox;
                            QComboBox *comboBox;
                            
                            
                            QStringList test;
                            test << "Double"<<"Int"<<"Booleen"<<"String";
							switch(test.indexOf(e.attribute( "type", "" )))
							{
								case 0:dspinBox=new QDoubleSpinBox(this);	
												dspinBox->setWhatsThis ("Double");
												dspinBox->setAccessibleDescription(nomParam);
												parametresSpin.push_back(dspinBox);
												formLayout->addRow(nomParam +" : ",parametresSpin[i]);
												i++;
										break;
										
								case 1:spinBox=new QSpinBox(this);
                                                                        spinBox->setMaximum(MAX_COORDONNEE);
                                                                        if(e.attribute( "nom", "" )=="A")
                                                                        {
                                                                            spinBox->setRange(-180,180);
                                                                        }
                                                                        spinBox->setWhatsThis ("Int");
                                                                        spinBox->setAccessibleDescription(nomParam);
                                                                        parametresSpin.push_back(spinBox);
                                                                        formLayout->addRow(nomParam +" : ",parametresSpin[i]);
                                                                        i++;
										break;
									
								case 2:	comboBox=new QComboBox(this);
												comboBox ->addItem("Vrai");
												comboBox ->addItem("Faux");
												comboBox->setWhatsThis ("Booleen");
												comboBox->setAccessibleDescription(nomParam);
												parametresCombo.push_back(comboBox);
												formLayout->addRow(nomParam +" : ",parametresCombo[j]);
												j++;
										break;
										
								case 3: comboBox=new QComboBox(this);
												int i=0;
												QString varNomChoix="string0";
												while(e.attribute(varNomChoix, "" )!=NULL)
												{
													comboBox ->addItem(e.attribute(varNomChoix, "" ));
													i++;
													varNomChoix="string"+QString::number(i);
												}
												
												
												comboBox->setWhatsThis ("String");
												comboBox->setAccessibleDescription(nomParam);
												parametresCombo.push_back(comboBox);
												formLayout->addRow(nomParam +" : ",parametresCombo[j]);
												j++;
										break;

                                                                // default : break ; // Empeche de compiler o_O ?!
								
							}
						}
					}

					n = n.nextSibling();
                }
            }
        }
    }

    //organisation du layout
	listeMessagePredefinis=new QListWidget();
        listeMessagePredefinis -> setMinimumSize ( 190, 140 );

	
	// ajout dans la combo box des differents messages CAN Predéfinis par lecture du fichier messagesCANPredefini.xml
	QDomDocument doc3( "messages CAN Predefini" );
    QFile file3( XML_MESSAGES_PREDEF  );
	
    if( file3.open(QIODevice::ReadOnly) )
    {  
        if( doc3.setContent( &file3 ) )
        {           
            file3.close();
                        
            QDomElement root = doc3.documentElement();
            
            if( root.tagName() == "listeMessagePredefini" )
			{
                        
                QDomNode n = root.firstChild();  
                while( !n.isNull() )
                {
					QDomElement e = n.toElement();
					if( !e.isNull() )
					{
						if( e.tagName() == "messageCANPredefini" )
						{   
							// ajout dans la combo box
							listeMessagePredefinis -> addItem(e.attribute( "alias", "" ));
						}
					}
                   
					n = n.nextSibling();
                }
            }
        }
    }
		
	
	envoyer2=new QPushButton("Envoyer");
	enregistrer=new QPushButton("Enregistrer");
	editer=new QPushButton("Editer");
	
	
	QVBoxLayout *vboxMessCANGauche = new QVBoxLayout;
		vboxMessCANGauche -> addWidget(messagesCAN);
	
	QGroupBox *nom= new QGroupBox("Nom :");
		nom -> setLayout(vboxMessCANGauche);
	
	QGroupBox *parametres= new QGroupBox("Paramètres :");
        parametres->setLayout(formLayout);
	
	QVBoxLayout *vboxMessagesPredefinis = new QVBoxLayout;
           
        listeMessagePredefinis -> setMinimumSize ( 190, 140 );
		vboxMessagesPredefinis -> addWidget(listeMessagePredefinis);
		vboxMessagesPredefinis -> addWidget(envoyer2);
		vboxMessagesPredefinis -> addWidget(editer);

	
		
	
	QVBoxLayout *vboxConsole = new QVBoxLayout;
		messagesRecus -> setMinimumSize ( 235, 170 );
		vboxConsole -> addWidget(messagesRecus);
		vboxConsole -> addWidget(effacer);
	

	
	QGroupBox *console = new QGroupBox("Console :");
		console -> setLayout(vboxConsole);

	//sélecteur de PORTCOMx avec détection des ports 	    
	port=new QPushButton("Modifier");
	QFormLayout *formRS232 = new QFormLayout;
	numPort = new QComboBox(this);
	detecterPorts();
	formRS232->addWidget(numPort);
	formRS232 -> addWidget(port);
	formRS232 -> addWidget(refresh);
	QGroupBox *portRS232 = new QGroupBox("Port :");
		portRS232 -> setLayout(formRS232);
	//---
	
	QVBoxLayout *vboxMessCANCentre = new QVBoxLayout;
		vboxMessCANCentre ->addWidget(parametres);
		vboxMessCANCentre ->addWidget(envoyer);
		vboxMessCANCentre ->addWidget(enregistrer);	
		vboxMessCANCentre ->addWidget(portRS232);
		vboxMessCANCentre ->addWidget(console);
	
	
	
    
    QGroupBox *messagesPredefinis= new QGroupBox("Messages Predefinis :");
		messagesPredefinis->setLayout(vboxMessagesPredefinis);
        
    QGroupBox *autoTellPosition= new QGroupBox("Auto ASSER_TELL_POSITION :");
        QVBoxLayout *autoTPLayer = new QVBoxLayout;
        activeTP=new QPushButton("Activer");
		autoTPLayer -> addWidget(activeTP);        
		autoTellPosition->setLayout(autoTPLayer);

        
    QVBoxLayout *vboxMessCANDroite = new QVBoxLayout;		
		vboxMessCANDroite ->addWidget(autoTellPosition);
		vboxMessCANDroite ->addWidget(messagesPredefinis);

    QVBoxLayout *vboxListesGauche = new QVBoxLayout;
                vboxListesGauche -> addWidget(nom);
                vboxListesGauche -> addLayout(vboxMessCANDroite);

        
        
    QHBoxLayout * hboxEnvoyerMessCAN= new QHBoxLayout;
        hboxEnvoyerMessCAN-> addLayout(vboxListesGauche);
        hboxEnvoyerMessCAN-> addLayout(vboxMessCANCentre);
        
	QGroupBox *envoyerMessCAN = new QGroupBox("Envoyer message CAN :");
		envoyerMessCAN -> setLayout(hboxEnvoyerMessCAN);

	QHBoxLayout *layoutPrincipal = new QHBoxLayout;
		layoutPrincipal->addWidget(envoyerMessCAN);

    resize(529,640);
    setLayout(layoutPrincipal);

	//initialisation des widget et connection entre signaux et slots
	initialisation();
	
}

/*---------------------------------------------------
Initialisation des widget et connection entre signaux et slots
---------------------------------------------------*/
void FenCAN::initialisation()
{
		
	for(int i=0;i<(int)parametresSpin.size();i++)
		parametresSpin[i]->setEnabled(false);
		
	for(int i=0;i<(int)parametresCombo.size();i++)
		parametresCombo[i]->setEnabled(false);

	//messagesCAN -> setCurrentIndex(-1);
	
	//Connexions
	// messageCAN widget du Qlist __ A la place du this -> instance de la classe avec la methode d'apres
	// faire dans fenetre principale daéns l'initialisation ou constructeur
	//connect(fenetreListe -> messagesCAN, SIGNAL(currentRowChanged(int)), fenetreCAN, SLOT(on_messagesCAN_currentIndexChanged(int)));
	connect(messagesCAN, SIGNAL(currentRowChanged(int)), this, SLOT(on_messagesCAN_currentIndexChanged(int)));
	connect(listeMessagePredefinis, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(on_envoyer2_clicked())); // Gère DoubleClick ET Entrée(sous Windows)
	connect(envoyer2, SIGNAL(clicked()), this, SLOT(on_envoyer2_clicked()));	
        connect(envoyer, SIGNAL(clicked()), this, SLOT(on_envoyer_clicked()));
        connect(messagesCAN, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(on_envoyer_clicked())); // Gère DoubleClick ET Entrée(sous Windows)
	connect(effacer, SIGNAL(clicked()), this, SLOT(on_effacer_clicked()));		
	connect(editer, SIGNAL(clicked()), this, SLOT(on_editer_clicked()));    	
	connect(activeTP, SIGNAL(clicked()), this, SLOT(switchAutoTellPosition()));	
	connect(port, SIGNAL(clicked()),this,SLOT(on_modifierRS232_clicked()));
	connect(refresh, SIGNAL(clicked()),this,SLOT(detecterPorts()));
	connect(enregistrer, SIGNAL(clicked()), this, SLOT(on_enregistrer_clicked())); 
	connect(readCANThread, SIGNAL(messageCANRecu(Uint8*)), this, SLOT(receiveMessageCAN(Uint8*)),Qt::DirectConnection);
	connect(readCANThread, SIGNAL(messageRecu(Uint8*,int)), this, SLOT(receiveMessage(Uint8*,int)));
	connect(this, SIGNAL(stopThreadReadCAN()), readCANThread, SLOT(interrompre()));
	connect(this, SIGNAL(stopThreadReplay()), readCANreplayThread, SLOT(interrompre()));
	connect(this, SIGNAL(trajectoireFinieSignal()),this,SLOT(trajectoireFinieTrue()));
	connect(envoieDeplacementThread, SIGNAL(ecrireMessage(Uint8*)),this,SLOT(ecritureMessageCAN(Uint8*)));
	connect(envoieDeplacementThread, SIGNAL(envoieMessageCANString(QString,QString)),this,SLOT(afficherMessageCANString(QString,QString)));
        connect(tellTimer, SIGNAL(timeout()), this, SLOT(autoTellPosition()));
        connect(baliseTimer, SIGNAL(timeout()), this, SLOT(autoBalisePosition()));
}

/*--------------------------------------------------------------------------------------------
Slot qui permet de modifier ou non les valeurs des perametres en fonction du message CAN séléctionné
-----------------------------------------------------------------------------------------------*/
void FenCAN::on_messagesCAN_currentIndexChanged(int machin)
{

	int i=0,j=0;

	//efface tous les parametres
	for(int i=0;i<(int)parametresSpin.size();i++)
	{
			delete (formLayout->labelForField ( parametresSpin[i]) ); // détruit le label du paramètre
			delete parametresSpin[i];
	}
	
	for(int i=0;i<(int)parametresCombo.size();i++)
	{
			delete (formLayout->labelForField ( parametresCombo[i]) ); // détruit le label du paramètre
			delete parametresCombo[i];
	}
	parametresSpin.clear();
	parametresCombo.clear();

    /*Cherche  le message CAN selectioné dans le fichier xml et active les parametres si ils lui sont utiles */
	
	//renvoi le noeud ou se trouve le message CAN recherché
    QDomNode n=returnNodeMessCAN("nom",messagesCAN -> currentItem () ->text());
   
	
	QDomNode parametre = n.firstChild();
									
	while(!parametre.isNull())
	{
		QDomElement eParametre = parametre.toElement();
		if( eParametre.tagName() == "parametre" )
		{
			// ajout des widgets correspondant aux parametres en fonction du type de ce dernier
			QString nomParam=eParametre.attribute( "nom", "" );
            QDoubleSpinBox *dspinBox;
            QSpinBox *spinBox;
            QComboBox *comboBox;
            
            QStringList test;
            test << "Double"<<"Int"<<"Booleen"<<"String";
            switch(test.indexOf(eParametre.attribute( "type", "" )))
			{
				case 0: dspinBox=new QDoubleSpinBox(this);
								dspinBox->setWhatsThis ("Double");
								dspinBox->setAccessibleDescription(nomParam);
								parametresSpin.push_back(dspinBox);
								formLayout->addRow(nomParam +" : ",parametresSpin[i]);
								i++;
						break;
						
				case 1: spinBox=new QSpinBox(this);
								spinBox->setMaximum(MAX_COORDONNEE);
								if(eParametre.attribute( "nom", "" )=="A")
								spinBox->setRange(-180,180);
								spinBox->setWhatsThis ("Int");
								spinBox->setAccessibleDescription(nomParam);
								parametresSpin.push_back(spinBox);
								formLayout->addRow(nomParam +" : ",parametresSpin[i]);
								i++;
						break;
                        
				case 2: comboBox=new QComboBox(this);
								comboBox ->addItem("Vrai");
								comboBox ->addItem("Faux");
								comboBox->setWhatsThis ("Booleen");
								comboBox->setAccessibleDescription(nomParam);
								parametresCombo.push_back(comboBox);
								formLayout->addRow(nomParam +" : ",parametresCombo[j]);
								j++;
						break;
						
				case 3: comboBox=new QComboBox(this);
								int i=0;
								QString varNomChoix="string0";
								while(eParametre.attribute(varNomChoix, "" )!=NULL)
								{
									comboBox ->addItem(eParametre.attribute(varNomChoix, "" ));
									i++;
									varNomChoix="string"+QString::number(i);
								}
								comboBox->setWhatsThis ("String");
								comboBox->setAccessibleDescription(nomParam);
								parametresCombo.push_back(comboBox);
								formLayout->addRow(nomParam +" : ",parametresCombo[j]);
								j++;
						break;

                         //default : break ; // Empeche de compiler o_O ?!
			}

		}										
		parametre = parametre.nextSibling();
	}

}

void FenCAN::on_effacer_clicked()
{
	messagesRecus ->clear();
}
/*-----------------------------------------------------------------------------------------------------------
Slot qui permet d'afficher le message envoyé au robot ainsi que ses paramètres lorsque l'on clique sur le bouton Envoyer
-------------------------------------------------------------------------------------------------------------*/
void FenCAN::on_envoyer_clicked()
{
	if(messagesCAN->currentItem()!=NULL)
	{
		bool theintok;//retour de toInt vers une base 16
		cout << "Debut Envoie Message CAN\n";
		
		messagesRecus->setTextColor (Qt::black);
		//affichage du message CAN
		messagesRecus->append(messagesCAN -> currentItem () ->text());


		QString data="";
		
		/*affichage des paramètres et de leur valeur*/
		for(int i=0;i<(int)parametresSpin.size();i++)
		{
			if(parametresSpin[i]->isEnabled()) 		
				data="DATA -> ";
		}
		
		for(int i=0;i<(int)parametresCombo.size();i++)
		{
			if(parametresCombo[i]->isEnabled()) 		
				data="DATA -> ";
		}
		
		for(int i=0;i<(int)parametresSpin.size();i++)
		{	
			if(parametresSpin[i]->isEnabled()) 
			{
				data+=parametresSpin[i]->accessibleDescription() + " : " +parametresSpin[i]->text()+" ";
			}
		}
			
		for(int i=0;i<(int)parametresCombo.size();i++)
		{
			if(parametresCombo[i]->isEnabled()) 
			{
				data+=parametresCombo[i]->accessibleDescription()+" : "+parametresCombo[i]->currentText()+" ";
			}
		}
		afficherMessage(data);
		
			
		//on test si l'utilisateur n'a pas cliqué sur envoyé sans avoir séléctionné de message auparavant
		if(messagesCAN -> currentItem () ->text().compare("")!=0)
		{
		
			// recuperation du code du message envoyé
			QString code;

			QDomNode n=returnNodeMessCAN("nom",messagesCAN -> currentItem () ->text());
			QDomElement e = n.toElement();
			code=e.attribute( "code", "" );

			// tableau d'octets pour les messages à envoyé
			Uint8 message[13];
			
			//debut de message = SOH
			message[0]= (Uint8)CAN_SOH;
			
			//on coupe le code du message en deux 
			Uint8 code0=(Uint8)((code.section ( "", 3, 3).toInt(&theintok,16))*16+code.section ( "", 4, 4).toInt(&theintok,16));
			Uint8 code1=(Uint8)((code.section ( "", code.length()-1, code.length()-1).toInt(&theintok,16))*16+code.section ( "", code.length(), code.length()).toInt(&theintok,16));
			
			// debut du code du message (sur 2 bytes)
			message[1]=code0;
			
			// fin du code du message (sur 2 bytes)
			message[2]=code1;  //4 au lieu de 1
			
			//Compte le nombre de données à envoyer
			int nb =0; 
			
			double angle;
			
			QString taille;
			
			//on parcourt le tableau des parametres de type int et double
			for(int i=0;i<(int)parametresSpin.size();i++)
			{
			
				//si le parametre est activé 
				if(parametresSpin[i]->isEnabled())
				{
				
					//recherche dans le fichier parametres.xml le parametre en cours de traitement et recupere l'attribut taille pour savoir sur combien d'Uint8 il est codé
					QDomDocument doc( "parametres" );
					QFile file( XML_PARAM_CAN );

					if( file.open(QIODevice::ReadOnly) )
					{  
						if( doc.setContent( &file ) )
						{           
							file.close();
										
							QDomElement root = doc.documentElement();
							
							if( root.tagName() == "listeParametres" )
							{
										
								QDomNode n = root.firstChild();     
								while( !n.isNull() )
								{
									QDomElement e = n.toElement();
									if( !e.isNull() )
									{
										if( e.tagName() == "parametre" )
										{   
											if(e.attribute( "nom", "" )== parametresSpin[i]->accessibleDescription())
											{	
												
												taille=e.attribute( "taille", "" );
												cout<<taille.toStdString();
											}
										}
									}
									n = n.nextSibling();
								}
							}
						}
					}
			
					//si le paramètre est un angle
					if(parametresSpin[i]->accessibleDescription()=="A")
					{
					
						//on calcul l'angle puis on le convertit en unsigned int de base 16 puis on le met dans le tableau à envoyer 
						angle=((parametresSpin[i]->text()).toInt()*2*PI/360.0)*4096;
						
						if(taille.compare("Uint8")==0)
						{
							message[nb+3]=(Uint8)(angle);
							nb++;
                        }
                        else 
                        {
							message[nb+3]=(Uint8)((int)angle/256);
							nb++;
							message[nb+3]=(Uint8)((int)angle%256);
							nb++;
						}						
                    }
                    else 
                    {				
						// si le parametre ne nécessite aucun traitement spéciale
						//on le met dans le tableau à envoyer 						
						if(taille.compare("Uint8")==0)
						{
							message[nb+3]=(Uint8)((parametresSpin[i]->text()).toInt());
							nb++;
                        }
                        else 
                        {				

							message[nb+3]=(Uint8)((parametresSpin[i]->text()).toInt()/256);
							nb++;
							message[nb+3]=(Uint8)((parametresSpin[i]->text()).toInt()%256);
							nb++;
						}
					}
						
				}
			}
			
			//on parcourt le tableau des parametres de type string
			for(int i=0;i<(int)parametresCombo.size();i++)
			{	
				//si le parametre est activé 
				if(parametresCombo[i]->isEnabled())
				{
				
					//recherche dans le fichier parametres.xml le parametre en cours de traitement et recupere l'attribut taille pour savoir sur combien d'Uint8 il est codé
					QDomDocument doc( "parametres" );
					QFile file( XML_PARAM_CAN );

					if( file.open(QIODevice::ReadOnly) )
					{  
						if( doc.setContent( &file ) )
						{           
							file.close();
										
							QDomElement root = doc.documentElement();
							
							if( root.tagName() == "listeParametres" )
							{
										
								QDomNode n = root.firstChild();     
								while( !n.isNull() )
								{
									QDomElement e = n.toElement();
									if( !e.isNull() )
									{
										if( e.tagName() == "parametre" )
										{   
											if(e.attribute( "nom", "" )== parametresCombo[i]->accessibleDescription())
											{	
												
												taille=e.attribute( "taille", "" );
												cout<<taille.toStdString();
											}
										}
									}
									n = n.nextSibling();
								}
							}
						}
					}
					//on le met dans le tableau à envoyer (on prend la valeur de l'indice de la comboBox)
					if(taille.compare("Uint8")==0)
					{
						message[nb+3]=(Uint8)(parametresCombo[i]->currentIndex()); // TODO : Modifier current index pour qu'on puisse attribuer une valeur à l'indice
						nb++;
					}else{
							message[nb+3]=(Uint8)(parametresCombo[i]->currentIndex()/256);
							nb++;
							message[nb+3]=(Uint8)(parametresCombo[i]->currentIndex()%256);
							nb++;
					}
					//permet d'initialiser la position du robot dans la vue 2d
					if(parametresCombo[i]->accessibleDescription()=="COULEUR")
					{
						if(messagesCAN -> currentItem () ->text().compare("BROADCAST_COULEUR")==0)
						{
							if (parametresCombo[i]->currentIndex()==0)
							{
								QPoint positionRobot=QPoint(ROBOT_INIT_Y1,ROBOT_INIT_X1);
								sendPositionRobot(positionRobot,ROBOT_INIT_A1);
							}
							else
                            {
								QPoint positionRobot=QPoint(ROBOT_INIT_Y2,ROBOT_INIT_X2);
								sendPositionRobot(positionRobot,ROBOT_INIT_A2);
							}
						}
					}
				}
			}
			
			//on met a 0 tous les champs inutilisés
			int j=nb;
			while (j<8)
			{
				message[j+3]=0;
				j++;
			}
			
			//nombre de parametres associés au message CAN
			message[j+3]=(Uint8)(nb);			
			//fin du message :EOT
			message[j+4]=(Uint8)CAN_EOT; 			
			//test pour voir ce que l'on envoi
			QString messageSend;
			for(int i=0;i<13;i++)
			{
				if((int)(message[i])<16)
				{
					messageSend+="0"+QString::number(message[i],16)+" ";
                }
                else 
                {				
                    messageSend+=QString::number(message[i],16)+" ";
                }
			}
			
			messagesRecus->append(messageSend);
			
			cout << "Ecriture message sur le CAN\n";
			//on ecrit le tableau à envoyer sur le port série
			connexion->writeCOM(message,13);
			

		}
		
	}else{
		//Creation d'une fenetre qui confirme l'ajout du message CAN
		cout<<"erreur";
		QDialog *erreur= new QDialog(this);
		erreur -> setWindowTitle("Erreur");
		
		QLabel *label = new QLabel(erreur);
		label ->setText("Il faut selectionner un message à envoyer");
	
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
			
		erreur ->setLayout (layoutPrincipal);
		erreur -> show();
		erreur -> resize(200,100);
		
	}
}

void FenCAN::on_envoyer2_clicked()
{
	bool theintok;//retour de toInt vers une base 16
	
	if(listeMessagePredefinis -> currentItem ()!=NULL)
	{
		QString data="";
		
		Uint8 message[13];
		
		int nb=0;
		int angle;
			
		//debut de message = SOH
		message[0]= (Uint8)CAN_SOH;
		
		cout << "Debut Envoie Message CAN Predefini\n";
		messagesRecus->setTextColor (Qt::black);
		
		//on parcours le fichier xml messagesCANPredefini pour récuperer les infos sur le message à envoyer
		
		QDomDocument doc( "messages CAN Predefini" );
		QFile file( XML_MESSAGES_PREDEF );
		
		if( file.open(QIODevice::ReadOnly) )
		{  
			if( doc.setContent( &file ) )
			{           
				file.close();
							
				QDomElement root = doc.documentElement();
				
				if( root.tagName() == "listeMessagePredefini" )
				{
							
					QDomNode n = root.firstChild();  
					while( !n.isNull() )
					{
						QDomElement e = n.toElement();
						if( !e.isNull() )
						{
							if( e.tagName() == "messageCANPredefini" )
							{   
								if(e.attribute( "alias", "" )==listeMessagePredefinis -> currentItem () ->text())
								{
									QString nom=e.attribute( "nom", "" );
									messagesRecus->append(nom);

									QString code=e.attribute( "code", "" );

									//on coupe le code du message en deux 
									Uint8 code0=(Uint8)((code.section ( "", 3, 3).toInt(&theintok,16))*16+code.section ( "", 4, 4).toInt(&theintok,16));
									Uint8 code1=(Uint8)((code.section ( "", code.length()-1, code.length()-1).toInt(&theintok,16))*16+code.section ( "", code.length(), code.length()).toInt(&theintok,16));

									// debut du code du message (sur 2 bytes)
									message[1]=code0;

									// fin du code du message (sur 2 bytes)
									message[2]=code1;  


									QDomNode n2 = n.firstChild(); 

									if(!n2.isNull())
									{
										data="DATA -> ";
									}
									while( !n2.isNull() )
									{
										QDomElement e2 = n2.toElement();
										if( e2.tagName() == "parametre")
										{
                                            QStringList test;
                                            QStringList test2;
                                            test << "Double"<<"Int"<<"Booleen"<<"String";
                                            switch(test.indexOf(e2.attribute( "type", "" )))
                                            {
                                                    case 0:;                                                        
                                                        break;
                                                    
                                                    case 1:
                                                        test2 << "A"<<"X"<<"Y";
                                                        switch(test2.indexOf(e2.attribute( "nom", "" )))
                                                        {
                                                            case 0://on calcul l'angle puis on le convertit en unsigned int de base 16 puis on le met dans le tableau à envoyer 
                                                                angle=(Uint8)(e2.attribute( "valeure", "" ).toInt(&theintok,16)*2*PI/360.0)*4096; 
                                                                message[nb+3]=(Uint8)((int)angle/256);
                                                                nb++;
                                                                message[nb+3]=(Uint8)((int)angle%256);
                                                                nb++;
                                                                break;
                                                            
                                                            case 1:;
                                                            case 2:message[nb+3]=(Uint8)(e2.attribute( "valeure", "" ).toInt(&theintok,16)/256);
                                                                    nb++;
                                                                    message[nb+3]=(Uint8)(e2.attribute( "valeure", "" ).toInt(&theintok,16)%256);
                                                                    nb++;
                                                                    break;
                                                            default:message[nb+3]=(Uint8)(e2.attribute( "valeure", "" ).toInt(&theintok,16));
                                                                    nb++;
                                                                    break;
                                                        }
                                                        break;
                                                    case 2:message[nb+3]=(Uint8)(e2.attribute( "valeure", "" ).toInt(&theintok,16));
                                                            nb++;
                                                        break;
                                                    case 3 ://on le met dans le tableau à envoyer (sous la forme d'un 0 ou d'un 1
                                                            message[nb+3]=(Uint8)(e2.attribute( "valeure", "" )).toInt(&theintok,16);
                                                            nb++;

                                                            //permet d'initialiser la position du robot dans la vue 2d
                                                            if(e2.attribute( "nom", "" )=="COULEUR")
                                                            {
                                                                if(e.attribute( "nom", "" )=="CARTE_ASSER_COULEUR")
                                                                {
                                                                    if (e2.attribute( "valeure", "" ).toInt(&theintok,16)==0)
                                                                    {
                                                                        QPoint positionRobot=QPoint(ROBOT_INIT_Y1,ROBOT_INIT_X1);
                                                                        sendPositionRobot(positionRobot,ROBOT_INIT_A1);
                                                                    }
                                                                    else 
                                                                    {				

                                                                        QPoint positionRobot=QPoint(ROBOT_INIT_Y2,ROBOT_INIT_X2);
                                                                        sendPositionRobot(positionRobot,ROBOT_INIT_A2);
                                                                    }
                                                                }
                                                            }
                                                        break;

                                                    default : break ;
                                                        												
                                            }

                                            data+=e2.attribute( "nom", "" )+":"+e2.attribute( "valeure", "" )+" ";

                                        }

                                        n2 = n2.nextSibling();
                                    }

                                }
                            }
                        }
       
                        n = n.nextSibling();
					}
				}
			}
		}
		
		//on met a 0 tous les champs inutilisé
        int j=nb;
        
                                                              cout<<nb<<endl;
        while (j<8)
        {
            message[j+3]=0;
            j++;
        }
        
        //nombre de parametre associés au message CAN
        message[11]=(Uint8)(nb);
        
        //fin du message :EOT        
        message[12]=(Uint8)CAN_EOT;
        
        //test pour voir ce que l'on envoie
        QString messageSend;
        for(int i=0;i<13;i++)
        {
            if((int)(message[i])<16)
            {
                messageSend+="0"+QString::number(message[i],16)+" ";
            }
            else messageSend+=QString::number(message[i],16)+" ";
        }
        
        messagesRecus->append(data);
        messagesRecus->append(messageSend);
        
        cout << "Ecriture message sur le CAN\n";
        //on ecrit le tableau à envoyer sur le port série
        connexion->writeCOM(message,13);
        

        
        /*test pour savoir combien d'octet ont été écrit*/
        //messagesRecus->append(QString::number(connexion -> getNbBytesWritten())) ;
        cout << "Redemarre thread lecture\n";
        cout << "test 1\n"<<message[j+4];
    }
    else 
    {				

		//Creation d'une fenetre qui confirme l'ajout du message CAN
        cout<<"erreur";
        QDialog *erreur= new QDialog(this);
        erreur -> setWindowTitle("Erreur");
        
        QLabel *label = new QLabel(erreur);
        label ->setText("Il faut selectionner un message à envoyer");
        QVBoxLayout *layoutPrincipal = new QVBoxLayout;
        layoutPrincipal->addWidget(label);
            
        erreur ->setLayout (layoutPrincipal);
        erreur -> show();
        erreur -> resize(200,100);
        
    }
}
/*------------------------------------------------------
Fonction pour afficher un message dans la boite de texte
--------------------------------------------------------*/
void FenCAN::afficherMessage(QString messageAAfficher)
{
	messagesRecus->append(messageAAfficher);

}


/*------------------------------------------------------
Slot qui effectue le changement de numero de port si celui ci est valide
--------------------------------------------------------*/
void FenCAN::receiveNumPort(int numPort)
{
	//on interromp la lecture de message CAN sur le port courrant
	emit(stopThreadReadCAN());
	//changement de port 
	if(connexion->setCOMnb(numPort))
    { 
		messagesRecus->append("Changement de port : COM"+QString::number(numPort));
	    
		//on modifie le port pour la lecture des messages et on redemarre le thread
		readCANThread->connexion =connexion ;
		readCANThread->start();
    }
    else 
    {				

        messagesRecus->append("Le port demandé n'a pu etre ouvert");
    }
}

/*-----------------------------------------------------------------------------------------------------------
Fonction qui permet d'afficher les messages non CAN reçus 
-------------------------------------------------------------------------------------------------------------*/
void FenCAN::receiveMessage(Uint8 *buffer, int sizeBuffer)
{
    //Les  messages reçus seront affiché en rouge pour les différencier des messages envoyés
	messagesRecus->setTextColor ( Qt::red );
	
	QString data;
	char buf[sizeBuffer];
	cout << "Reception message non CAN";
	QString string;
	if (sizeBuffer==1)
    {	
		sprintf(buf,"%c",buffer[0]);  data=QString::fromAscii(buf);
    }
    else 
    {				

		for(int i=0;i<sizeBuffer;i++)
        {
            sprintf(buf,"%c",buffer[i]);
            data+=QString::fromAscii(buf);
		}
	}
	messagesRecus->append(data);
}


/*-----------------------------------------------------------------------------------------------------------
Fonction qui permet de transformer  un message CAN au format Uint8 en une version comprehensible par l'utilisateur
-------------------------------------------------------------------------------------------------------------*/
void FenCAN::receiveMessageCAN(Uint8 *buffer)
{
	cout << "Debut traitement message CAN reçu\n";
	int angle=0;
	int x=0;
	int y=0;	
	
	QPoint positionRobot,pointMulti;
	//Les  messages reçus seront affiché en rouge pour les différencier des messages envoyés
	messagesRecus->setTextColor ( Qt::red );
	
	
	QString messageReceive="01 ";
	for(int i=0;i<12;i++)
    {
		if((int)(buffer[i])<16)
        {
			messageReceive+="0"+QString::number(buffer[i],16)+" ";
        }
        else 
        {				
            messageReceive+=QString::number(buffer[i],16)+" ";
        }
	}
	
	messagesRecus->append(messageReceive);
	
	//chaine de caractère ou les paramètres du message CAN et leur valeurs seront stocké après mise en forme
	QString data="";
		
	
	//recuperation et mise en forme du code du message 
	QString codeModif1=QString::number(buffer[0],16);
	while(codeModif1.size()<2)codeModif1="0"+codeModif1;	
	QString codeModif2=QString::number(buffer[1],16);
	while(codeModif2.size()<2)codeModif2="0"+codeModif2;
	
	QString code="0x"+codeModif1+codeModif2;
	
	//recuperation du nom du message CAN associé au code à l'aide du fichier messagesCAN.xml
	QDomNode n=returnNodeMessCAN("code",code);
	if(!(n.isNull () ))
    {
		QDomElement e = n.toElement();
		QString nom=e.attribute("nom", "" );
		
		
		//on met en forme le message CAN en fonction des paramètres qui lui sont associé (toujours d'apres le fichier messagesCAN.xml)
		QDomNode parametre = n.firstChild();
		
		int i=0;
		
		if(!parametre.isNull())data="DATA -> ";
		while(!parametre.isNull())
        {
			QDomElement eParametre = parametre.toElement();
			if( eParametre.tagName() == "parametre" )
            {
				//si le paramètre à mettre en forme est un string, on ne le traite pas de la même façon que les autres
				if(eParametre.attribute( "type", "" )=="String")
                {
					data+=eParametre.attribute( "nom", "" )+" : ";

					//il faut parcourir le fichier parametres.xml pour savoir quelles sont ses valeurs possibles (associées à 0 ou 1)
					QDomDocument doc2( "parametres" );
					QFile file2( XML_PARAM_CAN );
					if( file2.open(QIODevice::ReadOnly) )
					{  
						if( doc2.setContent( &file2 ) )
						{           
							file2.close();
										
							QDomElement root = doc2.documentElement();
							
							if( root.tagName() == "listeParametres" )
                            {
								QDomNode n = root.firstChild();
					  
								while( !n.isNull() )
								{
									QDomElement e = n.toElement();
									if( !e.isNull() )
									{
										if( e.tagName() == "parametre" )
										{   
											if(e.attribute( "nom", "" )==eParametre.attribute( "nom", "" )){
												QString str="string"+QString::number(buffer[i+2]);
												data+=e.attribute(str, "" )+" ";
												//else data+=e.attribute("string1", "" )+" ";
											}
										}
									}
								   
									n = n.nextSibling();
								}
							}
						}
					}
					//de plus le paramètre n'est codé que sur 1 octet contrairement aux autres qui sont sur deux octets
					i++;
                }
                else 
                {				

					// si le paramètre à mettre en forme est l'angle
                    if(eParametre.attribute( "nom", "" )=="A")
                    { 
                        signed short temp=buffer[i+2]*256+buffer[i+3];
                        
                        angle=(int)(round(((temp)*360.0)/(PI*2.0*4096.0)));
                        //pour retrouver la valeur de l'angle en degrés (marche pas)
                        data+=eParametre.attribute( "nom", "" )+" : "
                        +QString::number(angle);

            
                        //l'angle est codé sur deux octets
                        i+=2;
                    }
                    else 
                    {				
		
                        //pour les autres paramètres
                        if(eParametre.attribute( "nom", "" ).compare("X")==0) x=buffer[i+2]*256+buffer[i+3];
                        if(eParametre.attribute( "nom", "" ).compare("Y")==0) y=buffer[i+2]*256+buffer[i+3];
                        data+=eParametre.attribute( "nom", "" )+" : ";
                        data+=QString::number(buffer[i+2]*256+buffer[i+3])+" ";
                        //ils sont codés sur deux octets
                        i+=2;
                    }
                }										
                parametre = parametre.nextSibling();
            }	
		
            //affichage du nom du message CAN
            messagesRecus->append(nom);
            QStringList test;
            test << "DEBUG_ASSER_POINT_FICTIF"<<"CARTE_P_TRAJ_FINIE"<<"CARTE_P_POSITION_ROBOT"
                 <<"CARTE_P_ROBOT_FREINE"<<"CARTE_P_ASSER_ERREUR"<<"FOE_POS";
            switch(test.indexOf(nom))
            {
                case 0:
                        pointMulti.setX(x);
                        pointMulti.setY(y);
                        cout<<"DEBUG_ASSER_POINT_FICTIF:X:"<<pointMulti.x()<<"Y:"<<pointMulti.y();
                        emit(sendPointMulti(pointMulti));
                        break;
                case 1:
                        emit(trajectoireFinieSignal());
                                        //pas de break car on veut la position aussi                        
                case 2: 
                case 3:
                        positionRobot.setX(x);
                        positionRobot.setY(y);
                        emit(sendPositionRobot(positionRobot,angle));
                        break;

                case 4:
                        cout << "fin thread car asser erreur\n";
                        envoieDeplacementThread -> terminate();
                        break;
                case 5:
                        positionRobot.setX(x);
                        positionRobot.setY(y);
                        emit(sendPosWalle(positionRobot));
                        break;
                default: break;
            }
		}
    }
    else 
    {				

        data+=QString("Message CAN inconnu");
    }
	
	//affichage des paramètres du message CAN
	messagesRecus->append(data);
	
	cout << "Fin traitement message CAN reçu\n\n";
}



void FenCAN::envoiDeplacement(QPoint *points, int pointsNumber, int angle,bool angleAenvoyer)
{
	tellTimer->stop();
        baliseTimer->stop();
	envoieDeplacementThread -> points= points;
	envoieDeplacementThread -> angle=angle;
	envoieDeplacementThread -> pointsNumber= pointsNumber;
	envoieDeplacementThread -> angleAenvoyer=angleAenvoyer;
	envoieDeplacementThread -> run();
	trajectoireFinieTrue();
}

void FenCAN::trajectoireFinieTrue()
{
	envoieDeplacementThread->trajectoireFinie=1;	
}

void FenCAN::afficherMessageCANString(QString nom,QString parametres)
{
	
	//Les  messages reçus seront affiché en rouge pour les différencier des messages envoyés
	messagesRecus->setTextColor ( Qt::black );	
	//affichage du nom du message CAN
	messagesRecus->append(nom);
	//affichage des paramètres du message CAN
	messagesRecus->append(parametres);
	

}

void FenCAN::ecritureMessageCAN(Uint8 *message)
{

	//test pour voir ce que l'on envoie
	QString messageSend;
	for(int i=0;i<13;i++)
    {
		if((int)(message[i])<16)
        {
			messageSend+="0"+QString::number(message[i],16)+" ";
        }
        else 
        {				
            messageSend+=QString::number(message[i],16)+" ";
        }
	}
	
	messagesRecus->append(messageSend);
	
	connexion->writeCOM(message,13);
}

void FenCAN::basculeThreadReplay()
{
	emit(stopThreadReadCAN());
        //readCANreplayThread->start();
}

void FenCAN::basculeThreadReadCAN()
{
	emit(stopThreadReplay());
	readCANThread->start();
}

void FenCAN::on_editer_clicked()
{
	cout<<"ici";
	if(listeMessagePredefinis -> currentItem ()!=NULL)
    {
		cout << "Edition Message CAN \n";
		FenEditer *fenEditer=new FenEditer(this,listeMessagePredefinis -> currentItem () ->text());
		fenEditer->ancienAlias=new QString(listeMessagePredefinis -> currentItem () ->text());
		fenEditer -> exec();
        
	}
    else
    {
		//Creation d'une fenetre qui confirme l'ajout du message CAN
		cout<<"erreur";
		QDialog *erreur= new QDialog(this);
		erreur -> setWindowTitle("Erreur");
		
		QLabel *label = new QLabel(erreur);
		label ->setText("Il faut selectionner un message à editer");
	
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
			
		erreur ->setLayout (layoutPrincipal);
		erreur -> show();
		erreur -> resize(200,100);
		
	}
}

void FenCAN::on_enregistrer_clicked()
{

	if(messagesCAN->currentItem()!=NULL)
    {
		cout << "Enregistrement Message CAN\n";
		FenEnregistrer *fenEnregistrer=new FenEnregistrer(this);
		fenEnregistrer->nom=new QString(messagesCAN -> currentItem () ->text());

		QDomNode n=returnNodeMessCAN("nom",messagesCAN -> currentItem () ->text());
		QDomElement e = n.toElement();

		fenEnregistrer->code=new QString(e.attribute( "code", "" ));
		int nb=0;cout<<"ici";
		for(int i=0;i<(int)parametresSpin.size();i++)
        {	
			if(parametresSpin[i]->isEnabled())
            {
				fenEnregistrer->parametres[nb]->nom=parametresSpin[i]->accessibleDescription();
				fenEnregistrer->parametres[nb]->valeur=(parametresSpin[i]->text()).toFloat();
				fenEnregistrer->parametres[nb]->type=parametresSpin[i]->whatsThis();
				nb++;
			}
		}
		
		for(int i=0;i<(int)parametresCombo.size();i++)
        {
			if(parametresCombo[i]->isEnabled()) 
            {
				fenEnregistrer->parametres[nb]->nom=parametresCombo[i]->accessibleDescription() ;
				fenEnregistrer->parametres[nb]->valeur=(float)parametresCombo[i]->currentIndex();
				fenEnregistrer->parametres[nb]->type=parametresCombo[i]->whatsThis();
				nb++;
			}
		}
		
		fenEnregistrer->nbParametre=nb;
		fenEnregistrer -> exec();
	}
	else 
    {
        //Creation d'une fenetre qui confirme l'ajout du message CAN
		cout<<"erreur";
		QDialog *erreur= new QDialog(this);
		erreur -> setWindowTitle("Erreur");
		
		QLabel *label = new QLabel(erreur);
		label ->setText("Il faut selectionner un message à enregistrer");
	
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
			
		erreur ->setLayout (layoutPrincipal);
		erreur -> show();
		erreur -> resize(200,100);
		
	}
}
//Auto ASSER_TELL_POSSITION
void FenCAN::switchAutoTellPosition()
{
    //activation du telltimer si non actif et vice versa
    if(tellTimer->isActive())
    {
        tellTimer->stop();
        activeTP->setText("Activer");
    }
    else
    {
        activeTP->setText("Désactiver");
        tellTimer->start(TIME_TELL_POSITION);
    }
	
}


//interruption du telltimer
void FenCAN::autoTellPosition()
{

	Uint8 asserTellPosition[13];
	asserTellPosition[0]=CAN_SOH;
	asserTellPosition[1]=CAN_ASSER_TELL_POSITION_1;
	asserTellPosition[2]=CAN_ASSER_TELL_POSITION_2;
	for(int i=3;i<12;i++)
	{
		asserTellPosition[i]=0x00;	
	}
	asserTellPosition[12]=CAN_EOT;
	//on ecrit le tableau à envoyer sur le port série
	connexion->writeCOM(asserTellPosition,13);
	tellTimer->start(TIME_TELL_POSITION);
	
}

/*---------------------------------------------------
[Slot] appelé lorqu'on place le robot adverse sur le terrain
---------------------------------------------------*/
void FenCAN::basculeBaliseOn(QPoint position)
{
    baliseTimer->stop();
    walle_pos = position ;
    baliseTimer->start(TIME_BEACON_POS);
}

/*---------------------------------------------------
[Slot] appelé lorqu'onenlève le robot adverse sur le terrain
---------------------------------------------------*/
void FenCAN::basculeBaliseOff()
{
    baliseTimer->stop();
}

/*---------------------------------------------------
//interruption du balisetimer
---------------------------------------------------*/
void FenCAN::autoBalisePosition()
{

        Uint8 beaconPos[13];
        beaconPos[0]=CAN_SOH;
        beaconPos[1]=CAN_BEACON_POS_1;
        beaconPos[2]=CAN_BEACON_POS_2;
        beaconPos[3]=walle_pos.x()/255;
        beaconPos[4]=walle_pos.x()%255;
        beaconPos[5]=walle_pos.y()/255;
        beaconPos[6]=walle_pos.y()%255;
        for(int i=7;i<11;i++)
        {
                beaconPos[i]=0x00;
        }
        beaconPos[11]=0x04;
        beaconPos[12]=CAN_EOT;
        //on ecrit le tableau à envoyer sur le port série
        connexion->writeCOM(beaconPos,13);
        baliseTimer->start(TIME_BEACON_POS);
		/*
        for(int i=0 ; i<=12 ; i++){
            cout << QString::number(beaconPos[i],16).toInt() << " ";
        }
        cout << endl;*/
}

/*---------------------------------------------------
Slot pour détecter les ports viables[SLOT]
---------------------------------------------------*/
void FenCAN::detecterPorts()
{
	bool enabled;	
	char COMitem[6];
	numPort->clear();
	for(int x=0;x<=MAX_NUM_PORT;x++)
	{
		//test du port
		enabled=connexion->isOpenable(x);
		if(enabled)
		{
			//ajout de l'item
			sprintf(COMitem,"COM%d",x);
			numPort -> addItem(COMitem);
			Sleep(100);
		}
	}
}
//Modification du port COM
void FenCAN::on_modifierRS232_clicked()
{

	//on recupere la valeur du champ numPort
	int i_numPort;
	string Port=(numPort->currentText()).toStdString();
	sscanf(Port.c_str(),"COM%d",&i_numPort);

	//On envoi un signal à la fenetre qui gere la connexion rs232 (FenCAN)
	emit(receiveNumPort(i_numPort));

}

void FenCAN::stopRead()
{
	emit(stopThreadReadCAN());
	emit(stopThreadReplay());
}
