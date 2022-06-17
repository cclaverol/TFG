CC=g++
CFLAGS=-Wall -g

DEPS = model.h graph.h
OBJ = main.o model.o graph.o 


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tfg: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ)