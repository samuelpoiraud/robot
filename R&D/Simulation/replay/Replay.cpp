/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Replay.cpp
 *	Package : Clickodrome
 *	Description : Récupère les informations de la supervision pour refaire le match
 *	Auteur : Damien DUVAL
 *	Version 20100405
 */
#include "Replay.h"


/*---------------------------------------------------
Constructeur
Initialisation des propriétés à 0
---------------------------------------------------*/
Replay::Replay(QWidget *fp)
{	
    myParent = fp;
    reinitialisation();
}

/*---------------------------------------------------
Reset quand l'utilisateur clique sur nouveau ou charger
Re-initialisation des propriétés à 0
---------------------------------------------------*/
void Replay::reinitialisation()
{
    nb_instants = -1;
    emit changeMaxSlider(0);
    for(int t=0 ; t < N_MAX ; t++){
            instants[t].temps = 0.0 ;
            instants[t].angle = 0 ;
    }
}


/*---------------------------------------------------
Getters : Renvoie l'instant t
---------------------------------------------------*/
Instant Replay::getInstant(int t)
{
	return instants[t];
}

double Replay::getTemps(int t){
    return instants[t].temps;
}

QPoint Replay::getPos(int t){
    return instants[t].pos;
}

int Replay::getAngle(int t){
    return instants[t].angle;
}


/*---------------------------------------------------
Setters : Modifie l'instant t avec la valeur i
---------------------------------------------------*/
void Replay::setInstant(Instant i, int t)
{
	instants[t] = i ;
}

void Replay::setInstant(double temps, QPoint pos, int angle, int t){
    instants[t].temps = temps;
    instants[t].pos = pos;
    instants[t].angle = angle;
}

/*---------------------------------------------------
Slot appelé lorsque le Slider a été déplacé
---------------------------------------------------*/
void Replay::sliderDeplace(int t){
    double temps = getTemps(t);
    QPoint pos = getPos(t);
    int angle = getAngle(t);

    if(!pos.isNull()){
        emit changeTime( temps);
        emit sendPositionRobot(pos, angle);
    }
}

/*---------------------------------------------------
Slot appelé par le ReplayThread pour ajouter une position
---------------------------------------------------*/
void Replay::getPositionReplay(double temps, QPoint pos, int angle){
    nb_instants++;
    setInstant(temps,pos,angle, nb_instants);
    emit changeMaxSlider(nb_instants);
}

/*---------------------------------------------------
Execution du FileDialog
---------------------------------------------------*/
void Replay::dialogFichier()
{
        QDir dir = QDir::current();

        QString dirPath=(dir.isRelativePath(nom_fichier))?dir.relativeFilePath(REPLAY_FILE):dir.absoluteFilePath(REPLAY_FILE);

        nom_fichier = QFileDialog::getOpenFileName(myParent,"Ouvrir fichier Replay ", dirPath , "Fichier txt (*.txt)");

        if(nom_fichier.isNull())
            annuler = true;
        else
            annuler = false;

        nom_fichier = (dir.isRelativePath(nom_fichier))?dir.relativeFilePath(nom_fichier):dir.absoluteFilePath(nom_fichier);

        if(nom_fichier.isEmpty())
        {
                nom_fichier = REPLAY_FILE;
        }
        else
        {
                writeINI(REPLAY_FILE_INI,nom_fichier);
        }
}

/*---------------------------------------------------
Slot appelé par l'action Charger Replay dans le menu de la fenêtre principale TODO !
---------------------------------------------------*/
void Replay::chargerReplay(){
    emit(stopReplayThread());
    dialogFichier();
    
    if(!nom_fichier.isEmpty() && !annuler)
    {
        reinitialisation(); // Reset des données du replay

        QFile file( nom_fichier );	
    
        if( file.open( QIODevice::ReadOnly ) )
        {
                QTextStream ts( &file );
                QString text = ts.readAll(); // Lit tout le fichier
                /* On lit le fichier txt formaté comme suit =>  [t] <nom> : DATA -> <data> \n  */

                QStringList lignes = text.split("\n",QString::SkipEmptyParts);

                // On enlève l'entête
                for(int i=0 ; i < 3 ; i++) lignes.removeFirst();

                //Filtrage des messages CAN
                QStringList positions;
                for(int i = 0; i < lignes.size() - 1 ; i++){
                     if( lignes.at(i).contains("CARTE_P_POSITION_ROBOT") ||
                         lignes.at(i).contains("ASSER_GO_POSITION") ||
                         lignes.at(i).contains("ASSER_GO_ANGLE ") ||
                         lignes.at(i).contains("CARTE_P_ROBOT_FREINE") ||
                         lignes.at(i).contains("CARTE_P_TRAJ_FINIE")
                        )
                            positions << lignes.at(i); // On ajoute la ligne à la liste positions
                }

                bool conversion_ok; // pour vérifier s'il n'y a pas d'erreurs
                int angle = 0, x = 0, y = 0; // Pour conserver la valeur précédente au cas où l'angle ou la position n'est pas donnée


                //pour chaque position
                for(int i = 0; i < positions.size() - 1 ; i++){

                    //On sectionne le QString :

                    /** Temps **/
                    QString selection = positions.at(i).section("]",0,0); // On prend le premier élément.
                    selection.remove("[");

                    double temps = selection.toDouble(&conversion_ok);
                    if(!conversion_ok)
                    {
                        cout << "Erreur de conversion au niveau du Temps.\n";
                        break;
                    }

                    int index_str;

                    /** Angle **/
                    if(positions.at(i).contains(" A : ")){
                        index_str = positions.at(i).indexOf(" A : ");
                        selection = positions.at(i).section("",index_str+5,index_str+10); // On prends 5 chiffres.
                        selection.remove(QRegExp("[^0123456789]")); // on ne garde que les chiffres. ne marche ni avec \d ni avec [0-9]

                        angle = selection.toInt(&conversion_ok);
                        if(!conversion_ok)
                        {
                            cout << "Erreur de conversion au niveau de l'angle.\n";
                            break;
                        }
                    }

                    /** X **/
                    if(positions.at(i).contains(" X : ")){
                        index_str = positions.at(i).indexOf(" X : ");
                        selection = positions.at(i).section("",index_str+5,index_str+10); // On prends 5 chiffres.
                        selection.remove(QRegExp("[^0123456789]")); // on ne garde que les chiffres. ne marche ni avec \d ni avec [0-9]

                        x = selection.toInt(&conversion_ok);
                        if(!conversion_ok)
                        {
                            cout << "Erreur de conversion au niveau de X.\n";
                            break;
                        }
                    }

                    /** Y **/
                    if(positions.at(i).contains(" Y : ")){
                        index_str = positions.at(i).indexOf(" Y : ");
                        selection = positions.at(i).section("",index_str+5,index_str+10); // On prends 5 chiffres.
                        selection.remove(QRegExp("[^0123456789]")); // on ne garde que les chiffres. ne marche ni avec \d ni avec [0-9]

                        y = selection.toInt(&conversion_ok);
                        if(!conversion_ok)
                        {
                            cout << "Erreur de conversion au niveau de Y.\n";
                            break;
                        }
                    }


                    QPoint pos = QPoint(x,y);



                    // On envoie finalement les données
                    getPositionReplay(temps,pos,angle);
                }

        }	
                
        file.close();
    }    
}

































