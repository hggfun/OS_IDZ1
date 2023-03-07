Зиганшин Алим, БПИ216

Вариант 21

> Разработать программу, осуществляющую нахождение суммы
всех цифр в заданной ASCII-строке.
> 

Выполнил задания с 4 баллов по 8 баллов. Каждый подотдел представляет собой схему, краткий отчет, и исходный код с комментариями. Дополнительная информация (напримеры файлы тестов и файлы .с) находятся отдельно.

## Задание на 4

![photo_2023-03-07_22-15-48.jpg](https://github.com/hggfun/OS_IDZ1/blob/main/pictures/photo_2023-03-07_22-15-48.jpg)

Имеем три процесса (переменные pid_t). Изначально открываем файлы для ввода и вывода, далее создаем pipe (и сразу проверяем на корректность). Затем при помощи fork активируем по очереди три процесса.

```c
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
```

Файл с кодом ( .с ) и файлы с тестами ввода/вывода лежат в папке 4 point

## Задание на 5

![photo_2023-03-07_22-16-02.jpg](https://github.com/hggfun/OS_IDZ1/blob/main/pictures/photo_2023-03-07_22-16-02.jpg)

В данном случае схема похожа, но поскольку fifo именованный, она будет иметь конкретное место хранения потока данных. Код также похож на предыдущий, но теперь требуется создание fifo. Я использовал mkfifo (параметр 0666 показывает, что все имеют право записывать и считывать). В остальном процессы крайне схожи, только дескриптор теперь один и при обращении к потоку указываем каждый раз, мы собираемся читать или писать. Также заранее определяем расположение файла - myfifo.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_FILE "/tmp/myfifo"

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
    pid_t pid1, pid2, pid3;
    char buf[5000];
    mkfifo(FIFO_FILE, 0666);
    // Создание именованного канала

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
    } else {
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
        } else { // код для третьего дочернего процесса
            pid3 = fork();
            if (pid3 == -1) {
                printf("Ошибка создания третьего дочернего процесса");
                return 0;
            }
            if (pid3 == 0) {
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
    }

    return 0;
}
```

## Задание на 6

![photo_2023-03-07_22-16-08.jpg](https://github.com/hggfun/OS_IDZ1/blob/main/pictures/photo_2023-03-07_22-16-08.jpg)

Теперь, после отработки второго процесса, нам необходимо снова обратиться к первому. В остальном задание идентично первому.

```c
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
    pid_t pid1, pid2;
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
    } else {
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
        } else { // код для первого процесса
            pid1 = fork();
            if (pid1 == -1) {
                printf("Ошибка создания первого дочернего процесса");
                return 0;
            }
            if (pid1 == 0) {
                char output[5000];
                read(fd[0], output, sizeof(output));
                fprintf(file2, "%s", output);
                close (fd[0]);
            }
        }
    }

    return 0;
}
```

## Задание на 7

![photo_2023-03-07_22-16-13.jpg](https://github.com/hggfun/OS_IDZ1/blob/main/pictures/photo_2023-03-07_22-16-13.jpg)

В данном случае, первый процесс вводит данные в поток, а когда считывает, они уже изменены вторым процессом. В остальном же аналогично предыдущему (на 5) заданию

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_FILE "/tmp/myfifo"

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
    pid_t pid1, pid2, pid3;
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
    } else {
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
        } else { // код для первого дочернего процесса
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
    }

    return 0;
}
```

## Задание на 8

![photo_2023-03-07_22-16-13.jpg](https://github.com/hggfun/OS_IDZ1/blob/main/pictures/photo_2023-03-07_22-16-13.jpg)

Поскольку канал именванный, то нам по сути не особо важно, являются ли процессы родственными, поскольку дескриптор задается при открытии файла, в котором и хранится поток канала. Отличается только лишь внешний вид и форма запуска. Далее представлю две единицы компиляции: первый и второй процессы.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_FILE "/tmp/myfifo"

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
    } else {
            pid1 = fork();
            if (pid1 == -1) {
                printf("Ошибка создания первого дочернего процесса");
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
```

```c
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
```

Тут видно, что в обоих случаях, файловый дескриптор одинаковый, т.к. создается для tmp/myfifo (который я задал вначале через define).
