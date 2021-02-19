#include <filesystem>
#include <fstream>
#include <iostream>

#include "Common.h"

namespace fs = std::filesystem;
constexpr int kDefaultTimeoutSec = 5;

void consume(const std::string &path) {
  fs::path filepath;
  // Read the file whose name is the smallest number.
  for (const auto &entry : fs::directory_iterator(path)) {
    auto path = entry.path();
    if (filepath.empty() ||
        std::stoi(path.filename()) < std::stoi(filepath.filename())) {
      filepath = path;
    }
  }
  if (filepath.empty()) {
    std::cout << "ERROR: No file for the consumer to consume!" << std::endl;
  } else {
    std::fstream file(filepath.string(), std::ios::in);
    if (file.is_open()) {
      std::string line;
      while (getline(file, line)) {
        std::cout << line << std::endl;
      }
      file.close();
      fs::remove(filepath);
    } else {
      std::cout << "ERROR: Failed to open " << filepath.string() << std::endl;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " PATH NAME MAX_SLOTS" << std::endl;
    return -1;
  }

  const std::string path = argv[1];
  const std::string name = argv[2];
  const int maxSlots = std::stoi(argv[3]);

  sem_t *mutex, *semEmpty, *semProduced;
  getSemaphores(name, &mutex, &semEmpty, &semProduced, maxSlots);

  while(1) {
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

    // Get the mutex to the path
    if (sem_wait(mutex) == -1) {
      errExit("sem_wait(mutex)");
    }

    // Consume one file in the path
    consume(path);

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
