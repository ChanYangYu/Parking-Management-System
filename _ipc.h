#ifndef IPC_H
#define IPC_H
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

// Size
#define NAME_SIZE 64
#define CAR_NUMBER_SIZE 16
#define PHONE_NUMBER_SIZE 16
#define RESPONSE_SIZE 4096
#define BUFFER_SIZE 256

// MessageQueue Constants
#define MSG_NUMBER_LEN 256
#define MSG_QUEUE_KEY 0304
#define MSG_PERM 0666
#define MSG_TYPE_ALL 0
#define MSG_TYPE 3

// Message Type
#define MSG_REGISTER_REQ 1
#define MSG_REGISTER_RES 2
#define MSG_UPDATE_REQ 3
#define MSG_UPDATE_RES 4
#define MSG_LISTUP_ALL_REQ 5
#define MSG_LISTUP_ALL_RES 6 
#define MSG_LISTUP_PEN_REQ 7
#define MSG_LISTUP_PEN_RES 8
#define MSG_CAR_IN_REQ 9
#define MSG_CAR_IN_RES 10
#define MSG_CAR_OUT_REQ 11
#define MSG_CAR_OUT_RES 12
#define MSG_CAR_STATE_REQ 13
#define MSG_CAR_STATE_RES 14
#define MSG_FIND_CARS_REQ 15
#define MSG_FIND_CARS_RES 16
#define MSG_FIND_CARS_DETAIL_REQ 17
#define MSG_FIND_CARS_DETAIL_RES 18
#define MSG_FIND_CAR_NUMBER_REQ 19
#define MSG_FIND_CAR_NUMBER_RES 20
#define MSG_FIND_ALL_HISTORY_REQ 21
#define MSG_FIND_ALL_HISTORY_RES 22
#define MSG_FIND_USER_HISTORY_REQ 23
#define MSG_FIND_USER_HISTORY_RES 24

// Status code
#define REQ_SUCCESS 0
#define REQ_FAIL -1

typedef struct Register{
    long msgtype;                         //메세지 타입
    char name[NAME_SIZE];                 //이름
    char car_number[CAR_NUMBER_SIZE];     //자동차 번호
    char phone_number[PHONE_NUMBER_SIZE]; //전화번호
    int is_resident;                      //입주민 여부
    int errno;                            //에러코드
} Register;

// client 전반
typedef struct MyState {
    long msgtype;                     //메세지 타입
    int user_key;                     //key
    char car_number[CAR_NUMBER_SIZE]; //자동차번호
    int state;                        //주차여부
    char map[RESPONSE_SIZE];               //주차위치
    time_t unixtime;                  //입차시간
    int cost;                         //주차요금
    int errno;                        //에러코드
} MyState;

// manager 조회 전반
typedef struct Manage {
    long msgtype;                 //메세지 타입
    int pos;                      //주차장 위치번호
    char response[RESPONSE_SIZE]; //요청한 값을 저장할 문자열
    int errno;                    //에러코드
} Manage;

#endif
