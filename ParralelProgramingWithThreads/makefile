all: exe

exe: listdir.o
	gcc listdir.o -pthread -o grephTh

list.o: listdir.c
	gcc -c listdir.c 

clean:
	rm *.o grephTh log.txt
