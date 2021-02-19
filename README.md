# Overview
This repo contains example code of using Linux IPC (Inter-Process
Communication) mechanisms including message queue, semaphore and shared memory.
For each mechanism, there is one example of POSIX API and one example of system
V API.

# Message queue
The example code is a simple echo server/client system.
* [POSIX API
  implementation](https://github.com/hechaoli/linux_ipc_examples/tree/main/message_queue/posix).
* [System V API
  implementation](https://github.com/hechaoli/linux_ipc_examples/tree/main/message_queue/sysv).

# Semaphore
The example code is a simple producer/consumer system. The producer produces
files into a directory and the consumer consumes files from it.
* [POSIX API
  implementation](https://github.com/hechaoli/linux_ipc_examples/tree/main/semaphore/posix).
* [System V API
  implementation](https://github.com/hechaoli/linux_ipc_examples/tree/main/semaphore/sysv).

# Shared memory
Similar to semaphore example, this example is also simple producer/consumer
system. Different from the semaphore example, instead of using a directory as
the queue, this example uses shared memory as the queue.
* [POSIX API
  implementation](https://github.com/hechaoli/linux_ipc_examples/tree/main/shared_memory/posix).
* [System V API
  implementation](https://github.com/hechaoli/linux_ipc_examples/tree/main/shared_memory/sysv).
