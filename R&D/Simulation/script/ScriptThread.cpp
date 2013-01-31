#include "ScriptThread.h"

ScriptThread::ScriptThread(QDomElement message)
{
	e = message ;
}

void ScriptThread::run()
{

	// tableau d'octets pour les messages à envoyé
	Uint8 message[13];

	//debut de message = SOH
	message[0]= (Uint8)0x01;

	QString code = e.attribute( "SID", "");

	//on coupe le code du message en deux
	Uint8 code0=(Uint8)( code.section ( "", 1, 1).toInt() );
	Uint8 code1=(Uint8)((code.section ( "", code.length()-1, code.length()-1).toInt())*16+code.section ( "", code.length(), code.length()).toInt());

	// debut du code du message (sur 2 bytes)
	message[1]=code0;

	// fin du code du message (sur 2 bytes)
	message[2]=code1;  //4 au lieu de 1

	QList <QString> data = e.attribute("data","").split("");

	//Mise en forme des données
	int i;
	for(i = 0 ; i < data.size() ; i++)
	{
		message[i+3] = data.at(i).toInt(0,16);
	}

	//fin du message :EOT
	message[12]=(Uint8)(0x04);

	//test pour voir ce ke l'on envoie
	QString messageSend;
	for(int i=0;i<13;i++){
		if((int)(message[i])<16){
			messageSend+="0"+QString::number(message[i],16)+" ";
		}
		else messageSend+=QString::number(message[i],16)+" ";
	}
	cout << messageSend.toStdString() << endl;

	//Envoi du Message
	//envoyerMessageCAN(message);
	emit(scriptVersFenCAN(message));
	emit(messageEnvoye(true));
}

