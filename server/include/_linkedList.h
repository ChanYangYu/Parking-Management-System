#ifndef LINK_LIST_H
#define LINK_LIST_H
#define PARKING_LOT_SIZE 12
#include <stdlib.h>
#include <time.h>
#include "_ipc.h"

typedef struct LinkedList {
  int idx;
  int key;
  time_t in_time;
  struct LinkedList* next;
}LinkedList;

int insert_node(LinkedList **head, MyState *state_buf);
int delete_node(LinkedList **head, MyState *state_buf);

#endif
