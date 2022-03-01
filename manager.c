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
  
  ////////////////////////////////////////////////////////////////////
  // 차량 현황을 조회하기 위한 리퀘스트가 MSG_FIND_CAR_REQ가 맞나요?//
  ////////////////////////////////////////////////////////////////////
  
  manage_buf.msgtype = MSG_FIND_CARS_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&manage_buf, sizeof(Manage), MSG_FIND_CARS_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.errno == REQ_SUCCESS)
    printf("%s\n",manage_buf.response);
  else
    printf("Parking State Error!\n");
  
  // 세부 정보 조회 코드 추가 2022-03-01 17:58
  // 같은 함수 내에서 msgtype을 변경해도 되는건지 질문해야함
  // 안 된다면 쪼개기
  printf("정보를 조회하고 싶은 차량의 주차장 번호를 입력하세요: ");
  scanf("%d", &manage_buf.pos);		// scanf로 이렇게 받아도 되는건지 질문

  manage_buf.msgtype = MSG_FIND_CARS_DETAIL_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&manage_buf, sizeof(Manage), MSG_FIND_CARS_DETAIL_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }
  
  if(state_buf.errno == REQ_SUCCESS)		// 정보 조회 성공
    printf("%s\n",manage_buf.response);		// %s %s 로 쪼개서 표현 가능한지 질문
  else						// 정보 조회 실패
    printf("Detail Info Error!\n");
}

void parking_history(){
  int history_select_number;			// 통합 이력 조횐지, 개인 이력 조횐지 알기 위한 변수
  
  // 통합 이력을 조회할건지, 개인 이력을 조회할건지 선택해야함
  printf("통합 이력 조회를 하려면 1번, 개인 이력 조회를 하려면 2번을 눌러주세요 : ");
  scanf("%d", &history_select_number);

  if(history_select_number == 1){		// 통합 이력 조회
    
    manage_buf.msgtype = MSG_FIND_ALL_HISTORY_REQ;      // 메시지 큐 변수 확인하고 변경해야함
    if(msgsnd(key, (void *)&manage_buf, sizeof(Manage), 0) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }

    if(msgrcv(key, (void *)&manage_buf, sizeof(Manage), MSG_FIND_ALL_HISTORY_RES, 0) == -1){
      fprintf(stderr,"Error: msgrcv() error\n");
      exit(1);
    }
    
    if(state_buf.errno == REQ_SUCCESS)
      printf("%s\n",manage_buf.response);
    else{
      printf("Parking History Error!\n");
      exit(1);
    }
  }
  else if(history_select_number == 2){		// 개인 이력 조회
    
    manage_buf.msgtype = MSG_FIND_USER_HISTORY_REQ;      // 메시지 큐 변수 확인하고 변경해야함
    if(msgsnd(key, (void *)&manage_buf, sizeof(Manage), 0) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }

    if(msgrcv(key, (void *)&manage_buf, sizeof(Manage), MSG_FIND_USER_HISTORY_RES, 0) == -1){
      fprintf(stderr,"Error: msgrcv() error\n");
      exit(1);
    } 
    
    if(state_buf.errno == REQ_SUCCESS)
      printf("%s\n",manage_buf.response);
    else{
      printf("User History Error!\n");
      exit(1);
    }
  }

  else{
    printf("잘못된 번호를 누르셨습니다.\n");	// 이외의 번호를 눌렸을 때
  }
}

void update_user(){
  //
  //
  // 입주자 업데이트란
  // client에서 처음에 등록할 때 전달했던 예비입주자(1) 번호를 매니저가 직접 보고 입주자(2)로 바꿔주는것 아닌가요?
  // 
  // 맞다면 설계서 Manager 항목 1. 입주자 업데이트 에서
  // (자동차 번호 + 입주자 여부)말고 (자동차 번호)만 입력 받아 전달하면 되지 않을까요?
  //
  // 그리고 현재 설계서에 따르면
  //
  // 매니저가 차량 번호만 전달해주면 server 내에서 register_buf.is_resident 가 예비입주자(1) 인지 입주자(2) 인지에 따라
  // a) 차량 번호가 있는데 예비 입주자(1)인 경우 --> update 완료
  // b) 차량 번호가 있는데 입주자(2)인 경우 --> update 실패 (이미 존재)
  // c) 차량 번호가 없는 경우 --> Error
  //
  // 와 같이 3가지 경우로 나눠야 하지 않은가 생각해서 찬양님의 생각을 여쭤보고 싶습니다.
  // 3가지 경우로 나눠지는게 맞다면 register_buf.errno 을 REQ_SUCCESS와 REQ_FAIL 말고도 하나 더 추가 해야하는게 아닌가
  // 그리고 추가해야하는게 맞는데 상황이 여의치 않으면 명세에서 위의 보기 중 c 를 빼는게 낫지 않을까 싶어 질문 드립니다.
  //
  //
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

  if(register_buf.errno == REQ_SUCCESS)
    printf("Update Success\n");
  else
    printf("Update Fail\n");
}

void user_list_all(){
  manage_buf.msgtype = MSG_LISTUP_ALL_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&manage_buf, sizeof(Manage), MSG_LISTUP_ALL_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.errno == REQ_SUCCESS)
    printf("%s\n",manage_buf.response);
  else
    printf("[!] 조회할 유저가 없습니다.\n");
}

void user_list_pen(){
  manage_buf.msgtype = MSG_LISTUP_PEN_REQ;
  if(msgsnd(key, (void *)&manage_buf, sizeof(Manage), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&manage_buf, sizeof(Manage), MSG_LISTUP_PEN_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  if(state_buf.errno == REQ_SUCCESS)
    printf("%s\n",manage_buf.response);
  else
    printf("[!] 조회할 유저가 없습니다.\n");
}

void find_name_number(){
  
  register_buf.msgtype = MSG_FIND_CAR_NUMBER_REQ;
  if(msgsnd(key, (void *)&register_buf, sizeof(Register), 0) == -1){
    fprintf(stderr,"Error: msgsnd() error\n");
    exit(1);
  }

  if(msgrcv(key, (void *)&register_buf, sizeof(Register), MSG_FIND_CAR_NUMBER_RES, 0) == -1){
    fprintf(stderr,"Error: msgrcv() error\n");
    exit(1);
  }

  // 정보를 받아오는 코드
  // 받아온 정보를 printf 해주는 코드
  if(state_buf.errno == REQ_SUCCESS)
    printf("해당 차량번호 %s의 차주는 %s 님 이고, 휴대폰 번호는 %s 입니다.\n", register_buf.car_number, register_buf.name, register_buf.phone_number);		// 이렇게 쪼개서 표현 가능?
  else
    printf("잘못된 번호를 입력하셨거나, 없는 사용자입니다.\n");
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
    printf("1. 주차현황\n2. 이력조회\n3. 업데이트\n4. 유저 전체조회\n5. 입주민 보류자 조회\n6. 차주 및 연락처 조회\n: ");
    scanf("%d", &button);

    system("clear");
    
    switch(button){
      case 1: current_parking_state();break;
      case 2: parking_history();break;
      case 3: update_user();break;
      case 4: user_list_all();break;
      case 5: user_list_pen();break;
      
      // 차량 번호로 차주 및 연락처 조회 기능 추가
      // 2022-03-01 01:15
      case 6: find_name_number();break;
      default: fprintf(stderr, "Error : 잘못입력하셨습니다.\n");break;
    }
  }
  
  return 0;
}
