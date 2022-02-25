#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "_ipc.h"

int main()
{
  key_t key;
  int i;
  struct msgbuf mybuf;

  if(init(&key) == -1){
    fprintf(stderr, "Error: init() error\n");
    exit(1);
  }

  printf("Key is %d\n", key);

  while(1){
    memset(&mybuf, 0, sizeof(struct msgbuf));
    mybuf.msgtype = 3;
    mybuf.seq = i;
    sprintf(mybuf.msg, "hello");

    if(send(key, &mybuf) == -1){
      fprintf(stderr,"Error: send() error\n");
      exit(1);
    }

    printf("send [%d]\n", i++);
    sleep(1);
  }
  
  return 0;
}