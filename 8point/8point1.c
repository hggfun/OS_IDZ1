#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_FILE "/tmp/myfifo"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *file1;
FILE *file2;

int main(int argc, char **argv)
{
    if (argc == 3) {
        file1 = fopen(*(argv+1), "r");
        file2 = fopen(*(argv+2), "w");
    } else {
        printf("Недостаточно данных в аргументах командной строки");
        return 0;
    }
    int fd; // файловый дескриптор для канала
    pid_t pid1;
    char buf[5000];
    mkfifo(FIFO_FILE, 0666);
    // Создание неименованного канала

    // Создание первого дочернего процесса
    pid1 = fork();

    if (pid1 == -1) {
        printf("Ошибка создания первого дочернего процесса");
        return 0;
    }

    if (pid1 == 0) { // код для первого дочернего процесса
        int size = 5000;
        char message[5000];
        fgets(message, size, file1);
        fd = open(FIFO_FILE, O_WRONLY);
        if (fd == -1) {
            printf("Ошибка открытия именованного канала для записи");
            return 0;
        }
        write(fd, message, sizeof(message));
        close(fd);
    } else { // код для родительского процесса
            pid1 = fork();
            if (pid1 == -1) {
                printf("Ошибка создания третьего дочернего процесса");
                return 0;
            }
            if (pid1 == 0) {
                char output[5000];
                fd = open(FIFO_FILE, O_RDONLY);
                if (fd == -1) {
                    printf("Ошибка открытия именованного канала для чтения");
                    return 0;
                }
                read (fd, output, sizeof(output));
                fprintf(file2, "%s", output);
                close (fd);
            }
    }
    printf("finished process 1");
    return 0;
}
