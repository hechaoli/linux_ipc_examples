#include <fcntl.h> // For O_* constants
#include <mqueue.h>
#include <string.h> // For memset
#include <sys/stat.h> // For mode constants
#include <unistd.h> // For getpid()

#include <iostream>

#include "Common.h"

void usage(const std::string& cmd) {
    std::cout << "Usage: " << std::endl;
    std::cout << cmd << " SERVER_NAME" << std::endl;
}

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
  const std::string clientName = "/echo_client_" + std::to_string(getpid());
  // Create a client queue to receive replies from the server
  mqd_t clientId = mq_open(clientName.c_str(), O_RDONLY | O_CREAT, 0660, &attr);
  if (clientId == -1) {
    errExit("mq_open(client)");
  }

  const std::string serverName(argv[1]);
  // Open the server queue to send messages
  mqd_t serverId = mq_open(serverName.c_str(), O_WRONLY /* oflag */);
  if (serverId == -1) {
    errExit("mq_open(server)");
  }

  std::string line;
  Msg msg;
  setMsgName(msg, clientName);
  while (std::getline(std::cin, line)) {
    if (line.length() >= sizeof(msg.text)) {
      std::cout << "Message too long!" << std::endl;
      continue;
    }
    setMsgText(msg, line);
    if (line == "exit") {
      break;
    } else if (line == "login") {
      msg.type = MsgType::LOGIN;
    } else if (line == "logout") {
      msg.type = MsgType::LOGOUT;
    } else {
      msg.type = MsgType::ECHO;
    }

    // Send message to server
    if (mq_send(serverId, (char*)&msg, sizeof(msg), 0 /*msg_prio*/) == -1) {
      std::cout << "sizeof(msg): " << sizeof(msg);
      perror("mq_send");
      continue;
    }
    Msg reply;
    int bytes =
        mq_receive(clientId, (char*)&reply, sizeof(reply), NULL /*msg_prio*/);
    if (bytes == -1) {
      perror("msgrcv");
      continue;
    }
    if (bytes != sizeof(reply)) {
      std::cout << "ERROR: Received message size (" << bytes
                << ") != expected message size (" << sizeof(reply) << ")!"
                << std::endl;
      continue;
    }
    if (reply.type != msg.type) {
      std::cout << "ERROR: Received message type (" << reply.type
                << ") != expected message type (" << msg.type << ")!"
                << std::endl;
      continue;
    }
    const std::string name = std::string(reply.name);
    if (name != serverName) {
      std::cout << "ERROR: Received message name (" << name
                << ") != serverName (" << serverName << ")!" << std::endl;
      continue;
    }
    std::cout << std::string(reply.text) << std::endl;
  }
  if (mq_close(clientId) == -1) {
    errExit("mq_close(client)");
  }
  if (mq_unlink(clientName.c_str()) == -1) {
    errExit("mq_unlink");
  }
  if (mq_close(serverId) == -1) {
    errExit("mq_close(server)");
  }
  return 0;
}
