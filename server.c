#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "_ipc.h"

int main()
{ 
  key_t key;
  int msgtype = 3;
  struct msgbuf mybuf;
  
  if(init(&key)){
    fprintf(stderr,"Error: init() error\n");
    exit(1);
  }

  printf("Key is %d\n", key);

  while(1){
    memset(&mybuf, 0, sizeof(struct msgbuf));

    if(receive_sync(key, &mybuf, msgtype) == -1){
      fprintf(stderr,"Error: receive_sync() error\n");
      exit(1);
    }

    printf("recieve msg : %s [%d]\n", mybuf.msg, mybuf.seq);
    sleep(1);
  }

  return 0;
}