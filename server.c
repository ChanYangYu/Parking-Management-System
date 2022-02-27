#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "_ipc.h"

Register register_buf;
MyState state_buf;
Manage manage_buf;
key_t key;

void process_register(){
  if(msgrcv(key, (void *)&register_buf, sizeof(Register), MSG_REGISTER_REQ, IPC_NOWAIT) != -1){
    printf("[Register]\n");
    
    // 임시값 적용
    state_buf.user_key = 5;
    state_buf.msgtype = MSG_REGISTER_RES;
    state_buf.errno = REQ_SUCCESS;
    
    if(msgsnd(key, (void*)&state_buf, sizeof(MyState), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_update(){
  if(msgrcv(key, (void *)&register_buf, sizeof(Register), MSG_UPDATE_REQ, IPC_NOWAIT) != -1){
    printf("[Update]\n");
    
    register_buf.errno = REQ_SUCCESS;
    register_buf.msgtype = MSG_UPDATE_RES;
    
    if(msgsnd(key, (void*)&register_buf, sizeof(Register), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

int main()
{ 
  if((key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | MSG_PERM)) == -1){
    fprintf(stderr,"Error: msgget() error\n");
    exit(1);
  }

  printf("Key is %d\n", key);

  while(1){
    memset(&register_buf, 0, sizeof(Register));
    memset(&state_buf, 0, sizeof(MyState));
    memset(&manage_buf, 0, sizeof(Manage));

    process_register();
    process_update();

    usleep(200000);
  }

  return 0;
}