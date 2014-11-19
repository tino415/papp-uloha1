ALL = generator_pismenok lovci_a_zberaci maliari pekari slachtici_a_poddani repeatable_barrier maliari2 slp2 3slp sportovci

all: $(ALL)

%: %.c
	gcc -o $@ -g -O3 -Wall $< -lm -lpthread

clean:
	-rm $(ALL)
