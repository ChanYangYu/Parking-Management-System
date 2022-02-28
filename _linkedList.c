#include "_linkedList.h"

LinkedList* make_node(int idx, int user_key){
  LinkedList *tmp;
  tmp = (LinkedList*) malloc(sizeof(LinkedList));
  tmp->idx = idx;
  tmp->in_time = time(NULL);
  tmp->key = user_key;
  tmp->next = NULL;

  return tmp;
}

int insert_node(LinkedList **head, MyState *state_buf){
  LinkedList *next;
  LinkedList *cur = *head;

  if(*head == NULL){
    *head = make_node(1, state_buf->user_key);
    state_buf->unixtime = time(NULL);
    return 0;
  }

  while(cur != NULL){
    next = cur->next;
    if(cur->idx == PARKING_LOT_SIZE)
      return -1;

    // 마지막 노드인 경우
    if(next == NULL){
      cur->next = make_node(cur->idx + 1, state_buf->user_key);
      state_buf->unixtime = time(NULL);
      return 0;
    }
    // 중간에 비어있는 경우 
    else if(next->idx > cur->idx + 1){
      cur->next = make_node(cur->idx + 1, state_buf->user_key);
      cur->next->next = next;
      state_buf->unixtime = time(NULL);
      return 0;
    }
    
    cur = cur->next;
  }
  return -1;
}

int delete_node(LinkedList** head, MyState *state_buf){
  LinkedList *prev = NULL;
  LinkedList *cur = *head;

  while(cur != NULL){
    if(state_buf->user_key == cur->key){
      if(prev == NULL)
        *head = cur->next;
      else
        prev->next = cur->next;
      
      state_buf->unixtime = cur->in_time;
      free(cur);
      return 0;
    }
    prev = cur;
  }
  
  return -1;
}