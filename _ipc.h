#ifndef IPC_H
#define IPC_H
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdbool.h>

// Size
#define NAME_SIZE 64
#define CAR_NUMBER_SIZE 16
#define PHONE_NUMBER_SIZE 16
#define MAP_SIZE 1024
#define RESPONSE_SIZE 2048

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

// Status code
#define REQ_SUCCESS 0
#define REQ_FAIL -1

typedef struct Register{
    long msgtype; //메세지 타입
    char name[NAME_SIZE]; //이름
    char car_number[CAR_NUMBER_SIZE]; //자동차 번호
    char phone_number[PHONE_NUMBER_SIZE]; //전화번호
    int is_residents; //입주민 여부
    int errno; //에러코드
} Register;

// client 전반
typedef struct MyState {
    long msgtype; //메세지 타입
    int user_key; //key
    char car_number[CAR_NUMBER_SIZE]; //자동차번호
    bool state; //주차여부
    char map[MAP_SIZE]; //주차위치
    time_t unixtime; //입차시간
    int errno; //에러코드
} MyState;

// manager 조회 전반
typedef struct Manage {
    long msgtype; //메세지 타입
    char response[RESPONSE_SIZE]; //요청한 값을 저장할 문자열
    int errno; //에러코드
} Manage;

#endif
