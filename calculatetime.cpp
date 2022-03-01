#include "userclient.h"
#include <time.h>

int main()
{
    int user_time_file = open("utime.txt", O_RDONLY);
    
    char timebuf[100];

    memset(timebuf, 0, sizeof(timebuf));

    if(read(user_time_file, timebuf, sizeof(timebuf)) == -1)
    {
        fprintf(stderr,"Error read file -- checkkey.\n");
        exit(1);
    }

    int user_time = atoi(timebuf)

    user_time = time(NULL) - user_time;
    user_time = user_time / 60; 

    printf("%d ", user_time);

    return 0;
}