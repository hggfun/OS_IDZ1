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
    int fd[2]; // файловый дескриптор для канала
    pid_t pid1, pid2, pid3;
    char buf[5000];

    // Создание неименованного канала
    if (pipe(fd) == -1) {
        printf("Ошибка создания канала");
        return 0;
    }

    // Создание первого дочернего процесса
    pid1 = fork();

    if (pid1 == -1) {
        printf("Ошибка создания первого дочернего процесса");
        return 0;
    }

    if (pid1 == 0) { // код для первого дочернего процесса
        close(fd[0]); // закрытие чтения из канала
        int size = 5000;
        char message[5000];
        fgets(message, size, file1);
        write(fd[1], message, sizeof(message));
        // запись в канал
        close(fd[1]); // закрытие записи в канал
    } else { // код для родительского процесса
        // Создание второго дочернего процесса
        pid2 = fork();
        if (pid2 == -1) {
            printf("Ошибка создания второго дочернего процесса");
            return 0;
        }
        if (pid2 == 0) { // код для второго дочернего процесса
            //close(fd[1]); // закрытие записи в канал
            // чтение из канала
            read(fd[0], buf, sizeof(buf));
            close(fd[0]);
            int sum = 0;
            for (int i = 0; buf[i] != '\0'; i++) {
                if (buf[i] >= '0' && buf[i] <= '9') {
                    sum += buf[i] - '0';
                }
            }
            char res[5000];
            sprintf(res, "%d", sum);
            write(fd[1], res, sizeof(buf));
            close(fd[1]); // закрытие чтения из канала
        } else { // код для родительского процесса
            pid3 = fork();
            if (pid3 == -1) {
                printf("Ошибка создания третьего дочернего процесса");
                return 0;
            }
            if (pid3 == 0) {
                //close (fd[1]);
                char output[5000];
                read(fd[0], output, sizeof(output));
                fprintf(file2, "%s", output);
                close (fd[0]);
            }
        }
    }

    return 0;
}
