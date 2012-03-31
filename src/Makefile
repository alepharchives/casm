assembler: lists.o parser.o asmdsl.o gendsl.o DoubleLinkedList.o main.o
	gcc -o "assembler"  main.o lists.o parser.o asmdsl.o gendsl.o DoubleLinkedList.o 
	
lists.o: lists.c lists.h
	gcc -ansi -pedantic -Wall -g -c lists.c

parser.o: parser.c parser.h
	gcc -ansi -pedantic -Wall -g -c parser.c	

asmdsl.o: asmdsl.c asmdsl.h  
	gcc -ansi -pedantic -Wall -g -c asmdsl.c
	
gendsl.o: gendsl.c gendsl.h  
	gcc -ansi -pedantic -Wall -g -c gendsl.c	

DoubleLinkedList.o: DoubleLinkedList.c DoubleLinkedList.h  
	gcc -ansi -pedantic -Wall -g -c DoubleLinkedList.c	
				
main.o: main.c 
	gcc -ansi -pedantic -Wall -g -c main.c 

clean:
	rm -rf *.o assembler