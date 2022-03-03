#include "userclient.h"

//global varriable
key_t msg_key;
  
UserClient::UserClient()
{
    // Constructor

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

void UserClient::new_user_init(void)
{
USERINIT:
    engine->play2D(sound_src_regi_s);
    system("clear");
    printf("처음 오신것을 환영합니다. 신규 주차관리 유저 등록화면입니다\n");
    printf("이름을 입력해 주세요 : ");
    scanf("%s", basic_info.name);
    printf("자동차 번호를 입력해 주세요 : ");
    scanf("%s", basic_info.car_number);
    printf("전화 번호를 주세요 : ");
    scanf("%s", basic_info.phone_number);
 
    int numberbuf;
    printf("입주민 여부를 입력해주세요(맞으시면 1, 틀리시면 0) : ");
    scanf("%d", &numberbuf);
    if(numberbuf != 0 && numberbuf != 1)
    {
        goto RETRYGETNUMBER;
    }
    else
    {
        basic_info.is_resident = numberbuf;
        goto SIGNUP;
    }

RETRYGETNUMBER:
    printf("잘못 입력하셨습니다. 입주민 여부를 입력해주세요(맞으시면 1, 틀리시면 0) : ");
    scanf("%d", &numberbuf);
    if(numberbuf != 0 && numberbuf != 1)
    {
        goto RETRYGETNUMBER;
    }
    else
    {
        basic_info.is_resident = numberbuf;
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
    getchar();
    printf("계속하시려면 아무키나 눌러주세요.\n");
    char keyboardbuf = getkey();
    if(keyboardbuf == 27)
        __fpurge(stdin);
    engine->stopAllSounds();
}

void drawPoints()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(-0.5, 0.0);
	glVertex2f(0.5, 0.0);
	glVertex2f(0.0, 1.0);
	glEnd();
	glFlush();
}

pid_t pid;

void keyboard(unsigned char key, int x, int y)
{
	switch(key){
		case 'q':
			exit(pid);
			break;
	}
	glutPostRedisplay();
}

void UserClient::print_parking_map(void)
{
    printf("\n\n%s\n\n", my_info.map);
    /*
    pid = fork();
    if(pid == 0)
    {
	    printf("먼저 GL화면을 종료하시려면 q를 입력해주세요\n");
    	int mode = GLUT_RGB | GLUT_SINGLE;
    	int argc = 1;
    	char **argv;
    	glutInit(&argc, argv);
    	glutInitDisplayMode(mode);
    	glutInitWindowPosition(100,100);
    	glutInitWindowSize(400,400);
    	glutCreateWindow("OpenGL");
    	glutSetWindowTitle("exam");

    	glutDisplayFunc(drawPoints);
    	glutKeyboardFunc(keyboard);
    	glutMainLoop();
    }
    */
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

void UserClient::client_process_setup(void)
{
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
    printf("현재 [%d]님의 자동차의 주차위치\n", my_info.user_key);
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