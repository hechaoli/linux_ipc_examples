# Overview

This is an example usage of the Linux IPC (Inter-Process Communication)
mechanisms - message queue. The system consists of an echo server and client,
which communicate via a message queue.

[System V message queue
APIs](https://man7.org/linux/man-pages/man7/sysvipc.7.html) are used in
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
echo_server PATHNAME PROJECT_ID
```
* `PATHNAME` must refer to an existing, accessible file.
* `PROJECT_ID` must be a non-zero integer.

## Example usage
```bash
$ touch /tmp/echo
$ ./echo_server /tmp/echo 1
Server key: 17118454
```

# Client
An echo client talks to an echo server specified by `PATHNAME` and `PROJECT_ID`
or a `SERVER_KEY`. It reads messages from standard input and send them to the
server. Each message must be less than 80-character long.

## Usage
```
echo_client PATHNAME PROJECT_ID
```

Or

```
echo_client SERVER_KEY
```

`SERVER_KEY` is printed by the server when it starts.

There are 3 special messages - "login", "logout" and "exit".
* "login" must be the first message sent to server before sending other
  messages to echo.
* After "logout" is sent, no messages will be echoed before the next "login".
* "exit" will exit the client process.

## Example usage
```bash
$ ./echo_client /tmp/echo 1
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
