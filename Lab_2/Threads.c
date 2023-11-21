#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3
#define MAX_BUFFER_VALUE 15

int shared_buffer = 0;
pthread_mutex_t lock;

// Function to initialize the mutex.
// It initializes the global mutex variable 'lock'.
void initializeMutex();

// Function to create threads.
// It creates 'NUM_THREADS' threads using the 'pthread_create' function.
void createThreads(pthread_t *threads);

// Function to wait for threads to finish.
// It waits for each thread to finish using 'pthread_join' and prints the number
// of times each thread worked on the buffer.
void waitForThreads(pthread_t *threads);

// Function executed by each thread.
// It increments the shared buffer while respecting the maximum buffer value.
void *bufferIncrementer(void *arg);

// Function to print information about the current thread.
// It prints the thread ID, process ID, and the current buffer value.
void printThreadInfo();

int main() {
  initializeMutex();

  pthread_t threads[NUM_THREADS];

  createThreads(threads);

  waitForThreads(threads);

  pthread_mutex_destroy(&lock);

  return 0;
}

void initializeMutex() {
  int result = pthread_mutex_init(&lock, NULL);
  if (result != 0) {
    perror("Error initializing mutex using 'pthread_mutex_init'");
    exit(EXIT_FAILURE);
  }
}

void createThreads(pthread_t *threads) {
  int result;
  for (int i = 0; i < NUM_THREADS; i++) {
    result = pthread_create(&threads[i], NULL, &bufferIncrementer, NULL);

    if (result != 0) {
      perror("Error creating thread using 'pthread_create'");
      exit(EXIT_FAILURE);
    }
  }
}

void waitForThreads(pthread_t *threads) {
  int *count;
  int result;
  for (int i = 0; i < NUM_THREADS; i++) {
    result = pthread_join(threads[i], (void **)&count);

    if (result != 0) {
      perror("Error joining thread using 'pthread_join'");
      exit(EXIT_FAILURE);
    }

    printf("Thread ID %lu worked on the buffer %d times\n", threads[i], *count);
  }

  printf("Total buffer accesses: %d\n", shared_buffer);
}

void *bufferIncrementer(void *arg) {
  int *count = malloc(sizeof(int));
  *count = 0;

  while (1) {
    pthread_mutex_lock(&lock);

    if (shared_buffer >= MAX_BUFFER_VALUE) {
      pthread_mutex_unlock(&lock);
      break;
    }

    printThreadInfo();

    shared_buffer++;
    pthread_mutex_unlock(&lock);
    (*count)++;
    usleep(1);
  }
  return count;
}

void printThreadInfo() {
  pthread_t thread_id = pthread_self();
  pid_t process_id = getpid();

  printf("Thread ID: %lu, Process ID: %d, Buffer Value: %d\n", thread_id,
         process_id, shared_buffer);
}
