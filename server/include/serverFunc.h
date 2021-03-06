#ifndef SERVER_FUNC_H
#define SERVER_FUNC_H
#include <string.h>
#include <time.h>
#include "_parson.h"
#include "_ipc.h"
#include "_linkedList.h"

int add_user(Register register_buf, JSON_Value* root_value);
int update_user(Register register_buf, JSON_Value* root_value);
int make_user_list(JSON_Value *root_value, char* response, int flag);
void record_log(char *car_number, int flag);
void calc_fee(JSON_Value* root_value, MyState* state_buf);
void get_car_number(JSON_Value* root_value, MyState* state_buf);
int is_parking(LinkedList *head, int user_key);
int get_log(char* file_name, char* response);
int get_user_info(JSON_Value *root_value, Register *register_buf, int user_key);
void get_map(LinkedList *head, char *response, int key);
void get_map_for_GL(LinkedList *head, char *response, int key);
int get_user_key(LinkedList *head, int pos);

#endif