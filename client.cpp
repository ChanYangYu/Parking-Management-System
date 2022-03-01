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

        if(user_client.mycarstate == 1)
            user_client.parkingin();
        else
            user_client.parkingout();
    }

    user_client.set_cron_message();
    return 0;
}