#pragma once

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <chrono>
#include <thread>

const auto kInitializeTimeout = std::chrono::seconds(3);
const int kMaxQueueSize = 10;

struct shared_mem {
  int queue[kMaxQueueSize];
  int nextProduceIdx;
  int nextConsumeIdx;
};

union semun {
  int val;               /* Value for SETVAL */
  struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
  unsigned short *array; /* Array for GETALL, SETALL */
  struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
};

enum { IDX_MUTEX = 0, IDX_EMPTY = 1, IDX_PRODUCED = 2 };

void errExit(const std::string &msg) {
  perror(msg.c_str());
  exit(EXIT_FAILURE);
}

// The first process started will create and initialize the semaphores
int getSemSetId(const std::string &pathname, int projId) {
  key_t key = ftok(pathname.c_str(), projId);
  if (key == -1) {
    errExit("ftok(" + pathname + ", " + std::to_string(projId) + ")");
  }
  // Create a semaphore set with 3 semaphores
  // semaphore 0: Used as a mutex to update the file
  // semaphore 1: Count of empty slots
  // semaphore 2: Count of produced objects
  int semId = semget(key, 3, IPC_CREAT | IPC_EXCL | 0660);
  // Successfully created a new semaphore set
  if (semId != -1) {
    // Initialize the semaphores
    union semun arg;
    // Initialize the mutex to 1
    arg.val = 1;
    if (semctl(semId, IDX_MUTEX, SETVAL, arg) == -1) {
      errExit("semctl(semId, 0, SETVAL, 1)");
    }
    // Initialize the # of empty slots to kMaxQueueSize
    arg.val = kMaxQueueSize;
    if (semctl(semId, IDX_EMPTY, SETVAL, arg) == -1) {
      errExit("semctl(semId, 1, SETVAL, " + std::to_string(kMaxQueueSize) +
              ")");
    }
    // Initialize the # of produced objects to 0
    arg.val = 0;
    if (semctl(semId, IDX_PRODUCED, SETVAL, arg) == -1) {
      errExit("semctl(semId, 2, SETVAL, 0)");
    }
    // Change otime to non-zero value so that other processes know that the
    // semaphore set is initialized
    struct sembuf sop;
    sop.sem_num = IDX_PRODUCED;
    sop.sem_op = 0; // Wait until the value is 0 (basically no-op)
    sop.sem_flg = 0;

    if (semop(semId, &sop, 1) == -1) {
      errExit("semop(semId, &sop, 1)");
    }
  } else if (errno == EEXIST) {
    // This semaphore set is already created by another process
    semId = semget(key, 3, 0660);
    if (semId == -1) {
      errExit("semget(key, 3, 0666)");
    }
    // Wait until the semaphore is initialized
    struct semid_ds ds;
    union semun arg;
    arg.buf = &ds;
    const auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - start < kInitializeTimeout) {
      if (semctl(semId, IDX_PRODUCED, IPC_STAT, arg) == -1) {
        errExit("semctl(semId, 2, IPC_STAT, arg)");
      }
      if (ds.sem_otime != 0) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // The semaphore set is still not initalized
    if (ds.sem_otime == 0) {
      errExit("Timed out while waiting for semaphore set initialization");
    }
  } else {
    // Error
    errExit("semget(key, 3, IPC_CREAT | IPC_EXCL | 0660)");
  }
  return semId;
}

struct shared_mem *getSharedMem(const std::string &pathname, int projId) {
  key_t key = ftok(pathname.c_str(), projId);
  if (key == -1) {
    errExit("ftok(" + pathname + ", " + std::to_string(projId) + ")");
  }
  int shmId = shmget(key, sizeof(struct shared_mem), IPC_CREAT | 0660);
  if (shmId == -1) {
    errExit("shmget(key, sizeof(struct shared_mem), IPC_CREAT | 0660)");
  }
  struct shared_mem *mem = (struct shared_mem *)shmat(shmId, NULL, 0);
  if (mem == (void *)-1) {
    errExit("shmat(shmid, NULL, 0)");
  }
  return mem;
}
