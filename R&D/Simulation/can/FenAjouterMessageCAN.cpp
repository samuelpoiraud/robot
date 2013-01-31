/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenAjouterMessageCAN.cpp
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet d'ajouter un message CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */

#include "FenAjouterMessageCAN.h"

//Constructeur: Creation de la fenetre et de ses elements
FenAjouterMessageCAN::FenAjouterMessageCAN(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Ajout message CAN");
    
	//Creation des widgets
    ajouter=new QPushButton("Ajouter");   
    nom=new QLineEdit();
	code=new QLineEdit();
	
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Nom", nom);
	formLayout->addRow("Code   0x", code);
    
    //recupère  les parametres par lecture du fichier parametres.xml puis crée le nombre de checkbox correspondant
    QDomDocument doc( "parametres" );
    QFile file( XML_PARAM_CAN );
 
    QGroupBox *groupbox = new QGroupBox("Paramètres", this);

    QVBoxLayout *vbox = new QVBoxLayout;

    int i=0; 
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
							//Creation des checkbox
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
    layoutPrincipal->addLayout(formLayout);// Ajout du layout de formulaire
    layoutPrincipal->addWidget(groupbox); 
    layoutPrincipal->addWidget(ajouter);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	// Connexion des  signaux aux slots
    connect(ajouter, SIGNAL(clicked()), this, SLOT(on_ajouter_clicked()));

}

//Slot qui ajoute le messageCAN crée dans le fichier messagesCAN.xml 
void FenAjouterMessageCAN::on_ajouter_clicked(){

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
		// on parcours le fichier messagesCAN.xml pour tester si le code et le nom du message CAN  crée n'existe pas déjà, (on crée une fenetre d'erreur si ils existent)
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
										label ->setText("Ce message CAN existe déjà, trouvez un nouveau nom");
										
										QVBoxLayout *layoutPrincipal = new QVBoxLayout;
										layoutPrincipal->addWidget(label);
										
										erreur ->setLayout (layoutPrincipal);
										erreur -> show();
										erreur -> resize(200,100);
										
										return;
									}
									else if(e.attribute( "code", "" )== "0x"+codeModif){
										//fenetre d'erreur si le code existe deja
										QDialog *erreur= new QDialog(this);
										erreur -> setWindowTitle("Erreur");
										
										QLabel *label = new QLabel(erreur);
										label ->setText("Ce message CAN existe déjà, trouvez un nouveau code");
										
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

		 //si il n'y as pas eu de pb, on ajoute le message CAN au fichier messagesCAN.xml
	     QDomElement root = doc.documentElement();
		 QDomElement messageCAN = doc.createElement("messageCAN");
		 messageCAN.setAttribute("nom",nom -> text());
		 
		 messageCAN.setAttribute("code", "0x" + codeModif);
		 root.appendChild(messageCAN);
		 
		 QDomElement param; 
		 //QString type;
		 
		 //on ajoute les parametres necessaire(==> dont les check box on été cochée) au messageCAN crée dans le fichier messagesCAN.xml
		 for(int i=0;i<(int)parametres.size();i++){
			if(parametres[i]->isChecked()){	 

			    param = doc. createElement("parametre");
				param.setAttribute("nom", parametres[i] -> text());
				 
				//recuperation du type du parametre a partir de son nom (en parsant le fichier xml parametres.xml)
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

				//ajout du parametre dans le fichier xml
				messageCAN.appendChild(param);
			}
	    }
	     
	    //ecriture dans le fichier xml
	    if( file.open( QIODevice::WriteOnly ) )
	    {          
	        QTextStream ts( &file );
	        ts << doc.toString();
	                
	        file.close();
	    }
		 
		qobject_cast<FenPrincipale*>(parentWidget())->fenetreCAN->messagesCAN->addItem(nom->text());
		 
		 
		//Creation d'une fenetre qui confirme l'ajout du message CAN
		QDialog *confirmation= new QDialog(this);
		confirmation -> setWindowTitle("Confirmation");
			
		QLabel *label = new QLabel(confirmation);
		label ->setText("Le message CAN a bien été ajouté");
			
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
			
		confirmation ->setLayout (layoutPrincipal);
		confirmation -> show();
		confirmation -> resize(200,100);
	}
}

