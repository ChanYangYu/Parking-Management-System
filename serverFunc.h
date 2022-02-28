#ifndef SERVER_FUNC_H
#define SERVER_FUNC_H
#include "_parson.h"
#include "_ipc.h"

void add_user(Register register_buf, JSON_Value* root_value);
int update_user(Register register_buf, JSON_Object* root_object);

#endif