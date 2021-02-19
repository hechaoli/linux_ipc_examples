# Overview

This is an example usage of the Linux IPC (Inter-Process Communication)
mechanisms - semaphore. The system consists of a producer and a consumer, which
use semaphores to synchronize their actions.

[System V semaphore
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
The producer keeps producing files to a given directory until it reaches the
total number of files to produce. While producing files, if it reaches max
number of files allowed at any given time. It has to wait until a consumer
consumes some files.

## Usage
```
producer PATH PROJ_ID MAX_SLOTS TOTAL_PRODUCE
```
* `PATH` - Path to a directory to produce files.
* `PROJ_ID` - A non-zero integer value.
* `MAX_SLOTS` - Max number of files allowed at any given time. Once number of
  files produced reach this number without being consumed, the producer must
  wait until some files are consumed. This is the initial value of the
  semaphore.
* `TOTAL_PRODUCE` - Total number of files to produce before the producer exits.

## Example usage
```bash
$ mkdir /tmp/semaphore
$ ./producer /tmp/semaphore 1 10 100
```

# Consumer
A consumer keeps consuming (read and delete) files in the given directory until
it can't consume anything within the timeout (default timeout value is 5
seconds). Multiple consumer processes is allowed.

## Usage
```
consumer PATH PROJ_ID MAX_SLOTS
```

* `PATH` - Path to a directory to consume files.
* `PROJ_ID` - A non-zero integer value.
* `MAX_SLOTS` - Max number of files allowed at any given time. The consumer
  needs this because a consumer could start before a producer. In that case,
  the consumer is responsible for creating the semaphore and this is the
  initial value of the semaphore.

## Example usage
```bash
$ ./consumer /tmp/semaphore 1 10
```
