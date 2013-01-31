#ifndef FONCTIONS_H
#define FONCTIONS_H

#include <QtGui>
#include <QtXml>
#include <string>
#include <QString>
#include <QSettings>
#include <iostream>
#include <stdio.h>
#include "../can/xml_chemins.h"

using namespace std;

#define INI_FILE "config.ini"

        QDomNode returnNodeMessCAN(QString nom,QString valeur);

        QVariant readINI(char* path);
		void writeINI(char* path,QVariant value);

#endif // FONCTIONS_H
