#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "_ipc.h"

void get_number(struct msgbuf *mybuf);
void in_car(key_t k, struct msgbuf *mybuf);
void out_car(key_t k, struct msgbuf *mybuf);

void get_number(struct msgbuf *mybuf){
  printf("차량번호 : ");
  scanf("%s", mybuf->number); 
}

void in_car(key_t k, struct msgbuf *mybuf){
  get_number(mybuf);
  mybuf->msgtype = 3;
  mybuf->in_out = 1;
  
  if(send(k, mybuf) == -1){
    fprintf(stderr,"Error: send() error\n");
    exit(1);
  }
}

void out_car(key_t k, struct msgbuf *mybuf){
  get_number(mybuf);
  mybuf->msgtype = 3;
  mybuf->in_out = 0;

  if(send(k, mybuf) == -1){
    fprintf(stderr,"Error: send() error\n");
    exit(1);
  }

  if(receive_sync(k, mybuf, 4) == -1){
    fprintf(stderr,"Error: receive_sync() error\n");
    exit(1);
  }

  printf("정산요금 : %d원\n", mybuf->cost);
}

int main()
{
  key_t key;
  struct msgbuf mybuf;

  if(init(&key) == -1){
    fprintf(stderr, "Error: init() error\n");
    exit(1);
  }

  printf("Key is %d\n", key);

  while(1){
    int button;
    printf("1. 입차\n2. 출차\n3. 종료\n: ");
    scanf("%d", &button);

    system("clear");
    switch(button){
      case 1: in_car(key, &mybuf);break;
      case 2: out_car(key, &mybuf);break;
      case 3: exit(0);break;
      default: fprintf(stderr, "Error : 잘못입력하셨습니다.\n");break;
    }
  }
  
  return 0;
}