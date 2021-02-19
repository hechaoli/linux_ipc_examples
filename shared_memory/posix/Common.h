#pragma once

#include <fcntl.h>     /* For O_* constants */
#include <semaphore.h> /* For mode constants */
#include <sys/mman.h>
#include <unistd.h>

const int kMaxQueueSize = 10;

struct shared_mem {
  int queue[kMaxQueueSize];
  int nextProduceIdx;
  int nextConsumeIdx;
};

void errExit(const std::string &msg) {
  perror(msg.c_str());
  exit(EXIT_FAILURE);
}

// The first process started will create and initialize the semaphores
void getSemaphores(const std::string &prefix, sem_t **mutex, sem_t **semEmpty,
                   sem_t **semProduced) {
  *mutex = sem_open((prefix + "_mutex").c_str(), O_CREAT, 0660, 1);
  if (*mutex == SEM_FAILED) {
    errExit("sem_open(mutex, O_CREAT, 0660, 1)");
  }

  *semEmpty =
      sem_open((prefix + "_empty").c_str(), O_CREAT, 0660, kMaxQueueSize);
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

struct shared_mem *getSharedMem(const std::string &name, sem_t *mutex) {
  // Make sure only the first process can create and truncate the shared mem
  if (sem_wait(mutex) == -1) {
    errExit("sem_wait(mutex)");
  }

  int fd = shm_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);
  if (fd == -1) {
    if (errno == EEXIST) {
      fd = shm_open(name.c_str(), O_RDWR, 0);
      if (fd == -1) {
        errExit("shm_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660)");
      }
    } else {
      errExit("shm_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660)");
    }
  } else if (ftruncate(fd, sizeof(struct shared_mem)) == -1) {
    errExit("ftruncate(fd, sizeof(struct shared_mem)");
  }

  struct shared_mem *mem = (struct shared_mem *)mmap(
      NULL /* addr */, sizeof(struct shared_mem) /* length */,
      PROT_READ | PROT_WRITE /* prot */, MAP_SHARED /* flags */, fd,
      0 /* offset */);
  if (mem == MAP_FAILED) {
    errExit("mmap(NULL, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, "
            "MAP_SHARED, fd, 0)");
  }
  close(fd);

  if (sem_post(mutex) == -1) {
    errExit("sem_post(mutex)");
  }
  return mem;
}
