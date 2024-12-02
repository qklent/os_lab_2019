Давайте разберем эти системные вызовы в C:

1. alarm():
```c
unsigned int alarm(unsigned int seconds);
```
- Устанавливает таймер, который отправляет сигнал SIGALRM процессу через указанное количество секунд
- Возвращает количество секунд, оставшихся от предыдущего таймера, или 0
- Пример:
```c
alarm(5); // Отправит SIGALRM через 5 секунд
```

2. signal():
```c
void (*signal(int signum, void (*handler)(int)))(int);
```
- Устанавливает обработчик для определенного сигнала
- signum - номер сигнала
- handler - функция-обработчик или специальные константы (SIG_IGN, SIG_DFL)
- Пример:
```c
void handler(int sig) {
    printf("Получен сигнал: %d\n", sig);
}

signal(SIGALRM, handler); // Установка обработчика для SIGALRM
```

3. kill():
```c
int kill(pid_t pid, int sig);
```
- Отправляет сигнал процессу или группе процессов
- pid > 0: отправляет сигнал процессу с указанным PID
- pid = 0: отправляет сигнал всем процессам группы
- pid = -1: отправляет сигнал всем процессам
- Пример:
```c
kill(1234, SIGTERM); // Отправляет SIGTERM процессу с PID 1234
```

4. waitpid():
```c
pid_t waitpid(pid_t pid, int *status, int options);
```
- Ожидает изменения состояния дочернего процесса
- pid > 0: ждет конкретный процесс
- pid = -1: ждет любой дочерний процесс
- status возвращает информацию о завершении процесса
- options определяет поведение (WNOHANG, WUNTRACED и т.д.)
- Пример:
```c
int status;
pid_t child_pid = fork();

if (child_pid == 0) {
    // Код дочернего процесса
    exit(0);
} else {
    // Родительский процесс ждет завершения дочернего
    waitpid(child_pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Дочерний процесс завершился с кодом: %d\n", WEXITSTATUS(status));
    }
}
```

Полный пример использования:
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void alarm_handler(int sig) {
    printf("Получен сигнал SIGALRM!\n");
}

int main() {
    // Установка обработчика для SIGALRM
    signal(SIGALRM, alarm_handler);
    
    pid_t child_pid = fork();
    
    if (child_pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс спит 5 секунд...\n");
        alarm(5); // Установка будильника
        pause(); // Ждем сигнал
        exit(0);
    } else {
        // Родительский процесс
        int status;
        waitpid(child_pid, &status, 0);
        printf("Дочерний процесс завершился\n");
    }
    
    return 0;
}
```

Важные замечания:
1. signal() считается устаревшим, в современном коде лучше использовать sigaction()
2. Некоторые сигналы (например, SIGKILL и SIGSTOP) нельзя перехватить или игнорировать
3. waitpid() более гибкий, чем wait(), так как позволяет указать конкретный процесс и дополнительные опции
4. При использовании этих функций важно правильно обрабатывать ошибки и проверять возвращаемые значения