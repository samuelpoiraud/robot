#include "Hammer.h"



void HAMMER_init (){
    AX12_init();
    //Sint8  AX12_get_speed_percentage(BALL_GRABBER_ID_SERVO);
}
void HAMMER_goPose(queue_id_t queue_id,bool_e init){
    if(init){
        if(QUEUE_get_act(queue_id) == HAMMER)
        {
            if(QUEUE_get_arg(queue_id) == HAMMER_UP)
            {
                //metre la fonction go up
                 AX12_set_position(HAMMER_ID_SERVO,HAMMER_VAL_UP);
            }else if (QUEUE_get_arg(queue_id) == HAMMER_DOWN)
            {
                //fontion down
                AX12_set_position(HAMMER_ID_SERVO,HAMMER_VAL_DOWN);
            }else if (QUEUE_get_arg(queue_id) == HAMMER_TIDY)
            {
                //fonction tidy
                AX12_set_position(HAMMER_ID_SERVO,HAMMER_VAL_TIDY);
            }
        }
    }

}


