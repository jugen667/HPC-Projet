CC=g++
CCFLAGS= -Wall -O3 -std=c++11
LIBFLAGS= -Lnrc2/build/lib/ -lnrc -Inrc2/include -lpthread
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

EXEC= execHPC 


all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(SRC) $(CCFLAGS) $(LIBFLAGS) -o $@  

%.o: %.c
	$(CC) $(CCFLAGS) $(LIBFLAGS) -o $@ -c $<

.depend:
	gcc $(CCFLAGS) -MM $(SRC) > .depends
-include .depends

clean:
	rm -f $(OBJ) $(EXEC)



