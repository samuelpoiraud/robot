/*Strat 2014
	Zone maison (0,color_Y(0)) => (0, color_Y(900))
	Zone milieu 1 (0,color_Y(900)) => (0,color_Y(2100)) X allant de 0 à 1000
	Zone milieu 1a (0,color_Y(900)) => (0,color_Y(1500)) X allant de 1000 à 2000
	Zone milieu 2b (0,color_Y(1500)) => (0,color_Y(2100)) X allant de 1000 à 2000
	Zone adverse (0,color_Y(2100))=> (0,color_Y(3000))

*/




























/*Strat 2013


          en utilisant (x,color(y))


   red(2000,3000)                                       blue(2000,3000)
         Etage1Bougie1                 Etage1Bougie6
   _____________________________________________________________
   |      |        \                        /           |      |
   |Assiette5       \                      /            |AssietteEnnemi5
   |      |          \                    /             |      |
   |      |           \__________________/              |      |
   |      |                                             |      |
   |      |                                             |      |
   |      |                                             |      |
   |      |                                             |      |
   |      |       Verre3              VerreEnnemi3      |      |
   |      |      *     *                 *     *        |      |
   |      |                                             |      |
   |      |        *     *             *     *          |      |
   |      |                                             |      |
   |      |      *     *                 *     *        |      |
   |Départ|       Verre1              VerreEnnemi1      |      |
   |      |                                             |      |
   |Assiette1                                           |AssietteEnnemi1
   |      |                                             |      |
   _____________________________________________________________
            [][]       [][]           [][]          [][]
          Cadeau1     Cadeau2        Cadeau3       Cadeau4
   blue(0,0)                                                red(0,0)
   
																en (X,Y) -> (0,0)
																
																

   
************ STRAT VERRE **************   
avance pres des cadeau 
   (300,580) curve
   (300,2400) no curve

ramasse les deux premieres ligne de verres
   (675,2300) curve 
   (275,500) no curve

reviens en arriere en faisant un créneau
   (675,2300) curve
   (1100,2300) curve
   (1000,2300) curve

rammasse la seconde ligne de verres 
   (1050,2050) curve
   (1050,1000) curve
   (700,420) curve
   
*******************************************

***** STRAT GÂTEAU******

position de chaque balle sur le gateau (pour la pos du robot en fonction de sa distance à la balle Cf.tableau_gateau excel
angle	-7,5		-11,25		-22,5		-33,75		-37,5		-52,5		-56,25		-67,5		-78,75		-82,5		-97,5		-101,25		-112,5		-123,75		-127,5		-142,5		-146,25		-157,5		-168,75		-172,5
Y		805,988597	813,4503037	853,2843272	917,9712714	944,6526618	1073,867	1111,100837	1232,121597	1363,436775	1408,631665	1591,368335	1636,563225	1767,878403	1888,899163	1926,133	2055,347338	2082,028729	2146,715673	2186,549696	2194,011403
X		1908,631665	1863,436775	1732,121597	1611,100837	1573,867	1444,652662	1417,971271	1353,284327	1313,450304	1305,988597	1305,988597	1313,450304	1353,284327	1417,971271	1444,652662	1573,867	1611,100837	1732,121597	1863,436775	1908,631665


********************************************/


#ifndef GRILLE_H
	#define GRILLE_H

	typedef enum {
		GOAL_Cadeau0, //Le plus proche des cadeaux
		GOAL_Cadeau1,
		GOAL_Cadeau2,
		GOAL_Cadeau3,

		GOAL_Assiette0, //La plus proche des cadeaux
		GOAL_Assiette1,
		GOAL_Assiette2,
		GOAL_Assiette3,
		GOAL_Assiette4,

		GOAL_AssietteEnnemi0, //La plus proche des cadeaux
		GOAL_AssietteEnnemi1,
		GOAL_AssietteEnnemi2,
		GOAL_AssietteEnnemi3,
		GOAL_AssietteEnnemi4,

		GOAL_Verres0,  //2 par 2
		GOAL_Verres1,
		GOAL_Verres2,

		GOAL_VerresEnnemi0,  //2 par 2
		GOAL_VerresEnnemi1,
		GOAL_VerresEnnemi2,

		GOAL_Etage1Bougie0,  //Première bougie de notre coté
		GOAL_Etage1Bougie1,
		GOAL_Etage1Bougie2,
		GOAL_Etage1Bougie3,
		GOAL_Etage1Bougie4,  //Blanche
		GOAL_Etage1Bougie5,  //Blanche
		GOAL_Etage1Bougie6,  //Blanche
		GOAL_Etage1Bougie7,  //Blanche
		GOAL_Etage1Bougie8,
		GOAL_Etage1Bougie9,
		GOAL_Etage1Bougie10,
		GOAL_Etage1Bougie11,

		GOAL_Etage2Bougie0,  //Première bougie de notre coté
		GOAL_Etage2Bougie1,
		GOAL_Etage2Bougie2,
		GOAL_Etage2Bougie3,
		GOAL_Etage2Bougie4,  //Blanche
		GOAL_Etage2Bougie5,  //Blanche
		GOAL_Etage2Bougie6,  //Blanche
		GOAL_Etage2Bougie7,  //Blanche
		GOAL_Etage2Bougie8,
		GOAL_Etage2Bougie9,
		GOAL_Etage2Bougie10,
		GOAL_Etage2Bougie11
	} map_goal_e;

#endif /* GRILLE_H */
