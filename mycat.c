#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    int opt, file;
    int line = 0;
    int num = 1;
    int num2 = 1;
    char buffer[1024];
    ssize_t nread;
    struct stat finfo;
    mode_t modes;

    if (argc == 1)
    {
        while ((nread = read(0, buffer, 1024)) > 0)
        {
            write(1, buffer, nread);
        }
    }
    else if (argc > 1)
    {

        while ((opt = getopt(argc, argv, "n")) != -1)
        {
            switch (opt)
            {
            case 'n':
                line = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-n] filename \n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        if (argc == 2 && line == 1)
        {

            memset(buffer, 0, 512);
            while ((nread = read(0, buffer, 1024)) > 0)
            {

                printf("    %-3d %s", num2, buffer);
                num2++;
                memset(buffer, 0, 512);
            }
        }
        for (int i = 1; i < argc; i++)
        {

            if (strcmp("-n", argv[i]) == 0)
            { //n옵션을 만나면 넘어감
                continue;
            }
            if (access(argv[i], F_OK) == -1) //파일이 존재하지 않을 때
            {
                printf("%s", argv[i]);
                printf("그런 파일이 존재하지 않습니다.\n");
                continue;
            }
            else if (access(argv[i], R_OK) == -1)
            { //읽기 권한이 없으면
                printf("%s의 접근 권한이 없습니다.\n", argv[i]);
                close(file);
                continue;
            }
            else
            {
                file = open(argv[i], O_RDONLY);
                stat(argv[i], &finfo);
                modes = finfo.st_mode;
                if (!S_ISREG(modes))
                { //일반파일이 아니면
                    printf("일반 파일이 아닙니다.\n");
                    close(file);
                    continue;
                }
                else
                {
                    if (line == 1)
                    {
                        printf("\t%-3d", num);
                        file = open(argv[i], O_RDONLY);
                        while ((nread = read(file, buffer, 1)) > 0) //write와 printf에서 write가 우선순위가 printf보다 높음
                        {
                            if (buffer[0] == '\n')
                            {
                                num++;
                                printf("%c", buffer[0]);
                                printf("\t%-3d", num);
                            }
                            else
                            {
                                printf("%c", buffer[0]);
                            }
                        }
                        num++;
                        printf("\n");
                        close(file);
                    }
                    else if (line == 0)
                    {
                        file = open(argv[i], O_RDONLY);
                        while ((nread = read(file, buffer, 1024)) > 0)
                        {
                            if (write(1, buffer, nread) < nread)
                            {
                                printf("종료되었습니다.\n");
                                close(file);
                            }
                        }
                        close(file);
                    }
                }
            }
        }
    }
    return 0;
}