CC=clang
CFLAGS=-std=c11 -O3

ODIR=obj
LDIR =../lib

LIBS=-lm

_OBJ = utils.o vm.o stack.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

vm: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
