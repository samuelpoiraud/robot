/**
@Version :  0.01a
@Dernière modification :  07/09/08
@Auteur : A. OTTAVI
@Description : La fenêtre permettant d'ajouter un paramètre de message CAN,  necessite la librairie Xml de QT pour compiler.
**/

#include "FenAjouterParam.h"

FenAjouterParam::FenAjouterParam(QWidget *parent = 0) : QDialog(parent)
{                                                   
    setWindowTitle ("Ajout paramètres");
    
    ajouter=new QPushButton("Ajouter");   
    nom=new QLineEdit();
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Nom", nom);
    
    type=new QComboBox();
    type -> addItem("Double");
    type -> addItem("Int");
    type -> addItem("String");
    type -> addItem("Booleen");
    formLayout->addRow("Type", type);

	taille=new QComboBox();
	taille -> addItem("Uint8");
	taille -> addItem("Uint16");
	formLayout->addRow("Taille", taille);
	
	choix=new QLineEdit();
	choix -> setEnabled(false);
	formLayout -> addRow("Liste de choix:",choix);
	/*
	string1=new QLineEdit();
	string2=new QLineEdit();
	
	string1 -> setEnabled(false);
	string2 -> setEnabled(false);
	
	formLayout -> addRow("Choix 1",string1);
	formLayout -> addRow("Choix 2",string2);
	*/
	
    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    layoutPrincipal->addLayout(formLayout);
    layoutPrincipal->addWidget(ajouter);

    resize(250,300);
    setLayout(layoutPrincipal);
    
	connect(ajouter, SIGNAL(clicked()), this, SLOT(on_ajouter_clicked()));
	connect(type,SIGNAL(currentIndexChanged(int)), this, SLOT(on_type_currentIndexChanged(int)));  
}

void FenAjouterParam::on_type_currentIndexChanged(int machin){

	choix -> setEnabled(false);
	
	if(type -> currentText()=="String") {
			choix -> setEnabled(true);
	}
}
void FenAjouterParam::on_ajouter_clicked(){

	QDomDocument doc( "parametres" );
	QFile file( XML_PARAM_CAN );
	

	
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
		
	     if (file.open(QIODevice::ReadOnly))
	     {                                  
	         doc.setContent(&file);
	         file.close();
	     }      
	     
		 
		 
	                  
	     // Here we append a new element to the end of the document
	     QDomElement elem = doc.createElement("parametre");
	     elem.setAttribute("nom",nom -> text());
	     elem.setAttribute("type",type -> currentText());
		 elem.setAttribute("taille",taille -> currentText());
		 
		QString nomVarString;
		QStringList listeChoix=choix->text().split(";");
	
		 if(type -> currentText()=="String") {
			
			for(int i=0;i<listeChoix.size();i++){
				nomVarString="string"+QString::number(i);
				elem.setAttribute(nomVarString,listeChoix.at(i));
			}
		 }
	     
		 //QDomElement elem2 = doc.createElement("param");
	     //elem2.setAttribute("nom","X");
	     //elem2.setAttribute("type","Double");
	     
	     QDomElement root = doc.documentElement();

	     root.appendChild(elem);

	     //elem.appendChild(elem2);
     
     
            
	    if( file.open( QIODevice::WriteOnly ) )
	    {          
	        QTextStream ts( &file );
	        ts << doc.toString();
	                
	        file.close();
	    }
		
		
		
		QDialog *confirmation= new QDialog(this);
		confirmation -> setWindowTitle("Confirmation");
		
		QLabel *label = new QLabel(confirmation);
		label ->setText("Le paramètre a bien été ajouté");
		
		QVBoxLayout *layoutPrincipal = new QVBoxLayout;
		layoutPrincipal->addWidget(label);
		
		confirmation ->setLayout (layoutPrincipal);
		confirmation -> show();
		confirmation -> resize(200,100);
		
	}
}

