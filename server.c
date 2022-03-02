#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "_ipc.h"
#include "_linkedList.h"
#include "serverFunc.h"

Register register_buf;
MyState state_buf;
Manage manage_buf;
JSON_Value* root_value;
LinkedList* head;

void process_register(key_t msg_key);
void process_update(key_t msg_key);
void process_listup_all(key_t msg_key);
void process_listup_pending(key_t msg_key);
void process_in(key_t msg_key);
void process_out(key_t msg_key);
void process_find_my_car(key_t msg_key);
void process_find_user_all_history(key_t msg_key);
void process_find_user_history(key_t msg_key);
void process_find_by_car_number(key_t msg_key);
void process_find_state(key_t msg_key);
void process_find_detail_info(key_t msg_key);

int main()
{ 
  key_t msg_key;
  if((msg_key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | MSG_PERM)) == -1){
    fprintf(stderr,"Error: msgget() error\n");
    exit(1);
  }

  root_value = json_parse_file("users.json");
  head = NULL;
  printf("Key is %d\n", msg_key);

  while(1){
    memset(&register_buf, 0, sizeof(Register));
    memset(&state_buf, 0, sizeof(MyState));
    memset(&manage_buf, 0, sizeof(Manage));

    process_register(msg_key);
    process_update(msg_key);
    process_listup_all(msg_key);
    process_listup_pending(msg_key);
    process_in(msg_key);
    process_out(msg_key);
    process_find_my_car(msg_key);
    process_find_user_all_history(msg_key);
    process_find_user_history(msg_key);
    process_find_by_car_number(msg_key);
    process_find_state(msg_key);
    process_find_detail_info(msg_key);

    usleep(200000);
  }

  return 0;
}

void process_register(key_t msg_key){
  if(msgrcv(msg_key, (void *)&register_buf, sizeof(Register) - sizeof(long), MSG_REGISTER_REQ, IPC_NOWAIT) != -1){
    printf("[Register]\n");
    
    // 임시값 적용
    state_buf.user_key = add_user(register_buf, root_value);
    state_buf.msgtype = MSG_REGISTER_RES;
    
    if(msgsnd(msg_key, (void*)&state_buf, sizeof(MyState) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_update(key_t msg_key){
  if(msgrcv(msg_key, (void *)&register_buf, sizeof(Register) - sizeof(long), MSG_UPDATE_REQ, IPC_NOWAIT) != -1){
    printf("[Update]\n");
    
    register_buf.errno = update_user(register_buf, root_value);
    register_buf.msgtype = MSG_UPDATE_RES;
    
    if(msgsnd(msg_key, (void*)&register_buf, sizeof(Register) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_listup_all(key_t msg_key){
  if(msgrcv(msg_key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_LISTUP_ALL_REQ, IPC_NOWAIT) != -1){
    printf("[Listup-All]\n");
    
    if(make_user_list(root_value, manage_buf.response, 0) == -1)
      manage_buf.errno = REQ_FAIL;
    else
      manage_buf.errno = REQ_SUCCESS;
    manage_buf.msgtype = MSG_LISTUP_ALL_RES;
    
    if(msgsnd(msg_key, (void*)&manage_buf, sizeof(Manage) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_listup_pending(key_t msg_key){
  if(msgrcv(msg_key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_LISTUP_PEN_REQ, IPC_NOWAIT) != -1){
    printf("[Listup-Pre]\n");
    
    if(make_user_list(root_value, manage_buf.response, 1) == -1)
      manage_buf.errno = REQ_FAIL;
    else
      manage_buf.errno = REQ_SUCCESS;
    manage_buf.msgtype = MSG_LISTUP_PEN_RES;
    
    if(msgsnd(msg_key, (void*)&manage_buf, sizeof(Manage) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_in(key_t msg_key){
  if(msgrcv(msg_key, (void *)&state_buf, sizeof(MyState) - sizeof(long), MSG_CAR_IN_REQ, IPC_NOWAIT) != -1){
    printf("[Car-In]\n");
    
    if(insert_node(&head, &state_buf) == 0){
      state_buf.errno = REQ_SUCCESS;
      get_car_number(root_value, &state_buf);
      record_log(state_buf.car_number, 0);
      get_map(head, state_buf.map, state_buf.user_key);
      //todo: state_buf->map 업데이트
    }
    else
      state_buf.errno = REQ_FAIL;
    state_buf.msgtype = MSG_CAR_IN_RES;
    
    if(msgsnd(msg_key, (void*)&state_buf, sizeof(MyState) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_out(key_t msg_key){
  if(msgrcv(msg_key, (void *)&state_buf, sizeof(MyState) - sizeof(long), MSG_CAR_OUT_REQ, IPC_NOWAIT) != -1){
    printf("[Car-Out]\n");
    
    if(delete_node(&head, &state_buf) == 0){
      state_buf.errno = REQ_SUCCESS;
      calc_fee(root_value, &state_buf);
      get_car_number(root_value, &state_buf);
      record_log(state_buf.car_number, 1);
    }
    else
      state_buf.errno = REQ_FAIL;
    state_buf.msgtype = MSG_CAR_OUT_RES;
    
    if(msgsnd(msg_key, (void*)&state_buf, sizeof(MyState) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_find_my_car(key_t msg_key)
{
  if(msgrcv(msg_key, (void *)&state_buf, sizeof(MyState) - sizeof(long), MSG_CAR_STATE_REQ, IPC_NOWAIT) != -1){
    printf("[Find-My-Car]\n");
    
    // 입차 = 0, 출차 = 1
    if(is_parking(head, state_buf.user_key) == 0)
      state_buf.state = 0;  
    else
      state_buf.state = 1;
    
    //todo: state_buf->map 업데이트
    get_map(head, state_buf.map, state_buf.user_key);
    state_buf.errno = REQ_SUCCESS;
    state_buf.msgtype = MSG_CAR_STATE_RES;
    
    if(msgsnd(msg_key, (void*)&state_buf, sizeof(MyState) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_find_user_all_history(key_t msg_key){
  char *file_name = "internal.log";

  if(msgrcv(msg_key, (void *)&state_buf, sizeof(MyState) - sizeof(long), MSG_FIND_ALL_HISTORY_REQ, IPC_NOWAIT) != -1){
    printf("[User-All-History]\n");
    
    if(get_log(file_name, manage_buf.response) == -1)
      manage_buf.errno = REQ_FAIL;
    else
      manage_buf.errno = REQ_SUCCESS;
    manage_buf.msgtype = MSG_FIND_ALL_HISTORY_RES;
    
    if(msgsnd(msg_key, (void*)&manage_buf, sizeof(Manage) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_find_user_history(key_t msg_key){
  char file_name[BUFFER_SIZE];

  if(msgrcv(msg_key, (void *)&state_buf, sizeof(MyState) - sizeof(long), MSG_FIND_USER_HISTORY_REQ, IPC_NOWAIT) != -1){
    printf("[User-Personal-History]\n");
    
    sprintf(file_name, "%s.log", state_buf.car_number);
    if(get_log(file_name, manage_buf.response) == -1)
      manage_buf.errno = REQ_FAIL;
    else
      manage_buf.errno = REQ_SUCCESS;
    manage_buf.msgtype = MSG_FIND_USER_HISTORY_RES;
    
    if(msgsnd(msg_key, (void*)&manage_buf, sizeof(Manage) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_find_by_car_number(key_t msg_key){
  if(msgrcv(msg_key, (void *)&register_buf, sizeof(Register) - sizeof(long), MSG_FIND_CAR_NUMBER_REQ, IPC_NOWAIT) != -1){
    printf("[Find-by-CarNumber]\n");
  
    if(get_user_info(root_value, &register_buf, -1) == -1)
      register_buf.errno = REQ_FAIL;
    else
      register_buf.errno = REQ_SUCCESS;
    register_buf.msgtype = MSG_FIND_CAR_NUMBER_RES;
    
    if(msgsnd(msg_key, (void*)&register_buf, sizeof(Register) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}
void process_find_state(key_t msg_key){
  if(msgrcv(msg_key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_FIND_CARS_REQ, IPC_NOWAIT) != -1){
    printf("[Find-State]\n");
  
    get_map(head, manage_buf.response, -1);
    manage_buf.errno = REQ_SUCCESS;
    manage_buf.msgtype = MSG_FIND_CARS_RES;
    
    if(msgsnd(msg_key, (void*)&manage_buf, sizeof(Manage) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}


void process_find_detail_info(key_t msg_key){
  int user_key;
  if(msgrcv(msg_key, (void *)&manage_buf, sizeof(Manage) - sizeof(long), MSG_FIND_CARS_DETAIL_REQ, IPC_NOWAIT) != -1){
    printf("[Find-Detail-Info]\n");

    if((user_key = get_user_key(head, manage_buf.pos)) != -1){
      if(get_user_info(root_value, &register_buf, user_key) == -1)
        register_buf.errno = REQ_FAIL;
      else
        register_buf.errno = REQ_SUCCESS;
    }
    else
       register_buf.errno = REQ_FAIL;
    register_buf.msgtype = MSG_FIND_CARS_DETAIL_RES;
    
    if(msgsnd(msg_key, (void*)&register_buf, sizeof(Register) - sizeof(long), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}