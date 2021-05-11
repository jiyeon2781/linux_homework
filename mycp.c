#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    int file1, file2;
    char buffer[1024];
    ssize_t nread;
    struct stat finfo1, finfo2, finfodir;
    mode_t modes1, modes2, modesdir, usrModes;
    char nowDir[1024];

    if (argc == 1)
    { //파일 이름을 입력하지 않았을 때
        printf("파일 명령어가 누락되었습니다.\n");
        return 0;
    }
    else if (argc == 2)
    {
        //인자가 하나만 입력 되었을때
        printf("%s 다음에 대상 파일 명령이 누락되었습니다.\n", argv[1]);
        return 0;
    }
    else if (argc == 3)
    { //인자가 두개 들어갔을 때
        if (access(argv[1], F_OK) == -1)
        { //a에서 파일이 존재하지 않을 때
            printf("%s라는 파일이 존재하지 않습니다.\n", argv[1]);
            return 0;
        }
        else
        { //a 파일이 존재할 때
            stat(argv[1], &finfo1);
            modes1 = finfo1.st_mode;
            usrModes = ((modes1 & S_IRWXU) | (modes1 & S_IRWXG) | (modes1 & S_IRWXO));
            if (access(argv[1], R_OK) != -1) //a에 대한 읽기 권한이 있을 때
            {
                file1 = open(argv[1], O_RDONLY);        //파일이 존재하므로 열어준다.
                if (S_ISREG(modes1) || S_ISLNK(modes1)) //a가 일반 파일 또는 심볼릭 링크 일때
                {
                    if (access(argv[2], F_OK) != -1)
                    { //b파일이 존재할 때
                        stat(argv[2], &finfo2);
                        modes2 = finfo2.st_mode;

                        if (S_ISREG(modes2))
                        { //b가 일반파일일때
                            if (access(argv[2], W_OK != -1))
                            {
                                file2 = open(argv[2], O_RDWR | O_CREAT | O_TRUNC);
                                if (S_ISREG(modes1)) //일반파일 a
                                {
                                    while ((nread = read(file1, buffer, 1024)) > 0)
                                    {
                                        if (write(file2, buffer, nread) < nread)
                                        {
                                            printf("종료되었습니다.\n");
                                            close(file1);
                                            close(file2);
                                            exit(1);
                                        }
                                    }
                                }
                                else if (S_ISLNK(modes1)) //심볼릭 링크 a
                                {
                                    while ((nread = readlink(file1, buffer, 1024)) > 0)
                                    {
                                        if (write(file2, buffer, nread) < nread)
                                        {
                                            printf("종료되었습니다.\n");
                                            close(file1);
                                            close(file2);
                                            exit(1);
                                        }
                                    }
                                }
                                close(file1);
                                close(file2);
                            }
                            else
                            {
                                printf("%s 일반파일에서 쓰기 권한이 없습니다.\n", argv[2]);
                            }
                        }
                        else if (S_ISDIR(modes2))
                        { //b가 디렉터리일 때
                            if (access(argv[2], X_OK) != -1)
                            {
                                getcwd(nowDir, 1024);
                                chdir(argv[2]);
                                if (access(argv[1], F_OK) != -1) //b/a에서 a파일이 존재할 떄
                                {
                                    stat(argv[1], &finfodir);
                                    modesdir = finfodir.st_mode;
                                    usrModes = ((modesdir & S_IRWXU) | (modesdir & S_IRWXG) | (modesdir & S_IRWXO));
                                    if (S_ISDIR(modesdir)) //b/a에서 a파일이 디렉터리 일 때
                                    {
                                        printf("디렉터리 %s/%s 를 디렉터리가 아닌 것으로 복사할 수 없습니다.\n", argv[2], argv[1]);
                                        close(file1);
                                        return 0;
                                    }
                                    else if (S_ISREG(modesdir)) //b/a에서 a파일이 일반파일 일때
                                    {
                                        if (access(argv[1], W_OK) == -1)
                                        {
                                            printf("디렉터리 안 %s 파일의 권한이 없습니다\n", argv[1]);
                                            return 0;
                                        }
                                        else
                                        {
                                            file2 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC);
                                            if (S_ISREG(modes1)) //일반 파일 a
                                            {
                                                while ((nread = read(file1, buffer, 1024)) > 0)
                                                {
                                                    if (write(file2, buffer, nread) < nread)
                                                    {
                                                        printf("종료되었습니다.\n");
                                                        close(file1);
                                                        close(file2);
                                                        exit(1);
                                                    }
                                                }
                                            }
                                            else if (S_ISLNK(modes1)) // 심볼릭 링크 a
                                            {
                                                while ((nread = readlink(file1, buffer, 1024)) > 0)
                                                {
                                                    if (write(file2, buffer, nread) < nread)
                                                    {
                                                        printf("종료되었습니다.\n");
                                                        close(file1);
                                                        close(file2);
                                                        exit(1);
                                                    }
                                                }
                                            }

                                            close(file1);
                                            close(file2);
                                        }
                                    }
                                    else if (S_ISLNK(modesdir)) //b/a에서 a파일이 심볼릭 링크일 때
                                    {
                                        if (access(argv[1], W_OK) == -1)
                                        {
                                            printf("디렉터리 안 %s 파일의 권한이 없습니다\n", argv[1]);
                                            return 0;
                                        }
                                        else
                                        {
                                            file2 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC);
                                            if (S_ISREG(modes1)) //일반 파일 a
                                            {
                                                while ((nread = read(file1, buffer, 1024)) > 0)
                                                {
                                                    if (write(file2, buffer, nread) < nread)
                                                    {
                                                        printf("종료되었습니다.\n");
                                                        close(file1);
                                                        close(file2);
                                                        exit(1);
                                                    }
                                                }
                                            }
                                            else if (S_ISLNK(modes1)) //심볼릭 링크 a
                                            {
                                                while ((nread = readlink(file1, buffer, 1024)) > 0)
                                                {
                                                    if (write(file2, buffer, nread) < nread)
                                                    {
                                                        printf("종료되었습니다.\n");
                                                        close(file1);
                                                        close(file2);
                                                        exit(1);
                                                    }
                                                }
                                            }
                                        }

                                        close(file1);
                                        close(file2);
                                    }
                                }
                                else //b/a에서 a파일이 존재하지 않을 때
                                {

                                    file2 = open(argv[1], O_RDWR | O_CREAT, usrModes);
                                    if (S_ISREG(modes1)) //일반 파일 a
                                    {
                                        while ((nread = read(file1, buffer, 1024)) > 0)
                                        {
                                            if (write(file2, buffer, nread) < nread)
                                            {
                                                printf("종료되었습니다.\n");
                                                close(file1);
                                                close(file2);
                                                exit(1);
                                            }
                                        }
                                    }
                                    else if (S_ISLNK(modes1)) //심볼릭 링크 a
                                    {
                                        while ((nread = readlink(file1, buffer, 1024)) > 0)
                                        {
                                            if (write(file2, buffer, nread) < nread)
                                            {
                                                printf("종료되었습니다.\n");
                                                close(file1);
                                                close(file2);
                                                exit(1);
                                            }
                                        }
                                    }
                                    close(file1);
                                    close(file2);
                                }
                                chdir(nowDir);
                            }
                            else
                            {
                                printf("%s디렉터리의 실행권한이 없습니다.\n", argv[2]);
                            }
                        }
                        else if (S_ISLNK(modes2))
                        { //b가 심볼릭 링크 일때
                            if (access(argv[2], R_OK) != 1)
                            {
                                file2 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC);
                                if (S_ISREG(modes1)) //일반 파일 a
                                {
                                    while ((nread = read(file1, buffer, 1024)) > 0)
                                    {
                                        if (write(file2, buffer, nread) < nread)
                                        {
                                            printf("종료되었습니다.\n");
                                            close(file1);
                                            close(file2);
                                            exit(1);
                                        }
                                    }
                                }
                                else if (S_ISLNK(modes1)) //심볼릭 링크 a
                                {
                                    while ((nread = readlink(file1, buffer, 1024)) > 0)
                                    {
                                        if (write(file2, buffer, nread) < nread)
                                        {
                                            printf("종료되었습니다.\n");
                                            close(file1);
                                            close(file2);
                                            exit(1);
                                        }
                                    }
                                }

                                close(file1);
                                close(file2);
                            }
                            else
                            {
                                printf("%s 심볼릭 링크의 읽기 권한이 없습니다\n.", argv[2]);
                            }
                        }
                        else
                        { //세개 다 아닐 때
                            printf("%s 파일은 일반 파일이거나 디렉터리 파일, 심볼릭 링크만 가능합니다.\n", argv[2]);
                            close(file1);
                            return 0;
                        }
                    }
                    else
                    { //존재하지 않을 때
                        file2 = open(argv[2], O_RDWR | O_CREAT, usrModes);
                        while ((nread = read(file1, buffer, 1024)) > 0)
                        {
                            if (write(file2, buffer, nread) < nread)
                            {
                                printf("종료되었습니다.\n");
                                close(file1);
                                close(file2);
                                exit(1);
                            }
                        }
                        close(file1);
                        close(file2);
                    }
                }
                else if (S_ISDIR(modes1)) //a가 디렉터리일 때
                {
                    printf("디렉터리는 직접 복사할 수 없습니다. \n");
                    return 0;
                }
                else
                {
                    printf("%s는 일반파일이거나 심볼릭 링크여야 합니다. \n", argv[1]);
                    return 0;
                }
            }
            else
            { //a에 대한 읽기 권한이 없을 때
                printf("%s에 대한 권한이 없습니다\n", argv[1]);
                return 0;
            }
        }
    }

    else if (argc >= 4)
    {
        for (int i = 0; i < argc - 1; i++)
        {
            if (access(argv[i], F_OK | R_OK) == -1)
            {
                printf("%s의 파일이 존재하지 않거나 권한이 없습니다. \n", argv[i]);
                return 0;
            }
        }
        if (access(argv[argc - 1], F_OK | X_OK) == -1)
        {
            printf("%s의 파일이 존재하지 않거나 권한이 없습니다.\n", argv[argc - 1]);
            return 0;
        }
        stat(argv[argc - 1], &finfo2);
        modes2 = finfo2.st_mode;
        if (S_ISDIR(modes2)) //b가 디렉터리고 앞 인자들의 파일이 모두 존재할때
        {
            for (int i = 0; i < argc - 1; i++)
            {
                stat(argv[i], &finfo1);
                modes1 = finfo1.st_mode;
                file1 = open(argv[i], O_RDONLY);
                getcwd(nowDir, 1024);
                chdir(argv[argc - 1]);
                usrModes = ((modes1 & S_IRWXU) | (modes1 & S_IRWXG) | (modes1 & S_IRWXO));
                if (access(argv[i], F_OK) == -1)
                {
                    file2 = open(argv[i], O_RDWR | O_CREAT | O_TRUNC, usrModes);
                }
                else
                {
                    file2 = open(argv[i], O_RDWR | O_CREAT | O_TRUNC);
                }
                if (S_ISREG(modes1))
                {

                    while ((nread = read(file1, buffer, 1024)) > 0)
                    {
                        if (write(file2, buffer, nread) < nread)
                        {
                            printf("종료되었습니다.\n");
                            close(file1);
                            close(file2);
                            exit(1);
                        }
                    }
                    close(file1);
                    close(file2);
                }
                else if (S_ISLNK(modes1))
                {
                    file2 = open(argv[i], O_RDWR | O_CREAT | O_TRUNC);
                    while ((nread = readlink(file1, buffer, 1024)) > 0)
                    {
                        if (write(file2, buffer, nread) < nread)
                        {
                            printf("종료되었습니다.\n");
                            close(file1);
                            close(file2);
                            exit(1);
                        }
                    }
                    close(file1);
                    close(file2);
                }
                else
                {
                    printf("마지막을 제외한 인자는 일반파일이거나 심볼릭 링크여야 합니다.\n");
                    close(file1);
                    return 0;
                }
                chdir(nowDir);
            }
        }
        else
        {
            printf("%s는 디렉터리여야 합니다.\n", argv[argc - 1]);
            return 0;
        }
    }
    return 0;
}