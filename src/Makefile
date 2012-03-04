app: mat.o parser.o matdsl.o main.o
	gcc -o "main" main.o mat.o matdsl.o parser.o 
	
mat.o: mat.c mat.h
	gcc -ansi -pedantic -Wall -g -c mat.c

parser.o: parser.c parser.h
	gcc -ansi -pedantic -Wall -g -c parser.c	

matdsl.o: matdsl.c matdsl.h  
	gcc -ansi -pedantic -Wall -g -c matdsl.c	

main.o: main.c 
	gcc -ansi -pedantic -Wall -g -c main.c 

clean:
	rm -rf *.o main