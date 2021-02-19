#include <fstream>
#include <iostream>
#include <unistd.h>

#include "Common.h"

void produce(const std::string &pathname, int num) {
  std::ofstream ofs(pathname + "/" + std::to_string(num));
  std::cout << num << std::endl;
  ofs << num << std::endl;
  ofs.close();
}

// The producer produces
int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cout << "Usage: " << argv[0] << " PATH NAME MAX_SLOTS TOTAL_PRODUCE"
              << std::endl;
    return -1;
  }

  const std::string path = argv[1];
  const std::string name = argv[2];
  const int maxSlots = std::stoi(argv[3]);
  const int totalProduce = std::stoi(argv[4]);

  sem_t *mutex, *semEmpty, *semProduced;
  getSemaphores(name, &mutex, &semEmpty, &semProduced, maxSlots);

  for (int i = 0; i < totalProduce; i++) {
    // Decrease # of empty slots
    if (sem_wait(semEmpty) == -1) {
      errExit("sem_wait(semEmpty)");
    }

    // Get the mutex to the path
    if (sem_wait(mutex) == -1) {
      errExit("sem_wait(mutex)");
    }

    // Produce one file in the path
    produce(path, i);

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
