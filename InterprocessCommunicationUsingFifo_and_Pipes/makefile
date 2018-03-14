all: timeServer seeWhat ShowResult

timeServer: timeServer.o essentials.o 
	gcc timeServer.o essentials.o -o timeServer 

seeWhat :  seeWhat.o essentials.o 
	gcc  seeWhat.o essentials.o -o seeWhat

ShowResult : ShowResult.o essentials.o
	gcc	ShowResult.o essentials.o -o ShowResult


essentials.o: essentials.c
	gcc -c essentials.c 


timeServer.o: timeServer.c
	 gcc -c timeServer.c 

seeWhat.o: seeWhat.c
	gcc -c seeWhat.c 

ShowResult.o : ShowResult.c
	gcc -c ShowResult.c


clean:
	rm *.o seeWhat timeServer ShowResult *.txt logs/*
