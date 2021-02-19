#include <iostream>

#include "Common.h"

constexpr int kDefaultTimeoutSec = 5;

bool waitForProduced(int semId) {
  struct sembuf sops[2];

  // Decrease # of empty produced objects
  sops[0].sem_num = IDX_PRODUCED;
  sops[0].sem_op = -1;
  sops[0].sem_flg = 0;

  // Get the mutex to the path
  sops[1].sem_num = IDX_MUTEX;
  sops[1].sem_op = -1;
  sops[1].sem_flg = 0;

  struct timespec ts {
    .tv_sec = kDefaultTimeoutSec
  };

  if (semtimedop(semId, sops, 2, &ts) == -1) {
    if (errno == EAGAIN) {
      return false;
    }
    errExit("1: semop(semId, sops, 2)");
  }
  return true;
}

void notifyEmpty(int semId) {
  struct sembuf sops[2];

  // Release the mutex
  sops[0].sem_num = IDX_MUTEX;
  sops[0].sem_op = 1;
  sops[0].sem_flg = 0;

  // Increase # of empty slots
  sops[1].sem_num = IDX_EMPTY;
  sops[1].sem_op = 1;
  sops[1].sem_flg = 0;

  if (semop(semId, sops, 2) == -1) {
    errExit("2: semop(semId, sops, 2)");
  }
}

void consume(struct shared_mem *mem) {
  std::cout << mem->queue[mem->nextConsumeIdx] << std::endl;
  mem->nextConsumeIdx = (mem->nextConsumeIdx + 1) % kMaxQueueSize;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " PATH PROJ_ID" << std::endl;
    return -1;
  }

  const std::string pathname = argv[1];
  const int projId = std::stoi(argv[2]);

  struct shared_mem *mem = getSharedMem(pathname, projId);
  int semId = getSemSetId(pathname, projId);

  while (1) {
    if (!waitForProduced(semId)) {
      break;
    }

    // Consume one number from the queue
    consume(mem);

    notifyEmpty(semId);
  }
  if (shmdt(mem) == -1) {
    errExit("shmdt(mem)");
  }
  return 0;
}
