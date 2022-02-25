#ifndef IPC_H
#define IPC_H
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define MSG_NUMBER_LEN 256
#define MSG_QUEUE_KEY 0304
#define MSG_PERM 0666
#define MSG_TYPE_ALL 0
#define MSG_TYPE 3

// Structure
struct msgbuf
{
  long msgtype;
  int in_out;
  int cost;
  char number[MSG_NUMBER_LEN];
};

// Function
int init(key_t *k);
int send(key_t key, struct msgbuf *buf);
int receive_sync(key_t key, struct msgbuf* buf, int msgtype);
int receive_async(key_t key, struct msgbuf* buf, int msgtype);

#endif
