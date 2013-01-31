/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenSupprimerMessageCAN.cpp
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de supprimer un message CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */


#include "FenSupprimerMessageCAN.h"

//Constructeur: Creation de la fenetre et de ses elements
FenSupprimerMessageCAN::FenSupprimerMessageCAN(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Suppression de messages CAN");
    
	//creation des widgets composant la fenetre
    supprimer=new QPushButton("Supprimer");   
	
	messagesCAN=new QComboBox();
	

	/*ajout dans la combo box des differents messages CAN par lecture du fichier messagesCAN.xml*/	
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
						//ajout dans la combo box
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

    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);
    layoutPrincipal->addWidget(supprimer);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	// Connexion des  signaux aux slots
    connect(supprimer, SIGNAL(clicked()), this, SLOT(on_supprimer_clicked()));

}

//Slot qui supprime le message CAN séléctioné  dans le fichier messagesCAN.xml 
void FenSupprimerMessageCAN::on_supprimer_clicked(){

    QString currentMessage = messagesCAN -> currentText();
	
	//On supprime le messageCAN selectionné du fichier xml
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
                        if(e.attribute( "nom", "" )== currentMessage){
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
	
	
	//on enleve de la liste de message CAN celui qui vient d'etre supprimé
		 
	QList<QListWidgetItem *> selectedFiles=	qobject_cast<FenPrincipale*>(parentWidget())->fenetreCAN->messagesCAN->findItems(messagesCAN -> currentText (),Qt::MatchExactly);
		 
	foreach(QListWidgetItem *i, selectedFiles)
   {
      delete i;
   }

	//on enleve de la combo box le message CAN qui vient d'etre supprimer
	messagesCAN ->removeItem ( messagesCAN -> currentIndex () );
	
	/*Creation d'une fenetre qui confirme la suppression d'un message CAN */
	QDialog *confirmation= new QDialog(this);
	confirmation -> setWindowTitle("Confirmation");
		
	QLabel *label = new QLabel(confirmation);
	label ->setText("Le message CAN a bien été supprimé");
		
	QVBoxLayout *layoutPrincipal = new QVBoxLayout;
	layoutPrincipal->addWidget(label);
		
	confirmation ->setLayout (layoutPrincipal);
	confirmation -> show();
	confirmation -> resize(200,100);
}

