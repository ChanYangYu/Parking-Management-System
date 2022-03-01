#include "userclient.h"

int main()
{
    int user_key_file = open("key.txt", O_RDONLY);
    
    char keybuf[100];

    if(read(user_key_file, keybuf, sizeof(keybuf)) == -1)
    {
        fprintf(stderr,"Error read file -- checkkey.\n");
        exit(1);
    }

    int user_key = atoi(keybuf)

    printf("%d ", user_key);

    return 0;
}