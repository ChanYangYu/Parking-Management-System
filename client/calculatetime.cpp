#include "userclient.h"

int main(int argc, char *argv[])
{
    char filename[100];
    memset(filename, 0, sizeof(filename));
    strcat(filename, argv[1]);
    strcat(filename, "/utime.txt");

    int user_time_file = open(filename, O_RDONLY);
    
    char timebuf[100];

    memset(timebuf, 0, sizeof(timebuf));

    if(read(user_time_file, timebuf, sizeof(timebuf)) == -1)
    {
        fprintf(stderr,"Error read file -- checkkey.\n");
        exit(1);
    }

    int user_time = atoi(timebuf);

    user_time = time(NULL) - user_time;
    
    int min_user_time = user_time / 60;
    user_time = user_time % 60; 

    printf(" %d분 %d초", min_user_time, user_time);

    close(user_time_file);
    return 0;
}