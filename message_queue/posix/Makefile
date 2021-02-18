CC=g++

all: echo_server echo_client

echo_server: EchoServer.cpp
	$(CC) EchoServer.cpp -o echo_server -lrt
echo_client: EchoClient.cpp
	$(CC) EchoClient.cpp -o echo_client -lrt

clean:
	rm -f echo_server echo_client
