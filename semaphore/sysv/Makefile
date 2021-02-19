CC=g++
CFLAGS=-std=c++17

all: producer consumer

producer: Producer.cpp
	$(CC) $(CFLAGS) Producer.cpp -o producer
consumer: Consumer.cpp
	$(CC) $(CFLAGS) Consumer.cpp -o consumer

clean:
	rm -f producer consumer
