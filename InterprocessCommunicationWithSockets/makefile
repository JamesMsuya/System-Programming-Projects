all: clients server serverPool

clients: clients.o essentials.o 
	gcc clients.o essentials.o -o clients -pthread -lm

server :  server.o essentials.o 
	gcc  server.o essentials.o -o server -pthread -lm

serverPool :  serverPool.o essentials.o 
	gcc  serverPool.o essentials.o -o serverPool -pthread -lm

essentials.o: essentials.c
	gcc -c essentials.c 


clients.o: clients.c
	 gcc -c clients.c 

serverPool.o: serverPool.c
	gcc -c serverPool.c 


server.o: server.c
	gcc -c server.c 



clean:
	rm *.o clients server serverPool *.txt logsServer/*.txt logsClients/*.txt 
