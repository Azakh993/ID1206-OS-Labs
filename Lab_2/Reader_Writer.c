#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX 5
#define NUMBER_OF_PROCESSES 3

struct shared_memory {
  int VAR;
  sem_t read_lock;
  sem_t write_lock;
  int reader_count;
};

/*
 * Initializes the shared memory structure, setting initial values and
 * initializing semaphores.
 * Parameters: - shared: Pointer to the shared memory structure.
 */
void initialize_shared_memory(struct shared_memory *shared);

/*
 * Creates the specified number of processes (readers and a writer) using
 * fork().
 * Parameters: - shared: Pointer to the shared memory structure.
 */
void create_processes(struct shared_memory *shared);

/*
 * Writer process that continuously writes values to the shared variable within
 * the specified limit.
 * Parameters: - shared: Pointer to the shared memory structure.
 */
void writer(struct shared_memory *shared);

/*
 * Reader process that continuously reads values from the shared variable within
 * the specified limit.
 * Parameters: - shared: Pointer to the shared memory structure.
 */
void reader(struct shared_memory *shared);

/*
 * Description: Waits for all child processes to complete using wait().
 */
void wait_for_processes();

/*
 * Cleans up resources, destroys semaphores, and unmaps shared memory.
 Parameters: - shared: Pointer to the shared memory structure.
 */
void cleanup(struct shared_memory *shared);

int main() {
  struct shared_memory *shared =
      mmap(NULL, sizeof(struct shared_memory), PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  initialize_shared_memory(shared);
  create_processes(shared);
  wait_for_processes();
  cleanup(shared);

  return 0;
}

void initialize_shared_memory(struct shared_memory *shared) {
  shared->VAR = 0;
  shared->reader_count = 0;
  sem_init(&shared->read_lock, 1, 1);
  sem_init(&shared->write_lock, 1, 1);
}

void create_processes(struct shared_memory *shared) {
  for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
    pid_t process_id = fork();
    if (process_id == 0) {
      if (i == 0) {
        writer(shared);
      } else {
        reader(shared);
      }
      exit(0);
    }
  }
}

void writer(struct shared_memory *shared) {
  do {
    sem_wait(&shared->write_lock);
    printf("The writer acquires the lock.\n");
    shared->VAR++;
    printf("The writer (%d) writes the value %d\n", getpid(), shared->VAR);
    printf("The writer releases the lock.\n");
    sem_post(&shared->write_lock);
    sleep(1);
  } while (shared->VAR < MAX);
}

void reader(struct shared_memory *shared) {
  do {
    sem_wait(&shared->read_lock);
    shared->reader_count++;
    if (shared->reader_count == 1) {
      sem_wait(&shared->write_lock);
      printf("The first reader acquires the lock.\n");
    }
    sem_post(&shared->read_lock);
    usleep(1000);

    printf("The reader (%d) reads the value %d\n", getpid(), shared->VAR);

    sem_wait(&shared->read_lock);
    shared->reader_count--;
    if (shared->reader_count == 0) {
      printf("The last reader releases the lock.\n");
      sem_post(&shared->write_lock);
    }
    sem_post(&shared->read_lock);
    sleep(1);
  } while (shared->VAR < MAX);
}

void wait_for_processes() {
  for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
    wait(NULL);
  }
}

void cleanup(struct shared_memory *shared) {
  sem_destroy(&shared->read_lock);
  sem_destroy(&shared->write_lock);
  munmap(shared, sizeof(struct shared_memory));
}
