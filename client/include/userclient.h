#ifndef USER_CLINET_H
#define USER_CLINET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include "_ipc.h"
#include <termio.h>
#include "GL/freeglut.h"
#include "irrKlang/irrKlang.h"
#include "conio.h"
#include <stdio_ext.h>

class UserClient
{
private:
    char ttybuf[100];
    Register basic_info;

    const char* parking_bg_image_path = "textures/parking_bg.jpg";
    const char* car_image_path = "textures/car.jpg";
    const char* other_car_image_path = "textures/other_car.jpg";

    irrklang::ISoundEngine* engine;
    irrklang::ISoundSource* sound_src_regi_s = nullptr;
    irrklang::ISoundSource* sound_src_regi_e = nullptr;
    irrklang::ISoundSource* sound_src_cin_s = nullptr;
    irrklang::ISoundSource* sound_src_cin_e = nullptr;
    irrklang::ISoundSource* sound_src_cout_s = nullptr;
    irrklang::ISoundSource* sound_src_cout_e = nullptr;
    irrklang::ISoundSource* sound_src_sys_e = nullptr;

    int getkey(void);
    bool checkphonenumber(void);
    void new_user_init(void);
    void print_parking_map(void);
    void save_time_file(void);

public:
    UserClient();

    bool mycarstate = false;
    bool want_quit = false;
    char userdir[100];

    void get_tty(void);
    void set_cron_message(void);
    void delete_cron_message(void);
    void client_process_setup(int margc, char** margv);
    void parkingin(void);
    void parkingout(void);
    void finish_sound_src(void);
};

#endif
