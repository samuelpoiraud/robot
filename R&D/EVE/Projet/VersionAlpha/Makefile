# Makefile de compilation
# eve : compilation du logiciel EVE
# compile_strategie : compilation du code de strategie
#
# Auteur : Julien

eve:
	gcc -Wall -o EVE *.c EVE_QS/*.c IHM/*.c `pkg-config --cflags gtk+-2.0` -lm -lpthread `pkg-config --cflags --libs gtk+-2.0`

compile_actionneurs: CodeActionneurs

CodeActionneurs:
	gcc  -Wall  -o CodeActionneurs ../Robot/CarteActionneur/*.c ../Robot/CarteActionneur/QS/*.c EVE_QS/*.c -I. -lm `pkg-config --cflags gtk+-2.0` `pkg-config --cflags --libs gtk+-2.0`
#	gcc -o CodeActionneurs *.o -lm
#	rm *.o

compile_balises: CodeBalises

CodeBalises:
	gcc  -Wall  -o CodeBalises ../Robot/CarteBaliseReceptriceUS/*.c ../Robot/CarteBaliseReceptriceUS/QS/*.c EVE_QS/*.c  -I. -lm `pkg-config --cflags gtk+-2.0` `pkg-config --cflags --libs gtk+-2.0`
#	gcc -o CodeBalises *.o -lm
#	rm *.o

compile_propulsion: CodePropulsion

CodePropulsion:
	gcc  -Wall  -o CodePropulsion ../Robot/CartePropulsion/*.c ../Robot/CartePropulsion/QS/*.c EVE_QS/*.c -I. -Ihard_includes -lm `pkg-config --cflags gtk+-2.0` `pkg-config --cflags --libs gtk+-2.0`
#	gcc -o CodePropulsion *.o -lm
#	rm *.o

CodeStrategie_old:
	gcc  -Wall  -o CodeStrategie ../Robot/CarteStrategie/*.c ../Robot/CarteStrategie/QS/*.c EVE_QS/*.c -I. -lm `pkg-config --cflags gtk+-2.0` `pkg-config --cflags --libs gtk+-2.0`
#	gcc -o CodeStrategie *.o -lm
#	rm *.o

compile_strategie: CodeStrategie

CodeStrategie:
	gcc  -Wall  -o CodeStrategie ../Robot/CarteStrategie/*.c ../Robot/CarteStrategie/QS/*.c EVE_QS/*.c -I. -lm `pkg-config --cflags gtk+-2.0` `pkg-config --cflags --libs gtk+-2.0`
#	gcc -o CodeStrategie *.o -lm
#	rm *.o

compile_supervision: CodeSupervision

CodeSupervision:
	gcc  -Wall -o CodeSupervision ../Robot/CarteSupervision/*.c ../Robot/CarteSupervision/QS/*.c EVE_QS/*.c  -I. -lm `pkg-config --cflags gtk+-2.0` `pkg-config --cflags --libs gtk+-2.0`
#	gcc -o CodeSupervision *.o -lm
#	rm *.o

