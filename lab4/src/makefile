CC=gcc
CFLAGS=-I.
LDFLAGS=-pthread

# Добавляем цели для новой библиотеки и программы
parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

process_memory : 
	$(CC) -o process_memory process_memory.c $(CFLAGS)

# Компиляция библиотеки sum_lib
libsum.so: sum_lib.o
	$(CC) -shared -o libsum.so sum_lib.o

sum_lib.o: sum_lib.c sum_lib.h
	$(CC) -fPIC -c sum_lib.c $(CFLAGS)

# Компиляция parallel_sum
parallel_sum: parallel_sum.c libsum.so
	$(CC) -o parallel_sum parallel_sum.c -L. -lsum $(CFLAGS) $(LDFLAGS)

clean:
	rm -f utils.o find_min_max.o parallel_min_max process_memory
	rm -f sum_lib.o libsum.so parallel_sum

all: parallel_min_max process_memory parallel_sum