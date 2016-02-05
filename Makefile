download:download.c
	g++ -o download download.c
kill:kill.c
	 g++ -o kill kill.c
multithread: multithread.c
	g++ -o multithread multithread.c -pthread
all: client server download
client: client.c
	gcc -o client client.c
server: server.c
	gcc -o server server.c
clean: 
	rm client server

