CC = gcc
OUTFILE = shredder
OUTDIR = bin
CFLAGS = -Wall -g -O2
OBJS = shredder.c
OBJSDIR = src

all: compile
.PHONY: clean

compile:
	@echo "Compiling shredder..."
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o$(OUTDIR)/$(OUTFILE) $(OBJSDIR)/$(OBJS)

clean:
	@echo "Cleaning shredder..."
	rm -rf $(OUTDIR)/*.dSYM $(OUTDIR)/*.o
