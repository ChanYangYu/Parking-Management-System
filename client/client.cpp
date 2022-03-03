#include "userclient.h"

UserClient user_client;

int main(int argc, char *argv[])
{
    //setup
    user_client.get_tty();
    user_client.delete_cron_message();
    user_client.client_process_setup();

    //loop
    while(1)
    {
        if(user_client.want_quit == true)
            break;

        if(user_client.mycarstate == true)
            user_client.parkingin();
        else
            user_client.parkingout();
    }

    user_client.finish_sound_src();

    if(user_client.mycarstate == false)
	    user_client.set_cron_message();
    
    system("clear");
    return 0;
}
