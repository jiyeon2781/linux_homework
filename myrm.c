#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    int opt;
    int opti, optd, optr;
    char buffer[1024];
    ssize_t nread;
    struct stat finfo;
    mode_t modes;
    char ans[100];
    DIR *dirp;
    struct dirent *dentry;
    char *name;
    char nowDir[1024];

    if (argc == 1)
    {
        printf("파일 인자가 필요합니다.\n");
        return 0;
    }
    else
    {
        opti = 0; //한번더 물어보는 옵션
        optd = 0; //디렉터리 일때, 비어있을 때만 삭제 & 일반파일 그냥 삭제
        optr = 0; //디렉터리 일때, 하위 디렉터리 파일까지 모두 삭제
        while ((opt = getopt(argc, argv, "idr:")) != -1)
        {
            switch (opt)
            {
            case 'i':
                opti = 1;
                break;
            case 'r':
                optr = 1;
                break;

            case 'd':
                optd = 1;

                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-r] [-d] filename \n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-') //옵션 인자가 나올때
            {
                continue;
            }
            else if (access(argv[i], F_OK) == -1) //파일이나 디렉터리 존재하지 않을 때
            {
                printf("%s 파일이나 디렉터리를 찾을 수 없습니다. \n", argv[i]);
            }
            else //존재할 때
            {
                stat(argv[i], &finfo);
                modes = finfo.st_mode;
                if (S_ISDIR(modes)) //디렉터리 일때
                {
                    if (optr == 1)
                    {
                        if (access(argv[i], X_OK) == -1)
                        {
                            printf("%s를 지울 수 있는 권한이 없습니다.\n", argv[i]);
                            continue;
                        }
                        if (opti == 0 && optd == 0){
                            recursive(argv[i]);
                            continue;
                        }
                        if (opti == 1)
                        {
                            printf("%s 디렉터리를 지우겠습니까?", argv[i]);
                            scanf("%s", &ans);
                            if (ans[0] == 'y')
                            {
                                if (rmdir(argv[i]) == 0)
                                {
                                    continue;
                                }
                                else
                                {
                                    recursive(argv[i]);
                                    continue;
                                }
                            }
                        }
                        
                    }
                    else if (optd == 1)
                        {
                            if (rmdir(argv[i]) == 0)
                            {
                                continue;
                            }
                            else
                            {
                                printf("비지 않은 디렉터리는 지울 수 없습니다.\n");
                                continue;
                            }
                        }
                        else {
                            printf("디렉터리는 지울 수 없습니다.\n");
                            continue;
                        }
                }
                else //다른 파일들일때
                {
                    if (opti == 0)
                    {
                        if (access(argv[i], W_OK) == -1) //w 권한이 없을 때
                        {
                            printf("쓰기 보호된 일반파일입니다. 지울까요? ");
                            scanf("%s", &ans);
                            if (ans[0] == 'y')
                            {

                                if (unlink(argv[i]) == -1)
                                {
                                    printf("%s파일을 삭제하는데 실패하였습니다.\n", argv[i]);
                                }
                            }
                        }
                        else
                        {
                            if (unlink(argv[i]) == -1)
                            {
                                printf("%s파일을 삭제하는데 실패하였습니다.\n", argv[i]);
                            }
                        }
                    }
                    else if (opti == 1)
                    {
                        printf("일반 파일 %s를 제거할까요? ", argv[i]);
                        scanf("%s", &ans);
                        if (ans[0] == 'y')
                        {
                            if (unlink(argv[i]) == -1)
                            {
                                printf("%s파일을 삭제하는데 실패하였습니다.\n", argv[i]);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void recursive(char *argv)
{
    DIR *dirp;
    struct dirent *dentry;
    struct stat finfo;
    mode_t modes;
    char ans[512];
    char *nowDir[1024];
    char *name;

    dirp = opendir(argv);
    chdir(argv);
    while (dentry = readdir(dirp))
    {
        if (dentry->d_name[0] == '.') //부모링크와 현재 링크는 제외
        {
            if (strcmp(dentry->d_name, "..") == 0)
            {
            }
            continue;
        }
        name = dentry->d_name;
        stat(dentry->d_name, &finfo);
        modes = finfo.st_mode;
        if (S_ISDIR(modes)) //디렉터리일때
        {
            int result = rmdir(dentry->d_name);
            if (result == 0)
            {
                continue;
            }
            else if (result == -1)
            {

                recursive(dentry->d_name);
            }
        }
        else if (S_ISREG(modes)) //일반파일 일때
        {
            if (access(dentry->d_name, W_OK) == -1)
            {
                printf("쓰기 보호된 일반파일입니다. 지울까요? ");
                scanf("%s", &ans);
                if (ans[0] == 'y')
                {
                    if (unlink(dentry->d_name) == -1)
                    {
                        printf("%s파일을 삭제하는데 실패하였습니다.\n", dentry->d_name);
                    }
                }
            }
            else
            {
                if (unlink(dentry->d_name) == -1)
                {
                    printf("%s파일을 삭제하는데 실패하였습니다.\n", dentry->d_name);
                }
            }
        }
    }

    chdir("..");
    rmdir(argv);
    return 0;
}
