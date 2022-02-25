#include "_ipc.h"

int init(key_t *k){
  key_t key;

  if((key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | MSG_PERM)) == -1)
    return -1;
  
  *k = key;
  return 0;
}

int check_queue()
{
  key_t key;

  if((key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | IPC_EXCL | MSG_PERM)) != -1){
    msgctl(key, IPC_RMID, 0);
    return -1;
  }

  return 1;
}

int send(key_t key, struct msgbuf *buf){
  if(msgsnd(key, (void*)buf, sizeof(struct msgbuf), IPC_NOWAIT) == -1)
    return -1;
  return 0;
}

int receive_sync(key_t key, struct msgbuf* buf, int msgtype){
  if(msgrcv(key, (void *)buf, sizeof(struct msgbuf), msgtype, 0) == -1)
    return -1;
  return 0;
}

int receive_async(key_t key, struct msgbuf* buf, int msgtype){
  if(msgrcv(key, (void *)buf, sizeof(struct msgbuf), msgtype, IPC_NOWAIT) == -1)
    return -1;
  return 0;
}
