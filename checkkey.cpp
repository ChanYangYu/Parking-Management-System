#include "userclient.h"

int main(int argc, char *argv[])
{
    char filename[100];
    memset(filename, 0, sizeof(filename));
    strcat(filename, argv[1]);
    strcat(filename, "/key.txt");

    int user_key_file = open(filename, O_RDONLY);
    
    char keybuf[100];

    memset(keybuf, 0, sizeof(keybuf));

    if(read(user_key_file, keybuf, sizeof(keybuf)) == -1)
    {
        fprintf(stderr,"Error read file -- checkkey.\n");
        exit(1);
    }

    int user_key = atoi(keybuf);

    printf("%d ", user_key);
    close(user_key_file);
    return 0;
}