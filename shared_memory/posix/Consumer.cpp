#include <iostream>

#include "Common.h"

constexpr int kDefaultTimeoutSec = 5;

void consume(struct shared_mem *mem) {
  std::cout << mem->queue[mem->nextConsumeIdx] << std::endl;
  mem->nextConsumeIdx = (mem->nextConsumeIdx + 1) % kMaxQueueSize;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " SEMAPHORE_NAME SHARED_MEM_NAME"
              << std::endl;
    return -1;
  }

  const std::string semName = argv[1];
  const std::string shmName = argv[2];

  sem_t *mutex, *semEmpty, *semProduced;
  getSemaphores(semName, &mutex, &semEmpty, &semProduced);

  struct shared_mem *mem = getSharedMem(shmName, mutex);

  while (1) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
      errExit("clock_gettime(CLOCK_REALTIME, &ts)");
    }
    ts.tv_sec += kDefaultTimeoutSec;
    // Decrease # of produced slots
    if (sem_timedwait(semProduced, &ts) == -1) {
      if (errno == ETIMEDOUT) {
        break;
      }
      errExit("sem_wait(semProduced)");
    }

    // Get the mutex
    if (sem_wait(mutex) == -1) {
      errExit("sem_wait(mutex)");
    }

    // Consume one number from the queue
    consume(mem);

    // Release the mutex
    if (sem_post(mutex) == -1) {
      errExit("sem_post(mutex)");
    }

    // Increase # of empty slots
    if (sem_post(semEmpty) == -1) {
      errExit("sem_post(semEmpty)");
    }
  }
  closeSemaphores(mutex, semEmpty, semProduced);
  return 0;
}
