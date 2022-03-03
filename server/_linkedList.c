#include <stdio.h>
#include "_linkedList.h"
#include "serverFunc.h"

// 유저 등록 함수: key값 리턴
int add_user(Register register_buf, JSON_Value* root_value){
  JSON_Value* new_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);
  JSON_Object* new_object = json_value_get_object(new_value);
  int user_key = (int)json_object_get_number(root_object, "totalUserNumber") + 1;

  json_object_set_number(new_object, "userKey", user_key);
  json_object_set_boolean(new_object, "isParking", 0);	
  json_object_set_string(new_object, "name", register_buf.name);
  json_object_set_string(new_object, "carNumber", register_buf.car_number);
  json_object_set_string(new_object, "phoneNumber", register_buf.phone_number);
  json_object_set_number(new_object, "isResident", register_buf.is_resident);
  
  JSON_Array* user_array = json_object_get_array(root_object, "users");
  json_array_append_value(user_array, new_value);

  json_object_set_number(root_object, "totalUserNumber", user_key);
	json_serialize_to_file_pretty(root_value, "users.json");
  
  return user_key;
}


// 유저 정보 업데이트 함수: 에러코드 리턴
int update_user(Register register_buf, JSON_Value* root_value){
  JSON_Object* root_object = json_value_get_object(root_value);
  JSON_Array* user_array = json_object_get_array(root_object, "users");
  int i;

  for(i = 0; i < json_array_get_count(user_array); i++){
    JSON_Value *user_value = json_array_get_value(user_array, i);
    JSON_Object *user_object = json_value_get_object(user_value);

    if(!strcmp(json_object_get_string(user_object, "carNumber"), register_buf.car_number))
    {
      json_object_set_number(user_object, "isResident", register_buf.is_resident);
      json_serialize_to_file_pretty(root_value, "users.json");
      return REQ_SUCCESS;
    }
  }
  return REQ_FAIL;
}

// 유저 리스트업 문자열 리턴 함수
int make_user_list(JSON_Value* root_value, char* response, int flag){
  char line[BUFFER_SIZE];
  char name[NAME_SIZE];
  char car_number[CAR_NUMBER_SIZE];
  char phone_number[PHONE_NUMBER_SIZE];
  int is_resident;
  int i, size, count;
  char resident_state[3][NAME_SIZE] = {"외부인", "검토중", "입주민"};
  JSON_Object* root_object = json_value_get_object(root_value);
  JSON_Array* user_array = json_object_get_array(root_object, "users");
  
  size = json_array_get_count(user_array);
  if(size == 0)
    return -1;
  
  count = 0;
  memset(response, 0, sizeof(RESPONSE_SIZE));
  strcat(response, "이름\t차량번호\t연락처\t\t입주여부\n");
  strcat(response, "------------------------------------------------\n");
  for(i = 0; i < size; i++){
    JSON_Value *user_value = json_array_get_value(user_array, i);
    JSON_Object *user_object = json_value_get_object(user_value);
    
    strcpy(name, json_object_get_string(user_object, "name"));
    strcpy(car_number, json_object_get_string(user_object, "carNumber"));
    strcpy(phone_number, json_object_get_string(user_object, "phoneNumber"));

    is_resident = (int)json_object_get_number(user_object, "isResident");
    sprintf(line,"%-10s %-16s %-15s %s\n", name, car_number, "010-9968-1949", resident_state[is_resident]);
    // All
    if(flag == 0)
      strcat(response, line);
    // Pending
    else if(flag == 1 && is_resident == 1){
      strcat(response, line);
      count++;
    }
  }

  if(flag == 1 && count == 0){
    return -1;
  }
  return 0;
}

//flag(0 = 입차, 1 = 출차)
void record_log(char* car_number, int flag){
  FILE* fp, *fp2;
  time_t now;
  struct tm *tmbuf;
  char buf[BUFFER_SIZE];
  
  sprintf(buf, "%s.log", car_number);
  
  // 개인 로그파일 오픈
  if((fp = fopen(buf, "a+")) == NULL){
    fprintf(stderr, "fopen() error\n");
    return;
  }

  // 통합 로그파일 오픈
  if((fp2 = fopen("internal.log", "a+")) == NULL){
    fprintf(stderr, "fopen() error\n");
    return;
  }
   
  now = time(NULL);
  tmbuf = localtime(&now);
  
  if(flag == 0){
    fprintf(fp, "[%d-%02d-%02d %02d:%02d:%02d] %s 입차\n", tmbuf->tm_year + 1900, tmbuf->tm_mon + 1, tmbuf->tm_mday, tmbuf->tm_hour, tmbuf->tm_min, tmbuf->tm_sec, car_number);
    fprintf(fp2, "[%d-%02d-%02d %02d:%02d:%02d] %s 입차\n", tmbuf->tm_year + 1900, tmbuf->tm_mon + 1, tmbuf->tm_mday, tmbuf->tm_hour, tmbuf->tm_min, tmbuf->tm_sec, car_number);
  }
  else{
    fprintf(fp, "[%d-%02d-%02d %02d:%02d:%02d] %s 출차\n", tmbuf->tm_year + 1900, tmbuf->tm_mon + 1, tmbuf->tm_mday, tmbuf->tm_hour, tmbuf->tm_min, tmbuf->tm_sec, car_number);
    fprintf(fp2, "[%d-%02d-%02d %02d:%02d:%02d] %s 출차\n", tmbuf->tm_year + 1900, tmbuf->tm_mon + 1, tmbuf->tm_mday, tmbuf->tm_hour, tmbuf->tm_min, tmbuf->tm_sec, car_number);
  }
  fclose(fp);
  fclose(fp2);

}

void calc_fee(JSON_Value* root_value, MyState* state_buf){
  JSON_Object* root_object = json_value_get_object(root_value);
  JSON_Array* user_array = json_object_get_array(root_object, "users");
  time_t now;
  int i, is_resident, user_key;

  now = time(NULL);
  for(i = 0; i < json_array_get_count(user_array); i++){
    JSON_Value *user_value = json_array_get_value(user_array, i);
    JSON_Object *user_object = json_value_get_object(user_value);
    
    is_resident = (int)json_object_get_number(user_object, "isResident");
    user_key = (int)json_object_get_number(user_object, "userKey");

    // 입주민인 경우
    if(user_key == state_buf->user_key && is_resident == 2){
      state_buf->cost = ((now - state_buf->unixtime) * 100) / 2;
      return;
    }
  }
  state_buf->cost = (now - state_buf->unixtime) * 100;
}

void get_car_number(JSON_Value* root_value, MyState* state_buf){
  JSON_Object* root_object = json_value_get_object(root_value);
  JSON_Array* user_array = json_object_get_array(root_object, "users");
  int i, user_key;

  for(i = 0; i < json_array_get_count(user_array); i++){
    JSON_Value *user_value = json_array_get_value(user_array, i);
    JSON_Object *user_object = json_value_get_object(user_value);

    user_key = (int)json_object_get_number(user_object, "userKey");

    if(user_key == state_buf->user_key){
      strcpy(state_buf->car_number,json_object_get_string(user_object, "carNumber"));
      return;
    }
  }
}

int is_parking(LinkedList *head, int user_key){
  LinkedList *cur;

  cur = head;
  while(cur != NULL){
    if(cur->key == user_key)
      return 0;
    cur = cur->next;
  }

  return -1;
}

int get_log(char* file_name, char* response)
{
  FILE *fp;
  char buffer[BUFFER_SIZE];
  int log_size, len;

  if((fp = fopen(file_name, "r+")) == NULL)
    return -1;

  fseek(fp, 0, SEEK_END);
  log_size = ftell(fp); 
  fseek(fp, 0, SEEK_SET);

  memset(response, 0, sizeof(RESPONSE_SIZE));
  while(!feof(fp)){
    fgets(buffer, sizeof(buffer), fp);
  
    if(RESPONSE_SIZE - 100 > log_size)
      strcat(response, buffer);
    len = strlen(buffer);
    log_size -= len;
  }

  return 0;
}

int get_user_info(JSON_Value *root_value, Register *register_buf, int user_key){
  JSON_Object* root_object = json_value_get_object(root_value);
  JSON_Array* user_array = json_object_get_array(root_object, "users");
  int i;

  for(i = 0; i < json_array_get_count(user_array); i++){
    JSON_Value *user_value = json_array_get_value(user_array, i);
    JSON_Object *user_object = json_value_get_object(user_value);

    if(!strcmp(json_object_get_string(user_object, "carNumber"), register_buf->car_number)){
      strcpy(register_buf->name,json_object_get_string(user_object, "name"));
      strcpy(register_buf->phone_number,json_object_get_string(user_object, "phoneNumber"));
      return 0;
    }

    if((int)json_object_get_number(user_object, "userKey") == user_key){
      strcpy(register_buf->name,json_object_get_string(user_object, "name"));
      strcpy(register_buf->phone_number,json_object_get_string(user_object, "phoneNumber"));
      strcpy(register_buf->car_number,json_object_get_string(user_object, "carNumber"));
      return 0;
    }
  }
  return -1;
}

void get_map(LinkedList *head, char *response, int key){
  int i, j;
  int half = PARKING_LOT_SIZE / 2;
  int number = 15;
  LinkedList *cur;
  char buf[BUFFER_SIZE];
  char map[20][BUFFER_SIZE-1] = {
                    "┌─────┬─────┬─────┬─────┬─────┬─────┐",
                    "│     │     │     │     │     │     │",
                    "│     │     │     │     │     │     │",
                    "│     │     │     │     │     │     │",
                    "│     │     │     │     │     │     │",
                    "└─────┴─────┴─────┴─────┴─────┴─────┘",
                    "   1     2     3     4     5     6   ",
                    "┌───────────────────────────────────┐",
                    "│                                   │",
                    "│                                   │",
                    "│                                   │",
                    "└───────────────────────────────────┘",
                    "┌─────┬─────┬─────┬─────┬─────┬─────┐",
                    "│     │     │     │     │     │     │",
                    "│     │     │     │     │     │     │",
                    "│     │     │     │     │     │     │",
                    "│     │     │     │     │     │     │",
                    "└─────┴─────┴─────┴─────┴─────┴─────┘",
                    "  7      8     9     10    11    12  ",};
  
  memset(response, 0, sizeof(RESPONSE_SIZE));

  cur = head;
  while(cur != NULL){
    number = cur->idx;

    if(key != -1 && cur->key != key){
      cur = cur->next;
      continue; 
    }
    if(number < half+1){
      number -= 1;
      for(i = 1; i <= 4; i++)
        for(j = 4; j <= 6; j++)
          map[i][(8 * number) + j] = 'X';
    }
    else{
      number -= half+1;
      for(i = 13; i <= 16; i++)
          for(j = 4; j <= 6; j++)
            map[i][(8 * number) + j] = 'X';
    }
    cur = cur->next;
  }

  for(i = 0; i < 20; i ++) {
    sprintf(buf, "%s\n", map[i]);
    strcat(response, buf);
  }
}

int get_user_key(LinkedList *head, int pos){
  LinkedList *cur;

  cur = head;
  while(cur != NULL){
    if(cur->idx == pos){
      return cur->key;
    }
    cur = cur->next;
  }

  return -1;
}