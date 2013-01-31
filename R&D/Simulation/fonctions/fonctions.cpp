//***************************************************************************************
//* Fichier : fonctions.cpp *
//* *
//* Objet : fonctions utilisé par divers objets 
//* *
//* Commentaires: 
//* 
//* *
//* 2008       Création.        Damien DUVAL                
//* 20/12/2009 Modification.    Adrien GIRARD
//* *
//****************************************************************************************

#include "fonctions.h"

/*-------------------------------------------------------------------------------------------------------------------------------------------------
Fonction  qui recherche un message CAN dans le fichier xml messagesCAN.xml et qui retourne le noeud correspondant
---------------------------------------------------------------------------------------------------------------------------------------------------*/
QDomNode returnNodeMessCAN(QString nom,QString valeur){

	QDomDocument doc( "messagesCAN" );
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
							if(e.attribute( nom, "" )== valeur){
								//retourne le noeud
								return n;

							}
						}
					}
					n = n.nextSibling();
				}
			}
		}
   }
   return QDomNode ();
}


//***************************************************************************************
//* Fonction          : readINI()                    				                     *
//*                                                                                      *
//* Objet             : Lecture du fichier de configuration (*.ini)                      *
//*                                                                                      *
//* Paramètres        : Aucun				                                             *
//*                                                                                      *
//* Renvoie           : Rien 								                             *
//*                                                                                      *
//* Commentaires      :  						                                         *
//*                                                                                      *
//* Historique                                                                           *
//*                                                                                      *
//* Date       Commentaires          Nom                                                 *
//* ======== ===============================================                             *
//* 20/12/2009   Création.            Adrien GIRARD	                                     *
//*                                                                                      *
//****************************************************************************************/
QVariant readINI(char* path)
{
    
    QSettings theSettings(INI_FILE,QSettings::IniFormat);
    QVariant value = theSettings.value(path);
    return value;
}

//***************************************************************************************
//* Fonction          : writeINI()                    				                     *
//*                                                                                      *
//* Objet             : Lecture du fichier de configuration (*.ini)                      *
//*                                                                                      *
//* Paramètres        : Aucun				                                             *
//*                                                                                      *
//* Renvoie           : Rien 								                             *
//*                                                                                      *
//* Commentaires      :  						                                         *
//*                                                                                      *
//* Historique                                                                           *
//*                                                                                      *
//* Date       Commentaires          Nom                                                 *
//* ======== ===============================================                             *
//* 05/01/2010   Création.            Adrien GIRARD	                                     *
//*                                                                                      *
//****************************************************************************************/
void writeINI(char* path,QVariant value)
{
    
    QSettings theSettings(INI_FILE,QSettings::IniFormat);
    theSettings.setValue(path,value);
}