**Зомби-процессы (Zombie Process)**

Зомби-процесс - это процесс, который завершил свое выполнение, но все еще имеет запись в таблице процессов. Это происходит, когда дочерний процесс завершается, но его родительский процесс не считывает его статус завершения с помощью системного вызова wait().

**Как появляются:**
1. Родительский процесс создает дочерний процесс (fork())
2. Дочерний процесс завершает работу
3. Родительский процесс не вызывает wait() для получения статуса завершения

**Как исчезают:**
1. Родительский процесс вызывает wait() или waitpid()
2. Родительский процесс завершается (тогда зомби-процессы "усыновляются" процессом init, который автоматически выполняет wait())

**Пример программы, демонстрирующей создание зомби-процесса:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Создаем дочерний процесс
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {  // Дочерний процесс
        printf("Child process is running (PID: %d)\n", getpid());
        exit(0);
    } else {  // Родительский процесс
        printf("Parent process is running (PID: %d)\n", getpid());
        printf("Child process ID: %d\n", pid);
        
        // Ждем некоторое время, чтобы можно было увидеть зомби процесс
        sleep(30);
        
        // Раскомментируйте следующую строку, чтобы предотвратить появление зомби
        // wait(NULL);
        
        printf("Parent process is done\n");
    }

    return 0;
}
```

**Как проверить наличие зомби-процесса:**
```bash
ps aux | grep Z
```

**Опасность зомби-процессов:**
1. Занимают место в таблице процессов системы (хотя и минимальное)
2. При большом количестве могут исчерпать лимит процессов системы
3. Указывают на проблемы в дизайне программы

**Способы избавления от зомби-процессов:**
1. Корректная обработка дочерних процессов с помощью wait()/waitpid()
2. Использование signal handler для SIGCHLD:
```c
void handle_sigchld(int sig) {
    while (waitpid((pid_t)(-1), NULL, WNOHANG) > 0) {}
}

// В main():
signal(SIGCHLD, handle_sigchld);
```

3. Двойной fork() (техника "daemon process"):
```c
if (fork() == 0) {
    if (fork() == 0) {
        // выполнение работы
        exit(0);
    }
    exit(0);
}
wait(NULL);
```

**Улучшенная версия программы с предотвращением зомби:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void handle_sigchld(int sig) {
    while (waitpid((pid_t)(-1), NULL, WNOHANG) > 0) {}
}

int main() {
    // Устанавливаем обработчик сигнала SIGCHLD
    signal(SIGCHLD, handle_sigchld);

    pid_t pid;
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {  // Дочерний процесс
        printf("Child process is running (PID: %d)\n", getpid());
        exit(0);
    } else {  // Родительский процесс
        printf("Parent process is running (PID: %d)\n", getpid());
        printf("Child process ID: %d\n", pid);
        
        sleep(30);  // Ждем, но зомби не появится благодаря обработчику сигнала
        printf("Parent process is done\n");
    }

    return 0;
}
```

Эта версия программы автоматически обрабатывает завершение дочерних процессов, предотвращая появление зомби.