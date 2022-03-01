#include "userclient.h"

//global varriable
key_t msg_key;

UserClient::UserClient()
{
    // Constructor
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
    printf("처음 오신것을 환영합니다. 신규 주차관리 유저 등록화면입니다\n");
    printf("이름을 입력해 주세요 : ");
    scanf("%s", &basic_info.name);
    printf("자동차 번호를 입력해 주세요 : ");
    scanf("%s", &basic_info.car_number);
    printf("전화 번호를 주세요 : ");
    scanf("%s", &basic_info.phone_number);
 
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

    if(msgsnd(msg_key, (void *)&basic_info, sizeof(Register), 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error send message to server.\n");
        exit(1);
    }
    
    if(msgrcv(msg_key, (void *)&my_info, sizeof(MyState), MSG_REGISTER_RES, 0) == -1)
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
        printf("%s[%d]님 가입을 환영합니다.\n", basic_info.name, my_info.user_key);
    }

    int new_user_file = open("key.txt", O_CREAT|O_WRONLY);

    char keybuf[100];

    sprintf(keybuf, "%d", my_info.user_key);

    if(write(new_user_file, keybuf, sizeof(keybuf)) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error write file.\n");
        close(new_user_file);
        exit(1);
    }
    close(new_user_file);
}

void UserClient::print_parking_map(void)
{
    printf("\n\n주차장 사진\n\n");
}

void UserClient::save_time_file(void)
{
    int new_time_file = open("key.txt", O_CREAT|O_WRONLY|O_TRUNC);

    char timebuf[100];

    sprintf(timebuf, "%d", my_info.unixtime);

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

    if(read(tty_file, ttybuf, sizeof(ttybuf)) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error read file.\n");
        close(tty_file);
        exit(1);
    }
    close(tty_file);
    
    system("rm -f ttybuf.txt");
}

void UserClient::set_cron_message(void)
{
    save_time_file();
    system("g++ checkkey.cpp userclient.o -o checkky");
    system("g++ calculatetime.cpp userclient.o -o calculatetime");

    char sysmessage[1000];

    strcat(sysmessage, " { crontab -l & echo \'* * * * * ./checkkey > ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, " && echo -n 님이 주차하신지 > ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, " && ./calculatetime > ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, " && echo 분 지났습니다. > ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, "\'; } | crontab -");
    //system(" { crontab -l & echo \'* * * * * ./checkkey > /dev/pts/0 && echo -n 님이 주차하신지 > /dev/pts/0 && ./calculatetime > /dev/pts/0 && echo 분 지났습니다. > /dev/pts/0\'; } | crontab -");
}

void UserClient::delete_cron_message(void)
{
    char sysmessage[1000];

    strcat(sysmessage, " crontab -l | grep -v ");
    strcat(sysmessage, ttybuf);
    strcat(sysmessage, " | crontab -");
    //system(" crontab -l | grep -v '/dev/pts/0' | crontab -");
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

    if(msgsnd(msg_key, (void *)&my_info, sizeof(MyState), 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error send message to server.\n");
        exit(1);
    }

    if(msgrcv(msg_key, (void *)&my_info, sizeof(MyState), MSG_CAR_STATE_RES, 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error get message to server.\n");
        exit(1);
    }

    if(my_info.state == 1)
        mycarstate = true;
    else
        mycarstate = false;
}

void UserClient::parkingin(void)
{
    my_info.msgtype = MSG_CAR_IN_REQ;
    printf("입차 화면입니다 입차를 원하시면 아무키나 눌러주세요... (시스템 종료를 원하시면 q를 눌러주세요)\n");
    
    char keyboardbuf = getkey();

    if(keyboardbuf == 'q' || keyboardbuf == 'Q')
        return;

    printf("자동으로 최적의 위치로 입차를 진행합니다.\n");

    if(msgsnd(msg_key, (void *)&my_info, sizeof(MyState), 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error send message to server.\n");
        exit(1);
    }

    if(msgrcv(msg_key, (void *)&my_info, sizeof(MyState), MSG_CAR_IN_RES, 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error get message to server.\n");
        exit(1);
    }
    
    if(my_info.errno == REQ_SUCCESS)
    {
        printf("%s 자동차가 %ld에 입차가 완료되었습니다.\n", my_info.car_number, my_info.unixtime);
        set_cron_message();
    } 
    else
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error abnormal parkingin.\n");
        exit(1);
    }

    mycarstate = false;
}

void UserClient::parkingout(void)
{
    my_info.msgtype = MSG_CAR_OUT_REQ;

    print_parking_map();

    printf("출차 화면입니다 출차를 원하시면 아무키나 눌러주세요... (시스템 종료를 원하시면 q를 눌러주세요)\n");
    char keyboardbuf = getkey();

    if(keyboardbuf == 'q' || keyboardbuf == 'Q')
        return;

    printf("출차를 진행합니다.\n");

    if(msgsnd(msg_key, (void *)&my_info, sizeof(MyState), 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error send message to server.\n");
        exit(1);
    }

    if(msgrcv(msg_key, (void *)&my_info, sizeof(MyState), MSG_CAR_OUT_RES, 0) == -1)
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error get message to server.\n");
        exit(1);
    }
    
    if(my_info.errno == REQ_SUCCESS)
    {
        printf("%s 자동차가 %ld에 출차가 완료되었습니다.\n", my_info.car_number, my_info.unixtime);
    } 
    else
    {
        printf("프로그램 실행 에러 시스템을 종료합니다.\n");
        fprintf(stderr,"Error abnormal parkingout.\n");
        exit(1);
    }

    mycarstate = true;
}