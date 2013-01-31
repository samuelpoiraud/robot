/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : FenModifierParam.cpp
 *	Package : Clickodrome
 *	Description : crée une fenetre qui permet  de modifier un paramètre
 *	Auteur : Aurélie Ottavi
 *	Version 20080921
 */


#include "FenModifierParam.h"

/*---------------------------------------------------
Constructeur: Creation de la fenetre et de ses elements
---------------------------------------------------*/
FenModifierParam::FenModifierParam(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Modification des paramètres");
    
	/*Creation des widgets*/
    modifier=new QPushButton("Modifier");   
	
	parametres=new QComboBox();
	
	nom=new QLineEdit();
	
	type=new QComboBox();
	type -> addItem("Double");
    type -> addItem("Int");
    type -> addItem("String");
    type -> addItem("Booleen");

	taille=new QComboBox();
	taille -> addItem("Uint8");
	taille -> addItem("Uint16");
	
	
	choix=new QLineEdit();
	
	//widgets qui ne servent que si  on ajoute un paramètre de type string
	choix -> setEnabled(false);

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
						// ajout dans la combo box
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
    formLayout->addRow("Paramètre", parametres);
	formLayout->addRow("Nom", nom);
	formLayout->addRow("Type", type);
	formLayout->addRow("Taille", taille);
	formLayout -> addRow("Liste de choix:",choix);
    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);
    layoutPrincipal->addWidget(modifier);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	parametres-> setCurrentIndex(-1);
	
	// Connexion des  signaux aux slots
    connect(modifier, SIGNAL(clicked()), this, SLOT(on_modifier_clicked()));
	connect(parametres,SIGNAL(currentIndexChanged(int)), this, SLOT(on_parametres_currentIndexChanged(int)));
	connect(type,SIGNAL(currentIndexChanged(int)), this, SLOT(on_type_currentIndexChanged(int)));
}

/*--------------------------------------------------------------------------------------------
Slot qui permet d'afficher les informations duparamètre séléctionné pour pouvoir les modifier ensuite
-----------------------------------------------------------------------------------------------*/
void FenModifierParam::on_parametres_currentIndexChanged(int machin){

	//recherche dans le fichier parametres.xml le parametre sélectioné  et recupere ses arguments pour remplir les champs nom, type voir choix1 et choix2
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
							if(e.attribute( "nom", "" )== (parametres -> currentText())){	
								
								//remplissage des champs nom, type voir choix1 et choix2
								nom -> setText(e.attribute( "nom", "" ));
								type -> setCurrentIndex( type -> findText(e.attribute( "type", "" )));
								taille -> setCurrentIndex( taille -> findText(e.attribute( "taille", "" )));
								
								if(e.attribute( "type", "" )=="String"){
									int i=0;
									QString varNomChoix="string0";
									QString choixTexte;
									while(e.attribute(varNomChoix, "" )!=NULL){
										if(i!=0) choixTexte+=";";
										choixTexte+=e.attribute(varNomChoix,"" );
										i++;
										varNomChoix="string"+QString::number(i);
									}
									
									
									choix -> setText(choixTexte);
								}
								else {
									choix-> setText("");
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
Slot qui permet d'activer  les widgets string1, string2 si le type choisit est String
-----------------------------------------------------------------------------------------------*/
void FenModifierParam::on_type_currentIndexChanged(int machin){

	//active les widgets string1 et string2
	if(type->currentText()=="String"){
		choix -> setEnabled(true);
	}
	else {
		//desactive les widgets string1 et string2
		choix -> setEnabled(false);
	}
}

/*--------------------------------------------------------------------------------------------
Slot qui modifie les information du paramètre séléctioné  dans le fichier parametres.xml 
-----------------------------------------------------------------------------------------------*/
void FenModifierParam::on_modifier_clicked(){

    QString currentMessage = parametres -> currentText();
	QDomDocument doc( "parametres" );
	QFile file( XML_PARAM_CAN );
	
	// On verifie que le champ nom a bien été renseigné, sinon on ouvre une fenetre avec message d'erreur
	if (nom ->text()==""){
		QDialog *erreur= new QDialog(this);
		erreur -> setWindowTitle("Erreur");
									
		QLabel *label = new QLabel(erreur);
		label ->setText("Vous devez donner un nom au paramètre");
									
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
									
		erreur ->setLayout (layoutPrincipal);
		erreur -> show();
		erreur -> resize(200,100);
									
		return;
	}
	else if(choix->text()=="" && choix->isEnabled()==true){
		QDialog *erreur= new QDialog(this);
		erreur -> setWindowTitle("Erreur");
									
		QLabel *label = new QLabel(erreur);
		label ->setText("Aucun choix n'a été specifié");
									
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
									
		erreur ->setLayout (layoutPrincipal);
		erreur -> show();
		erreur -> resize(200,100);
									
		return;
	}
	else{
		// si le nom du parametre a été modifier  on test si  ce nouveau nom n'existe pas déjà, (on crée une fenetre d'erreur si il existe)
		if(!(parametres -> currentText()== nom ->text())){
			
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
			                        if(e.attribute( "nom", "" )== nom ->text()){
									
										//fenetre d'erreur si le nom existe deja
										QDialog *erreur= new QDialog(this);
										erreur -> setWindowTitle("Erreur");
										
										QLabel *label = new QLabel(erreur);
										label ->setText("Ce paramètre existe déjà, trouvez un nouveau nom");
										
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
		
		//modification du parametre dans le fichier parametres.xml
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
								
									//On modifie le nom, le type voir string1 et string2
									e.setAttribute( "nom", nom -> text());
									e.setAttribute( "type", type -> currentText());	
									e.setAttribute("taille",taille -> currentText());
									if(type -> currentText()=="String") {
									
										QString nomVarString="string0";
										
										int num=0;
			
										while(e.attribute(nomVarString, "" )!=NULL){
											e.removeAttribute(nomVarString);
											num++;
											nomVarString="string"+QString::number(num);
										}
										
										QStringList listeChoix=choix->text().split(";");
										for(int i=0;i<listeChoix.size();i++){
											nomVarString="string"+QString::number(i);
											e.setAttribute(nomVarString,listeChoix.at(i));
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
		 
		//on supprime l'ancien parametre et on ajoute le nouveau dans la combo box
		parametres ->addItem ( nom-> text() );  
		parametres ->removeItem ( parametres-> currentIndex () );  
		
		/*Creation d'une fenetre qui confirme la modification du paramètre*/
		QDialog *confirmation= new QDialog(this);
		confirmation -> setWindowTitle("Confirmation");
		
		QLabel *label = new QLabel(confirmation);
		label ->setText("Le paramètre a bien été modifié");
		
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
		
		confirmation ->setLayout (layoutPrincipal);
		confirmation -> show();
		confirmation -> resize(200,100);

	}
}
