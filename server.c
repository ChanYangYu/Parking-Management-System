#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define MSG_MAX_SIZE 256

int key;

// SIGINT 발생(종료)시 메세지큐 제거
void handler(int signo){
  msgctl(key, IPC_RMID, NULL);
  exit(0);
}

struct msgbuf
{
  long msgtype;
  int seq;
  char msg[MSG_MAX_SIZE];
};

int main()
{ 
  int msgtype = 3;
  key_t key_id;
  struct msgbuf mybuf;
  
  signal(SIGINT, handler);
  
  key_id = msgget((key_t)1234, IPC_CREAT | 0666);
  if(key_id == -1){
    fprintf(stderr,"Error: msgget error\n");
    exit(1);
  }

  key = key_id;
  printf("Key is %d\n", key_id);

  while(1){
    memset(&mybuf, 0, sizeof(struct msgbuf));

    if(msgrcv(key_id, (void *)&mybuf, sizeof(struct msgbuf), msgtype, 0) == -1){
      fprintf(stderr,"Error: msgrcv error\n");
      exit(1);
    }

    printf("recieve msg : %s [%d]\n", mybuf.msg, mybuf.seq);
    sleep(1);
  }

  return 0;
}