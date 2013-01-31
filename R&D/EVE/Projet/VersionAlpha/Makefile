# Makefile de compilation
# eve : compilation du logiciel EVE
# compile_strategie : compilation du code de strategie
#
# Auteur : Julien

eve:
	gcc -Wall -o EVE *.c EVE_QS/*.c IHM/*.c -lm -lpthread `pkg-config --cflags --libs gtk+-2.0`

compile_actionneurs:
	gcc -Wall -c CodesCartes/Actionneurs/*.c CodesCartes/Actionneurs/QS/*.c EVE_QS/*.c
	gcc -o CodeActionneurs *.o -lm
	rm *.o

compile_balises:
	gcc -Wall -c CodesCartes/Balises/*.c CodesCartes/Balises/QS/*.c EVE_QS/*.c
	gcc -o CodeBalises *.o -lm
	rm *.o

compile_propulsion:
	gcc -Wall -c ../../../../propulsion/Software/project_2011/*.c ../../../../propulsion/Software/project_2011/QS/*.c ../../../../propulsion/Software/project_2011/QS/EVE_QS/*.c
	gcc -o CodePropulsion *.o -lm
	rm *.o

compile_strategie_old:
	gcc -Wall -c CodesCartes/Strategie/*.c CodesCartes/Strategie/QS/*.c EVE_QS/*.c
	gcc -o CodeStrategie *.o -lm
	rm *.o

compile_strategie:
	gcc -Wall -c ../../../../carteS/Software\(2011-2012\)/Project/*.c ../../../../carteS/Software\(2011-2012\)/Project/QS/*.c ../../../../carteS/Software\(2011-2012\)/Project/QS/EVE_QS/*.c
	gcc -o CodeStrategie *.o -lm
	rm *.o

compile_supervision:
	gcc -Wall -c CodesCartes/Supervision/*.c CodesCartes/Supervision/QS/*.c EVE_QS/*.c
	gcc -o CodeSupervision *.o -lm
	rm *.o

