#include "Ball_grabber.h"

void BALL_GRABBER_init (){
    AX12_init();
    //Sint8  AX12_get_speed_percentage(BALL_GRABBER_ID_SERVO);
}
void BALL_GRABBER_goPose(queue_id_t queue_id,bool_e init){
    if(init){
        if(QUEUE_get_act(queue_id) == BALL_GRABBER)
        {
            if(QUEUE_get_arg(queue_id) == BALL_GRABBER_UP)
            {
                //metre la fonction go up
                 AX12_set_position(BALL_GRABBER_ID_SERVO,BALL_GRABBER_VAL_UP);
            }else if (QUEUE_get_arg(queue_id) == BALL_GRABBER_DOWN)
            {
                //fontion down
                AX12_set_position(BALL_GRABBER_ID_SERVO,BALL_GRABBER_VAL_DOWN);
            }else if (QUEUE_get_arg(queue_id) == BALL_GRABBER_TIDY)
            {
                //fonction tidy
                AX12_set_position(BALL_GRABBER_ID_SERVO,BALL_GRABBER_VAL_TIDY);
            }
        }
    }

}

