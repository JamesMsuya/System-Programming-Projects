all: exe

exe: listdir.o
	gcc listdir.o -o exe

list.o: listdir.c
	gcc -c listdir.c

clean:
	rm *.o exe log.txt
