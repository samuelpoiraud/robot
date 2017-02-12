#include "comSessionManager.h"

#ifdef USE_XBEE

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../QS/QS_outputlog.h"

	#define MAX_NB_SESSION	10

	typedef struct{
		bool_e used;
		comSession_s session;
	}comSessionMng_s;

	static comSessionMng_s comSessionMng[MAX_NB_SESSION];

	static volatile comMsgId_t idFrame = 0;

	comMsgId_t COM_SESSION_MNG_getNextId(){

		comMsgId_t ret = idFrame;

		if(idFrame >= 32767)
			idFrame = 0;
		else
			idFrame++;

		return ret;
	}


	void COM_SESSION_MNG_init(){
		Uint8 i;
		for(i=0; i<MAX_NB_SESSION; i++){
			comSessionMng[i].used = FALSE;
		}
	}

	void COM_SESSION_MNG_processMain(){
		Uint8 i;
		for(i=0; i<MAX_NB_SESSION; i++){
			if(comSessionMng[i].used && comSessionMng[i].session.run != NULL)
				comSessionMng[i].session.run(&(comSessionMng[i].session));
		}
	}

	bool_e COM_SESSION_MNG_openSession(comSession_s * session){
		assert(session->run != NULL);

		Uint8 i;
		for(i=0; i < MAX_NB_SESSION; i++){
			if(comSessionMng[i].used == FALSE){
				comSessionMng[i].used = TRUE;
				comSessionMng[i].session = (*session);
				comSessionMng[i].session.sessionID = i;
				return TRUE;
			}
		}

		return FALSE;
	}

	void OSE_SESSION_MNG_closeSession(comSession_s * session){
		comSessionMng[session->sessionID].used = FALSE;
	}

	void OSE_SESSION_MNG_connectSession(comMsg_t *Msg){
		Uint8 i;
		for(i=0; i < MAX_NB_SESSION; i++){
			if(comSessionMng[i].used == TRUE
					&& comSessionMng[i].session.idFrame == Msg->header.id
					&& comSessionMng[i].session.connect != NULL){
				comSessionMng[i].session.connect(&(comSessionMng[i].session), Msg);
			}
		}
	}

#endif
