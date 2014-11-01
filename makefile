ALL = generator_pismenok lovci_a_zberaci maliari pekari skladnici slachtici_a_poddani

all: $(ALL)

%: %.c
	gcc -o $@ -g -O3 -Wall $< -lm -lpthread

clean:
	-rm $(ALL)