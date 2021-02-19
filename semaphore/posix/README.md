# Overview

This is an example usage of the Linux IPC (Inter-Process Communication)
mechanisms - semaphore. The system consists of a producer and a consumer, which
use semaphores to synchronize their actions.

[POSIX semaphore
APIs](https://man7.org/linux/man-pages/man7/sem_overview.7.html) are used in
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
producer PATH NAME MAX_SLOTS TOTAL_PRODUCE
```
* `PATH` - Path to a directory to produce files.
* `NAME` - Semaphore name of of the form `/somename`; that is, a
  null-terminated string of up to `NAME_MAX`-4 (i.e., 251) characters
  consisting of an initial slash, followed by one or more characters, none of
  which are slashes.
* `MAX_SLOTS` - Max number of files allowed at any given time. Once number of
  files produced reach this number without being consumed, the producer must
  wait until some files are consumed. This is the initial value of the
  semaphore.
* `TOTAL_PRODUCE` - Total number of files to produce before the producer exits.

## Example usage
```bash
$ mkdir /tmp/semaphore
$ ./producer /tmp/semaphore /test 10 100
```

# Consumer
A consumer keeps consuming (read and delete) files in the given directory until
it can't consume anything within the timeout (default timeout value is 5
seconds). Multiple consumer processes is allowed.

## Usage
```
consumer PATH NAME MAX_SLOTS
```

* `PATH` - Path to a directory to consume files.
* `NAME` - Semaphore name of of the form `/somename`; that is, a
  null-terminated string of up to `NAME_MAX`-4 (i.e., 251) characters
  consisting of an initial slash, followed by one or more characters, none of
  which are slashes.
* `MAX_SLOTS` - Max number of files allowed at any given time. The consumer
  needs this because a consumer could start before a producer. In that case,
  the consumer is responsible for creating the semaphore and this is the
  initial value of the semaphore.

## Example usage
```bash
$ ./consumer /tmp/semaphore /test 10
```
