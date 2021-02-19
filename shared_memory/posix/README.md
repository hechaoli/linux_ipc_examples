# Overview

This is an example usage of the Linux IPC (Inter-Process Communication)
mechanisms - shared memory. The system consists of a producer and a consumer,
which use semaphores to synchronize their actions and use shared memory as the
queue.

[POSIX shared memory
APIs](https://man7.org/linux/man-pages/man7/shm_overview.7.html) are used in
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
producer SEMAPHORE_NAME SHARED_MEM_NAME TOTAL_PRODUCE
```

* `SEMAPHORE_NAME` - Name of the semaphore. Must be the same for both producer
  and consumer. 
* `SEMAPHORE_NAME` - Name of the shared memory. Must be the same for both
  producer and consumer.
* `TOTAL_PRODUCE` - Total number of integers to produce before the producer
  exits.

## Example usage
```bash
$ ./producer sem shm 100
```

# Consumer
A consumer keeps consuming numbers from the queue in shared memory until it
can't consume anything within the timeout (default timeout value is 5 seconds).
Multiple consumer processes is allowed.

## Usage
```
consumer SEMAPHORE_NAME SHARED_MEM_NAME
```

* `SEMAPHORE_NAME` - Name of the semaphore. Must be the same for both producer
  and consumer. 
* `SEMAPHORE_NAME` - Name of the shared memory. Must be the same for both
  producer and consumer.

## Example usage
```bash
$ ./consumer sem shm
```
