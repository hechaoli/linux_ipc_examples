#pragma once

#include <assert.h>

#define MAX_NAME_LEN (64)
#define MAX_TEXT_LEN (80)

enum MsgType {
  INVALID = 0,
  LOGIN,
  ECHO,
  LOGOUT
};

struct Msg {
  long type; /* message type, must be > 0 */
  char name[MAX_NAME_LEN]; /* sender's name */
  char text[MAX_TEXT_LEN]; /* message content */
};

void errExit(const char* msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

// Default in /proc/sys/fs/mqueue/msg_max
constexpr int kMaxMessages = 10;

void strToCharArray(char buf[], int bufSize, const std::string& str) {
  assert(str.length() < bufSize);

  memset(buf, 0, bufSize);
  memcpy(buf, str.c_str(), str.length());
}

void setMsgText(Msg& msg, const std::string& text) {
  strToCharArray(msg.text, sizeof(msg.text), text);
}

void setMsgName(Msg& msg, const std::string& name) {
  strToCharArray(msg.name, sizeof(msg.name), name);
}
