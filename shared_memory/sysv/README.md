# Overview

This is an example usage of the Linux IPC (Inter-Process Communication)
mechanisms - shared memory. The system consists of a producer and a consumer,
which use semaphores to synchronize their actions and use shared memory as the
queue.

[System V shared memory
APIs](https://man7.org/linux/man-pages/man7/sysvipc.7.html) are used in
this project.

# Build
```bash
# Build both producer and consumer
$ make

# Build only producer
$ make producer

# Build only consumer
$ make consumer
```

# Producer
The producer keeps producing integers to a queue in shared memory until it
reaches the total number of integers to produce. While producing, if the queue
is full, then the producer must wait until a consumer consumes some numbers.

## Usage
```
producer PATH PROJ_ID TOTAL_PRODUCE
```

* `PATH` - Refer to an existing, accessible file. 
* `PROJ_ID` - A non-zero integer.
* `TOTAL_PRODUCE` - Total number of integers to produce before the producer
  exits.

## Example usage
```bash
$ touch /tmp/shm
$ ./producer /tmp/shm 1 100
```

# Consumer
A consumer keeps consuming numbers from the queue in shared memory until it
can't consume anything within the timeout (default timeout value is 5 seconds).
Multiple consumer processes is allowed.

## Usage
```
consumer PATH PROJ_ID
```

* `PATH` - Refer to an existing, accessible file. Must match the one used by
  the producer.
* `PROJ_ID` - A non-zero integer. Must match the one used by the producer

## Example usage
```bash
$ ./consumer /tmp/shm
```
