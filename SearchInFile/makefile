all: exe

exe: list.o
	gcc list.o -o exe

list.o: list.c
	gcc -c list.c

clean:
	rm *.o exe
