all: main

main.o:
	gcc -c main.c -ansi -pedantic-errors -Wall

main:	main.o
	gcc main.o -o listdir
clean:
	rm *.o listdir
