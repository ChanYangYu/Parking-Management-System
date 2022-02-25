#ifndef IPC_H
#define IPC_H
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define MSG_MAX_SIZE 256
#define MSG_QUEUE_KEY 0304
#define MSG_PERM 0666

// Structure
struct msgbuf
{
  long msgtype;
  int seq;
  char msg[MSG_MAX_SIZE];
};

// Function
int init(key_t *k);
int check_queue();

#endif
