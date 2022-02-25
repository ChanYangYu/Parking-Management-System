#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define MSG_MAX_SIZE 256

struct msgbuf
{
  long msgtype;
  int seq;
  char msg[MSG_MAX_SIZE];
};

int main()
{
  key_t key_id;
  int i;
  struct msgbuf mybuf;

  // server가 가동중이지 않은 경우
  if((key_id = msgget((key_t)1234, IPC_CREAT | IPC_EXCL | 0666)) != -1){
    fprintf(stderr, "Error: message queue가 없습니다.\n");
    msgctl(key_id, IPC_RMID, NULL);
    exit(0);
  }

  key_id = msgget((key_t)1234, IPC_CREAT | 0666);

  if(key_id == -1){
    fprintf(stderr,"Error: msgget error\n");
    exit(1);
  }

  printf("Key is %d\n", key_id);

  while(1){
    memset(&mybuf, 0, sizeof(struct msgbuf));
    mybuf.msgtype = 3;
    mybuf.seq = i;
    sprintf(mybuf.msg, "hello");
    if(msgsnd(key_id, (void*)&mybuf, sizeof(struct msgbuf), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd error\n");
      exit(1);
    }

    printf("send [%d]\n", i++);
    sleep(1);
  }
  
  return 0;
}