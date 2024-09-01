CC = g++
CFLAGS = -I. -std=c++11 -Wall

build: client

prerequisites:
	sudo apt-get install -y nlohmann-json3-dev

client: client.cpp requests.cpp helpers.cpp
	$(CC) $(CFLAGS) -o client client.cpp requests.cpp helpers.cpp

run: client
	./client

clean:
	rm -f *.o client
