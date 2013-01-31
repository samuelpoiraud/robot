/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenSupprimerParam.cpp
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet de msuprimer un paramètre
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */


#include "FenSupprimerParam.h"

//Constructeur: Creation de la fenetre et de ses elements
FenSupprimerParam::FenSupprimerParam(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Suppression de paramètres");
    
	//creation des widgets composant la fenetre
    supprimer=new QPushButton("Supprimer");   
	
	parametres=new QComboBox();
	
	nom=new QLineEdit();
	
	/*ajout dans la combo box des differents paramètres par lecture du fichier parametres.xml*/	
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
							//ajout dans la combo box
	                        QString nomParam=e.attribute( "nom", "" );
							parametres -> addItem(nomParam);
	                    }
	                  }
                   
                  n = n.nextSibling();
                }
            }
        }
   }
	
	//organisation du layout
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Parametre", parametres);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);
    layoutPrincipal->addWidget(supprimer);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	// Connexion des  signaux aux slots
    connect(supprimer, SIGNAL(clicked()), this, SLOT(on_supprimer_clicked()));

}

//Slot qui supprime le  paramètre séléctioné  dans le fichier parametres.xml 
void FenSupprimerParam::on_supprimer_clicked(){

	QString currentMessage = parametres -> currentText();
	QDomDocument doc( "parametres" );
	QFile file( XML_PARAM_CAN );

	//On supprime le parametre selectionné du fichier xml
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
	
	//on enleve de la combo box le paramètre qui vient d'etre supprimer
	parametres ->removeItem ( parametres-> currentIndex () ); 
	
	/*Creation d'une fenetre qui confirme la suppression d'un paramètre */
	QDialog *confirmation= new QDialog(this);
	confirmation -> setWindowTitle("Confirmation");
		
	QLabel *label = new QLabel(confirmation);
	label ->setText("Le paramètre a bien été supprimé");
		
	QVBoxLayout *layoutPrincipal = new QVBoxLayout;
	layoutPrincipal->addWidget(label);
		
	confirmation ->setLayout (layoutPrincipal);
	confirmation -> show();
	confirmation -> resize(200,100);

}
