# Overview

This is an example usage of the Linux IPC (Inter-Process Communication)
mechanisms - message queue.  The system consists of an echo server and client,
which communicate via a message queue.

[POSIX message queue
APIs](https://man7.org/linux/man-pages/man7/mq_overview.7.html) are used in
this project.

# Build
```bash
# Build both server and clent 
$ make

# Build only server
$ make echo_server

# Build only client
$ make echo_client
```

# Server
Once started, the echo server keeps polling the message queue to receive
messages from clients.

## Usage
```
echo_server SERVER_NAME
```
Note that `SERVER_NAME` must be a name of the form `/somename`; that is, a
null-terminated string of up to `NAME_MAX` (i.e., 255) characters consisting of
an initial slash, followed by one or more characters, none of which are
slashes.

## Example usage
```bash
$ ./echo_server /s
```

# Client
An echo client talks to an echo server specified by server name. It reads
messages from standard input and send them to the server. Each message must be
less than 80-character long.

## Usage
```
echo_client SERVER_NAME
```

There are 3 special messages - "login", "logout" and "exit".
* "login" must be the first message sent to server before sending other
  messages to echo.
* After "logout" is sent, no messages will be echoed before the next "login".
* "exit" will exit the client process.

Example usage:
```bash
$ ./echo_client /s
Hi
Not logged in yet!
login
Logged in successfully!
Hello
Hello
World
World
logout
Logged out successfully!
Hi
Not logged in yet!
exit
```
