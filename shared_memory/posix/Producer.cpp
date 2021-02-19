#include <iostream>

#include "Common.h"

void produce(struct shared_mem *mem, int num) {
  mem->queue[mem->nextProduceIdx] = num;
  std::cout << num << std::endl;
  mem->nextProduceIdx = (mem->nextProduceIdx + 1) % kMaxQueueSize;
}

// The producer produces
int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0]
              << " SEMAPHORE_NAME SHARED_MEM_NAME TOTAL_PRODUCE" << std::endl;
    return -1;
  }

  const std::string semName = argv[1];
  const std::string shmName = argv[2];
  const int totalProduce = std::stoi(argv[3]);

  sem_t *mutex, *semEmpty, *semProduced;
  getSemaphores(semName, &mutex, &semEmpty, &semProduced);

  struct shared_mem *mem = getSharedMem(shmName, mutex);

  for (int i = 0; i < totalProduce; i++) {
    // Decrease # of empty slots
    if (sem_wait(semEmpty) == -1) {
      errExit("sem_wait(semEmpty)");
    }

    // Get the mutex to the path
    if (sem_wait(mutex) == -1) {
      errExit("sem_wait(mutex)");
    }

    // Produce one number
    produce(mem, i);

    // Release the mutex
    if (sem_post(mutex) == -1) {
      errExit("sem_post(mutex)");
    }

    // Increase # of produced files
    if (sem_post(semProduced) == -1) {
      errExit("sem_post(semProduced)");
    }
  }
  closeSemaphores(mutex, semEmpty, semProduced);
  return 0;
}
