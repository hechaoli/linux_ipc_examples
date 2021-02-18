#include <assert.h>
#include <string.h> // For memset
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <iostream>
#include <unordered_set>

#include "Common.h"

void setMsgText(Msgbuf &msgbuf, const std::string &str) {
  int size = sizeof(msgbuf.msg.text);
  assert(str.length() < size);

  char *text = msgbuf.msg.text;
  memset(text, 0, size);
  memcpy(text, str.c_str(), str.length());
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " PATHNAME PROJECT_ID" << std::endl;
    return -1;
  }
  const std::string pathname = argv[1];
  const std::string projectId = argv[2];

  // Generate System V IPC key
  key_t key = ftok(pathname.c_str(), std::stoi(projectId));
  if (key == -1) {
    errExit("ftok");
  }

  // Create a message queue with permission 0660
  int serverId = msgget(key, IPC_CREAT | 0660);
  if (serverId == -1) {
    errExit("msgget");
  }
  std::cout << "Server key: " << key << std::endl;

  std::unordered_set<int> clients;
  // Start server
  while (1) {
    Msgbuf msgbuf;

    // Read the first message from the queue
    int bytes = msgrcv(serverId, &msgbuf, sizeof(struct Msg), 0 /*msgtyp*/,
                       0 /*msgflg*/);
    if (bytes == -1) {
      errExit("msgrcv");
    }
    if (bytes != sizeof(struct Msg)) {
      std::cout << "ERROR: Received message size != expected message size!"
                << std::endl;
      continue;
    }

    // Parse message by type
    int clientId = msgbuf.msg.id;
    char *text = msgbuf.msg.text;
    int size = sizeof(msgbuf.msg.text);
    switch (msgbuf.type) {
    case MsgType::LOGIN:
      clients.insert(clientId);
      setMsgText(msgbuf, "Logged in successfully!");
      break;
    case MsgType::ECHO:
      if (clients.find(clientId) == clients.end()) {
        setMsgText(msgbuf, "Not logged in yet!");
      }
      break;
    case MsgType::LOGOUT:
      clients.erase(clientId);
      setMsgText(msgbuf, "Logged out successfully!");
      break;
    default:
      setMsgText(msgbuf,
                 "Unknown message type: " + std::to_string(msgbuf.type));
      break;
    }
    msgbuf.msg.id = serverId;
    // Send reply back to the client
    if (msgsnd(clientId, &msgbuf, sizeof(struct Msg), 0) == -1) {
      perror("msgsnd");
    }
  }
  return 0;
}
