/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : ReadCANThread.cpp
 *	Package : Clickodrome
 *	Description : gere la lecture de messages CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20081126
 */

 
 #include "ReadCANThread.h"
 
 void ReadCANThread::run()
{	
	//variable qui permet d'interrompre le thread quand on la met à un
	interrompue=0;

	//buffer dans lequel on stocke les données quand elle sont lues une par une
	Uint8 buffer1[1];

	//buffer dans lequel on stocke les données quand elle sont lues par 12 (cas ou l'on a détecter un message CAN)
	Uint8 buffer2[12];

				
        //Variable qui permet de savoir si on a detecté le caractère SOH qui définit le debut d'un message CAN
	int SOH=0;
	cout << "Start readCAN\n";
	while(interrompue==0)
	{
		// Cas ou l'on a detecté le caractère SOH
		if (SOH==1)
		{
                        //On  lit les 12 prochains UINT8 inscrits sur le CAN
			connexion ->readCOM(buffer2,12);
			
                        /*//Affichage sur la console du nombre d'octets lus pour debuguage
			cout << connexion -> getNbBytesRead();*/
			
                        //si on à lu 12  UINT8 et que le dernier correspond au caractère EOT c'est que l'on a bien un message CAN
			if(connexion -> getNbBytesRead()==12 && buffer2[11]==(Uint8)CAN_EOT)
			{
				//on emet alors un signal qui appelera la methode receiveMessageCAN (dans le fichier FenCAN.cpp) qui est la methode qui permet d'afficher le message apres formatage  
					emit(messageCANRecu(buffer2));
			}
			
			/*else
			{
				//vérification qu'il s'agisse d'un message texte
				//on emet alors un signal qui permet d'appeler la fonction receiveMessage (dans le fichier FenCAN.cpp)  qui permet d'afficher le texte reçu
				emit(messageRecu(buffer2,connexion -> getNbBytesRead()));
			}*/
			SOH=0;
		}
		//On lit UINT8 par UINT8 et on vérifie à chaque fois si le caractère lu est le caractère SOH sinon on l'affiche en ASCII
		else
		{
			connexion ->readCOM(buffer1,1);
			
			if(connexion->getNbBytesRead()>0)
			{
                                //cout<<buffer1<<endl;
				if (buffer1[0]==(Uint8)CAN_SOH)
				{
					SOH=1;
				}
				//appel de la methode pour afficher le caractère ASCII
				else
				{
					emit(messageRecu(buffer1,1));
				}
			}
		}
	}
}

void ReadCANThread::interrompre(){

	interrompue=1;
}
