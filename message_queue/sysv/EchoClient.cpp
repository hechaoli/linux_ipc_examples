#include <string.h> // For memset
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <iostream>

#include "Common.h"

void usage(const std::string &cmd) {
  std::cout << "Usage: " << std::endl;
  std::cout << cmd << " PATHNAME PROJECT_ID or" << std::endl;
  std::cout << cmd << " SERVER_KEY" << std::endl;
}

int main(int argc, char *argv[]) {
  key_t key;
  if (argc == 2) {
    key = strtol(argv[1], NULL /* endptr */, 0);
  } else if (argc == 3) {
    const std::string pathname = argv[1];
    const std::string projectId = argv[2];
    // Get server key by pathname and project id
    key = ftok(pathname.c_str(), std::stoi(projectId));
    if (key == -1) {
      errExit("ftok");
    }
  } else {
    usage(argv[0]);
    return -1;
  }

  // Get server queue id
  int serverId = msgget(key, 0 /* msgflg */);
  if (serverId == -1) {
    errExit("msgget(key, 0)");
  }
  // Create a client queue to receive replies from the server
  int clientId = msgget(IPC_PRIVATE, 0660);
  if (clientId == -1) {
    errExit("msgget(IPC_PRIVATE, 0660)");
  }

  std::string line;
  Msgbuf msgbuf;
  msgbuf.msg.id = clientId;
  while (std::getline(std::cin, line)) {
    if (line.length() >= sizeof(msgbuf.msg.text)) {
      std::cout << "Message too long!" << std::endl;
      continue;
    }
    memset(msgbuf.msg.text, 0, sizeof(msgbuf.msg.text));
    if (line == "exit") {
      break;
    } else if (line == "login") {
      msgbuf.type = MsgType::LOGIN;
    } else if (line == "logout") {
      msgbuf.type = MsgType::LOGOUT;
    } else {
      msgbuf.type = MsgType::ECHO;
      memcpy(msgbuf.msg.text, line.c_str(), line.length());
    }

    // Send message to server
    if (msgsnd(serverId, &msgbuf, sizeof(struct Msg), 0 /*msgflg*/) == -1) {
      perror("client: msgsnd");
      continue;
    }
    Msgbuf reply;
    int bytes = msgrcv(clientId, &reply, sizeof(struct Msg), 0 /*msgtyp*/,
                       0 /*msgflg*/);
    if (bytes == -1) {
      perror("msgrcv");
      continue;
    }
    if (bytes != sizeof(struct Msg)) {
      std::cout << "ERROR: Received message size != expected message size!"
                << std::endl;
      continue;
    }
    if (reply.type != msgbuf.type) {
      std::cout << "ERROR: Received message type (" << reply.type
                << ")!= expected message type (" << msgbuf.type << ")!"
                << std::endl;
      continue;
    }
    if (reply.msg.id != serverId) {
      std::cout << "ERROR: Received message id (" << reply.msg.id
                << ") != serverId (" << serverId << ")!" << std::endl;
      continue;
    }
    std::cout << std::string(reply.msg.text) << std::endl;
  }
  if (msgctl(clientId, IPC_RMID, NULL /* buf */) == -1) {
    errExit("msgctl");
  }
  return 0;
}
