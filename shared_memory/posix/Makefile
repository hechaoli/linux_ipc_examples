CC=g++
CFLAGS=-std=c++17
LIBS=-lrt -pthread

all: producer consumer

producer: Producer.cpp
	$(CC) $(CFLAGS) Producer.cpp -o producer $(LIBS)
consumer: Consumer.cpp
	$(CC) $(CFLAGS) Consumer.cpp -o consumer $(LIBS)

clean:
	rm -f producer consumer
