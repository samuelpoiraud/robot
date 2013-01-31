/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Brain_US.c
 *	Package : Projet Balise M�re
 *	Description : R�tentions des infos re�ues par les balises US et calculs des positions adverses.
 *	Auteur : Nirgal
 *	Version 201208
 */
	#include "Brain_US.h"
	#include "Array_US.h"
	#include "Secretary.h"
	
	
/*
O : balises fixes
R : 1 robot adverse

  ----y->				3m
  |		________________________________O  Corner
  x		|							  /	|
  |		|							 d1	|
  v		|				    	    / 	|
		O-------------d2-----------R   	|  2m
		Midle					  	\	|
		|							 d0	|
		|							  \	|
		________________________________O  Mother

*/


	#define ERROR_OBSOLESCENCE 	0b10000000
	#define ERROR_ALGORYTHM 	0b01000000	//Le calcul n'a pas fourni de donn�e satisfaisante...
	
	typedef struct
	{
		Uint8 distance; //[mm*128]
		Uint8 fiability;
		Uint8 timeout;
		bool_e datas_ready;
	}adversary_data_t;
	
	typedef struct
	{
		adversary_data_t adversary_datas[ADVERSARY_NUMBER];
	}beacon_datas_t;
	
	volatile beacon_datas_t beacon_datas[BEACONS_NUMBER];
	
	void BRAIN_US_init(void)
	{
		adversary_e adversary;
		beacon_id_e beacon_id;
		
		for(beacon_id = 0; beacon_id < BEACONS_NUMBER; beacon_id++)
		{
			for(adversary = 0; adversary < ADVERSARY_NUMBER; adversary++)
			{
				beacon_datas[beacon_id].adversary_datas[adversary].timeout = 0;
				beacon_datas[beacon_id].adversary_datas[adversary].fiability = ERROR_OBSOLESCENCE;
				beacon_datas[beacon_id].adversary_datas[adversary].datas_ready = FALSE;
			}	
		}	
	}	
	
	
	void BRAIN_US_add_datas(beacon_id_e beacon_id, adversary_e adversary, Sint16 distance, Uint8 fiability)
	{
		if(beacon_id >= BEACONS_NUMBER || adversary >= ADVERSARY_NUMBER)	//S�curit�...
			return;	//Ne dois jamais se produire...
		beacon_datas[beacon_id].adversary_datas[adversary].distance = (Uint8)((distance<0)?0:distance/128);	// [mm*128]
		beacon_datas[beacon_id].adversary_datas[adversary].fiability = fiability;
		beacon_datas[beacon_id].adversary_datas[adversary].timeout = 200;	//P�remption de la donn�e dans : 200ms
		beacon_datas[beacon_id].adversary_datas[adversary].datas_ready = TRUE;	//Les donn�es sont pr�tes.
	}
	

	void BRAIN_US_timeout_update(Uint16 ms)
	{
		adversary_e adversary;
		beacon_id_e beacon_id;
		
		for(beacon_id = 0; beacon_id < BEACONS_NUMBER; beacon_id++)
		{
			for(adversary = 0; adversary < ADVERSARY_NUMBER; adversary++)
			{
				if(beacon_datas[beacon_id].adversary_datas[adversary].timeout > ms)
				{
					beacon_datas[beacon_id].adversary_datas[adversary].timeout -= ms;
				}
				else
				{
					beacon_datas[beacon_id].adversary_datas[adversary].timeout = 0;
					beacon_datas[beacon_id].adversary_datas[adversary].fiability |= ERROR_OBSOLESCENCE;
				}	
			}	
		}	
		
	}
	
	
	void BRAIN_US_adversary_smart_watching(adversary_e adversary, Sint16 x, Sint16 y, Uint8 fiability)
	{
		//Cette fonction est d�pendante du sujet, et assure une surveillance �ventuelle de certaines activit�s de l'adversaire.
		
		//Exemple... si on voit l'adversaire s'approcher d'une certaine zone du terrain dont l'approche nous int�resse...
		// 			Alors on pr�vient la carte strat�gie...par un envoi de message sp�cifique !
		
			
	}	
	
	
	/*
	Traite les donn�es US re�ues correspondantes � l'adversaire donn� en param�tre.
	@pre Des donn�es de chacune des balises pour cet adversaire doivent �tre disponibles.
	@note nous pourrions ici remplacer les instructions semblables par un for... mais c'est nettement plus lisible comme cela !!!
	*/
	void BRAIN_US_process_adversary(adversary_e adversary)
	{
		Sint16 x, y;
		Uint8 d0, d1, d2;
		bool_e error;
		Uint8 fiability;

		//R�cup�ration des distances (voir sch�ma du terrain en haut du fichier.
		d0 = beacon_datas[BEACON_ID_MOTHER].adversary_datas[adversary].distance;
		d1 = beacon_datas[BEACON_ID_CORNER].adversary_datas[adversary].distance;
		d2 = beacon_datas[BEACON_ID_MIDLE].adversary_datas[adversary].distance;

		//debug_printf("d0:%d, d1:%d, d2:%d\n", d0, d1, d2);

		//fiability ?
		error = find_one_point_from_d0_d1_d2(&x, &y, d0, d1, d2);
		fiability = (error)?ERROR_ALGORYTHM:0;
		fiability |= beacon_datas[BEACON_ID_MOTHER].adversary_datas[adversary].fiability;
		fiability |= beacon_datas[BEACON_ID_CORNER].adversary_datas[adversary].fiability;
		fiability |= beacon_datas[BEACON_ID_MIDLE].adversary_datas[adversary].fiability;
		
		//RAZ flags datas_ready
		beacon_datas[BEACON_ID_MOTHER].adversary_datas[adversary].datas_ready = FALSE;
		beacon_datas[BEACON_ID_CORNER].adversary_datas[adversary].datas_ready = FALSE;
		beacon_datas[BEACON_ID_MIDLE].adversary_datas[adversary].datas_ready = FALSE;

//if(!fiability) 
//	debug_printf ("d0=%2d d1=%2d d2=%2d | f0=%x f1=%x f2=%x | x:%d,y:%d\n",d0, d1, d2, beacon_datas[BEACON_ID_MOTHER].adversary_datas[adversary].fiability, beacon_datas[BEACON_ID_CORNER].adversary_datas[adversary].fiability, beacon_datas[BEACON_ID_MIDLE].adversary_datas[adversary].fiability, x,y);
#warning "compte rendu tests 11/11/12 ici :"
/*
Les couches hautes et la com' semblent fonctionner.
Les principaux probl�mes et instabilit�s sont dues aux couches basses.
Il serait utile de valider la r�ception US hardware, et d'impl�menter une bonne gestion des amplis PGA (quand on est loin, on voit rien...)
Un algo de d�cision de la valeur PGA selon la saturation et la faiblesse du signal pourrait suffire !
Sans une plus grande fiabilit� des couches basses, le syst�me ne peut assurer un fonctionnement sain et n'est pas utilisable.
Attention �galement aux piles des �metteurs... qui rendent le signal plus faible !

*/	
		//Transmission des donn�es trouv�es
		SECRETARY_send_adversary_position(TRUE, adversary, x, y, fiability);
		
		//Surveillance de ce que fait l'adversaire (selon le sujet.. nous sommes libres de surveiller ce que l'on souhaite !)
		BRAIN_US_adversary_smart_watching(adversary, x, y, fiability);
	}	



	void BRAIN_US_process_main(void)
	{
		adversary_e adversary;

		for(adversary = 0; adversary < ADVERSARY_NUMBER; adversary++)
		{
			//On ne traite les donn�es re�ues pour un adversaire que lorsque on les a toutes re�ues.
			//CELA SUPPOSE QUE TOUTES CES BALISES SOIENT RELIEES...
			//A la louche, nous avons moins de 200ms pour op�rer ce traitement... m'enfin on sera probablement plus rapides...	
			if(	
				beacon_datas[BEACON_ID_MOTHER].adversary_datas[adversary].datas_ready &&
				beacon_datas[BEACON_ID_CORNER].adversary_datas[adversary].datas_ready &&
				beacon_datas[BEACON_ID_MIDLE].adversary_datas[adversary].datas_ready
			  )
				BRAIN_US_process_adversary(adversary);		//Traiter les donn�es de l'adversaire "adversary".
		}		
	}	
