#pragma once
#include <windows.h>
#include <cstdlib>
#include <conio.h>
#include <iostream>
#include <cstring>
#include <stdio.h>


class RS232
{
	public:
		
		RS232(void);
		~RS232(void);
		int getCOMnb();
		bool setCOMnb(int);			// renvoie si l'ouverture s'est correctement effectuée
									// avec le nouveau numero de port.
		bool isOpenable(int);
		bool openCOM();
		bool readCOM(void* buffer, int nBytesToRead);
		int getNbBytesRead();
		bool writeCOM(void* buffer, int nBytesToWrite);
		int getNbBytesWritten();
		bool closeCOM();
		
		int m_nId; //numero du port COM
	private:
		static const int RX_SIZE =			4096;				/* taille tampon d'entrée  */
		static const int TX_SIZE =			4096;				/* taille tampon de sortie */
		static const int MAX_WAIT_READ =	25;				/* temps max d'attente pour lecture (en ms) */
		int m_nBytesWritten;
		int m_nBytesRead;
		HANDLE g_hCOM;
		COMMTIMEOUTS g_cto;
		DCB g_dcb;
};
