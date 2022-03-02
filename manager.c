#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "_ipc.h"

Register register_buf;
Manage manage_buf;
MyState state_buf;
key_t key;

void current_parking_state(){
  
  manage_buf.msgtype = MSG_FIND_CARS_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_FIND_CARS_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(manage_buf.errno == REQ_SUCCESS)
    printf("%s\n",manage_buf.response);
  else
    printf("Parking State Error!\n");
  
  printf("정보를 조회하고 싶은 차량의 주차장 번호를 입력하세요: ");
  scanf("%d", &manage_buf.pos);      // scanf로 이렇게 받아도 되는건지 질문

  manage_buf.msgtype = MSG_FIND_CARS_DETAIL_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&register_buf, sizeof(Register) - sizeof(long), MSG_FIND_CARS_DETAIL_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }
  
  if(register_buf.errno == REQ_SUCCESS)      // 정보 조회 성공
    printf("%s\n",register_buf.name);      // %s %s 로 쪼개서 표현 가능한지 질문
  else                  // 정보 조회 실패
    printf("Detail Info Error!\n");
}

void parking_history(){
  int history_select_number;
  
  printf("통합 이력 조회를 하려면 1번, 개인 이력 조회를 하려면 2번을 눌러주세요 : ");
  scanf("%d", &history_select_number);

  if(history_select_number == 1){      // 통합 이력 조회
    state_buf.msgtype = MSG_FIND_ALL_HISTORY_REQ;      // 메시지 큐 변수 확인하고 변경해야함
    if(msgsnd(key, (void *)&state_buf, sizeof(MyState) - sizeof(long), 0) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }

    if(msgrcv(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_FIND_ALL_HISTORY_RES, 0) == -1){
      fprintf(stderr,"Error: msgrcv() error\n");
      exit(1);
    }
    
    if(state_buf.errno == REQ_SUCCESS)
      printf("%s\n",manage_buf.response);
    else{
      printf("Parking History Error!\n");
    }
  }
  
  else if(history_select_number == 2){
    
    state_buf.msgtype = MSG_FIND_USER_HISTORY_REQ;      // 메시지 큐 변수 확인하고 변경해야함
    if(msgsnd(key, (void *)&state_buf, sizeof(MyState) - sizeof(long), 0) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }

    if(msgrcv(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_FIND_USER_HISTORY_RES, 0) == -1){
      fprintf(stderr,"Error: msgrcv() error\n");
      exit(1);
    } 
    
    if(manage_buf.errno == REQ_SUCCESS)
      printf("%s\n",manage_buf.response);
    else{
      printf("User History Error!\n");
      exit(1);
    }
  }

  else{
    printf("잘못된 번호를 누르셨습니다.\n");
  }
}

void user_list_all(){
  manage_buf.msgtype = MSG_LISTUP_ALL_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_LISTUP_ALL_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(manage_buf.errno == REQ_SUCCESS)
    printf("%s\n",manage_buf.response);
  else
    printf("[!] 조회할 유저가 없습니다.\n");
}

void user_list_pen(){
  manage_buf.msgtype = MSG_LISTUP_PEN_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_LISTUP_PEN_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(manage_buf.errno == REQ_SUCCESS)
    printf("%s\n",manage_buf.response);
  else
    printf("[!] 조회할 유저가 없습니다.\n");
}

void update_user(){
  
  user_list_pen();
  register_buf.msgtype = MSG_UPDATE_REQ;
  
  printf("차량 번호와 입주자 여부(외부인 : 0 , 입주자 : 2)를 입력해주세요 :\n");
  scanf("%s %d", register_buf.car_number, &register_buf.is_resident);
  
  if(msgsnd(key, (void *)&register_buf, sizeof(Register) - sizeof(long), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&register_buf, sizeof(Register) - sizeof(long), MSG_UPDATE_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(register_buf.errno == REQ_SUCCESS)
    printf("Update Success\n");
  else
    printf("Update Fail\n");
}

void find_name_number(){
  
  printf("검색하고 싶은 차량 번호를 입력하세요 : ");
  scanf("%s", register_buf.car_number);

  register_buf.msgtype = MSG_FIND_CAR_NUMBER_REQ;
  if(msgsnd(key, (void *)&register_buf, sizeof(Register) - sizeof(long), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&register_buf, sizeof(Register) - sizeof(long), MSG_FIND_CAR_NUMBER_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(register_buf.errno == REQ_SUCCESS)
    printf("해당 차량번호 %s의 차주는 %s 님 이고, 휴대폰 번호는 %s 입니다.\n", register_buf.car_number, register_buf.name, register_buf.phone_number);
  else
    printf("잘못된 번호를 입력하셨거나, 없는 사용자입니다.\n");
}


FILE* make_password(){
  FILE *fp;
  char password[BUFFER_SIZE];

  if((fp = fopen(".passwd", "a+")) == NULL){
    fprintf(stderr, "fopen() error\n");
    exit(1);
  }
  
  printf("새로운 관리자 비밀번호를 입력해주세요 : ");
  scanf("%s", password);

  fprintf(fp,"%s", password);
  return fp;
}

void get_password(char* password){
  FILE *fp;

  if((fp = fopen(".passwd", "r")) == NULL)
    fp = make_password();
  
  fscanf(fp, "%s", password);
}

void check_password(char *password){
  char input[BUFFER_SIZE];

  get_password(password);
  printf("비밀번호를 입력해주세요 : ");
  scanf("%s", input);

  if(strcmp(password, input) != 0){
    fprintf(stderr, "Error : 비밀번호가 틀렸습니다.\n");
    exit(1);
  }
  system("clear");
}

int main()
{
  char password[BUFFER_SIZE];

  check_password(password);
  if((key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | MSG_PERM)) == -1){
    fprintf(stderr,"Error: msgget() error\n");
    exit(1);
  }

  while(1){
    int button;
    printf("1. 주차현황\n2. 이력조회\n3. 업데이트\n4. 유저 전체조회\n5. 입주민 보류자 조회\n6. 차주 및 연락처 조회\n: ");
    scanf("%d", &button);

    system("clear");
    
    switch(button){
      case 1: current_parking_state();break;
      case 2: parking_history();break;
      case 3: update_user();break;
      case 4: user_list_all();break;
      case 5: user_list_pen();break;
      case 6: find_name_number();break;
      default: fprintf(stderr, "Error : 잘못입력하셨습니다.\n");break;
    }
  }
  
  return 0;
}