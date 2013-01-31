#include <QApplication>
#include <QtGui>
#include "fenprincipale/FenPrincipale.h"
#include "fenprincipale/ui_FenPrincipale.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FenPrincipale fenetre;

    fenetre.show();

    return app.exec();
}
