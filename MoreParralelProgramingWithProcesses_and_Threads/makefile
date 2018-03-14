all: exe

exe: listdir.o
	gcc listdir.o -pthread -o grepSh

list.o: listdir.c
	gcc -c listdir.c 

clean:
	rm *.o grepSh log.txt
