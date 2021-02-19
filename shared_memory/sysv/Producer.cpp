#include <iostream>

#include "Common.h"

void waitForEmpty(int semId) {
  struct sembuf sops[2];

  // Decrease # of empty slots
  sops[0].sem_num = IDX_EMPTY;
  sops[0].sem_op = -1;
  sops[0].sem_flg = 0;

  // Get the mutex to the path
  sops[1].sem_num = IDX_MUTEX;
  sops[1].sem_op = -1;
  sops[1].sem_flg = 0;

  if (semop(semId, sops, 2) == -1) {
    errExit("1: semop(semId, sops, 2)");
  }
}

void notifyProduced(int semId) {
  struct sembuf sops[2];

  // Release the mutex
  sops[0].sem_num = IDX_MUTEX;
  sops[0].sem_op = 1;
  sops[0].sem_flg = 0;

  // Increase # of produced files
  sops[1].sem_num = IDX_PRODUCED;
  sops[1].sem_op = 1;
  sops[1].sem_flg = 0;

  if (semop(semId, sops, 2) == -1) {
    errExit("2: semop(semId, sops, 2)");
  }
}

void produce(struct shared_mem *mem, int num) {
  mem->queue[mem->nextProduceIdx] = num;
  std::cout << num << std::endl;
  mem->nextProduceIdx = (mem->nextProduceIdx + 1) % kMaxQueueSize;
}

// The producer produces
int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " PATH PROJ_ID TOTAL_PRODUCE"
              << std::endl;
    return -1;
  }

  const std::string pathname = argv[1];
  const int projId = std::stoi(argv[2]);
  const int totalProduce = std::stoi(argv[3]);

  struct shared_mem *mem = getSharedMem(pathname, projId);
  int semId = getSemSetId(pathname, projId);

  for (int i = 0; i < totalProduce; i++) {
    waitForEmpty(semId);

    produce(mem, i);

    notifyProduced(semId);
  }
  if (shmdt(mem) == -1) {
    errExit("shmdt(mem)");
  }
  return 0;
}
