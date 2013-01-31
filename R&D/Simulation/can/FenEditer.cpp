/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenEditer.cpp
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de modifier ou de supprimer un message CAN predefini
 *	Auteur : Aurélie Ottavi
 *	Version 20090407
 */

#include "FenEditer.h"

//Constructeur: Creation de la fenetre et de ses elements
FenEditer::FenEditer(QWidget *parent = 0,QString text) : QDialog(parent)
{                                                   
    setWindowTitle ("Editer message CAN predefinis");
    
	
	//Creation des widgets
    modifier=new QPushButton("Modifier");   
	supprimer=new QPushButton("Supprimer");   
    nouvelAlias=new QLineEdit();
	nouvelAlias->setText(text);
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Alias", nouvelAlias);
	
    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);// Ajout du layout de formulaire
    layoutPrincipal->addWidget(modifier);
	layoutPrincipal->addWidget(supprimer);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	// Connexion des  signaux aux slots
    connect(supprimer, SIGNAL(clicked()), this, SLOT(on_supprimer_clicked()));
	connect(modifier, SIGNAL(clicked()), this, SLOT(on_modifier_clicked()));

}

//Slot qui ajoute le messageCAN crée dans le fichier messagesCAN.xml 
void FenEditer::on_modifier_clicked(){
	
	QDomDocument doc("messagesCANPredefini");
    QFile file(XML_MESSAGES_PREDEF);
	
	// On verifie que le champ alias a bien été renseigné, sinon on ouvre une fenetre avec message d'erreur
	if (nouvelAlias ->text()==""){
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
			                        if(e.attribute( "alias", "" )== nouvelAlias ->text()){
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
									else if(e.attribute( "alias", "" )== ancienAlias){
										//cout << "ici";
										e.setAttribute("alias",nouvelAlias -> text());
									}
								}
							}
							n = n.nextSibling();
						}
		            }         
		        }
		    } 
	      
	    //ecriture dans le fichier xml
	    if( file.open( QIODevice::WriteOnly ) )
	    {        
	        QTextStream ts( &file );


	        ts << doc.toString();
	                
	        file.close();
	    }
		
		//on edite(supprime puis ajoute) le message dans la liste des messages predefinis
		QList<QListWidgetItem *> selectedFiles=	qobject_cast<FenCAN*>(parentWidget())->listeMessagePredefinis->findItems(*ancienAlias,Qt::MatchExactly);
		 
		foreach(QListWidgetItem *i, selectedFiles)
	   {
	      delete i;
	   }
		
		qobject_cast<FenCAN*>(parentWidget())->listeMessagePredefinis->addItem(nouvelAlias->text());
		}
		
		//on quitte la fenêtre	
		QCloseEvent close;
		closeEvent (&close);
		
}

void FenEditer::on_supprimer_clicked(){

QDomDocument doc("messagesCANPredefini");
QFile file(XML_MESSAGES_PREDEF);
	
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
			                       
									if(e.attribute( "alias", "" )== ancienAlias){
										root.removeChild(n);
									}
								}
							}
							n = n.nextSibling();
						}
		            }         
		        }
		    } 
	      
	    //ecriture dans le fichier xml
	    if( file.open( QIODevice::WriteOnly ) )
	    {        
	        QTextStream ts( &file );
	        ts << doc.toString();
	                
	        file.close();
	    }
		
		//on edite(supprime puis ajoute) le message dans la liste des messages predefinis
		QList<QListWidgetItem *> selectedFiles=	qobject_cast<FenCAN*>(parentWidget())->listeMessagePredefinis->findItems(*ancienAlias,Qt::MatchExactly);
		 
		foreach(QListWidgetItem *i, selectedFiles)
	   {
	      delete i;
	   }
	   
	   //on quitte la fenêtre	
		QCloseEvent close;
		closeEvent (&close);
		
}
