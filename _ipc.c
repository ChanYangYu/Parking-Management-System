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
