/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenModifierMessageCAN.cpp
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet  de modifier  un message CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */

#include "FenModifierMessageCAN.h"

/*---------------------------------------------------
Constructeur: Creation de la fenetre et de ses elements
---------------------------------------------------*/
FenModifierMessageCAN::FenModifierMessageCAN(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Modification messages CAN");
	
	/*Creation des widgets*/
    modifier=new QPushButton("Modifier");   
	
	messagesCAN=new QComboBox();

    nom=new QLineEdit();
	
	code=new QLineEdit();
	
	// ajout dans la combo box des differents messages CAN  par lecture du fichier messagesCAN.xml
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
						// ajout dans la combo box
                        QString nomParam=e.attribute( "nom", "" );
						messagesCAN -> addItem(nomParam);
                    }
                  }
                   
                  n = n.nextSibling();
                }
            }
        }
    }

	//organisation du layout
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Message CAN", messagesCAN);
    
    formLayout->addRow("Nom", nom);
	formLayout->addRow("Code       0x", code);
    
	//recupère  les parametres par lecture du fichier parametres.xml puis crée le nombre de checkbox correspondant  
    QDomDocument doc2( "parametres" );
    QFile file2( XML_PARAM_CAN );
 
    QGroupBox *groupbox = new QGroupBox("Paramètres", this);

    QVBoxLayout *vbox = new QVBoxLayout;

    int i=0; 
    if( file2.open(QIODevice::ReadOnly) )
    {  
        if( doc2.setContent( &file2 ) )
        {           
            file2.close();
                        
            QDomElement root = doc2.documentElement();
            
            if( root.tagName() == "listeParametres" ){
                        
                QDomNode n = root.firstChild();
      
                while( !n.isNull() )
                {
                  QDomElement e = n.toElement();
                  if( !e.isNull() )
                  {
                    if( e.tagName() == "parametre" )
                    {   
					   //ajout d'une checkbox correspondant a un paramètre dans le tableau parametres
                       QString nomParam=e.attribute( "nom", "" );
                       parametres.push_back(new QCheckBox(nomParam , this));
                       vbox->addWidget(parametres[i]);
                       i++;
                    }
                  }
                   
                  n = n.nextSibling();
                }
            }
        }
    }
   
    //organisation du layout
	groupbox->setLayout(vbox);
    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);
	layoutPrincipal->addWidget(groupbox);
    layoutPrincipal->addWidget(modifier);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	messagesCAN -> setCurrentIndex(-1);
	
	// Connexion des  signaux aux slots
    connect(modifier, SIGNAL(clicked()), this, SLOT(on_modifier_clicked()));
    connect(messagesCAN,SIGNAL(currentIndexChanged(int)), this, SLOT(on_messagesCAN_currentIndexChanged(int)));  
	
}

/*--------------------------------------------------------------------------------------------
Slot qui permet d'afficher les informations du message CAN séléctionné pour pouvoir les modifier ensuite
-----------------------------------------------------------------------------------------------*/
void FenModifierMessageCAN::on_messagesCAN_currentIndexChanged(int machin){

	//recherche dans le fichier messagesCAN.xml le message CAN sélectioné  et recupere ses arguments pour remplir les champs nom, code et paramètre
    QDomDocument doc( "messagesCAN" );
    QFile file( XML_MESSAGES_CAN );

	for(int j=0;j<(int)parametres.size();j++){
		parametres[j] -> setChecked (false);
	}
	
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
							if(e.attribute( "nom", "" )== (messagesCAN -> currentText())){	
								
								//remplissage des champs nom, code et paramètre(checkbox
								nom -> setText(e.attribute( "nom", "" ));
								
								//on sectionne pour enlever le 0x
								code -> setText(e.attribute( "code", "" ).section ( "", 3, 6));
								
								QDomNode parametre = n.firstChild();
								
								while(!parametre.isNull()){
									QDomElement eParametre = parametre.toElement();
										if( eParametre.tagName() == "parametre" ){
											for(int i=0;i<(int)parametres.size();i++){
											    if(parametres[i] -> text() == eParametre.attribute( "nom", "" ))
													parametres[i] -> setChecked (true);
											}
										}										
									parametre = parametre.nextSibling();
								}
								
					        }
	                    }
					}
					n = n.nextSibling();
                }
            }
        }
    }
}


/*--------------------------------------------------------------------------------------------
Slot qui modifie les information du paramètre séléctioné  dans le fichier parametres.xml 
-----------------------------------------------------------------------------------------------*/
void FenModifierMessageCAN::on_modifier_clicked(){

	QString codeModif=code -> text();
	while(codeModif.size()<4)codeModif="0"+codeModif;

	QDomDocument doc("messagesCAN");
    QFile file(XML_MESSAGES_CAN);

	// On verifie que les champs nom et code ont bien été renseigné, sinon on ouvre une fenetre avec message d'erreur
	if (nom ->text()=="" || code ->text()==""){
		QDialog *erreur= new QDialog(this);
		erreur -> setWindowTitle("Erreur");
									
		QLabel *label = new QLabel(erreur);
		
		if (nom ->text()=="")
		label ->setText("Vous devez donner un nom au message CAN");
		else label ->setText("Vous devez donner un code au message CAN");
									
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
									
		erreur ->setLayout (layoutPrincipal);
		erreur -> show();
		erreur -> resize(200,100);
									
		return;
	}
	
	else{
		
		//On recupere l'ancien code du message CAN que l'on souhaite modifier
		QString currentCode;
	
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
								if(e.attribute("nom","")==messagesCAN -> currentText())	
									currentCode=e.attribute("code","");
							}
						}
						n = n.nextSibling();
					}
		        }         
		    }    
		}
	
	
		// si le nom du message CAN a été modifier  on test si  ce nouveau nom n'existe pas déjà, (on crée une fenetre d'erreur si il existe)
		if(!(messagesCAN -> currentText()== nom ->text())){
			
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
										
			                        if(e.attribute( "nom", "" )== nom ->text()){
									
										//fenetre d'erreur si le nom existe deja
										QDialog *erreur= new QDialog(this);
										erreur -> setWindowTitle("Erreur");
										
										QLabel *label = new QLabel(erreur);
										label ->setText("Ce message CAN existe déjà, vous devez lui donner un autre nom");
										
										QVBoxLayout *layoutPrincipal = new QVBoxLayout;
										layoutPrincipal->addWidget(label);
										
										erreur ->setLayout (layoutPrincipal);
										erreur -> show();
										erreur -> resize(200,100);
										
										return;
									}								
								}
							}
							n = n.nextSibling();
						}
		            }         
		        }
		    }
		}
		
		// de meme avec le code du message CAN
		if(!(currentCode== "0x"+codeModif)){
			
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
			                        if(e.attribute( "code", "" )== "0x"+codeModif){
										QDialog *erreur= new QDialog(this);
										erreur -> setWindowTitle("Erreur");
										
										QLabel *label = new QLabel(erreur);
										label ->setText("Ce message CAN existe déjà, vous devez lui donner un autre code");
										
										QVBoxLayout *layoutPrincipal = new QVBoxLayout;
										layoutPrincipal->addWidget(label);
										
										erreur ->setLayout (layoutPrincipal);
										erreur -> show();
										erreur -> resize(200,100);
										
										return;
									}								
								}
							}
							n = n.nextSibling();
						}
		            }         
		        }
		    }
		}

		//modification du message CAN dans le fichier messagesCAN.xml
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
		                        if(e.attribute( "nom", "" )== 	messagesCAN -> currentText()){
									//On modifie le nom et le code 
									e.setAttribute( "nom", nom -> text());
									e.setAttribute( "code", "0x" + codeModif);
									
									//On supprime les parametres existant
									QDomNode child=n.firstChild();
									QDomNode child2;
									
									while(!(child.isNull())){
										child2=child;
										child=child.nextSibling();
										n.removeChild(child2);
									}
										
									//on ajoute les nouveau paramètres
									QDomElement param; 
									
									for(int i=0;i<(int)parametres.size();i++){
										if(parametres[i]->isChecked()){	 
											param = doc. createElement("parametre");
											param.setAttribute("nom", parametres[i] -> text());	
											
											//recuperation du type du parametre a partir de son nom (en parsant le fichier parametres.xml) pour l'ajouter comme attribut
											QDomDocument doc2( "parametres" );
											QFile file2( XML_PARAM_CAN );
										    if( file2.open(QIODevice::ReadOnly) )
										    {  
										        if( doc2.setContent( &file2 ) )
										        {           
										            file2.close();
										                        
										            QDomElement root = doc2.documentElement();
										            
										            if( root.tagName() == "listeParametres" ){
										                        
										                QDomNode n = root.firstChild();		      
										                while( !n.isNull() )
										                {
										                  QDomElement e = n.toElement();
															if( !e.isNull() )
															{
																if( e.tagName() == "parametre" )
																{   
																	QString nomParam=e.attribute( "nom", "" );
																	if(nomParam==parametres[i] -> text()){
																		//Copie du paramètre pour avoir la liste des choix.
																		param = (e.cloneNode(false)).toElement();
																		param.removeAttribute ("Taille"); // Facultatif
																	}
																}
															}
										                  n = n.nextSibling();
										                }
										            }
										        }
										    }											
											
											n.appendChild(param);
										} 
									}
							    }									
							}
						}
						n = n.nextSibling();
					}
	            }         
	        }
	    }
	 
		//on ecrit dans le fichier xml
	    if( file.open( QIODevice::WriteOnly ) )
	    {          
	         QTextStream ts( &file );
	         ts << doc.toString();
	                
	         file.close();
	    }
		
		//on supprime l'ancien message CAN et on ajoute le nouveau dans la liste de message CAN
		qobject_cast<FenPrincipale*>(parentWidget())->fenetreCAN->messagesCAN->addItem(nom->text());
		
		QList<QListWidgetItem *> selectedFiles=	qobject_cast<FenPrincipale*>(parentWidget())->fenetreCAN->messagesCAN->findItems(messagesCAN -> currentText (),Qt::MatchExactly);
		 
		foreach(QListWidgetItem *i, selectedFiles)
	   {
	      delete i;
	   }
		
		//on supprime l'ancien message CAN et on ajoute le nouveau dans la combo box
		messagesCAN ->addItem ( nom-> text() );
		messagesCAN ->removeItem ( messagesCAN-> currentIndex () );  
		
		/*Creation d'une fenetre qui confirme la modification du message CAN */
		QDialog *confirmation= new QDialog(this);
		confirmation -> setWindowTitle("Confirmation");
		
		QLabel *label = new QLabel(confirmation);
		label ->setText("Le message CAN a bien été modifié");
		
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
		
		confirmation ->setLayout (layoutPrincipal);
		confirmation -> show();
		confirmation -> resize(200,100);
		
	}
}

