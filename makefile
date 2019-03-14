CC = clang
CFLAGS = -std=gnu99 -pedantic -Wall -Werror
#TARGET = poker

poker: cards.o deck.o eval.o future.o input.o main.o
	$(CC) $(CFLAGS) -o poker cards.o deck.o eval.o future.o input.o main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

cards.o: cards.c cards.h
	$(CC) $(CFLAGS) -c cards.c

deck.o: deck.c deck.h cards.h 
	$(CC) $(CFLAGS) -c deck.c 

eval.o: eval.c eval.h 
	$(CC) $(CFLAGS) -c eval.c 

future.o: future.c future.h deck.h 
	$(CC) $(CFLAGS) -c future.c 

input.o: input.c input.h future.h deck.h 
	$(CC) $(CFLAGS) -c input.c 

.PHONY: clean
clean:
	rm -f *~ *.o poker
