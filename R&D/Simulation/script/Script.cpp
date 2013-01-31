/*
 *	Club Robot ESEO 2009 - 2010
 *	Archi-Tech'
 *
 *	Fichier : Script.cpp
 *	Package : Clickodrome
 *	Description : Utilise le fichier script.xml pour envoyer une séquence de messages CAN au robot avec attente (par évenement ou temporisation).
 *	Auteur :  Damien DUVAL
 *	Modification : Adrien GIRARD
 *	Version 20091228
 */
#include "Script.h"


/*---------------------------------------------------
Constructeur
---------------------------------------------------*/
Script::Script(QWidget *parent) : QDialog(parent)
{
	setupUi(this); // Utilise ll'interface utilisateur ui_script.h
		
	numero_etape = PREMIER_ETAPE; // numéro de la première étape
	message_envoye = true; // utilisé pour n'envoyer qu'un message à la fois
    nom_fichier = SCRIPT_FILE;
	script_modifie=false;
	xmlVersEditeur();
	
    


	// ajout dans la combo box des differents messages CAN par lecture du fichier messagesCAN.xml

	QDomDocument doc( "messages CAN" );
	QFile file( XML_MESSAGES_CAN );

	if( file.open(QIODevice::ReadOnly) )
	{
		if( doc.setContent( &file ) )
		{
			file.close();

			QDomElement root = doc.documentElement();

			if( root.tagName() == "listeMessageCAN" ){

				QDomNode n = root.firstChild();
				while( !n.isNull() )
				{
				  QDomElement e = n.toElement();
				  if( !e.isNull() )
				  {
					if( e.tagName() == "messageCAN" )
					{
						pListeMessages -> addItem(e.attribute( "nom", "" ));
					}
				  }

				  n = n.nextSibling();
				}
			}
		}
	}

}



/*---------------------------------------------------
Traduit le fichier xml dans la zone d'édition
---------------------------------------------------*/
void Script::xmlVersEditeur()
{
	QDomDocument doc( "script" );
	QFile file(nom_fichier);

	if( file.open(QIODevice::ReadOnly) )
	{
		if( doc.setContent( &file ) )
		{
			file.close();

			QDomElement root = doc.documentElement();
			int nombreElements = nombre_etapes();

			if( root.tagName() == "listeEtapes" ){

				QDomNode n ;

				for(int numero=1 ; numero<=nombreElements ; numero++)
				{
					n = root.firstChild();

					while( !n.isNull() )
					{
						QDomElement e = n.toElement();
						if( !e.isNull() )
						{
							if( e.tagName() == "etape"  && (e.attribute( "num", "" ).toInt() == numero) )
							{
								QString ligne= e.attribute( "num", "" ) +"; " ;
								ligne += "t = " + e.attribute( "t", "") +"; " ;
								if(e.hasAttribute("eSID"))
									ligne += "eSID = " + e.attribute( "eSID", "") +"; " ;
								ligne += "SID = " + e.attribute( "SID", "") +"; " ;
								ligne += "DATA = " + e.attribute( "data", "") +";" ;
								//ligne += "SIZE = " + e.attribute( "size", "") +";" ;
								editeur->append(ligne); // affichage dans la fenêtre
							}
						}
						n = n.nextSibling();
					}
				}
			}
		}
	}
	file.close();
}

/*---------------------------------------------------
Renvoi le nombre d'étapes dans le fichier XML
---------------------------------------------------*/
int Script::nombre_etapes()
{
	int nombreEtapes = 0;

	QDomDocument doc( "script" );
	QFile file( nom_fichier );

	if( file.open(QIODevice::ReadOnly) )
	{
		if( doc.setContent( &file ) )
		{
			file.close();

			QDomElement root = doc.documentElement();

			if( root.tagName() == "listeEtapes" )
			{

				QDomNode n = root.firstChild();

				while( !n.isNull() )
				{
					QDomElement e = n.toElement();
					if( !e.isNull() )
					{
						if( e.tagName() == "etape" )
						{
							nombreEtapes++;
						}
					}
					n = n.nextSibling();
				}
			}
		}
	}
	file.close();
	return nombreEtapes;
}
/*---------------------------------------------------
Execution du FileDialog
---------------------------------------------------*/
void Script::dialogFichier()
{
	script_modifie=false;
	QDir dir = QDir::current();
	
	QString dirPath=(dir.isRelativePath(nom_fichier))?dir.relativeFilePath(SCRIPT_FILE):dir.absoluteFilePath(SCRIPT_FILE);
	
	nom_fichier = QFileDialog::getOpenFileName(this,"Ouvrir fichier XML", dirPath , "Fichier XML (*.xml)");
	
	nom_fichier = (dir.isRelativePath(nom_fichier))?dir.relativeFilePath(nom_fichier):dir.absoluteFilePath(nom_fichier);
	
	if(nom_fichier.isEmpty())
	{
		nom_fichier = SCRIPT_FILE;							
	}
	else
	{
		writeINI(SCRIPT_FILE_INI,nom_fichier);
	}
}

/*---------------------------------------------------
Bouton de sauvegarde, créé ou remplace un fichier XML. [SLOT]
---------------------------------------------------*/
void Script::on_boutonSauvegarder_clicked()
{
	dialogFichier();

	if(!nom_fichier.isEmpty())
	{
		editeur->append(QString("")); // Retour charriot pour sauvegarder la dernière ligne

		labelNomFichier->setText(nom_fichier.section('/', -1)); // juste le nom du fichier (sans le chemin)

		QDomDocument doc( "script" );
		QDomElement root = doc.createElement("listeEtapes");
		doc.appendChild(root);

		QFile file( nom_fichier );		

		QDomElement elem ;
		QString ligne;
		QStringList liste, sousListe;

		//Créé un curseur au début de l'éditeur pour pouvoir sauvegarder ligne par ligne facilement.
		QTextCursor tc(editeur->document());

		while(!tc.atEnd())
		{
			tc.select(QTextCursor::LineUnderCursor);//Sélection de la ligne en cours
			ligne = (tc.selectedText()).simplified();//On enlève les espaces en trop
			if(ligne.isEmpty()) break;

			liste = ligne.split(";", QString::SkipEmptyParts);

			// Vérification du numéro de la ligne sous la forme ### (000 à 999)
			QRegExp regExp("^\\d{3}$");
			if ( regExp.exactMatch((liste.first()).simplified()) )
			{
				elem = doc.createElement("etape");
				elem.setAttribute("num",liste.first().simplified());

				for (int i = 1; i < liste.size(); i++)
				{
					sousListe = (liste.at(i)).split("=", QString::SkipEmptyParts);
					if ((sousListe.first()).simplified() == "t" )						elem.setAttribute("t",sousListe.last().simplified());
					if ((sousListe.first()).contains("SID", Qt::CaseInsensitive) )		elem.setAttribute("SID",sousListe.last().simplified());
					if ((sousListe.first()).contains("eSID", Qt::CaseInsensitive) )		elem.setAttribute("eSID",sousListe.last().simplified());
					if ((sousListe.first()).contains("DATA", Qt::CaseInsensitive) )		elem.setAttribute("data",sousListe.last().simplified());
					//if ((sousListe.first()).contains("SIZE", Qt::CaseInsensitive) )		elem.setAttribute("size",sousListe.last().simplified());
				}
				root.appendChild(elem);
			}

			tc.movePosition(QTextCursor::Down); //Prochaine ligne
		}

		//Sauvegarde du Document

		if( file.open( QIODevice::WriteOnly ) )
		{
			QTextStream ts( &file );
			ts << doc.toString();

			file.close();
		}
	}
}

/*---------------------------------------------------
Activation de l'éditeur[SLOT]
---------------------------------------------------*/
void Script::on_edition_clicked()
{
	
	script_modifie=true;
	
}

/*---------------------------------------------------
Bouton de chargement, ouvre un fichier xml et le formalise dans l'éditeur. [SLOT]
---------------------------------------------------*/
void Script::on_boutonCharger_clicked()
{
	
	dialogFichier();	
	
	if(!nom_fichier.isEmpty())
	{
		labelNomFichier->setText(nom_fichier.section('/', -1));
		editeur->clear();
		xmlVersEditeur();
	}
}

/*---------------------------------------------------
Bouton de lancement, "éxécute" le fichier xml	[SLOT] TODO
---------------------------------------------------*/
void Script::on_boutonLancer_clicked()
{
        Uint16 sid;
		bool ok; 
	  
	//On sauvegarde le fichier xml si il ya eu modification
	if(script_modifie)
	{
		on_boutonSauvegarder_clicked();
	}

	//On désactive l'éditeur
	editeur->setReadOnly(true);
	edition->setChecked(false);
	
	//affiche une boite de dialogue pour renseigner l'utilisateur de l'avancement
	QProgressBar *generalBar= new QProgressBar();//barre d'avancement général
	int indexMessage=0;
	generalBar->setMaximum(nombre_etapes());
	
	QProgressBar *localBar= new QProgressBar();//barre d'avancement local
	
	QString textDialog;
	QLabel *labelDialog=new QLabel("Message...");
	QLabel *generalLabel=new QLabel("Avancement...");
	
	QPushButton *bAnnuler=new QPushButton("Annuler");
	connect(bAnnuler,SIGNAL(clicked()),this,SLOT(Annuler_clicked()));
	
	QVBoxLayout *layoutBar = new QVBoxLayout;
	layoutBar->setSpacing(10);
	layoutBar->addWidget(generalLabel);
    layoutBar->addWidget(generalBar);
	layoutBar->addWidget(labelDialog);
    layoutBar->addWidget(localBar);
	layoutBar->addWidget(bAnnuler);
	
	QProgressDialog  *avancementDialog= new QProgressDialog();
	avancementDialog->setWindowTitle("Execution du script...");
	avancementDialog->setModal(true);	
	avancementDialog->setLayout(layoutBar);
	avancementDialog->setBar(localBar);
	avancementDialog->setCancelButton(bAnnuler);
	
	
	//initialisation du curseur pour la coloration des lignes
	cursor = editeur->textCursor();
	cursor.movePosition(QTextCursor::Start);
		//réinitialisation des couleurs
		statusMessage=STATUS_START;
		colorMessage();		
	cursor.movePosition(QTextCursor::Start);
	
	//fin de l'initialisation

	bool continuer = true;
	numero_etape = 1;
	majListeEtapes();
	QTime temps;
	temps.start();
	//Boucle principale
	while(continuer)
	{
		cout << "\nnumero_etape = " << numero_etape << endl;
		message_envoye = false;
        
		
        if( liste_etapes.first().attribute("eSID","0").toUInt(&ok,16) == 0 ) // Renverra 0 même si l'attribut eSID n'est pas trouvé.
		{
			
			statusMessage=STATUS_OK;
			for(int index=0;index<liste_etapes.size();index++)
			{
				QDomElement element=liste_etapes.at(index);
				int msec =element.attribute("t","0").toInt() ;
				avancementDialog->setValue(0);
				textDialog=QString("Etape %1 message %2 : attente %3 ms...").arg(numero_etape).arg(index).arg(msec);
				labelDialog->setText(textDialog);
				avancementDialog->setMaximum(msec);
				temps.restart();
				statusMessage=STATUS_OK;
				while(temps.elapsed()<=msec && statusMessage==STATUS_OK)
				{
					avancementDialog->setValue(temps.elapsed());
				}
				colorMessage();
				envoyerProchainMessage(element);

				while( !message_envoye ) {}
				cout << "Message envoye !\n";
			}
			indexMessage++;
			
		}
		else // Cas de l'attente d'évenement :
		{
			//TIME OUT	pour abandonner en cas de message non reçu
			
			statusMessage=STATUS_UNDEFINED;
			buffer=NULL;
			textDialog=QString("Etape %1 : Time out %2 ms...").arg(numero_etape).arg(TIME_OUT);	
			labelDialog->setText(textDialog);	
			avancementDialog->setMaximum(TIME_OUT);
			temps.restart();
			while(statusMessage==STATUS_UNDEFINED && temps.elapsed()<=TIME_OUT)
			{
				//vérifie si le buffer est bien rempli
				avancementDialog->setValue(temps.elapsed());
				if(buffer != NULL)
				{
					//extraire sid
					sid = buffer[0]*256 + buffer[1];//sid lit le sid contenu dans le message can
						
				   //test du eSID
					if(sid==liste_etapes.first().attribute("eSID").toUInt(&ok,16))
					{
						statusMessage=STATUS_OK;		
					}
				}
			}	
			colorMessage();
			indexMessage++;
			avancementDialog->setValue(TIME_OUT);
		}
		continuer=(statusMessage==STATUS_OK)?true:false;

		generalBar->setValue(indexMessage);

		//Sélection du numéro des étapes qui vont être exécutées.
		
		numero_etape++;
		majListeEtapes();

		//S'il n'y a pas d'étapes correspondant à ce numéro, on stop.
		if(liste_etapes.isEmpty())
		{
			continuer = false;
		}
	}
	avancementDialog->close();
	editeur->setReadOnly(false);
}
/*---------------------------------------------------
Fonction de coloration des messages du script
---------------------------------------------------*/
void Script::colorMessage()
{
	QTextCharFormat style;
	if(statusMessage==STATUS_START)
	{
		cursor.select(QTextCursor::Document);
	}
	else
	{
		cursor.select(QTextCursor::LineUnderCursor);
	}
    switch(statusMessage)
	{
		
		case STATUS_START:style.setBackground(Qt::transparent);
			break;
		case STATUS_ABORT:style.setBackground(Qt::yellow);
			break;
		case STATUS_OK:style.setBackground(Qt::green);
			break;
		case STATUS_UNDEFINED:;
		case STATUS_ERROR:style.setBackground(Qt::red);
			break;
	}
	cursor.insertText(cursor.selectedText(),style);
	cursor.movePosition(QTextCursor::Down);
	
}

/*---------------------------------------------------
Slot pour annuler l'envoi des messages	[SLOT]
---------------------------------------------------*/
void Script::Annuler_clicked()
{
    statusMessage=STATUS_ABORT;
}

/*---------------------------------------------------
Slot pour lire un message	[SLOT]
---------------------------------------------------*/
void Script::lireMessage(Uint8* message)
{
    //lire message
	buffer = message;
}


/*---------------------------------------------------
Slot pour envoyer le prochain message	[SLOT]
Envoie une action de l'étape active
---------------------------------------------------*/
void Script::envoyerProchainMessage(QDomElement e)
{
	//Creation du thread d'envoi:
	ScriptThread *st = new ScriptThread(e);

	connect(st,SIGNAL(scriptVersFenCAN(Uint8*)),this, SLOT(scriptVersFenCAN(Uint8*)));
	connect(st,SIGNAL(messageEnvoye(bool)),this,SLOT(setMessageEnvoye(bool)));

	cout << "Envoi du script\n";
	st->run();


}

/*---------------------------------------------------
Slot utilisé par le script thread
---------------------------------------------------*/
void Script::scriptVersFenCAN(Uint8* message){
	emit(envoyerMessageCAN(message));
}


/*---------------------------------------------------
Met à jour la QList liste_etapes en fonction du numero_etape actuel
---------------------------------------------------*/
void Script::majListeEtapes()
{
	QDomDocument doc( "script" );
	QFile file(nom_fichier);
	
	if( file.open(QIODevice::ReadOnly) )
	{
		if( doc.setContent( &file ) )
		{
			file.close();

			//On met à 0 l'ancienne liste
			liste_etapes.clear();

			QDomElement root = doc.documentElement();

			if( root.tagName() == "listeEtapes" )
			{

				QDomNode n = root.firstChild();

				while( !n.isNull() )
				{
					QDomElement e = n.toElement();
					if( !e.isNull() )
						if( e.tagName() == "etape" && e.attribute("num", "0" ).toInt() == numero_etape)
							liste_etapes.append(e);
					n = n.nextSibling();
				}
			}
		}
	}
	file.close();
}

void Script::setMessageEnvoye(bool message) // SLOT
{
	message_envoye = message;
}

/*--------------------------------------------------------------------------------------------
Slot qui permet de modifier ou non les valeurs des perametres en fonction du message CAN séléctionné
-----------------------------------------------------------------------------------------------*/
void Script::on_pListeMessages_itemSelectionChanged(){

	//renvoi le noeud ou se trouve le message CAN recherché
	QDomNode n=returnNodeMessCAN("nom",pListeMessages -> currentItem () ->text());
	//Attribution du code du message
	pSID->setText(n.toElement().attribute("code","000"));

	//Partie récupérée de FenCAN.cpp
	//Changement des paramètres :

	int i=0,j=0;

	//efface tous les parametres
	for(int i=0;i<(int)parametresSpin.size();i++){
			delete (paramLayout->labelForField ( parametresSpin[i]) ); // détruit le label du paramètre
			delete parametresSpin[i];
		}
	for(int i=0;i<(int)parametresCombo.size();i++){
			delete (paramLayout->labelForField ( parametresCombo[i]) ); // détruit le label du paramètre
			delete parametresCombo[i];
		}
	parametresSpin.clear();
	parametresCombo.clear();

	/*Cherche  le message CAN selectioné dans le fichier xml et active les parametres si ils lui sont utiles */

	QDomNode parametre = n.firstChild();

	while(!parametre.isNull()){
		QDomElement eParametre = parametre.toElement();
		if( eParametre.tagName() == "parametre" ){
			// ajout des widgets correspondant aux parametres en fonction du type de ce dernier
			QString nomParam=eParametre.attribute( "nom", "" );

			if(eParametre.attribute( "type", "" )=="Double"){
				QDoubleSpinBox *spinBox=new QDoubleSpinBox(this);
				spinBox->setWhatsThis ("Double");
				spinBox->setAccessibleDescription(nomParam);
				parametresSpin.push_back(spinBox);
				paramLayout->addRow(nomParam +" : ",parametresSpin[i]);
				i++;
			}
			else if (eParametre.attribute( "type", "" )=="Int"){
				QSpinBox *spinBox=new QSpinBox(this);
				spinBox->setMaximum(2999);
				if(eParametre.attribute( "nom", "" )=="A")
				spinBox->setRange(-180,180);
				spinBox->setWhatsThis ("Int");
				spinBox->setAccessibleDescription(nomParam);
				parametresSpin.push_back(spinBox);
				paramLayout->addRow(nomParam +" : ",parametresSpin[i]);
				i++;
			}
			else if (eParametre.attribute( "type", "" )=="Booleen"){

				QComboBox *comboBox=new QComboBox(this);
				comboBox ->addItem("Vrai");
				comboBox ->addItem("Faux");
				comboBox->setWhatsThis ("Booleen");
				comboBox->setAccessibleDescription(nomParam);
				parametresCombo.push_back(comboBox);
				paramLayout->addRow(nomParam +" : ",parametresCombo[j]);
				j++;
			}
			else if (eParametre.attribute( "type", "" )=="String"){

				QComboBox *comboBox=new QComboBox(this);

				int i=0;
				QString varNomChoix="string0";
				while(eParametre.attribute(varNomChoix, "" )!=NULL){
					comboBox ->addItem(eParametre.attribute(varNomChoix, "" ));
					i++;
					varNomChoix="string"+QString::number(i);
				}


				comboBox->setWhatsThis ("String");
				comboBox->setAccessibleDescription(nomParam);
				parametresCombo.push_back(comboBox);
				paramLayout->addRow(nomParam +" : ",parametresCombo[j]);
				j++;
			}

		}
		parametre = parametre.nextSibling();
	}
}


/*--------------------------------------------------------------------------------------------
Slot qui écrit dans l'éditeur le message CAN et ses informations associées
-----------------------------------------------------------------------------------------------*/
void Script::on_boutonAjouter_clicked(){
	script_modifie=true;


	QString ligne = QString();


	//Numéro d'étape de la forme ###
	if(pNum->value() < 100)
		ligne += "0";
	if(pNum->value() < 10)
		ligne += "0";

	QString num ;
	ligne += num.setNum(pNum->value()) + "; ";

	//Temps
	ligne += QString("t = ") + num.setNum(pT->value()) + "; ";

	//Evenement à attendre (eSID)
	if(!pESID->text().isEmpty() && (pESID->text() != "0"))
		ligne += QString("eSID = ") + pESID->text() + "; ";

	//id du message (SID)
	ligne += QString("SID = ") + pSID->text() + "; ";

	//Datas
	//Partie récupérée de FenCAN.cpp

	//Compte le nombre de données à envoyer
		int nb =0;

		// tableau d'octets pour les messages à envoyé
		// utilisé juste pour l'adaptation du code
		Uint8 message[10];

		double angle;

		QString taille;

		//on parcourt le tableau des parametres de type int et double
		for(int i=0;i<(int)parametresSpin.size();i++){

			//si le parametre est activé
			if(parametresSpin[i]->isEnabled()){

							//recherche dans le fichier parametres.xml le parametre en cours de traitement et recupere l'attribut taille pour savoir sur combien d'Uint8 il est codé
			QDomDocument doc( "parametres" );
			QFile file( XML_PARAM_CAN );

				if( file.open(QIODevice::ReadOnly) )
				{
					if( doc.setContent( &file ) )
					{
						file.close();

						QDomElement root = doc.documentElement();

						if( root.tagName() == "listeParametres" ){

							QDomNode n = root.firstChild();
							while( !n.isNull() )
							{
								QDomElement e = n.toElement();
								if( !e.isNull() )
								{
									if( e.tagName() == "parametre" )
									{
										if(e.attribute( "nom", "" )== parametresSpin[i]->accessibleDescription()){

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
				if(parametresSpin[i]->accessibleDescription()=="A"){

					//on calcul l'angle puis on le convertit en unsigned int de base 16 puis on le met dans le tableau à envoyer
					angle=((parametresSpin[i]->text()).toInt()*2*PI/360.0)*4096;

					if(taille.compare("Uint8")==0){
						message[nb]=(Uint8)(angle);
						nb++;
					}
					else{
						message[nb]=(Uint8)((int)angle/256);
						nb++;
						message[nb]=(Uint8)((int)angle%256);
						nb++;
					}

				}

				// si le parametre n'est pas un angle
				else{
					//on le met dans le tableau à envoyer

					if(taille.compare("Uint8")==0){
						message[nb]=(Uint8)((parametresSpin[i]->text()).toInt());
						nb++;
					}
					else{
						message[nb]=(Uint8)((parametresSpin[i]->text()).toInt()/256);
						nb++;
						message[nb]=(Uint8)((parametresSpin[i]->text()).toInt()%256);
						nb++;
					}
				}

			}
		}

		//on parcourt le tableau des parametres de type string
		for(int i=0;i<(int)parametresCombo.size();i++){
			//si le parametre est activé
			if(parametresCombo[i]->isEnabled()){

				//recherche dans le fichier parametres.xml le parametre en cours de traitement et recupere l'attribut taille pour savoir sur combien d'Uint8 il est codé
				QDomDocument doc( "parametres" );
				QFile file( XML_PARAM_CAN );

				if( file.open(QIODevice::ReadOnly) )
				{
					if( doc.setContent( &file ) )
					{
						file.close();

						QDomElement root = doc.documentElement();

						if( root.tagName() == "listeParametres" ){

							QDomNode n = root.firstChild();
							while( !n.isNull() )
							{
								QDomElement e = n.toElement();
								if( !e.isNull() )
								{
									if( e.tagName() == "parametre" )
									{
										if(e.attribute( "nom", "" )== parametresCombo[i]->accessibleDescription()){

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
				if(taille.compare("Uint8")==0){
					message[nb]=(Uint8)(parametresCombo[i]->currentIndex());
					nb++;
				}
				else{
						message[nb]=(Uint8)(parametresCombo[i]->currentIndex()/256);
						nb++;
						message[nb]=(Uint8)(parametresCombo[i]->currentIndex()%256);
						nb++;
				}
			}
		}
	//on met a 0 tous les champs inutilisés
		int j=nb;
		while (j<8){
			message[j]=0;
			j++;
		}

		//nombre de parametres associés au message CAN
		message[j]=(Uint8)(nb);


	//Coder les datas...
	ligne += QString("DATA = ") ;
	for(int i=0; i<=j ; i++)
		ligne += num.setNum(message[i]);
	ligne += ";" ;

	editeur->append(ligne);

}





















