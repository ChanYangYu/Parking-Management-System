#ifndef LINK_LIST_H
#define LINK_LIST_H
#define PARKING_LOT_SIZE 10
#include <time.h>

typedef struct LinkedList {
  int idx;
  int key;
  time_t in_time;
  struct LinkedList* next;
}LinkedList;

LinkedList* make_node(int idx, int user_key);
int insert_node(LinkedList **head, int user_key);
int delete_node(LinkedList** head, int key);

#endif
