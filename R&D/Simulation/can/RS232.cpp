#include "RS232.h"

using namespace std;

RS232::RS232(void)
{
	//par defaut on se sert du port COM3
	m_nId=3;
	
	m_nBytesWritten=0;
	m_nBytesRead=0;
	
	//configuration du temps d'attente
	g_cto.ReadIntervalTimeout = MAX_WAIT_READ;
	g_cto.ReadTotalTimeoutMultiplier = 0;
	g_cto.ReadTotalTimeoutConstant = MAX_WAIT_READ;
	g_cto.WriteTotalTimeoutMultiplier = 0;
	g_cto.WriteTotalTimeoutConstant = 0;

	//Configuration du Port
	g_dcb.DCBlength = sizeof(DCB);
	g_dcb.BaudRate = 9600;
	g_dcb.fBinary = true;
	g_dcb.fParity = false;
	g_dcb.fOutxCtsFlow = false;
	g_dcb.fOutxDsrFlow = false;
	g_dcb.fDtrControl = DTR_CONTROL_ENABLE;
	g_dcb.fDsrSensitivity = false;
	g_dcb.fTXContinueOnXoff = false;
	g_dcb.fOutX = false;
	g_dcb.fInX = false;
	g_dcb.fErrorChar = false;
	g_dcb.fNull = false;
	g_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	g_dcb.fAbortOnError = false;
	g_dcb.fDummy2 = 0;
	g_dcb.wReserved = 0;
	g_dcb.XonLim = 0x100;
	g_dcb.XoffLim = 0x100;
	g_dcb.ByteSize = 8;
	g_dcb.Parity = NOPARITY;
	g_dcb.StopBits = ONESTOPBIT;
	g_dcb.XonChar = 0x11;
	g_dcb.XoffChar = 0x13;
	g_dcb.ErrorChar = '?';
	g_dcb.EofChar = 0x1A;
	g_dcb.EvtChar = 0x10;
}

RS232::~RS232(void)
{
}

int RS232::getCOMnb()
{
	return m_nId;
}

bool RS232::isOpenable(int nId)
{
	char szCOM[20];
	closeCOM();	
	m_nId = nId;
	sprintf(szCOM, "\\\\.\\COM%d", m_nId);
	g_hCOM = CreateFileA(szCOM, GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	return (g_hCOM != INVALID_HANDLE_VALUE);
}

bool RS232::setCOMnb(int nId)
{
	closeCOM();
	m_nId = nId;
	return openCOM();
}


bool RS232::openCOM()
{
	char szCOM[20];

    /* construction du nom du port, tentative d'ouverture */
	
	//if(m_nId<10)
    //sprintf (szCOM, "COM%d",m_nId);
	sprintf(szCOM, "\\\\.\\COM%d", m_nId);
	
    g_hCOM = CreateFileA(szCOM, GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
    if(g_hCOM == INVALID_HANDLE_VALUE)

    {
        cout << "Erreur lors de l'ouverture du port COM" << m_nId;
        return FALSE;
    }

    /* affectation taille des tampons d'émission et de réception */
    SetupComm(g_hCOM, RX_SIZE, TX_SIZE);

    /* configuration du port COM */
    if(!SetCommTimeouts(g_hCOM, &g_cto) || !SetCommState(g_hCOM, &g_dcb))
    {
        //cout << "Erreur lors de la configuration du port COM" << nId;
        CloseHandle(g_hCOM);
        return FALSE;
    }

    /* on vide les tampons d'émission et de réception, mise à 1 DTR */
    PurgeComm(g_hCOM, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);
    EscapeCommFunction(g_hCOM, SETDTR);
    return TRUE;
}

bool RS232::readCOM(void* buffer, int nBytesToRead)
{
	return static_cast<bool>(ReadFile(g_hCOM, buffer, nBytesToRead, (LPDWORD)&m_nBytesRead, NULL));
}

int RS232::getNbBytesRead()
{
	return m_nBytesRead;
}
bool RS232::writeCOM(void* buffer, int nBytesToWrite)
{
    return static_cast<bool>(WriteFile(g_hCOM, buffer, nBytesToWrite, (LPDWORD)&m_nBytesWritten, NULL));
}

int RS232::getNbBytesWritten()
{
	return m_nBytesWritten;
}

bool RS232::closeCOM()
{
    CloseHandle(g_hCOM);
    return true;
}