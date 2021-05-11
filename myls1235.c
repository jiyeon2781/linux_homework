#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define MAX_NUM 1024

typedef struct linfo
{
    char mode[11];     //유형 및 권한
    int hardlink;      //하드링크 계수
    char userID[100];  //사용자 ID
    char groupID[100]; //그룸 ID
    int bytes;         //크기
    int month;
    int day;
    int hour;
    int minute;
    char times[30];
    char allfile[1024]; //모든 파일
} linfomation;

typedef struct info
{
    int inode;          //i옵션
    int block;          //s옵션
    char allfile[1024]; //a 옵션
    char type;          //F옵션
} infomation;

int opta, opti, opts, optF, optl, optR, optr;

opta = 0;
opti = 0;
opts = 0;
optF = 0;
optl = 0;
optR = 0;
optr = 0;

int count = 0;
linfomation linfo[MAX_NUM];
infomation info[MAX_NUM];
int sum = 0;

int main(int argc, char *argv[])
{
    int opt;
    char buffer[1024];
    ssize_t nread;
    struct stat finfo;
    mode_t modes;
    char ans[100];
    DIR *dirp;
    struct dirent *dentry;
    int flag = 0;
    int filecount = 0;

    if (argc == 1)
    {

        if ((dirp = opendir(".")) == NULL)
            exit(1);
        while (dentry = readdir(dirp))
        {
            if ((strcmp(dentry->d_name, ".") == 0) || (strcmp(dentry->d_name, "..") == 0))
            {
                continue;
            }
            printf("%-10s", dentry->d_name);
        }
        printf("\n");
    }
    else
    {
        while ((opt = getopt(argc, argv, "aisrFlR")) != -1)
        {
            switch (opt)
            {
            case 'a':
                opta = 1;
                break;
            case 'i':
                opti = 1;
                break;
            case 's':
                opts = 1;
                break;
            case 'F':
                optF = 1;
                break;
            case 'l':
                optl = 1;
                break;
            case 'R':
                optR = 1;
                break;
            case 'r':
                optr = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-F] [-l] [-R] [-r] filename \n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] != '-')
            { //파일이 존재할 때
                filecount++;
                continue;
            }
            else
            {
                if (i == argc - 1)
                {
                    if (filecount == 0)
                    {
                        flag = 1; //파일이 존재하지 않을 때
                    }
                }
                continue;
            }
        }
        if (flag == 1)
        {

            if (optR == 1)
            {
                recursive(".");
            }
            listsave(".");
            output();
        }

        if (filecount >= 1)
        {
            for(int i = 1; i< argc;i++){
                if (access(argv[i], F_OK) == -1)
                {
                    filecount--;
                    continue;
                }
            }
            for (int i = 1; i < argc; i++)
            {
                if (argv[i][0] == '-')
                {
                    continue;
                }
                if (access(argv[i], F_OK) == -1)
                {
                    filecount--;
                    printf("%s에 접근할 수 없습니다. 그런 파일이나 디렉토리가 없습니다.\n", argv[i]);
                    continue;
                }
                stat(argv[i], &finfo);
                modes = finfo.st_mode;
                if (S_ISDIR(modes))
                {
                    if (access(argv[i], X_OK) != -1)
                    {
                        listsave(argv[i]);
                        if (opta == 0 && opti == 0 && opts == 0 && optF == 0 && optl == 0 && optR == 0 && optr == 0)
                        {
                            if(filecount == 1){

                            for(int j = 0; j< count ;j++){
                                
                                if(info[j].allfile[0] == '.'){
                                    continue;
                                }
                                printf("%-15s",info[j].allfile);
                            }
                            printf("\n");
                            }
                            else {
                                printf("%s :\n",argv[i]);
                                for(int j = 0; j< count ;j++){
                                
                                if(info[j].allfile[0] == '.'){
                                    continue;
                                }
                                printf("%-15s",info[j].allfile);
                               
                            }
                             printf("\n\n");
                            }
                            
                            continue;
                        }
                        
                        output();
                        printf("\n");
                    }
                    else
                    {
                        printf("%s 디렉터리를 열 수 있는 권한이 없습니다.\n", argv[i]);
                        continue;
                    }

                    if (optR == 1)
                    {
                        recursive(argv[i]);
                        continue;
                    }
                }
                else
                {
                    if ((opti == 1) || (opts == 1) || (optF == 1) || (optl == 1) || (opta == 1))
                    {
                        listsave(".");
                        for (int j = 0; j < count; j++)
                        {
                            if (strcmp(argv[i], info[j].allfile) == 0)
                            {
                                int num = j;
                                if (opta == 1)
                                {
                                    if (opti == 1)
                                    {

                                        printf("%-10d", info[num].inode);
                                    }
                                    if (opts == 1)
                                    {
                                        printf("%-5d", info[num].block);
                                    }
                                    if (optl == 1)
                                    {
                                        printf("%-10s", linfo[num].mode);
                                        printf("%4d", linfo[num].hardlink);
                                        printf("%10s", linfo[num].userID);
                                        printf("%10s", linfo[num].groupID);
                                        printf("%10d", linfo[num].bytes);
                                        printf("   %d월 %d  %2d:%2d ", linfo[num].month, linfo[num].day, linfo[num].hour, linfo[num].minute);
                                    }
                                    printf(" %s", info[num].allfile);
                                    if (optF == 1)
                                    {
                                        printf("%-1c", info[num].type);
                                    }
                                    printf("\n");
                                }
                                else if (opta == 0)
                                {
                                    if (info[num].allfile[0] == '.')
                                    {
                                        num++;
                                        continue;
                                    }
                                    if (opti == 1)
                                    {
                                        printf("%-10d", info[num].inode);
                                    }
                                    if (opts == 1)
                                    {
                                        printf("%-5d", info[num].block);
                                    }
                                    if (optl == 1)
                                    {
                                        printf("%-10s", linfo[num].mode);
                                        printf("%4d", linfo[num].hardlink);
                                        printf("%10s", linfo[num].userID);
                                        printf("%10s", linfo[num].groupID);
                                        printf("%10d", linfo[num].bytes);
                                        printf("   %d월 %d  %2d:%2d ", linfo[num].month, linfo[num].day, linfo[num].hour, linfo[num].minute);
                                    }
                                    printf(" %s", info[num].allfile);
                                    if (optF == 1)
                                    {
                                        printf("%-1c", info[num].type);
                                    }
                                    printf("\n");
                                }
                            }
                        }
                    }
                    else
                    {
                        printf("%s\n", argv[i]);
                    }
                }
            }
        }
    }
}

void listsave(char filename[]) //출력할 정보 저장
{
    struct stat finfo;
    ino_t inode;
    mode_t modes;
    DIR *dirp;
    struct dirent *dentry;
    struct tm *lt;
    time_t time;
    struct group *group;
    struct passwd *pwd;

    dirp = opendir(filename);
    chdir(filename);
    count = 0;
    sum = 0;
    while (dentry = readdir(dirp))
    {
        strcpy(linfo[count].allfile, dentry->d_name);
        strcpy(info[count].allfile, dentry->d_name);
        count++;
    }

    if (opti == 1) //숨김파일, 링크까지 저장됨
    {
        for (int i = 0; i < count; i++)
        {
            stat(info[i].allfile, &finfo);
            inode = finfo.st_ino;
            info[i].inode = inode;
        }
    }
    if (opts == 1 || optl == 1) //숨김파일 , 링크까지 저장됨
    {
        for (int i = 0; i < count; i++)
        {
            stat(info[i].allfile, &finfo);
            modes = finfo.st_mode;
            if (S_ISLNK(modes))
            {
                info[i].block = 0;
                continue;
            }
            info[i].block = (finfo.st_blocks / 2);
            sum += info[i].block;
        }
    }
    if (optF == 1)
    {
        for (int i = 0; i < count; i++)
        {
            stat(info[i].allfile, &finfo);
            modes = finfo.st_mode;
            if (S_ISDIR(modes)) //디렉터리
                info[i].type = '/';
            else if (S_ISLNK(modes)) //심볼릭 링크
                info[i].type = '@';
            else if (S_ISSOCK(modes)) //소켓
                info[i].type = '=';
            else if ((access(info[i].allfile, X_OK) == 0)) //실행 가능 파일
                info[i].type = '*';
            else
                info[i].type = '\0';
        }
    }
    if (optl == 1)
    {
        for (int i = 0; i < count; i++) //권한 설정
        {
            stat(linfo[i].allfile, &finfo);
            modes = finfo.st_mode;
            if (S_ISDIR(modes))
                linfo[i].mode[0] = 'd';
            else if (S_ISLNK(modes))
                linfo[i].mode[0] = 'l';
            else if (S_ISSOCK(modes))
                linfo[i].mode[0] = 's';
            else if (S_ISBLK(modes))
                linfo[i].mode[0] = 'b';
            else if (S_ISCHR(modes))
                linfo[i].mode[0] = 'c';
            else if (S_ISREG(modes))
                linfo[i].mode[0] = '-';

            if ((modes & S_IRUSR) == S_IRUSR)
                linfo[i].mode[1] = 'r';
            else
                linfo[i].mode[1] = '-';

            if ((modes & S_IWUSR) == S_IWUSR)
                linfo[i].mode[2] = 'w';
            else
                linfo[i].mode[2] = '-';

            if ((modes & S_IXUSR) == S_IXUSR)
                linfo[i].mode[3] = 'x';
            else
                linfo[i].mode[3] = '-';

            if ((modes & S_IRGRP) == S_IRGRP)
                linfo[i].mode[4] = 'r';
            else
                linfo[i].mode[4] = '-';

            if ((modes & S_IWGRP) == S_IWGRP)
                linfo[i].mode[5] = 'w';
            else
                linfo[i].mode[5] = '-';

            if ((modes & S_IXGRP) == S_IXGRP)
                linfo[i].mode[6] = 'x';
            else
                linfo[i].mode[6] = '-';

            if ((modes & S_IROTH) == S_IROTH)
                linfo[i].mode[7] = 'r';
            else
                linfo[i].mode[7] = '-';

            if ((modes & S_IWOTH) == S_IWOTH)
                linfo[i].mode[8] = 'w';
            else
                linfo[i].mode[8] = '-';

            if ((modes & S_IXOTH) == S_IXOTH)
                linfo[i].mode[9] = 'x';
            else
                linfo[i].mode[9] = '-';
            linfo[i].hardlink = finfo.st_nlink; //하드링크 계수
            pwd = getpwuid(finfo.st_uid);       //사용자 ID
            group = getgrgid(finfo.st_gid);     //그룹 ID
            strcpy(linfo[i].userID, pwd->pw_name);
            strcpy(linfo[i].groupID, group->gr_name);
            linfo[i].bytes = finfo.st_size;  //바이트 크기
            lt = localtime(&finfo.st_atime); //시간 월 일 분 초
            linfo[i].month = lt->tm_mon + 1;
            linfo[i].day = lt->tm_mday;
            linfo[i].hour = lt->tm_hour;
            linfo[i].minute = lt->tm_min;
        }
    }
    chdir("..");
}

void output() //출력
{
    int num = 0;
    linfomation ltemp;
    infomation temp;
    if (optr == 1)
    {
        for (int i = 0; i < count - 1; i++)
        {
            for (int j = 1; j < count - i; j++)
            {
                if (strcmp(linfo[j].allfile, linfo[j - 1].allfile) > 0)
                {
                    ltemp = linfo[j - 1];
                    linfo[j - 1] = linfo[j];
                    linfo[j] = ltemp;
                }
                if (strcmp(info[j].allfile, info[j - 1].allfile) > 0)
                {
                    temp = info[j - 1];
                    info[j - 1] = info[j];
                    info[j] = temp;
                }
            }
        }
        if ((opti == 0) && (opts == 0) && (optF == 0) && (optl == 0) && (opta == 0))
        {
            for (int i = 0; i < count; i++)
            {
                if (info[i].allfile[0] == '.')
                {
                    continue;
                }
                printf("%-10s", info[i].allfile);
            }
            printf("\n");
            return;
        }
    }

    while (num < count)
    {

        if (opta == 0)
        {
            if (info[num].allfile[0] == '.')
            {
                sum -= info[num].block;
                num++;
                continue;
            }
            num++;
            continue;
        }
        num++;
    }
    num = 0;
    if ((opts == 1) || (optl == 1))
    {
        printf("합계 : %d\n", sum);
    }

    while (num < count)
    {
        if (opta == 1)
        {
            if (opti == 1)
            {

                printf("%-10d", info[num].inode);
            }
            if (opts == 1)
            {
                printf("%-5d", info[num].block);
            }
            if (optl == 1)
            {
                printf("%-10s", linfo[num].mode);
                printf("%4d", linfo[num].hardlink);
                printf("%10s", linfo[num].userID);
                printf("%10s", linfo[num].groupID);
                printf("%10d", linfo[num].bytes);
                printf("   %d월 %d  %2d:%2d ", linfo[num].month, linfo[num].day, linfo[num].hour, linfo[num].minute);
            }
            printf(" %s", info[num].allfile);
            if (optF == 1)
            {
                printf("%-1c", info[num].type);
            }
            printf("\n");
        }
        else if (opta == 0)
        {
            if (info[num].allfile[0] == '.')
            {
                num++;
                continue;
            }
            if (opti == 1)
            {
                printf("%-10d", info[num].inode);
            }
            if (opts == 1)
            {
                printf("%-5d", info[num].block);
            }
            if (optl == 1)
            {
                printf("%-10s", linfo[num].mode);
                printf("%4d", linfo[num].hardlink);
                printf("%10s", linfo[num].userID);
                printf("%10s", linfo[num].groupID);
                printf("%10d", linfo[num].bytes);
                printf("   %d월 %d  %2d:%2d ", linfo[num].month, linfo[num].day, linfo[num].hour, linfo[num].minute);
            }
            printf(" %s", info[num].allfile);
            if (optF == 1)
            {
                printf("%-1c", info[num].type);
            }
            printf("\n");
        }
        num++;
    }
}

void recursive(char filename[])
{
    DIR *dirp;
    struct dirent *dentry;
    struct stat finfo;
    mode_t modes;
    char ans[512];
    char *nowDir[1024];
    char *name;
    char *file[1024];
    int i = 0;
    dirp = opendir(filename);
    chdir(filename);
    printf("<%s>\n", filename);
    while (dentry = readdir(dirp))
    {

        if (dentry->d_name[0] == '.') //부모링크와 현재 링크는 제외
        {
            if (strcmp(dentry->d_name, "..") == 0)
            {
            }
            continue;
        }
        file[i] = dentry->d_name;
        i++;
    }

    for (int j = 0; j < i; j++)
    {

        printf("%-10s", file[j]);
    }
    printf("\n\n");

    for (int j = 0; j < i; j++)
    {
        stat(file[j], &finfo);
        modes = finfo.st_mode;
        if (S_ISDIR(modes))
        {
            recursive(file[j]);
        }
    }
    close(filename);
    chdir("..");

    return 0;
}
