#include <fstream>
#include <iostream>
#include <filesystem>

#include "Common.h"

namespace fs = std::filesystem;
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

void consume(const std::string& path) {
  fs::path filepath;
  for (const auto& entry : fs::directory_iterator(path)) {
    const auto path = entry.path();
    if (filepath.empty() ||
        std::stoi(path.filename()) < std::stoi(filepath.filename())) {
      filepath = path;
    }
  }
  if (filepath.empty()) {
    std::cout << "ERROR: No file for the consumer to consume!" << std::endl;
  } else {
    std::fstream file(filepath.string(), std::ios::in);
    if (file.is_open()){
      std::string line;
      while(getline(file, line)){
        std::cout << line << std::endl;
      }
      file.close();
      fs::remove(filepath);
    } else {
      std::cout << "ERROR: Failed to open " << filepath.string() << std::endl;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " PATH PROJ_ID MAX_SLOTS" << std::endl;
    return -1;
  }

  const std::string pathname = argv[1];
  const int projId = std::stoi(argv[2]);
  const int maxSlots = std::stoi(argv[3]);
  int semId = getSemSetId(pathname, projId, maxSlots);

  while (1) {
    if (!waitForProduced(semId)) {
      break;
    }

    // Consume one file in the path
    consume(pathname);

    notifyEmpty(semId);
  }
  return 0;
}
