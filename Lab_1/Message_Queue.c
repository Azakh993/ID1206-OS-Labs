#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define MQ_MODE (S_IRUSR | S_IRGRP | S_IROTH)

/**
 * Creates and configures a message queue.
 * @param mq_name The name of the message queue.
 * @param attr The attributes to configure the message queue.
 * @return The message queue descriptor.
 */
mqd_t createAndConfigureMessageQueue(const char *mq_name,
                                     const struct mq_attr *attr);
/**
 * Reads the content of a file and sends it through a message queue.
 * @param mq_name The name of the message queue.
 * @param attr The attributes to configure the message queue.
 */
void sendFileContentThroughMessageQueue(const char *mq_name,
                                        const struct mq_attr *attr);

/**
 * Reads the content from a message queue, counts the words, and prints the
 * result.
 * @param mq_name The name of the message queue.
 * @param attr The attributes to configure the message queue.
 */
void receiveAndCountWordsFromMessageQueue(const char *mq_name,
                                          const struct mq_attr *attr);

int main() {
  const char *my_message_queue = "/my_message_queue";
  struct mq_attr attr;
  attr.mq_msgsize = 1024;
  attr.mq_maxmsg = 10;

  pid_t pid = fork();

  if (pid == -1) {
    perror("Something went wrong with fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    receiveAndCountWordsFromMessageQueue(my_message_queue, &attr);
  } else {
    sendFileContentThroughMessageQueue(my_message_queue, &attr);
    wait(NULL);
  }

  exit(0);
}

mqd_t createAndConfigureMessageQueue(const char *mq_name,
                                     const struct mq_attr *attr) {
  mqd_t message_queue_destination =
      mq_open(mq_name, O_RDONLY | O_CREAT, MQ_MODE, (struct mq_attr *)attr);
  if (message_queue_destination == (mqd_t)-1) {
    perror("Error opening message queue");
    exit(EXIT_FAILURE);
  }
  return message_queue_destination;
}

void sendFileContentThroughMessageQueue(const char *mq_name,
                                        const struct mq_attr *attr) {
  FILE *message = fopen("file.txt", "r");
  char write_message[attr->mq_msgsize + 1];
  fread(write_message, sizeof(char), attr->mq_msgsize, message);
  fclose(message);

  if (!write_message) {
    perror("Something went wrong when reading file.");
    exit(EXIT_FAILURE);
  }

  mqd_t message_queue_destination = mq_open(mq_name, O_WRONLY);
  mq_send(message_queue_destination, write_message, strlen(write_message), 0);
  mq_close(message_queue_destination);
}

void receiveAndCountWordsFromMessageQueue(const char *mq_name,
                                          const struct mq_attr *attr) {
  int MAX_SIZE = 100;
  char buf[MAX_SIZE];

  mqd_t message_queue_destination =
      createAndConfigureMessageQueue(mq_name, attr);
  mq_receive(message_queue_destination, buf, attr->mq_msgsize, NULL);

  int wordCount = 0;
  char *word = strtok(buf, " \t\n");

  while (word != NULL) {
    wordCount++;
    word = strtok(NULL, " \t\n");
  }

  printf("Word count: %d\n", wordCount);
  exit(0);
}
