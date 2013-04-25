/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : ReceptionUS.c
 *	Package : Projet Balise Récepteur US
 *	Description : Gestion de la réception US...
 *	Auteur : Nirgal
 *	Version 201012
 */

#include "ReceptionUS.h"
#include <timer.h>
#include "QS/QS_spi.h"
#include "QS/QS_extern_it.h"
#include "ports.h"
#include "QS/QS_timer.h"
#include "QS/QS_adc.h"
#include "PGA.h"
#include "Secretary.h"
#include "QS_can.h"
#include "QS_CANmsgList.h"

#define PERIODE_FLASH	50		//Période de répétition des flashs [nombre de step]	//Période du flash en µs = PERIODE_FLASH * DUREE_STEP

volatile bool_e measures_ready = FALSE;	//Des données sont prêtes pour être traitées. Les mesures sont suspendues jusqu'à la fin du traitement.
volatile Uint16 step = 0;			//Déroulement du temps référencé à "step = 0" <=> "instant 0 de l'émetteur"
volatile Uint16 distance = 0;		//distance entre les deux balises. 0 si distance inconnue.
volatile Uint16 position_pic = 0;	//Position du maximum de la convolution. (c'est l'index image de la distance !)
volatile bool_e dataready = FALSE;

//Fiabilité, détection d'erreur...

volatile Uint8 fiabilite = ERREUR_PAS_DE_SYNCHRO;

volatile Sint16 max1visu, max2visu; //Temporaire...
volatile Sint16	indexmax2visu;

#define OFFSET_MASSE_VIRTUELLE 509	//ATTENTION, penser à bien mesurer l'offset de masse virtuelle !

#define TAILLE_FENETRE 416

#define PERIODE_ACQUISITION 125 	//us (Attention, c'est lié à la fenêtre et au nombre de points qui s'y trouvent !)
#define DUREE_ACQUISITION	66		//ms (26*2 ms de motif = 54 + durée maximale du parcourt de l'onde US (12ms) = 66 ms) 
#define TAILLE_ACQUISITION 528		// "= Durée d'acquistion / période d'acquisition"
#define TAILLE_CONVOLUTION (TAILLE_ACQUISITION+TAILLE_FENETRE)	//2* pour simplifier les algos...

Sint16 acquisition[TAILLE_ACQUISITION+TAILLE_FENETRE];	//Taille_acquisition suffirait, mais cela permet de simplifier les algos..
Sint16 convolution[TAILLE_CONVOLUTION];

Uint16 acquisition_index;	//Position courante de l'acquisition

#define PERIODE_FLASH	50		//Période de répétition des flashs [nombre de step]	//Période du flash en µs = PERIODE_FLASH * DUREE_STEP
#define NOMBRE_BALISES_EMETTRICES 2

volatile adversary_e current_adversary = ADVERSARY_1;

void ReceptionUS_init(void)
{
	Uint16 i;
	acquisition_index = 0;
//	for(i=0;i<TAILLE_ACQUISITION+TAILLE_FENETRE;i++)
//		acquisition[i] = 0;	//Pas nécesaire.
	for(i=0;i<TAILLE_CONVOLUTION;i++)
		convolution[i] = 0;	//Impératif !
}	


#define DISTANCE_PAR_POINT 177152//en [mm/4096]. Théoriquement : environ 4,325cm par point...
//  Position_pic maximal = taille_acquisition + taille_fenetre = 288
//  288 * 174080 = 50135040 = 0x02FD0000 = acceptable sur 32 bits...
#define OFFSET_DISTANCE 80 //[mm]
Uint16 calcul_distance(Uint16 position_pic)
{
	Sint32 ret;	//0 signifie distance inconnue.
	
	//TODO : introduire une correction dans distance par point dépendant de la température
	
	ret = ((Sint32)(position_pic) - TAILLE_FENETRE) * DISTANCE_PAR_POINT;
	if (ret < 0)
		return 0;
	else
		return (Uint16)(ret/4096) - OFFSET_DISTANCE;	//Distance en mm
}	


void ReceptionUS_process_main(void)
{
	if(!measures_ready) 	//Si pas de données dispo, on se casse !
		return;
	//Chouette, des données sont disponibles !
	
	LED_USER = 1;													//Pour voir la durée du traitement.

	ReceptionUS_traiter_signal();
	
				//	debug_printf("\n\nSIGNAL RECU");
				//	ReceptionUS_afficher_signal(acquisition, TAILLE_ACQUISITION);
				//	debug_printf("\n\nSIGNAL CONVOLUE");
				//	ReceptionUS_afficher_signal(convolution, TAILLE_CONVOLUTION);
				//	debug_printf("\nposition_pic:%d",position_pic);

	distance = calcul_distance(position_pic);						//Calcul de la distance.
	if(distance == 0)
		fiabilite |= ERREUR_TROP_PROCHE;
	SECRETARY_add_datas(current_adversary, distance, fiabilite);	//On informe le secrétaire qu'une série de mesure a été effectuée et traitée.
	
	ReceptionUS_init();
																	//MAJ de l'ampli en fonction de la distance de l'autre adversaire... !
	//PGA_maj_ampli(SECRETARY_get_distance((current_adversary==ADVERSARY_1)?ADVERSARY_2:ADVERSARY_1);	
	
	measures_ready = FALSE; 										//On autorise une nouvelle campagne de mesures.

/*	#warning : "ces debug_printf et ce warning doivent être commentés en match !"
	debug_printf("\n pic=%d,\td=%d,err=%x",position_pic - TAILLE_FENETRE, distance, fiabilite);
	debug_printf("  max1=%d,max2=%d,\tidx=%d\n",max1visu, max2visu, indexmax2visu);
*/	LED_USER = 0;
}
	

#define OUVERTURE_PIC_CONVOLUTION		(31)
#define	ECART_MINI_ENTRE_DEUX_PICS		(1000)		//TRES IMPORTANT...
													// En cas de pic secondaire plus élevé que le pic attendu, la fausse information sera considérée valide si le pic secondaire dépasse le pic attendu de plus que ce coeff !
													// En cas de pic attendu plus élevé que le pic secondaire, la bonne information sera considérée comme invalide si le pic attendu ne dépasse pas le pic secondaire de plus que ce coeff !
#define SEUIL_SATURATION				(100)		//((1023 - OFFSET_MASSE_VIRTUELLE)/4 - 28)	//20 : valeur grosso modo pour dire "pas trop loin de la saturation"
#define CONVOLUTION_VALEUR_MINI         (256)		//Valeur minimale du max de la convolution. Si le max de la convolution est en dessous de cette valeur, le signal de la balise est insuffisant pour mesurer une distance. (ou correspond à du bruit)
#define NOMBRE_MAXIMAL_VALEURS_SATUREES	(350)	// 16 bits * 14 symboles "1" + un chouilla
#define MINI_POSITION_PIC				(TAILLE_FENETRE+1)//(3)		//en dessous, on est trop proche pour pouvoir fournir un résultat fiable

void ReceptionUS_traiter_signal(void)
{
	Uint16 s, m = 0, nombre_valeurs_saturees;
	Sint16 max = 0;
	Sint16 max2 = 0;
	
/*	for(s=0;s<TAILLE_ACQUISITION; s++)
	{
		tmp = 0;
		for(f=0;f<TAILLE_FENETRE;f++)
		{
			if(fenetre[f])
				tmp += acquisition[s+f];
		}
		if(tmp > max)
		{
			max = tmp;
			m = s;
		}
		convolution[s] = tmp;
	}
*/
	nombre_valeurs_saturees = 0;
	for(s=0;s<TAILLE_ACQUISITION; s++)	//Recherche du nombre de valeurs en saturation
	{
		if(acquisition[s] > SEUIL_SATURATION)
			nombre_valeurs_saturees++;
	}	

	if(nombre_valeurs_saturees > NOMBRE_MAXIMAL_VALEURS_SATUREES)
		fiabilite |= ERREUR_SIGNAL_SATURE;
	else
		fiabilite &= ~ERREUR_SIGNAL_SATURE;
		
	for(s=0;s<TAILLE_CONVOLUTION; s++)	//Recherche du pic de convolution....
	{
		if(convolution[s] > max)
		{
			max = convolution[s];
			m = s;	//Ca va plus vite sans passer par la variable volatile position_pic...
		}
	}
	position_pic = m;
	max1visu = convolution[m];

	//Si le max est trop faible, on place le flag SIGNAL_INSUFFISANT
	if(max < CONVOLUTION_VALEUR_MINI)
		fiabilite |= ERREUR_SIGNAL_INSUFFISANT;
	else fiabilite &= ~ERREUR_SIGNAL_INSUFFISANT;
	
	//On a détecté un maximum. On cherche maintenant à qualifier ce maximum, en cherchant s'il s'agit d'un pic franc ou pas.
	//on reparcourt l'ensemble du produit de convolution SAUF la zone autour du PIC.
	//Le second maximum trouvé hors cette zone doit avoir un écart minimal...avec le premier
	for(s=0;s<TAILLE_CONVOLUTION; s++)
	{
		if(m - s < OUVERTURE_PIC_CONVOLUTION)
			s += 2*OUVERTURE_PIC_CONVOLUTION;	//On saute toute une fenetre autour du max de convolution
		if(convolution[s] > max2)
		{
			max2 = convolution[s];
			#warning "temporaire :"	
				indexmax2visu = s;
		}	
	}
	#warning "temporaire :" 
		max2visu = max2;
	
//	if(max - max2 < ECART_MINI_ENTRE_DEUX_PICS)
//		fiabilite |= ERREUR_CONVOLUTION_NON_CONFORME;
//	else
//		fiabilite &= ~ERREUR_CONVOLUTION_NON_CONFORME;

	#warning "if(max - max2 < ECART_MINI_ENTRE_DEUX_PICS): changement par Alexis"
	if(max - max2 < ECART_MINI_ENTRE_DEUX_PICS)             //SI LES DEUX PICS SONT PROCHES EN "puissance"
		position_pic = MIN(position_pic, indexmax2visu);   //ALORS, on choisit le premier des deux, dans le temps !

	if(position_pic < MINI_POSITION_PIC)
		fiabilite |= ERREUR_TROP_PROCHE;
	else
		fiabilite &= ~ERREUR_TROP_PROCHE;
}

volatile bool_e request_reset_step = FALSE;

void ReceptionUS_step_init(void)
{
	request_reset_step = TRUE;
	fiabilite &= ~ERREUR_PAS_DE_SYNCHRO;	//On a une synchro !
											//Attention, on considère qu'on l'a pour toujours !!! (alors qu'au bout d'une heure, c'est pas le cas)
}

void ReceptionUS_next_step(void)
{
	LED_RUN = (step==0 || step == PERIODE_FLASH)?1:0;
	
	
	if(request_reset_step == TRUE && step > 0 && step <99)
		step = 0;	
	else
		step = (step==2*PERIODE_FLASH-1)?0:step+1;		//Période de 100ms...* nb balises émettrices = 200ms = (5 mesures par seconde !) 
	request_reset_step = FALSE;
	
	if((step == 0 || step == PERIODE_FLASH)&& !measures_ready)	//Il n'y a plus de données à traiter, et on est au step 0, on lance une série d'acquisitions !
	{
		TIMER2_run_us(125); //fréquence de 8kHz	 //C'est parti mon kiki !!!	
		if(step == 0)
			current_adversary = ADVERSARY_1;
		else
			current_adversary = ADVERSARY_2;
	}
}



void _ISR _T2Interrupt()	//Acquisition à 8kHz...
{
	Sint16 acq;
	Sint16 * pconv;
	LED_USER = 1;
	if(acquisition_index < TAILLE_ACQUISITION)
	{
		acq = (ADC_getValue(0) - OFFSET_MASSE_VIRTUELLE) >>2;	//On soustrait l'offset de masse virtuelle...
		//La division par deux permet d'éviter les dépassements de Sint16 sur la convolution.
			//Explication : le max pour une case de convolution = 224 additions de (1023-OFFSET_MASSE_VIRTUELLE)/4
			//													= 224 * (1023 - environ 500) /4
													//			= 29288 < 32767 ! ok...
		pconv = &(convolution[acquisition_index]);
		pconv += TAILLE_FENETRE;
		//Calcul en "live" du produit de convolution...
		#warning "Il est impératif de compiler cette portion de code avec un niveau d'optimisation -O1 pour que le compilo remplace chaque ligne par seulement trois instrutions assembleur... Vérifiez dans le desassembly listing !"
		
		//  *( ADRESSE 		) += acq;		Avec ADRESSE = acquisition_index - 16*(numéro du bit dans le motif) + (nombre allant de -15 à 0 pour chaque bit du motif) + OFFSET de 208+7 pour par écrire hors tableau...
		*(pconv-	1	) += acq;
		*(pconv-	2	) += acq;
		*(pconv-	3	) += acq;
		*(pconv-	4	) += acq;
		*(pconv-	5	) += acq;
		*(pconv-	6	) += acq;
		*(pconv-	7	) += acq;
		*(pconv-	8	) += acq;
		*(pconv-	9	) += acq;
		*(pconv-	10	) += acq;
		*(pconv-	11	) += acq;
		*(pconv-	12	) += acq;
		*(pconv-	13	) += acq;
		*(pconv-	14	) += acq;
		*(pconv-	15	) += acq;
		*(pconv-	16	) += acq;
		*(pconv-	17	) += acq;
		*(pconv-	18	) += acq;
		*(pconv-	19	) += acq;
		*(pconv-	20	) += acq;
		*(pconv-	21	) += acq;
		*(pconv-	22	) += acq;
		*(pconv-	23	) += acq;
		*(pconv-	24	) += acq;
		*(pconv-	25	) += acq;
		*(pconv-	26	) += acq;
		*(pconv-	27	) += acq;
		*(pconv-	28	) += acq;
		*(pconv-	29	) += acq;
		*(pconv-	30	) += acq;
		*(pconv-	31	) += acq;
		*(pconv-	32	) += acq;
		*(pconv-	49	) += acq;
		*(pconv-	50	) += acq;
		*(pconv-	51	) += acq;
		*(pconv-	52	) += acq;
		*(pconv-	53	) += acq;
		*(pconv-	54	) += acq;
		*(pconv-	55	) += acq;
		*(pconv-	56	) += acq;
		*(pconv-	57	) += acq;
		*(pconv-	58	) += acq;
		*(pconv-	59	) += acq;
		*(pconv-	60	) += acq;
		*(pconv-	61	) += acq;
		*(pconv-	62	) += acq;
		*(pconv-	63	) += acq;
		*(pconv-	64	) += acq;
		*(pconv-	65	) += acq;
		*(pconv-	66	) += acq;
		*(pconv-	67	) += acq;
		*(pconv-	68	) += acq;
		*(pconv-	69	) += acq;
		*(pconv-	70	) += acq;
		*(pconv-	71	) += acq;
		*(pconv-	72	) += acq;
		*(pconv-	73	) += acq;
		*(pconv-	74	) += acq;
		*(pconv-	75	) += acq;
		*(pconv-	76	) += acq;
		*(pconv-	77	) += acq;
		*(pconv-	78	) += acq;
		*(pconv-	79	) += acq;
		*(pconv-	80	) += acq;
		*(pconv-	129	) += acq;
		*(pconv-	130	) += acq;
		*(pconv-	131	) += acq;
		*(pconv-	132	) += acq;
		*(pconv-	133	) += acq;
		*(pconv-	134	) += acq;
		*(pconv-	135	) += acq;
		*(pconv-	136	) += acq;
		*(pconv-	137	) += acq;
		*(pconv-	138	) += acq;
		*(pconv-	139	) += acq;
		*(pconv-	140	) += acq;
		*(pconv-	141	) += acq;
		*(pconv-	142	) += acq;
		*(pconv-	143	) += acq;
		*(pconv-	144	) += acq;
		*(pconv-	145	) += acq;
		*(pconv-	146	) += acq;
		*(pconv-	147	) += acq;
		*(pconv-	148	) += acq;
		*(pconv-	149	) += acq;
		*(pconv-	150	) += acq;
		*(pconv-	151	) += acq;
		*(pconv-	152	) += acq;
		*(pconv-	153	) += acq;
		*(pconv-	154	) += acq;
		*(pconv-	155	) += acq;
		*(pconv-	156	) += acq;
		*(pconv-	157	) += acq;
		*(pconv-	158	) += acq;
		*(pconv-	159	) += acq;
		*(pconv-	160	) += acq;
		*(pconv-	177	) += acq;
		*(pconv-	178	) += acq;
		*(pconv-	179	) += acq;
		*(pconv-	180	) += acq;
		*(pconv-	181	) += acq;
		*(pconv-	182	) += acq;
		*(pconv-	183	) += acq;
		*(pconv-	184	) += acq;
		*(pconv-	185	) += acq;
		*(pconv-	186	) += acq;
		*(pconv-	187	) += acq;
		*(pconv-	188	) += acq;
		*(pconv-	189	) += acq;
		*(pconv-	190	) += acq;
		*(pconv-	191	) += acq;
		*(pconv-	192	) += acq;
		*(pconv-	225	) += acq;
		*(pconv-	226	) += acq;
		*(pconv-	227	) += acq;
		*(pconv-	228	) += acq;
		*(pconv-	229	) += acq;
		*(pconv-	230	) += acq;
		*(pconv-	231	) += acq;
		*(pconv-	232	) += acq;
		*(pconv-	233	) += acq;
		*(pconv-	234	) += acq;
		*(pconv-	235	) += acq;
		*(pconv-	236	) += acq;
		*(pconv-	237	) += acq;
		*(pconv-	238	) += acq;
		*(pconv-	239	) += acq;
		*(pconv-	240	) += acq;
		*(pconv-	241	) += acq;
		*(pconv-	242	) += acq;
		*(pconv-	243	) += acq;
		*(pconv-	244	) += acq;
		*(pconv-	245	) += acq;
		*(pconv-	246	) += acq;
		*(pconv-	247	) += acq;
		*(pconv-	248	) += acq;
		*(pconv-	249	) += acq;
		*(pconv-	250	) += acq;
		*(pconv-	251	) += acq;
		*(pconv-	252	) += acq;
		*(pconv-	253	) += acq;
		*(pconv-	254	) += acq;
		*(pconv-	255	) += acq;
		*(pconv-	256	) += acq;
		*(pconv-	273	) += acq;
		*(pconv-	274	) += acq;
		*(pconv-	275	) += acq;
		*(pconv-	276	) += acq;
		*(pconv-	277	) += acq;
		*(pconv-	278	) += acq;
		*(pconv-	279	) += acq;
		*(pconv-	280	) += acq;
		*(pconv-	281	) += acq;
		*(pconv-	282	) += acq;
		*(pconv-	283	) += acq;
		*(pconv-	284	) += acq;
		*(pconv-	285	) += acq;
		*(pconv-	286	) += acq;
		*(pconv-	287	) += acq;
		*(pconv-	288	) += acq;
		*(pconv-	305	) += acq;
		*(pconv-	306	) += acq;
		*(pconv-	307	) += acq;
		*(pconv-	308	) += acq;
		*(pconv-	309	) += acq;
		*(pconv-	310	) += acq;
		*(pconv-	311	) += acq;
		*(pconv-	312	) += acq;
		*(pconv-	313	) += acq;
		*(pconv-	314	) += acq;
		*(pconv-	315	) += acq;
		*(pconv-	316	) += acq;
		*(pconv-	317	) += acq;
		*(pconv-	318	) += acq;
		*(pconv-	319	) += acq;
		*(pconv-	320	) += acq;
		*(pconv-	337	) += acq;
		*(pconv-	338	) += acq;
		*(pconv-	339	) += acq;
		*(pconv-	340	) += acq;
		*(pconv-	341	) += acq;
		*(pconv-	342	) += acq;
		*(pconv-	343	) += acq;
		*(pconv-	344	) += acq;
		*(pconv-	345	) += acq;
		*(pconv-	346	) += acq;
		*(pconv-	347	) += acq;
		*(pconv-	348	) += acq;
		*(pconv-	349	) += acq;
		*(pconv-	350	) += acq;
		*(pconv-	351	) += acq;
		*(pconv-	352	) += acq;
		*(pconv-	385	) += acq;
		*(pconv-	386	) += acq;
		*(pconv-	387	) += acq;
		*(pconv-	388	) += acq;
		*(pconv-	389	) += acq;
		*(pconv-	390	) += acq;
		*(pconv-	391	) += acq;
		*(pconv-	392	) += acq;
		*(pconv-	393	) += acq;
		*(pconv-	394	) += acq;
		*(pconv-	395	) += acq;
		*(pconv-	396	) += acq;
		*(pconv-	397	) += acq;
		*(pconv-	398	) += acq;
		*(pconv-	399	) += acq;
		*(pconv-	400	) += acq;
		*(pconv-	401	) += acq;
		*(pconv-	402	) += acq;
		*(pconv-	403	) += acq;
		*(pconv-	404	) += acq;
		*(pconv-	405	) += acq;
		*(pconv-	406	) += acq;
		*(pconv-	407	) += acq;
		*(pconv-	408	) += acq;
		*(pconv-	409	) += acq;
		*(pconv-	410	) += acq;
		*(pconv-	411	) += acq;
		*(pconv-	412	) += acq;
		*(pconv-	413	) += acq;
		*(pconv-	414	) += acq;
		*(pconv-	415	) += acq;
		*(pconv-	416	) += acq;

		/*	
			Equivalent, en plus éléguant, mais (beaucoup) moins rapide !
			
			Uint32 motif = 0b11011000110100110101010011000000;
			#define TAILLE_MOTIF (26)
			for(i=0;i<TAILLE_MOTIF;i++)
			{	
				if(motif & (0x80000000 >> i))	//Si manque de temps, remplacer par du statique... (224 additions à faire, seule varie l'acquisition index !)
				{
					for(j=-15;j<=0;j++)
					{
						convolution_index = acquisition_index - 16*i + j;
						if(convolution_index > 0)
							convolution[convolution_index] += acq;
					}	
				}
			}
		*/		
		acquisition[acquisition_index++] = acq;
	}	
	else	// (acquisition_index == (TAILLE_ACQUISITION)	//528 acquisitions...
	{	//On a terminé les acquisitions !
		TIMER2_stop();
		acquisition_index = 0;
		measures_ready = TRUE;
	}
	
	LED_USER = 0;
	IFS0bits.T2IF = 0;
}


void ReceptionUS_afficher_signal(Sint16 * signal, Uint16 taille)
{
	Uint16 i;
	for(i = 0; i < taille; i++)
	//	debug_printf("%u\t",signal[i]/64);	//Converti décimal
	//	debug_printf("%x\t",signal[i]);		//Hexa pur
		debug_printf("%d\n",signal[i]);		//décimal pur
}

bool_e Reception_US_get_dataready(void)
{
	return dataready;
}

void Reception_US_reset_dataready(void)
{
	dataready = FALSE;
}	

Uint16 ReceptionUS_get_distance_cm(void)
{
	return distance/10;
}

Uint8 ReceptionUS_get_fiabilite(void)
{
	return fiabilite;
}	

