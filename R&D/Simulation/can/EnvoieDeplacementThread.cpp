/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : EnvoieDeplacementThread.h
 *	Package : Clickodrome
 *	Description : gere l'envoie d'une trajectoire point par point a partir du clickodrome
 *	Auteur : Aurélie Ottavi
 *	Version 20090217
 */
 
 #include "EnvoieDeplacementThread.h"

 
 void EnvoieDeplacementThread::run()
 {	
	int envoiInterrompue=0;
	Uint8 message[13];
	Uint8 message2[13];
	Uint8 message11[11];
	trajFinie=false;

	cout << "Debut Envoie deplacement\n";
	
	//envoi de tout les points
	for (int i=0; i<pointsNumber;i++)
	{
		cout << "compteur nbrPoint envoyés :i=";
		cout << i;
		cout << "\n";

		//debut de message = SOH
		message2[0]= (Uint8)CAN_SOH;
	
		//code du message
		message2[1]=(Uint8)CAN_ASSER_GO_POSITION_1;
		message2[2]=(Uint8)CAN_ASSER_GO_POSITION_2;
					
		//parametre Mode multi points
		message2[3]=(Uint8)CAN_MULTI_POINTS;
		
		//parametre x
		message2[4]=(Uint8)((points[i].x())/256);
		message2[5]=(Uint8)((points[i].x())%256);
		
		//parametre y
		message2[6]=(Uint8)((points[i].y())/256);
		message2[7]=(Uint8)((points[i].y())%256);
		
		//parametre direction
		message2[8]=(Uint8)0x00;
		
		//parametre vitesse
		message2[9]=(Uint8)CAN_LENT;
		
		//parametre rayon de courbure
		message2[10]=(Uint8)0x00;
		
		//taille des parametres
		message2[11]=(Uint8)(8);
		
		//fin du message :EOT
		message2[12]=(Uint8)CAN_EOT;
	
		Uint8 message11[11];
		for(int i=0;i<11;i++)message11[i]=message2[i+1];
	
		//on affiche ce que lon envoie
		CANtoString(message11);
	
		cout << "Envoie Point\n";
		//emettre un signal pour ecrire le message sur le port serie
		emit(ecrireMessage(message2));
	}

	if(angleAenvoyer)
	{
		//debut de message = SOT
		message[0]= (Uint8)CAN_SOH;
	
		//code du message
		message[1]=(Uint8)CAN_ASSER_GO_ANGLE_1;
		message[2]=(Uint8)CAN_ASSER_GO_ANGLE_2;
					
		//parametre Mode multi points
		message[3]=(Uint8)CAN_MULTI_POINTS;
		
		//parametre angle
		if(angle<0)
		{
			angle=(int)(((angle+360)*2*PI/360.0)*4096);
        }
        else 
        {
			angle=(int)((angle*2*PI/360.0)*4096);
		}
		
		message[4]=((angle)/256);
		message[5]=((angle)%256);
		
		//parametre vide
		message[6]=0;
		message[7]=0;
		
		//parametre direction
		message[8]=(Uint8)0x00;
		
		//parametre vitesse
		message[9]=(Uint8)CAN_LENT;
		
		//parametre rayon de courbure
		message[10]=(Uint8)0x00;
		
		//taille des parametres
		message[11]=(Uint8)(8);
		
		//fin du message :EOT
		message[12]=(Uint8)CAN_EOT;
			
		for(int i=0;i<11;i++)
		{
			message11[i]=message[i+1];
		}
		
		//on affiche ce que lon envoie
		CANtoString(message11);
		
		cout << "Envoie Angle\n";
		//emettre un signal pour ecrire le message sur le port serie
		emit(ecrireMessage(message));		
	}
	cout<< angleAenvoyer<<endl;
	cout << "finThread\n";
	
	trajFinie=true;
	envoiInterrompue=0;
	trajectoireFinie=1;
}

void EnvoieDeplacementThread::CANtoString(Uint8 *buffer)
{
	//chaine de caractère ou les paramètres du message CAN et leur valeurs seront stocké après mise en forme
	QString data="";
	
	//recuperation et mise en forme du code du message 
	QString codeModif1=QString::number(buffer[0],16);
	while(codeModif1.size()<2)codeModif1="0"+codeModif1;	
	QString codeModif2=QString::number(buffer[1],16);
	while(codeModif2.size()<2)codeModif2="0"+codeModif2;
	
	QString code="0x"+codeModif1+codeModif2;
	
	//recuperation du nom du message CAN associé au code à l'aide du fichier messagesCAN.xml
	QDomNode n=returnNodeMessCAN("code",code);
	QString nom;
	
	if(!(n.isNull () ))
	{
		QDomElement e = n.toElement();
		nom=e.attribute("nom", "" );
		
		
		//on met en forme le message CAN en fonction des paramètres qui lui sont associé (toujours d'apres le fichier messagesCAN.xml)
		QDomNode parametre = n.firstChild();
		
		int i=0;
		
		if(!parametre.isNull())
        {
            data="DATA -> ";
        	while(!parametre.isNull())
            {
                
                QDomElement eParametre = parametre.toElement();
                if( eParametre.tagName() == "parametre" )
                {
                    QStringList test;
                    QString sTaille;
                    QDomDocument doc2( "parametres" );
                    QFile file2( XML_PARAM_CAN );
                    
                    test << "Double"<<"Int"<<"Booleen"<<"String";
                    switch(test.indexOf(eParametre.attribute( "type", "" )))
                    {
                        case 0:;
                            break;
                        case 1:sTaille = eParametre.attribute( "taille", "" );
                            if(QString::compare(sTaille,"Uint8")==0)
                            {
                                data+=eParametre.attribute( "nom", "" )+" : ";
                                data+=QString::number(buffer[i+2])+" ";
                                i++;
                            }
                            else
                            {
                                signed short temp=buffer[i+2]*256+buffer[i+3]; ;
                                // si le paramètre à mettre en forme est l'angle
                                if(eParametre.attribute( "nom", "" )=="A")
                                { 
                                    temp=(int)round(((temp)*360.0)/(PI*2.0*4096.0));
                                }	
                                data+=eParametre.attribute( "nom", "" )+" : "
                                +QString::number(temp);

                    
                                //l'angle est codé sur deux octets
                                i+=2;
                            }
                            break;
                        case 3: data+=eParametre.attribute( "nom", "" )+" : ";

                                    //il faut parcourir le fichier parametres.xml pour savoir quelles sont ses valeurs possibles (associées à 0 ou 1)
                                   
                                    if( file2.open(QIODevice::ReadOnly) )
                                    {  
                                        if( doc2.setContent( &file2 ) )
                                        {           
                                            file2.close();
                                                        
                                            QDomElement root = doc2.documentElement();
                                            
                                            if( root.tagName() == "listeParametres" )
                                            {
                                                        
                                                QDomNode n = root.firstChild();
                                      
                                                while( !n.isNull() )
                                                {
                                                    QDomElement e = n.toElement();
                                                    if( !e.isNull() )
                                                    {
                                                        if( e.tagName() == "parametre" )
                                                        {   
                                                            if(e.attribute( "nom", "" )==eParametre.attribute( "nom", "" ))
                                                            {
                                                                if(QString::number(buffer[i+2])=="0")
                                                                {
                                                                    data+=e.attribute("string1", "" )+" ";
                                                                }
                                                                else 
                                                                {
                                                                    data+=e.attribute("string2", "" )+" ";
                                                                }
                                                            }
                                                        }
                                                    }
                                                   
                                                    n = n.nextSibling();
                                                }
                                            }
                                        }
                                    }
                                    //de plus le paramètre n'est codé que sur 1 octet contrairement aux autres qui sont sur deux octets
                                    i++;
                                break;    
                        default:;
                            break;
                    }										
                    parametre = parametre.nextSibling();
                }
            }
        }
		else
        {
            data+=QString("Message CAN inconnu");
        }
		
		envoieMessageCANString(nom,data);

	}
}
	
/*-------------------------------------------------------------------------------------------------------------------------------------------------
Fonction  qui recherche un message CAN dans le fichier xml messagesCAN.xml et qui retourne le noeud correspondant 
---------------------------------------------------------------------------------------------------------------------------------------------------*/
QDomNode EnvoieDeplacementThread::returnNodeMessCAN(QString nom,QString valeur)
{

    QDomDocument doc( "messagesCAN" );
    QFile file( XML_MESSAGES_CAN );
	
    if( file.open(QIODevice::ReadOnly) )
    {  
        if( doc.setContent( &file ) )
        {           
            file.close();
                        
            QDomElement root = doc.documentElement();
            
            if( root.tagName() == "listeMessageCAN" )
			{
                        
                QDomNode n = root.firstChild();     
                while( !n.isNull() )
                {
	                QDomElement e = n.toElement();
	                if( !e.isNull() )
	                {
	                    if( e.tagName() == "messageCAN" )
	                    {   
							if(e.attribute( nom, "" )== valeur)
							{
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
