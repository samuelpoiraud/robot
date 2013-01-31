#ifndef SCRIPTTHREAD_H
#define SCRIPTTHREAD_H

#include <QThread>
#include <iostream>
#include <QtXml>

using namespace std;
typedef unsigned char Uint8;

class ScriptThread : public QThread
{
	Q_OBJECT

	public:
		ScriptThread(QDomElement);
		void run();

	signals:
		void messageEnvoye(bool);
		void scriptVersFenCAN(Uint8*);

	private:
		QDomElement e;
};

#endif // SCRIPTTHREAD_H
