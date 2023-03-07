#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_FILE "/tmp/myfifo"


int main()
{
    int fd; // файловый дескриптор для канала
    pid_t pid2;
    char buf[5000];
        pid2 = fork();
        if (pid2 == -1) {
            printf("Ошибка создания второго дочернего процесса");
            return 0;
        }
        if (pid2 == 0) { // код для второго дочернего процесса
            fd = open(FIFO_FILE, O_RDONLY);
            if (fd == -1) {
                printf("Ошибка открытия именованного канала для чтения");
                return 0;
            }
            read (fd, buf, sizeof(buf));
            int sum = 0;
            for (int i = 0; buf[i] != '\0'; i++) {
                if (buf[i] >= '0' && buf[i] <= '9') {
                    sum += buf[i] - '0';
                }
            }
            char res[5000];
            sprintf(res, "%d", sum);
            close(fd);
            fd = open(FIFO_FILE, O_WRONLY);
            write(fd, res, sizeof(res));
            close(fd);
    }
        printf("finished process 2");
    return 0;
}
