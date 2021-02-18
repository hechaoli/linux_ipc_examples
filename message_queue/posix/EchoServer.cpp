#include <fcntl.h> // For O_* constants
#include <mqueue.h>
#include <string.h> // For memset
#include <sys/stat.h> // For mode constants

#include <iostream>
#include <unordered_set>

#include "Common.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " SERVER_NAME" << std::endl;
    return -1;
  }
  struct mq_attr attr = {
      .mq_flags = 0, /* Flags (ignored for mq_open()) */
      .mq_maxmsg = kMaxMessages, /* Max. # of messages on queue */
      .mq_msgsize = sizeof(struct Msg), /* Max. message size (bytes) */
      .mq_curmsgs =
          0 /* # of messages currently in queue (ignored for mq_open()) */
  };
  // Create a message queue to receive messages only
  const std::string serverName(argv[1]);
  mqd_t serverId = mq_open(serverName.c_str(), O_RDONLY | O_CREAT, 0660, &attr);
  if (serverId == -1) {
    errExit("mq_open(server)");
  }

  std::unordered_set<std::string> clients;
  // Start server
  while(1) {
    Msg msg;

    // Read the first message from the queue
    int bytes =
        mq_receive(serverId, (char*)&msg, sizeof(msg), NULL /*msg_prio*/);
    if (bytes == -1) {
      errExit("mq_receive");
    }
    if (bytes != sizeof(msg)) {
      std::cout << "ERROR: Received message size != expected message size!"
                << std::endl;
      continue;
    }

    std::string clientName(msg.name);
    char *text = msg.text;
    // Parse message by type
    switch(msg.type) {
      case MsgType::LOGIN:
        clients.insert(clientName);
        setMsgText(msg, "Logged in successfully!");
        break;
      case MsgType::ECHO:
        if (clients.find(clientName) == clients.end()) {
          setMsgText(msg, "Not logged in yet!");
        }
        break;
      case MsgType::LOGOUT:
        clients.erase(clientName);
        setMsgText(msg, "Logged out successfully!");
        break;
      default:
        setMsgText(msg, "Unknown message type: " + std::to_string(msg.type));
        break;
    }
    setMsgName(msg, serverName);

    mqd_t clientId = mq_open(clientName.c_str(), O_WRONLY);
    if (clientId == -1) {
      perror("mq_open(client)");
      continue;
    }
    // Send reply back to the client
    if (mq_send(clientId, (char*)&msg, sizeof(msg), 0 /*msg_prio*/) == -1) {
      perror("mq_send");
    }

    if (mq_close(clientId) == -1) {
      perror("mq_close(client)");
    }
  }
}
