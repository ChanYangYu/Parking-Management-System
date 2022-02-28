#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "_ipc.h"
#include "serverFunc.h"

Register register_buf;
MyState state_buf;
Manage manage_buf;
JSON_Value* root_value;

void process_register(key_t msg_key);
void process_update(key_t msg_key);
void process_listup(key_t msg_key);
void process_listup_all(key_t msg_key);
void process_listup_pre(key_t msg_key);

int main()
{ 
  key_t msg_key;
  if((msg_key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | MSG_PERM)) == -1){
    fprintf(stderr,"Error: msgget() error\n");
    exit(1);
  }

  root_value = json_parse_file("users.json");
  printf("Key is %d\n", msg_key);

  while(1){
    memset(&register_buf, 0, sizeof(Register));
    memset(&state_buf, 0, sizeof(MyState));
    memset(&manage_buf, 0, sizeof(Manage));

    process_register(msg_key);
    process_update(msg_key);

    usleep(200000);
  }

  return 0;
}

void process_register(key_t msg_key){
  if(msgrcv(msg_key, (void *)&register_buf, sizeof(Register), MSG_REGISTER_REQ, IPC_NOWAIT) != -1){
    printf("[Register]\n");
    
    // 임시값 적용
    state_buf.user_key = add_user(register_buf, root_value);
    state_buf.msgtype = MSG_REGISTER_RES;
    
    if(msgsnd(msg_key, (void*)&state_buf, sizeof(MyState), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_update(key_t msg_key){
  if(msgrcv(msg_key, (void *)&register_buf, sizeof(Register), MSG_UPDATE_REQ, IPC_NOWAIT) != -1){
    printf("[Update]\n");
    
    register_buf.errno = update_user(register_buf, root_value);
    register_buf.msgtype = MSG_UPDATE_RES;
    
    if(msgsnd(msg_key, (void*)&register_buf, sizeof(Register), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_listup_all(key_t msg_key){
  if(msgrcv(msg_key, (void *)&manage_buf, sizeof(Manage), MSG_LISTUP_ALL_REQ, IPC_NOWAIT) != -1){
    printf("[Listup-All]\n");
    
    manage_buf.errno = REQ_SUCCESS;
    manage_buf.msgtype = MSG_LISTUP_ALL_RES;
    
    if(msgsnd(msg_key, (void*)&manage_buf, sizeof(Manage), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_listup_pre(key_t msg_key){
  if(msgrcv(msg_key, (void *)&manage_buf, sizeof(Manage), MSG_LISTUP_PRE_REQ, IPC_NOWAIT) != -1){
    printf("[Listup-Pre]\n");
    
    manage_buf.errno = REQ_SUCCESS;
    manage_buf.msgtype = MSG_LISTUP_PRE_RES;
    
    if(msgsnd(msg_key, (void*)&manage_buf, sizeof(Manage), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_in(key_t msg_key){
  if(msgrcv(msg_key, (void *)&state_buf, sizeof(MyState), MSG_CAR_IN_REQ, IPC_NOWAIT) != -1){
    printf("[Car-In]\n");
    
    state_buf.errno = REQ_SUCCESS;
    state_buf.msgtype = MSG_CAR_IN_RES;
    
    if(msgsnd(msg_key, (void*)&state_buf, sizeof(MyState), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_out(key_t msg_key){
  if(msgrcv(msg_key, (void *)&state_buf, sizeof(MyState), MSG_CAR_OUT_REQ, IPC_NOWAIT) != -1){
    printf("[Car-Out]\n");
    
    state_buf.errno = REQ_SUCCESS;
    state_buf.msgtype = MSG_CAR_OUT_RES;
    
    if(msgsnd(msg_key, (void*)&state_buf, sizeof(MyState), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}

void process_find_my_car(key_t msg_key)
{
  if(msgrcv(msg_key, (void *)&state_buf, sizeof(MyState), MSG_CAR_OUT_REQ, IPC_NOWAIT) != -1){
    printf("[Find-My-Car]\n");
    
    state_buf.errno = REQ_SUCCESS;
    state_buf.msgtype = MSG_CAR_OUT_RES;
    
    if(msgsnd(msg_key, (void*)&state_buf, sizeof(MyState), IPC_NOWAIT) == -1){
      fprintf(stderr,"Error: msgsnd() error\n");
      exit(1);
    }
  }
}