/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : ReadCANreplayThread.cpp
 *	Package : Clickodrome
 *	Description : gere la lecture de messages CAN et du temps pour le replay du match
 *	Auteur : Aur�lie Ottavi && Damien DUVAL
 *	Version 20090407
 */

#include "ReadCANreplayThread.h"
 /*------------------------------------------------------------------
 Ecrit le message CAN re�u � l'instant t dans un fichier situ� dans release/replays � la date et � l'heure de l'ex�cution
 Format d'une ligne du fichier : 	[t] <nom> : DATA -> <data> \n
 --------------------------------------------------------------------*/
 void ReadCANreplayThread::run()
         {	
                        // Reset des donn�es du replay
                        emit(reinitialiserReplay());

			//variable qui permet d'interrompre le thread quand on la met � un
			interrompue3=0;
			
			//buffer dans lequel on stocke les donn�es quand elle sont lues une par une
			Uint8 buffer1[1];
			
			//buffer dans lequel on stocke les donn�es quand elle sont lues par 12 (cas ou l'on a d�tecter un message CAN)
			Uint8 buffer2[12];
			
			//buffer dans lequel on stocke la valeur du temps (en 1/4 de seconde)
			Uint8 buffer3[4];
			
			//Variable qui permet de savoir si on � detect� le caract�re SOH qui d�finit le debut d'un message CAN 
			int SOH=0;
                        int STX=0;
			
                        //Variable qui permet de g�rer le temps
                        double temps=0;
			
			cout << "Start readCAN for replay\n";
			
			//R�cup�re la date et l'heure d'aujourd'hui
			QDate date = QDate::currentDate();
			QTime heure = QTime::currentTime();
			
			// Cree un fichier nomm� replay_06_04_2009_13-37-42.txt � 13h37mn et 42s.
			file.setFileName("replays/replay_" + date.toString("dd_MM_yyyy_") + heure.toString("HH-mm-ss") + ".txt");
		    if(file.open(QIODevice::WriteOnly | QIODevice::Text)) cout << "Fichier Replay cree\n"; // "Text " pour pouvoir placer des retours � la ligne o� on le souhaite.
			
			QString ligne ;
			ligne = "Fichier cr�� le " + date.toString("dd-MM-yyyy")+ " � " + heure.toString("HH:mm:ss") + "\nReplay du match\n\n" ;
			file.write(qPrintable(ligne),(qint64)ligne.size());
			file.flush();
			
			while(!interrompue3)
			{
				connexion ->readCOM(buffer1,1);
				if(connexion ->getNbBytesRead()>0)
				{
					SOH=(buffer1[0]==(Uint8)0x01); // indique si l'octet lu est SOH
					STX=(buffer1[0]==(Uint8)0x02); // indique si l'octet lu est STX
					if (SOH || STX)
					{
						//Timer pour eviter des pb lorsque l'on envoie un message texte lettre par lettre,
						for(int i=0;i<5000000;i++);
					}
				}

				if (STX==1)
				{
					// Cas ou on d�tecte le caract�re STX
					//lire les deux octets du temps et le ETX finissant le bloc
					connexion ->readCOM(buffer3,3);

                                        //Affichage sur la console du nombre d'octets lus pour debuguage
                                        //cout << connexion -> getNbBytesRead();

					//V�rification du Message temps
					if(connexion -> getNbBytesRead()==3 && buffer3[2]==(Uint8)0x03)
					{
						cout<<"message temps bien re�u\t";
						temps = (buffer3[0]*255 + buffer3[1])  /4.0 ;
					}
					STX = 0; //mis � jour � chaque fois, donc inutile dans la forme actuelle du programme
				}
				// Cas ou l'on a detect� le caract�re SOH
				if (SOH==1)
				{
                                        //On  lit les 12 prochains UINT8 inscrits sur le CAN
					connexion ->readCOM(buffer2,12);
						
                                        //Affichage sur la console du nombre d'octects lus pour debuguage
                                        //cout << connexion -> getNbBytesRead();
						
                                        //si on a lu 12  UINT8 et que le dernier correspond au caract�re EOT c'est que l'on a bien un message CAN
					if(connexion -> getNbBytesRead()==12 && buffer2[11]==(Uint8)0x04)
					{
							/*
							ligne="01";
							//On met le buffer en QString pour l'�crire dans le fichier
							for(int i=0; i<=10 ; i++)
								ligne += QString::number(buffer2[i]);
							
							ligne+="04\n";
							file.write(qPrintable(ligne),(qint64)ligne.size());
							file.flush();
							*/
						 //cout<<"message CAN bien re�u";
						ligne = "[" + QString::number(  temps ,'f',2) + "] ";
						file.write(qPrintable(ligne),(qint64)ligne.size());
						file.flush();
						receiveMessageCAN(buffer2, temps); // Met en forme le buffer2 (messageCAN) � la suite du temps !
					}
					SOH=0; //mis � jour � chaque fois, donc inutile dans la forme actuelle du programme
				}
			}
			
			file.close();
         }

void ReadCANreplayThread::interrompre(){

	interrompue3=1;
}

/*-----------------------------------------------------------------------------------------------------------
R�cup�ration et modification de la fonction du m�me nom de FenCAN.cpp
Fonction qui permet de transformer  un message CAN au format Uint8 en une version comprehensible par l'utilisateur
Ecrit le message CAN dans un fichier au lieu de l'�crire dans la fen�tre
Le temps est mis en param�tre pour pouvoir l'envoyer dans la classe Replay
-------------------------------------------------------------------------------------------------------------*/
void ReadCANreplayThread::receiveMessageCAN(Uint8 *buffer, double temps){

	///cout << "Debut traitement message CAN re�u\n";
	int angle=0;
	int x=0;
	int y=0;	
	QPoint positionRobot;
	
	//chaine de caract�re ou les param�tres du message CAN et leur valeurs seront stock� apr�s mise en forme
	QString data="";
	
	//recuperation et mise en forme du code du message 
	QString codeModif1=QString::number(buffer[0],16);
	while(codeModif1.size()<2)codeModif1="0"+codeModif1;	
	QString codeModif2=QString::number(buffer[1],16);
	while(codeModif2.size()<2)codeModif2="0"+codeModif2;
	
	QString code="0x"+codeModif1+codeModif2;
	
	//recuperation du nom du message CAN associ� au code � l'aide du fichier messagesCAN.xml
	QDomNode n=returnNodeMessCAN("code",code);
	if(!(n.isNull () )){
		QDomElement e = n.toElement();
		QString nom=e.attribute("nom", "" );
		
		
		//on met en forme le message CAN en fonction des param�tres qui lui sont associ� (toujours d'apres le fichier messagesCAN.xml)
		QDomNode parametre = n.firstChild();
		
		int i=0;
		
		if(!parametre.isNull())data=" : DATA -> ";
		while(!parametre.isNull()){
			
			QDomElement eParametre = parametre.toElement();
			if( eParametre.tagName() == "parametre" ){
				//si le param�tre � mettre en forme est un string, on ne le traite pas de la m�me fa�on que les autres
				if(eParametre.attribute( "type", "" )=="String"){
				
					data+=eParametre.attribute( "nom", "" )+" : ";

					//il faut parcourir le fichier parametres.xml pour savoir quelles sont ses valeurs possibles (associ�es � 0 ou 1)
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
											if(e.attribute( "nom", "" )==eParametre.attribute( "nom", "" )){
												if(QString::number(buffer[i+2])=="0")
													data+=e.attribute("string1", "" )+" ";  //pas comprendre
												else data+=e.attribute("string2", "" )+" ";
											}
										}
									}
								   
									n = n.nextSibling();
								}
							}
						}
					}
					//de plus le param�tre n'est cod� que sur 1 octet contrairement aux autres qui sont sur deux octets
					i++;
				}
				
				// si le param�tre � mettre en forme est l'angle
				else if(eParametre.attribute( "nom", "" )=="A"){ 
					signed short temp=buffer[i+2]*256+buffer[i+3];
					
					angle=(round)(((temp)*360.0)/(3.1415*2.0*4096.0));
					
					//pour retrouver la valeur de l'angle en degr�s (marche pas)
					data+=eParametre.attribute( "nom", "" )+" : "
					+QString::number((int) angle);

		
					//l'angle est cod� sur deux octets
					i+=2;
				}
				
				//pour les autres param�tres
				else{
					if(eParametre.attribute( "nom", "" ).compare("X")==0) x=buffer[i+2]*256+buffer[i+3];
					if(eParametre.attribute( "nom", "" ).compare("Y")==0) y=buffer[i+2]*256+buffer[i+3];
					data+=eParametre.attribute( "nom", "" )+" : ";
					data+=QString::number(buffer[i+2]*256+buffer[i+3])+" ";
					//ils sont cod�s sur deux octets
					i+=2;
				}
			}										
			parametre = parametre.nextSibling();
		}	

                //affichage de la position reele renvoy�e par le robot
                if(nom.compare("CARTE_P_TRAJ_FINIE")==0 || nom.compare("CARTE_P_ROBOT_FREINE")==0 || nom.compare("CARTE_P_POSITION_ROBOT")==0 ) {
		
			positionRobot.setX(x);
			positionRobot.setY(y);
			
                        emit(sendPositionReplay(temps,positionRobot,angle));
		
                }
		///nom +=" : ";
		file.write(qPrintable(nom),(qint64)nom.size());
		file.flush();
	}
        else {
            data+=QString("Message CAN inconnu : ");
            //data+= code ;
			//Affichage du code en hexad�cimal
			QString messageHexa;
			for(int i=0;i<13;i++)
			{
				if((int)(buffer[i])<16)
				{
					messageHexa+="0"+QString::number(buffer[i],16)+" ";
				}
				else messageHexa+=QString::number(buffer[i],16)+" ";
			}
        }
	
	//affichage des param�tres du message CAN
	data += "\n";
	file.write(qPrintable(data),(qint64)data.size());
	file.flush();
	
	///cout << "Fin traitement message CAN re�u\n\n";
}


