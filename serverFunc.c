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
      return REQ_SUCCESS;
    }
  }
  return REQ_FAIL;
}



