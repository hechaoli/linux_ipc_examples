#pragma once

#include <fcntl.h>
#include <semaphore.h>

void errExit(const std::string &msg) {
  perror(msg.c_str());
  exit(EXIT_FAILURE);
}

// The first process started will create and initialize the semaphores
void getSemaphores(const std::string &prefix, sem_t **mutex, sem_t **semEmpty,
                   sem_t **semProduced, int maxSlots) {
  *mutex = sem_open((prefix + "_mutex").c_str(), O_CREAT, 0660, 1);
  if (*mutex == SEM_FAILED) {
    errExit("sem_open(mutex, O_CREAT, 0660, 1)");
  }

  *semEmpty = sem_open((prefix + "_empty").c_str(), O_CREAT, 0660, maxSlots);
  if (*semEmpty == SEM_FAILED) {
    errExit("sem_open(empty, O_CREAT, 0660, 1)");
  }

  *semProduced = sem_open((prefix + "_produced").c_str(), O_CREAT, 0660, 0);
  if (*semProduced == SEM_FAILED) {
    errExit("sem_open(produced, O_CREAT, 0660, 1)");
  }
}

void closeSemaphores(sem_t *mutex, sem_t *semEmpty, sem_t *semProduced) {
  if (sem_close(mutex) == -1) {
    perror("sem_close(mutex)");
  }
  if (sem_close(semEmpty) == -1) {
    perror("sem_close(semEmpty)");
  }
  if (sem_close(semProduced) == -1) {
    perror("sem_close(semProduced)");
  }
}
