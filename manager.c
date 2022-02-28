#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "_ipc.h"

Register register_buf;
MyState state_buf;
key_t key;

void update_user(){
  register_buf.msgtype = MSG_UPDATE_REQ;
  register_buf.is_resident = 2;
  strcpy(register_buf.car_number, "경기가5288");
  if(msgsnd(key, (void *)&register_buf, sizeof(Register), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&register_buf, sizeof(Register), MSG_UPDATE_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.errno == REQ_SUCCESS)
    printf("Update Success\n");
  else
    printf("Update Fail\n");
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
    printf("1. 주차현황\n2. 이력조회\n3. 업데이트\n: ");
    scanf("%d", &button);

    system("clear");
    
    switch(button){
      case 1: break;
      case 2: break;
      case 3: update_user();break;
      default: fprintf(stderr, "Error : 잘못입력하셨습니다.\n");break;
    }
  }
  
  return 0;
}