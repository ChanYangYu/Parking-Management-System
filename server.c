#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "_ipc.h"

int main()
{ 
  key_t key;
  struct msgbuf mybuf;
  
  if(init(&key)){
    fprintf(stderr,"Error: init() error\n");
    exit(1);
  }

  printf("Key is %d\n", key);

  while(1){
    memset(&mybuf, 0, sizeof(struct msgbuf));
    // Lock
    if(receive_sync(key, &mybuf, 5) == -1){
      fprintf(stderr,"Error: receive_sync() : 4 error\n");
      exit(1);
    }

    // wake up Client
    mybuf.msgtype = 6;
    if(send(key, &mybuf) == -1){
        fprintf(stderr,"Error: send() error\n");
        exit(1);
      }

    // Request Listen
    if(receive_sync(key, &mybuf, MSG_TYPE) == -1){
      fprintf(stderr,"Error: receive_sync() : 3 error\n");
      exit(1);
    }

    if(mybuf.in_out == 0){
      printf("[OUT] %s\n", mybuf.number);
      
      // 주차요금 정산
      mybuf.cost = 9000;
      mybuf.msgtype = 4;
      if(send(key, &mybuf) == -1){
        fprintf(stderr,"Error: send() error\n");
        exit(1);
      }
    }
    else if(mybuf.in_out == 1){
      printf("[In] %s\n", mybuf.number);
    }

    sleep(1);
  }

  return 0;
}