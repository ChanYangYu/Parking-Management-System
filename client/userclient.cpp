#include "userclient.h"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

//global varriable
int argc;
char** argv;
pid_t pid;

MyState my_info;
key_t msg_key;

unsigned char* PBG;
int bgwidth;
int bgheight;
int bgchannels;

unsigned char* MYCAR;
int carwidth;
int carheight;
int carchannels;

unsigned char* NOTMYCAR;
int ncarwidth;
int ncarheight;
int ncarchannels;


double locate_car[12][2];

GLuint bgtexture;
GLuint mycartexture;
GLuint notmycartexture;

UserClient::UserClient()
{
    // Constructor
    locate_car[0][0] = -0.69;
    locate_car[0][1] = 0.45;
    locate_car[1][0] = -0.41;
    locate_car[1][1] = 0.45;
    locate_car[2][0] = -0.14;
    locate_car[2][1] = 0.45;
    locate_car[3][0] = 0.135;
    locate_car[3][1] = 0.45;
    locate_car[4][0] = 0.41;
    locate_car[4][1] = 0.45;
    locate_car[5][0] = 0.685;
    locate_car[5][1] = 0.45;
    locate_car[6][0] = -0.69;
    locate_car[6][1] = -0.52;
    locate_car[7][0] = -0.41;
    locate_car[7][1] = -0.52;
    locate_car[8][0] = -0.14;
    locate_car[8][1] = -0.52;
    locate_car[9][0] = 0.135;
    locate_car[9][1] = -0.52;
    locate_car[10][0] = 0.41;
    locate_car[10][1] = -0.52;
    locate_car[11][0] = 0.685;
    locate_car[11][1] = -0.52;

    engine = irrklang::createIrrKlangDevice();
    if(!engine)
    {
	    printf("프로그램 실행 에러 시스템을 종료합니다.\n");
	    fprintf(stderr,"Error sound engine init.\n");
	    exit(1);
    }

    sound_src_regi_s = engine->addSoundSourceFromFile("mobject/register_start.wav");
    sound_src_regi_e = engine->addSoundSourceFromFile("mobject/register_end.wav");
    sound_src_cin_s = engine->addSoundSourceFromFile("mobject/carin_start.wav");
    sound_src_cin_e = engine->addSoundSourceFromFile("mobject/carin_end.wav");
    sound_src_cout_s = engine->addSoundSourceFromFile("mobject/carout_start.wav");
    sound_src_cout_e = engine->addSoundSourceFromFile("mobject/carout_end.wav");
    sound_src_sys_e = engine->addSoundSourceFromFile("mobject/system_end.wav");

    sound_src_regi_s->setDefaultVolume(1.0f);
    sound_src_regi_e->setDefaultVolume(1.0f);
    sound_src_cin_s->setDefaultVolume(1.0f);
    sound_src_cin_e->setDefaultVolume(1.0f);
    sound_src_cout_s->setDefaultVolume(1.0f);
    sound_src_cout_e->setDefaultVolume(1.0f);
    sound_src_sys_e->setDefaultVolume(1.0f);


    PBG = stbi_load(parking_bg_image_path, &bgwidth, &bgheight, &bgchannels, 4);
    if (!PBG)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error can't find texture image.\n");
        exit(1);
    }
    MYCAR = stbi_load(car_image_path, &carwidth, &carheight, &carchannels, 4);
    if (!MYCAR)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error can't find texture image.\n");
        exit(1);
    }
    NOTMYCAR = stbi_load(other_car_image_path, &ncarwidth, &ncarheight, &ncarchannels, 4);
    if (!NOTMYCAR)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error can't find texture image.\n");
        exit(1);
    }

}

int UserClient::getkey(void)
{
    int ch;
    struct termios old;
    struct termios bnew;
    tcgetattr(0, &old);
    bnew = old;
    bnew.c_lflag &= ~(ICANON|ECHO);
    bnew.c_cc[VMIN] = 1;
    bnew.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &bnew);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &old);
    return ch;
}

bool UserClient::checkphonenumber(void)
{
    for(int i=0; i<11; i++)
    {
        if(basic_info.phone_number[i] < 48 || basic_info.phone_number[i] > 57)
            return false;
        
    }
    return true;
}

void UserClient::new_user_init(void)
{
USERINIT:
    engine->play2D(sound_src_regi_s);
    system("clear");
    printf("처음 오신것을 환영합니다. 신규 주차관리 유저 등록화면입니다\n");
    printf("이름을 입력해 주세요 : ");
    fgets(basic_info.name, sizeof(basic_info.name), stdin);
    basic_info.name[strlen(basic_info.name) - 1] = '\0';
    __fpurge(stdin);

    printf("자동차 번호를 입력해 주세요 : ");
    fgets(basic_info.car_number, sizeof(basic_info.car_number), stdin);
    basic_info.car_number[strlen(basic_info.car_number) - 1] = '\0';
    __fpurge(stdin);

    printf("전화 번호를 입력해 주세요(11자리 숫자) : ");
REGETPHONENUM:
    fgets(basic_info.phone_number, sizeof(basic_info.phone_number), stdin);
    basic_info.phone_number[strlen(basic_info.phone_number) - 1] = '\0';
    __fpurge(stdin);

    if(checkphonenumber() == false || strlen(basic_info.phone_number) != 11)
    {
        printf("잘못 입력하셨습니다. 전화 번호를 입력해 주세요(11자리 숫자) : ");
        memset(basic_info.phone_number, 0, sizeof(basic_info.phone_number));
        goto REGETPHONENUM;
    }

    char numberbuf;
    printf("입주민 여부를 입력해주세요(맞으시면 1, 틀리시면 0) : ");

    scanf("%c", &numberbuf);
    __fpurge(stdin);
    if(numberbuf != 48 && numberbuf != 49)
    {
        goto RETRYGETNUMBER;
    }
    else
    {
        basic_info.is_resident = numberbuf - 48;
        goto SIGNUP;
    }

RETRYGETNUMBER:
    printf("잘못 입력하셨습니다. 입주민 여부를 입력해주세요(맞으시면 1, 틀리시면 0) : ");
    scanf("%c", &numberbuf);
    __fpurge(stdin);
    if(numberbuf != 48 && numberbuf != 49)
    {
        goto RETRYGETNUMBER;
    }
    else
    {
        basic_info.is_resident = numberbuf - 48;
    }

SIGNUP:
    printf("가입진행 중 입니다.....\n");

    basic_info.msgtype = MSG_REGISTER_REQ;

    if(msgsnd(msg_key, (void *)&basic_info, sizeof(Register) - sizeof(long), 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error send message to server.\n");
        exit(1);
    }
    
    if(msgrcv(msg_key, (void *)&my_info, sizeof(MyState) - sizeof(long), MSG_REGISTER_RES, 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error get message to server.\n");
        exit(1);
    }
    
    if(my_info.errno != REQ_SUCCESS)
    {
        printf("가입서버에 에러가 있습니다. 죄송하지만 다시 시도해주세요\n");
        fprintf(stderr,"Error incorrect subscription.\n");
        goto USERINIT;
    }
    else
    {
        engine->stopAllSounds();
        engine->play2D(sound_src_regi_e);
        system("clear");
        printf("%s[%d]님 가입을 환영합니다.\n", basic_info.name, my_info.user_key);
    }

    int new_user_file = open("key.txt", O_CREAT|O_WRONLY, 0755);

    char keybuf[100];

    memset(keybuf, 0, sizeof(keybuf));

    sprintf(keybuf, "%d", my_info.user_key);

    if(write(new_user_file, keybuf, sizeof(keybuf)) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error write file.\n");
        close(new_user_file);
        exit(1);
    }
    close(new_user_file);
    __fpurge(stdin);
    printf("계속하시려면 아무키나 눌러주세요.\n");
    char keyboardbuf = getkey();
    if(keyboardbuf == 27)
        __fpurge(stdin);
    engine->stopAllSounds();
}

//내부 GL 함수

void init_texture()
{
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &bgtexture);
    glBindTexture(GL_TEXTURE_2D, bgtexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bgwidth, bgheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, PBG);
}

void drawBG()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POLYGON);
    glTexCoord2d(0.0, 1.0f);      glVertex3d(-1.0f, -1.0f, 0.0);
    glTexCoord2d(0.0, 0.0);      glVertex3d(-1.0f, 1.0f, 0.0);
    glTexCoord2d(1.0f, 0.0);      glVertex3d(1.0f, 1.0f, 0.0);
    glTexCoord2d(1.0f, 1.0f);      glVertex3d(1.0f, -1.0f, 0.0);
	glEnd();
	glFlush();
}

void init_texture_car()
{
    glGenTextures(1, &mycartexture);
    glBindTexture(GL_TEXTURE_2D, mycartexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, carwidth, carheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, MYCAR);
}

void init_texture_othercar()
{
    glGenTextures(1, &notmycartexture);
    glBindTexture(GL_TEXTURE_2D, notmycartexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ncarwidth, ncarheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NOTMYCAR);
}

void drawCAR(int mycar_where)
{
    double a = locate_car[mycar_where][0];
    double b = locate_car[mycar_where][1];
    glBegin(GL_POLYGON);
    glTexCoord2d(0.0, 1.0f);      glVertex3d((-0.13 +a) * 1.0f, (-0.35 +b) * 1.0f, 0.0);
    glTexCoord2d(0.0, 0.0);      glVertex3d((-0.13 +a) * 1.0f, (0.35 +b) * 1.0f, 0.0);
    glTexCoord2d(1.0f, 0.0);      glVertex3d((0.13 +a) * 1.0f, (0.35 +b) * 1.0f, 0.0);
    glTexCoord2d(1.0f, 1.0f);      glVertex3d((0.13 +a) * 1.0f, (-0.35 +b) * 1.0f, 0.0);
	glEnd();
	glFlush();
}

void reshape(GLint w, GLint h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 30.0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
    {
		case 'q':
			exit(pid);
			break;
        case 'Q':
            exit(pid);
			break;
	}
	glutPostRedisplay();
}

void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    init_texture();
    drawBG();

    int mycar_where = 0;
    int not_mycar_where[11];
    memset(not_mycar_where, 0, sizeof(not_mycar_where));
    int check = 0;

    for(int i=0; i<12; i++)
    {
        if(my_info.map[i] == 1)
            mycar_where = i;

        if(my_info.map[i] == 2)
        {
            not_mycar_where[check] = i;
            check++;
        }
    }

    init_texture_car();
    drawCAR(mycar_where);

    init_texture_othercar();
    for(int i=0; i<check; i++)
    {
        drawCAR(not_mycar_where[i]);
    }
    

    glutSwapBuffers();
}

void UserClient::print_parking_map(void)
{
    pid = fork();

    if(pid == 0)
    {
        printf("\n잠시 기다리시면 그래픽 화면으로 주차된 자동차를 보실 수 있습니다.\n");
    	glutInit(&argc, argv);
    	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
        glutInitWindowSize(500,500);
    	glutInitWindowPosition(200,100);
    	glutCreateWindow("Parking lot");
    	glutSetWindowTitle("Parking lot");;
        glutReshapeFunc(reshape);

        printf("먼저 그래픽 주차장 화면을 종료하시려면 q를 입력해주세요\n");
    	glutDisplayFunc(display);
    	glutKeyboardFunc(keyboard);
    	glutMainLoop();
    } 
}

void UserClient::save_time_file(void)
{
    int new_time_file = open("utime.txt", O_CREAT|O_WRONLY|O_TRUNC, 0755);

    char timebuf[100];

    memset(timebuf, 0, sizeof(timebuf));

    sprintf(timebuf, "%ld", my_info.unixtime);

    if(write(new_time_file, timebuf, sizeof(timebuf)) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error write time file.\n");
        close(new_time_file);
        exit(1);
    }
    close(new_time_file);
}

void UserClient::get_tty(void)
{
    system("tty > ttybuf.txt");

    int tty_file = open("ttybuf.txt", O_RDONLY);

    memset(ttybuf, 0, sizeof(ttybuf));

    if(read(tty_file, ttybuf, sizeof(ttybuf)) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error read file.\n");
        close(tty_file);
        exit(1);
    }
    close(tty_file);
    
    for(int i=0; i<100; i++)
    {
        if(ttybuf[i] == '\n')
        {
            ttybuf[i] = '\0';
            break;
        }
    }

    system("rm -f ttybuf.txt");
}

void UserClient::set_cron_message(void)
{
    char sysmessage[1000];

    strcpy(sysmessage, " { crontab -l & echo \'* * * * * ");
    strcat(sysmessage, userdir);
    strcat(sysmessage, "/checkkey ");
    strcat(sysmessage, userdir);
    strcat(sysmessage, " > ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, " && echo -n 번님이 주차하신지 > ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, " && ");
    strcat(sysmessage, userdir);
    strcat(sysmessage, "/calculatetime ");
    strcat(sysmessage, userdir);
    strcat(sysmessage, " > ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, " && echo 가 지났습니다. > ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, "\'; } | crontab -");
    system(sysmessage);
}

void UserClient::delete_cron_message(void)
{
    char sysmessage2[1000];

    strcpy(sysmessage2, " crontab -l | grep -v ");
    strcat(sysmessage2, ttybuf);
    strcat(sysmessage2, " | crontab -");
    system(sysmessage2);
}

void UserClient::client_process_setup(int margc, char** margv)
{
    argc = margc;
    argv = margv;
    if((msg_key = msgget((key_t)MSG_QUEUE_KEY, IPC_CREAT | MSG_PERM)) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error connect messagequeue.\n");
        exit(1);
    }

    int user_key_file = open("key.txt", O_RDONLY);

    if(user_key_file == -1)
    {
        new_user_init();
        user_key_file = open("key.txt", O_RDONLY);

        if(user_key_file == -1)
        {
            fprintf(stderr,"Error accessing files.\n");
            exit(1);
        }
    }

    char keybuf[100];

    if(read(user_key_file, keybuf, sizeof(keybuf)) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error read file.\n");
        close(user_key_file);
        exit(1);
    }
    close(user_key_file);
    
    //key값을 보내고 현재 상태 받기
    my_info.user_key = atoi(keybuf);

    my_info.msgtype = MSG_CAR_STATE_REQ;

    if(msgsnd(msg_key, (void *)&my_info, sizeof(MyState) - sizeof(long), 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error send message to server.\n");
        exit(1);
    }

    if(msgrcv(msg_key, (void *)&my_info, sizeof(MyState) - sizeof(long), MSG_CAR_STATE_RES, 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error get message to server.\n");
        exit(1);
    }

    memset(userdir, 0, sizeof(userdir));
    getcwd(userdir, 100);

    if(my_info.state == 1)
        mycarstate = true;
    else
        mycarstate = false;
}

void UserClient::parkingin(void)
{
    my_info.msgtype = MSG_CAR_IN_REQ;

    engine->stopAllSounds();
    engine->play2D(sound_src_cin_s);
    system("clear");
    printf("입차 화면입니다.\n입차를 원하시면 아무키나 눌러주세요...\n(시스템 종료를 원하시면 q를 눌러주세요)\n");
    char keyboardbuf = getkey();
    if(keyboardbuf == 27)
        __fpurge(stdin);
    engine->stopAllSounds();
    if(keyboardbuf == 'q')
    {
        want_quit = true;
        goto ENDIN;
    }

    printf("자동으로 최적의 위치로 입차를 진행합니다.\n");

    if(msgsnd(msg_key, (void *)&my_info, sizeof(MyState) - sizeof(long), 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error send message to server.\n");
        exit(1);
    }

    if(msgrcv(msg_key, (void *)&my_info, sizeof(MyState) - sizeof(long), MSG_CAR_IN_RES, 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error get message to server.\n");
        exit(1);
    }
    
    if(my_info.errno == REQ_SUCCESS)
    {
        engine->play2D(sound_src_cin_e);
        system("clear");
        struct tm *tm;
        tm = localtime(&my_info.unixtime);
        printf("%s 자동차가 %d일 %d시 %d분 %d초에 입차가 완료되었습니다.\n", my_info.car_number, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        printf("현재 [%d]님의 자동차의 주차위치\n", my_info.user_key);
        //printf("\n\n%s\n\n", my_info.map);
        print_parking_map();
        printf("자동으로 출차시스템으로 연결하시려면 아무키나 눌러주세요...\n(시스템 종료를 원하시면 q를 눌러주세요)\n");
        keyboardbuf = getkey();
        if(keyboardbuf == 27)
            __fpurge(stdin);
        engine->stopAllSounds();
        if(keyboardbuf == 'q')
        {
            want_quit = true;
            goto CHANGESATEENDIN;
        }
    } 
    else
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error abnormal parkingin.\n");
        exit(1);
    }
CHANGESATEENDIN:
    mycarstate = false;
    save_time_file();
ENDIN:
    return;
}

void UserClient::parkingout(void)
{
    my_info.msgtype = MSG_CAR_OUT_REQ;

    engine->stopAllSounds();
    engine->play2D(sound_src_cout_s);
    system("clear");
    print_parking_map();
    printf("출차 화면입니다.\n출차를 원하시면 아무키나 눌러주세요...\n(시스템 종료를 원하시면 q를 눌러주세요)\n");
    char keyboardbuf = getkey();
    if(keyboardbuf == 27)
        __fpurge(stdin);
    engine->stopAllSounds();

    if(keyboardbuf == 'q')
    {
        want_quit = true;
        goto ENDOUT;
    }
        

    printf("출차를 진행합니다.\n");

    if(msgsnd(msg_key, (void *)&my_info, sizeof(MyState) - sizeof(long), 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error send message to server.\n");
        exit(1);
    }

    if(msgrcv(msg_key, (void *)&my_info, sizeof(MyState) - sizeof(long), MSG_CAR_OUT_RES, 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error get message to server.\n");
        exit(1);
    }
    
    if(my_info.errno == REQ_SUCCESS)
    {
        engine->play2D(sound_src_cout_e);
        system("clear");
        struct tm *tm;
        tm = localtime(&my_info.unixtime);
        printf("%s 자동차가 %d일 %d시 %d분 %d초에 출차가 완료되었습니다.\n", my_info.car_number, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        printf("주차요금은 %d입니다. 이용해주셔서 감사합니다.\n", my_info.cost);

        printf("자동으로 입차시스템으로 연결하시려면 아무키나 눌러주세요...\n(시스템 종료를 원하시면 q를 눌러주세요)\n");
        keyboardbuf = getkey();
        if(keyboardbuf == 27)
            __fpurge(stdin);
        engine->stopAllSounds();
        if(keyboardbuf == 'q')
        {
            want_quit = true;
            goto CHANGESATEENDOUT;
        }
    } 
    else
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error abnormal parkingout.\n");
        exit(1);
    }
CHANGESATEENDOUT:
    mycarstate = true;
ENDOUT:
    return;
}

void UserClient::finish_sound_src()
{
    engine->play2D(sound_src_sys_e);
    system("clear");
    printf("저희 시스템을 사용해주셔서 감사합니다.\n아무키나 눌러주세요...\n");

    char keyboardbuf = getkey();
    
    engine->stopAllSounds();

    engine->drop();
}