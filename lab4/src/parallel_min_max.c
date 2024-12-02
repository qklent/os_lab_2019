#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

#define min(a, b) ((a) < (b) ? (a) : (b))


static pid_t *g_child_pids = NULL;
static int g_num_children = 0;


void printArray(int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void kill_children(void) {
    printf("Timeout reached. Killing all child processes.\n");
    exit(1);
}



void cleanup_and_exit(void) {
    if (g_child_pids) {
        for (int i = 0; i < g_num_children; i++) {
            if (g_child_pids[i] > 0) {
                kill(g_child_pids[i], SIGKILL);
            }
        }
        free(g_child_pids);
    }
    printf("Timeout reached. Killing all child processes.\n");
    exit(1);
}

void timeout_handler(int signum) {
    (void)signum;  // Unused parameter
    cleanup_and_exit();
}


int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;
  
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("Seed must be a positive integer\n");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
                printf("Array size must be a positive integer\n");
                return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("Number of processes must be a positive integer\n");
                return 1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            if (timeout <= 0) {
                printf("Timeout must be a positive integer\n");
                return 1;
            }
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  // printArray(array, array_size);
  int active_child_processes = 0;

  int pipes[pnum][2];
  pid_t child_pids[pnum];
  for (int i = 0; i < pnum; i++) {
    if (pipe(pipes[i]) == -1) {
      printf("Pipe failed!\n");
      return 1;
    }
  }


  if (timeout > 0) {
      // Allocate global array for child PIDs
      g_child_pids = malloc(sizeof(pid_t) * pnum);
      if (g_child_pids == NULL) {
          printf("Failed to allocate memory for child PIDs\n");
          return 1;
      }
      g_num_children = pnum;
      
      // Set up signal handler
      if (signal(SIGALRM, timeout_handler) == SIG_ERR) {
          printf("Failed to set up signal handler\n");
          free(g_child_pids);
          return 1;
      }
      
      // Set alarm
      alarm(timeout);
  }


  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  int array_size_for_pnum = array_size / pnum; 

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      if (child_pid > 0) {
        // parent process
        if (timeout > 0) {
            g_child_pids[i] = child_pid;
        }
        child_pids[i] = child_pid;
      }
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        for (int j = 0; j < pnum; j++) {
            if (j != i) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
        }
        close(pipes[i][0]); 
        int left = i * array_size_for_pnum;
        int right = min(array_size_for_pnum * (i + 1), array_size);
        struct MinMax min_max = GetMinMax(array, left, right);

        if (with_files) {
          char filename[256];
          sprintf(filename, "result_%d.txt", i);
          FILE *f = fopen(filename, "w");
          if (f == NULL) {
              printf("Error opening file!\n");
              exit(1);
          }
          fprintf(f, "%d %d", min_max.min, min_max.max);
          fclose(f);
        } else {
          write(pipes[i][1], &min_max, sizeof(struct MinMax));
          close(pipes[i][1]);
          _exit(0);
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  for (int i = 0; i < pnum; i++) {
      close(pipes[i][1]);  
  }

  while (active_child_processes > 0) {
      int status;
      pid_t terminated_pid = wait(&status);
      
      if (terminated_pid == -1) {
          if (timeout > 0) {
              // Убить оставшиеся процессы
              for (int i = 0; i < pnum; i++) {
                  kill(child_pids[i], SIGKILL);
              }
              printf("Timeout reached. Killed all child processes.\n");
              return 1;
          }
      }
      active_child_processes -= 1;
  }

  if (timeout > 0) {
      alarm(0);
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
        char filename[256];
        sprintf(filename, "result_%d.txt", i);
        FILE *f = fopen(filename, "r");
        if (f == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
        fscanf(f, "%d %d", &min, &max);
        fclose(f);
        remove(filename);
    } else {
        struct MinMax temp;
        if (read(pipes[i][0], &temp, sizeof(struct MinMax)) == sizeof(struct MinMax)) {
            min = temp.min;
            max = temp.max;
        } else {
            printf("Error reading from pipe %d\n", i);
        }
        close(pipes[i][0]);
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max; 
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  if (timeout > 0 && g_child_pids) {
      alarm(0);
      free(g_child_pids);
  }
  return 0;
}
