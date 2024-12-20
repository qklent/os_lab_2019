Deadlock (взаимная блокировка или тупиковая ситуация) - это ситуация в многозадачной среде или операционной системе, когда два или более процесса находятся в состоянии бесконечного ожидания ресурсов, занятых друг другом.

Для возникновения deadlock должны одновременно выполняться четыре условия (условия Коффмана):

1. Взаимное исключение (Mutual Exclusion):
   - Ресурс может использоваться только одним процессом одновременно
   - Другие процессы должны ждать, пока ресурс не освободится

2. Удержание и ожидание (Hold and Wait):
   - Процесс удерживает как минимум один ресурс и ожидает получения дополнительных ресурсов, занятых другими процессами

3. Отсутствие перехвата (No Preemption):
   - Ресурсы не могут быть принудительно отняты у процесса, они должны быть освобождены самим процессом добровольно

4. Циклическое ожидание (Circular Wait):
   - Существует замкнутая цепь процессов, где каждый процесс ожидает ресурс, удерживаемый следующим процессом в цепи

Пример из реальной жизни:
Представьте перекресток с четырьмя машинами, где:
- Каждая машина заняла часть перекрестка (удерживает ресурс)
- Каждой машине нужно проехать вперед (требуется дополнительный ресурс)
- Никто не может двигаться, потому что путь заблокирован другими машинами
- Образуется замкнутый круг ожидания

Простой пример в программировании:
```c
// Поток 1                  // Поток 2
lock(ресурс_A);            lock(ресурс_B);
lock(ресурс_B);            lock(ресурс_A);
// использование ресурсов   // использование ресурсов
unlock(ресурс_B);          unlock(ресурс_A);
unlock(ресурс_A);          unlock(ресурс_B);
```

Методы предотвращения deadlock:

1. Предотвращение хотя бы одного из условий Коффмана:
   ```c
   // Пример: всегда захватывать ресурсы в одном порядке
   // Поток 1 и Поток 2
   lock(ресурс_A);
   lock(ресурс_B);
   // работа с ресурсами
   unlock(ресурс_B);
   unlock(ресурс_A);
   ```

2. Использование тайм-аутов:
   ```c
   if (trylock(ресурс, timeout)) {
       // успешно получен ресурс
   } else {
       // освободить все ресурсы и попробовать снова
   }
   ```

3. Определение приоритетов:
   ```c
   // Всегда захватывать ресурсы по возрастанию приоритета
   if (приоритет_A < приоритет_B) {
       lock(ресурс_A);
       lock(ресурс_B);
   } else {
       lock(ресурс_B);
       lock(ресурс_A);
   }
   ```

4. Обнаружение и восстановление:
   - Периодически проверять наличие циклов в графе ожидания ресурсов
   - При обнаружении deadlock принудительно завершать один из процессов

Последствия deadlock:
- Зависание программы
- Потеря данных
- Необходимость перезапуска системы
- Снижение производительности

Поэтому важно:
1. Правильно проектировать многопоточные приложения
2. Использовать проверенные паттерны синхронизации
3. Тщательно тестировать на возможность возникновения deadlock
4. Иметь механизмы обнаружения и восстановления после deadlock





Вот пример программы на языке C, демонстрирующей ситуацию взаимной блокировки (deadlock) с использованием двух потоков и двух мьютексов:

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Объявляем два мьютекса
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// Функция для первого потока
void* thread1_function(void* arg) {
    printf("Thread 1: Trying to acquire mutex1\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 1: Acquired mutex1\n");
    
    // Небольшая задержка, чтобы увеличить вероятность возникновения deadlock
    sleep(1);
    
    printf("Thread 1: Trying to acquire mutex2\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 1: Acquired mutex2\n");
    
    // Критическая секция
    printf("Thread 1: Critical section\n");
    
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    
    return NULL;
}

// Функция для второго потока
void* thread2_function(void* arg) {
    printf("Thread 2: Trying to acquire mutex2\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 2: Acquired mutex2\n");
    
    // Небольшая задержка, чтобы увеличить вероятность возникновения deadlock
    sleep(1);
    
    printf("Thread 2: Trying to acquire mutex1\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 2: Acquired mutex1\n");
    
    // Критическая секция
    printf("Thread 2: Critical section\n");
    
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    // Создаем потоки
    pthread_create(&thread1, NULL, thread1_function, NULL);
    pthread_create(&thread2, NULL, thread2_function, NULL);
    
    // Ждем завершения потоков
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    // Уничтожаем мьютексы
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    
    return 0;
}
```

Как работает эта программа:

1. Создаются два мьютекса (`mutex1` и `mutex2`).

2. Создаются два потока, каждый выполняет свою функцию:
   - Первый поток пытается захватить сначала `mutex1`, потом `mutex2`
   - Второй поток пытается захватить сначала `mutex2`, потом `mutex1`

3. Взаимная блокировка возникает следующим образом:
   - Поток 1 захватывает `mutex1`
   - Поток 2 захватывает `mutex2`
   - Поток 1 пытается захватить `mutex2` (который уже захвачен потоком 2)
   - Поток 2 пытается захватить `mutex1` (который уже захвачен потоком 1)

Чтобы скомпилировать и запустить программу в Unix-подобной системе:
```bash
gcc -o deadlock deadlock.c -pthread
./deadlock
```

Вывод программы может выглядеть примерно так:
```
Thread 1: Trying to acquire mutex1
Thread 1: Acquired mutex1
Thread 2: Trying to acquire mutex2
Thread 2: Acquired mutex2
Thread 1: Trying to acquire mutex2
Thread 2: Trying to acquire mutex1
```

После этого программа "зависнет", так как оба потока будут ждать освобождения мьютексов, которые удерживаются другим потоком.

Чтобы избежать deadlock в реальных приложениях, можно использовать следующие подходы:
1. Всегда захватывать мьютексы в одном и том же порядке
2. Использовать trylock вместо lock
3. Использовать иерархию блокировок
4. Применять тайм-ауты при захвате блокировок