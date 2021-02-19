#include <unistd.h>

#include <fstream>
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

void produce(const std::string &pathname, int num) {
  std::ofstream ofs(pathname + "/" + std::to_string(num));
  std::cout << num << std::endl;
  ofs << num << std::endl;
  ofs.close();
}

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cout << "Usage: " << argv[0] << " PATH PROJ_ID MAX_SLOTS TOTAL_PRODUCE"
              << std::endl;
    return -1;
  }

  const std::string pathname = argv[1];
  const int projId = std::stoi(argv[2]);
  const int maxSlots = std::stoi(argv[3]);
  const int totalProduce = std::stoi(argv[4]);
  int semId = getSemSetId(pathname, projId, maxSlots);

  for (int i = 0; i < totalProduce; i++) {
    waitForEmpty(semId);

    // Produce one file in the path
    produce(pathname, i);

    notifyProduced(semId);
  }
  return 0;
}
