#include <stdio.h> // For perror
#include <stdlib.h> // For exit

struct Msg {
  int id;
  char text[80];
};

struct Msgbuf {
  long type; /* message type, must be > 0 */
  Msg msg;   /* message data */
};

enum MsgType {
  INVALID = 0,
  LOGIN,
  ECHO,
  LOGOUT
};

void errExit(const char* msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
