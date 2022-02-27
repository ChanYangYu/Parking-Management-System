#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "_ipc.h"

Register register_buf;
MyState state_buf;
key_t key;

void register_user(){
  
  register_buf.msgtype = MSG_REGISTER_REQ;
  if(msgsnd(key, (void *)&register_buf, sizeof(Register), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&state_buf, sizeof(MyState), MSG_REGISTER_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.errno == REQ_SUCCESS)
    printf("Register Success\n");
  else
    printf("Register Fail\n");
}

int main()
{
  if((key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | MSG_PERM)) == -1){
    fprintf(stderr,"Error: msgget() error\n");
    exit(1);
  }

  printf("Key is %d\n", key);

  while(1){
    int button;
    printf("1. 입차\n2. 출차\n3. 등록\n: ");
    scanf("%d", &button);

    system("clear");
    
    switch(button){
      case 1: break;
      case 2: break;
      case 3: register_user();break;
      default: fprintf(stderr, "Error : 잘못입력하셨습니다.\n");break;
    }
  }
  
  return 0;
}