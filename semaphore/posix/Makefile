CC=g++
CFLAGS=-std=c++17

all: producer consumer

producer: Producer.cpp
	$(CC) $(CFLAGS) Producer.cpp -o producer -pthread
consumer: Consumer.cpp
	$(CC) $(CFLAGS) Consumer.cpp -o consumer -pthread

clean:
	rm -f producer consumer
