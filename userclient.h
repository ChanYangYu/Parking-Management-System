#ifndef USER_CLINET_H
#define USER_CLINET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "_ipc.h"
#include <termio.h>

class UserClient
{
private:
    Register basic_info;
    MyState my_info;
    char ttybuf[100];

    int getkey(void);
    void new_user_init(void);
    void print_parking_map(void);
    void save_time_file(void);

public:
    UserClient();

    bool mycarstate = false;
    bool want_quit = false;

    void get_tty(void);
    void set_cron_message(void);
    void delete_cron_message(void);
    void client_process_setup(void);
    void parkingin(void);
    void parkingout(void);
};

#endif