/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenEnregistrer.cpp
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet d'enregistrer un message CAN avec des parametres predefinis sous un alias
 *	Auteur : Aurélie Ottavi
 *	Version 20090407
 */

#include "FenEnregistrer.h"

//Constructeur: Creation de la fenetre et de ses elements
FenEnregistrer::FenEnregistrer(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Enregistrer message CAN predefinis");
    
	for(int i=0;i<9;i++)
	parametres[i]=new Parametre();
	
	//Creation des widgets
    enregistrer=new QPushButton("Enregistrer");   
    alias=new QLineEdit();
	
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Alias", alias);
   
    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);// Ajout du layout de formulaire
    layoutPrincipal->addWidget(enregistrer);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	// Connexion des  signaux aux slots
    connect(enregistrer, SIGNAL(clicked()), this, SLOT(on_enregistrer_clicked()));

}

//Slot qui ajoute le messageCAN crée dans le fichier messagesCAN.xml 
void FenEnregistrer::on_enregistrer_clicked(){
	
	
	QDomDocument doc("messagesCANPredefini");
    QFile file(XML_MESSAGES_PREDEF);
	
	// On verifie que le champ alias a bien été renseigné, sinon on ouvre une fenetre avec message d'erreur
	if (alias ->text()==""){
		QDialog *erreur= new QDialog(this);
		erreur -> setWindowTitle("Erreur");
									
		QLabel *label = new QLabel(erreur);
		
		label ->setText("Vous devez donner un alias au message CAN");
							
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
									
		erreur ->setLayout (layoutPrincipal);
		erreur -> show();
		erreur -> resize(200,100);
									
		return;
	}
	
	
	else{
		// on parcours le fichier messagesCANPredefini.xml pour tester si l'alias du message CAN  crée n'existe pas déjà, (on crée une fenetre d'erreur si ils existent)
		if( file.open(QIODevice::ReadOnly) )
		    {  
		        if( doc.setContent( &file ) )
		        {           
		            file.close();
		                        
		            QDomElement root = doc.documentElement();
		            
		            if( root.tagName() == "listeMessagePredefini" ){
		                        
		                QDomNode n = root.firstChild();      
		                while( !n.isNull() )
		                {
							QDomElement e = n.toElement();
							if( !e.isNull() )
							{
			                    if( e.tagName() == "messageCANPredefini" )
			                    {   
			                        if(e.attribute( "alias", "" )== alias ->text()){
										//fenetre d'erreur si le nom existe deja
										QDialog *erreur= new QDialog(this);
										erreur -> setWindowTitle("Erreur");
										
										QLabel *label = new QLabel(erreur);
										label ->setText("Ce message CAN predefini existe déjà, trouvez un nouveau nom");
										
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
		
		
	     if (file.open(QIODevice::ReadOnly))
	     {                                  
	         doc.setContent(&file);
	         file.close();
	     }      

		 //si il n'y as pas eu de pb, on ajoute le message CAN au fichier messagesCANPredefini.xml
	     QDomElement root = doc.documentElement();
		 QDomElement messageCAN = doc.createElement("messageCANPredefini");
		 messageCAN.setAttribute("alias",alias -> text());
		 messageCAN.setAttribute("nom",*nom);
		 messageCAN.setAttribute("code",*code);
		 root.appendChild(messageCAN);
		 
		 QDomElement param; 
		 
		 //on ajoute les parametres necessaire(==> dont les check box on été cochée) au messageCAN crée dans le fichier messagesCAN.xml
		 for(int i=0;i<nbParametre;i++){
			    param = doc. createElement("parametre");
				param.setAttribute("nom", parametres[i] -> nom);
				param.setAttribute("type", parametres[i] -> type);
				param.setAttribute("valeure", parametres[i] -> valeur);
				//ajout du parametre dans le fichier xml
				messageCAN.appendChild(param);
			}
	    }
	      
	    //ecriture dans le fichier xml
	    if( file.open( QIODevice::WriteOnly ) )
	    {        
	        QTextStream ts( &file );
			ts.setCodec("ANSI");

	        ts << doc.toString();
	                
	        file.close();
	    }
		
		
		
		//on ajoute le message dans la liste des messages predefinis
		
		qobject_cast<FenCAN*>(parentWidget())->listeMessagePredefinis->addItem(alias->text());
		
		//on quitte la fenêtre	
		QCloseEvent close;
		closeEvent (&close);
}

