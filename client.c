#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "_ipc.h"

Register register_buf;
MyState state_buf;
key_t key;

void register_user(){
  
  // 테스트 dummy 데이터
  register_buf.msgtype = MSG_REGISTER_REQ;
  register_buf.is_resident = 1;
  strcpy(register_buf.car_number, "경기87바5288");
  strcpy(register_buf.name, "유찬양");
  strcpy(register_buf.phone_number, "01012345678");

  if(msgsnd(key, (void *)&register_buf, sizeof(Register), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&state_buf, sizeof(MyState), MSG_REGISTER_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.errno == REQ_SUCCESS)
    printf("Register Success!\nkey : %d\n", state_buf.user_key);
  else
    printf("Register Fail!\n");
}

void in_car(){
  state_buf.msgtype = MSG_CAR_IN_REQ;
  state_buf.user_key = 1;
  if(msgsnd(key, (void *)&state_buf, sizeof(MyState), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&state_buf, sizeof(MyState), MSG_CAR_IN_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.errno == REQ_SUCCESS)
    printf("Car-in Success!\nin_time : %ld\n", state_buf.unixtime);
  else
    printf("Car-in Fail!\n");
}

void out_car(){
  state_buf.msgtype = MSG_CAR_OUT_REQ;
  state_buf.user_key = 1;
  if(msgsnd(key, (void *)&state_buf, sizeof(MyState), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&state_buf, sizeof(MyState), MSG_CAR_OUT_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.errno == REQ_SUCCESS)
    printf("Car-out Success!\nfee : %d\n", state_buf.cost);
  else
    printf("Car-out Fail!\n");
}


void find_car(){
  state_buf.msgtype = MSG_CAR_STATE_REQ;
  state_buf.user_key = 1;
  if(msgsnd(key, (void *)&state_buf, sizeof(MyState), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&state_buf, sizeof(MyState), MSG_CAR_STATE_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.state == 0)
    printf("Your Car is in the Building!\n");
  else
    printf("Your Car is not in the Building!\n");
}

int main(int argc, char **argv)
{
  if((key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | MSG_PERM)) == -1){
    fprintf(stderr,"Error: msgget() error\n");
    exit(1);
  }

  printf("Key is %d\n", key);

  while(1){
    int button;
    printf("1. 입차\n2. 출차\n3. 등록\n4. 내 차찾기\n: ");
    scanf("%d", &button);

    system("clear");
    
    switch(button){
      case 1: in_car();break;
      case 2: out_car();break;
      case 3: register_user();break;
      case 4: find_car();break;
      default: fprintf(stderr, "Error : 잘못입력하셨습니다.\n");break;
    }
  }
  
  return 0;
}