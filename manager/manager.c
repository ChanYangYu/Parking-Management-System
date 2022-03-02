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
  else{
    printf("-----------------------------------------------\n");
    printf("[!] 주차현황 조회에 실패했습니다.\n");
    printf("-----------------------------------------------\n");
  }
  printf("정보를 조회하고 싶은 차량의 주차장 번호를 입력하세요: ");
  scanf("%d", &manage_buf.pos);

  manage_buf.msgtype = MSG_FIND_CARS_DETAIL_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&register_buf, sizeof(Register) - sizeof(long), MSG_FIND_CARS_DETAIL_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }
  
  system("clear");
  printf("-----------------------------------------------\n");
  if(register_buf.errno == REQ_SUCCESS)
    printf("차주 이름 : %s\n차주 연락처 : %s\n차량번호 : %s\n",register_buf.name, register_buf.phone_number, register_buf.car_number);
  else 
    printf("[!] 해당 위치에는 주차된 차량이 없습니다.\n");
  printf("-----------------------------------------------\n");
}

void parking_history(){
  int history_select_number;
  
  printf("1.통합 이력 조회\n2.개인 이력 조회\n:");
  scanf("%d", &history_select_number);

  if(history_select_number == 1){ 
    state_buf.msgtype = MSG_FIND_ALL_HISTORY_REQ;     
    if(msgsnd(key, (void *)&state_buf, sizeof(MyState) - sizeof(long), 0) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }

    if(msgrcv(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_FIND_ALL_HISTORY_RES, 0) == -1){
      fprintf(stderr,"Error: msgrcv() error\n");
      exit(1);
    }
    system("clear");
    if(state_buf.errno == REQ_SUCCESS){
      printf("-----------------------------------------------\n");
      printf("%s\n",manage_buf.response);
      printf("-----------------------------------------------\n");
    }
    else{
      printf("-----------------------------------------------\n");
      printf("[!] 통합 주차이력이 없습니다.\n");
      printf("-----------------------------------------------\n");
    }
  }
  
  else if(history_select_number == 2){
    printf("차량 번호를 입력해주세요 : ");
    scanf("%s", state_buf.car_number);

    state_buf.msgtype = MSG_FIND_USER_HISTORY_REQ;     
    if(msgsnd(key, (void *)&state_buf, sizeof(MyState) - sizeof(long), 0) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }

    if(msgrcv(key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_FIND_USER_HISTORY_RES, 0) == -1){
      fprintf(stderr,"Error: msgrcv() error\n");
      exit(1);
    } 
    
    system("clear");
    if(manage_buf.errno == REQ_SUCCESS){
      printf("------------------------------------------------\n");
      printf("%s\n",manage_buf.response);
      printf("------------------------------------------------\n");
    }
    else{
      printf("-----------------------------------------------\n");
      printf("[!] %s 차량의 주차이력이 없습니다.\n", state_buf.car_number);
      printf("-----------------------------------------------\n");
    }
  }

  else{
    system("clear");
    printf("-----------------------------------------------\n");
    printf("[!] 잘못된 번호를 누르셨습니다.\n");
    printf("-----------------------------------------------\n");
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

  system("clear");
  if(manage_buf.errno == REQ_SUCCESS){
    printf("------------------------------------------------\n");
    printf("%s\n",manage_buf.response);
    printf("------------------------------------------------\n");
  }
  else{
    printf("------------------------------------------------\n");
    printf("[!] 조회할 주차장 이용자가 없습니다.\n");
    printf("------------------------------------------------\n");
  }
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

  if(manage_buf.errno == REQ_SUCCESS){
    printf("------------------------------------------------\n");
    printf("%s\n",manage_buf.response);
    printf("------------------------------------------------\n");
    
  }
  else{
    printf("------------------------------------------------\n");
    printf("[!] 예비 입주자가 없습니다.\n");
    printf("------------------------------------------------\n");
  }
  
}

void update_user(){
  user_list_pen();
  register_buf.msgtype = MSG_UPDATE_REQ;
  
  printf("차량 번호를 입력해주세요 : ");
  scanf("%s", register_buf.car_number);
  printf("입주자 인지 확인해주세요.\n(외부인 = 0 ,입주민 = 2)\n: ");
  scanf("%d", &register_buf.is_resident);
  
  if(msgsnd(key, (void *)&register_buf, sizeof(Register) - sizeof(long), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&register_buf, sizeof(Register) - sizeof(long), MSG_UPDATE_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  system("clear");
  if(register_buf.errno == REQ_SUCCESS){
    printf("-----------------------------------------------\n");
    printf("[!] 입주민 정보를 업데이트했습니다.\n");
    printf("-----------------------------------------------\n");
  }
  else{
    printf("-----------------------------------------------\n");
    printf("[!] 업데이트에 실패했습니다.\n");
    printf("-----------------------------------------------\n");
  }
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

  if(register_buf.errno == REQ_SUCCESS){
    printf("-----------------------------------------------\n");
    printf("차량번호 : %s\n차주 : %s\n휴대폰 번호 : %s\n", register_buf.car_number, register_buf.name, register_buf.phone_number);
    printf("-----------------------------------------------\n");
  }
    
  else{
    printf("-----------------------------------------------\n");
    printf("[!] 잘못된 번호를 입력하셨거나, 없는 사용자입니다.\n");
    printf("-----------------------------------------------\n");    
  }
}


void make_password(){
  FILE *fp;
  char password[BUFFER_SIZE];

  if((fp = fopen(".passwd", "a+")) == NULL){
    fprintf(stderr, "fopen() error\n");
    exit(1);
  }
  
  printf("새로운 관리자 비밀번호를 입력해주세요 : ");
  scanf("%s", password);

  fprintf(fp,"%s", password);
  fclose(fp);
}

// 0: 신규 비밀번호, 1: 기존 비밀번호
int get_password(char* password){
  FILE *fp;

  if((fp = fopen(".passwd", "r")) == NULL){
    make_password();
    return 0;
  }
  else{
    fscanf(fp, "%s", password);
    fclose(fp);
    return 1;
  }
}

void check_password(char *password){
  char input[BUFFER_SIZE];

  if(get_password(password) == 1){
    printf("비밀번호를 입력해주세요 : ");
    scanf("%s", input);

    if(strcmp(password, input) != 0){
      fprintf(stderr, "Error : 비밀번호가 틀렸습니다.\n");
      exit(1);
    }
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
    system("cat manager_title");
    printf("1. 주차현황 조회\n2. 주차이력 조회\n3. 입주민 업데이트\n4. 이용자 전체조회\n5. 입주민 보류자 조회\n6. 차주 및 연락처 조회\n9. 종료\n: ");
    scanf("%d", &button);

    system("clear");
    
    switch(button){
      case 1: current_parking_state();break;
      case 2: parking_history();break;
      case 3: update_user();break;
      case 4: user_list_all();break;
      case 5: user_list_pen();break;
      case 6: find_name_number();break;
      case 9: exit(0);break;
      default: fprintf(stderr, "Error : 잘못입력하셨습니다.\n");break;
    }
  }
  
  return 0;
}