#include "userclient.h"

UserClient user_client;

int main(int argc, char *argv[])
{
    //setup
    user_client.client_process_setup();

    //loop
    while(1)
    {
        if(user_client.mystate == false)
            user_client.parkingin();
        else
            user_client.parkingout();
    }

    return 0;
}