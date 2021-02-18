CC=g++

all: echo_server echo_client

echo_server: EchoServer.cpp
	$(CC) EchoServer.cpp -o echo_server
echo_client: EchoClient.cpp
	$(CC) EchoClient.cpp -o echo_client

clean:
	rm -f echo_server echo_client
