#include "receiveCANMsg.h"

#ifdef USE_XBEE

	#include "comSessionAnswer.h"

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../../../../QS_outputlog.h"

	static const char sessionName[] = "RECEIVE_CAN_MSG";

	void receiveCANMsg_create(comSession_s * session, time32_t timeout, Sint8 availableSend, comMsg_t * msg){

		comMsgId_t idFrame = msg->header.id;

		comSessionAnswerCreate(session, sessionName, timeout, availableSend, idFrame);

		session->msg.header.id = idFrame;
		session->msg.header.type = COM_MSG_TYPE_CAN_MSG;
		session->msg.header.way = COM_MSG_WAY_ACK;
		session->msg.header.size = COM_MSG_SIZE_CAN_ACK;

		/*bool_e error = oseSessionAnswerMsgChecker(session, msg, COM_MSG_SAVE_TRAJECTORY, SIZE_SAVE_TRAJECTORY_RECEIVE);

		if(error){

		}else if(IS_TRAJECTORY_ELEMENTARY(Msg->body.formated.saveTrajectory.trajectoryType) == FALSE){
			error_printf("create : %s : Le type de trajectoire est inexistant.\n", sessionName);
			ADD_FOCUS(Session->msgTransmit.header.formated.statusCode, COM_FOCUS_TRAJECTORY);
			ADD_PARAM(Session->msgTransmit.header.formated.statusCode, COM_PARAM_TYPE);
			ADD_ERROR(Session->msgTransmit.header.formated.statusCode, COM_ERROR_NOT_FOUND);

		}else if(speedIsCorrect(Msg->body.formated.saveTrajectory.trajectoryType, Msg->body.formated.saveTrajectory.speed) == FALSE){
			error_printf("create : %s : La valeur de la vitesse est en dehors des spécifications du système.\n", sessionName);
			ADD_PARAM(Session->msgTransmit.header.formated.statusCode, COM_PARAM_SPEED);
			ADD_ERROR(Session->msgTransmit.header.formated.statusCode, COM_ERROR_OVERFLOW);

		}else{

			if(Msg->body.formated.saveTrajectory.sequenceId == 4){
				display(Msg);
			}

			trajectoryPack_t pack;
			pack.finalTraj = Msg->body.formated.saveTrajectory.finalTraj;
			pack.trajectory.x = Msg->body.formated.saveTrajectory.x;
			pack.trajectory.y= Msg->body.formated.saveTrajectory.y;
			pack.trajectory.r = Msg->body.formated.saveTrajectory.r;
			pack.trajectory.smooth = Msg->body.formated.saveTrajectory.smooth;
			pack.trajectory.speed = Msg->body.formated.saveTrajectory.speed;
			pack.trajectory.teta = Msg->body.formated.saveTrajectory.teta;
			pack.trajectory.trajectoryType = Msg->body.formated.saveTrajectory.trajectoryType;
			pack.trajectory.traj_way = Msg->body.formated.saveTrajectory.way;

			bool_e ret = RESSOURCE_setTrajectory(Msg->body.formated.saveTrajectory.sequenceId, Msg->body.formated.saveTrajectory.trajectoryId, pack);

			if(!ret){
				error_printf("create : %s : Impossible d'enregistrer la trajectoire dans la mémoire.\n", sessionName);
				ADD_FOCUS(Session->msgTransmit.header.formated.statusCode, COM_FOCUS_MEMORY);
				ADD_ERROR(Session->msgTransmit.header.formated.statusCode, COM_ERROR_NO_ANSWER);
			}
		}*/

		//info_printf("OSE : %d OPEN\n", Session->idFrameOse);
	}

#endif
