sim : sim.o
	gcc -o sim sim.o -lm 

sim.o : sim.c
	gcc -c sim.c -Wall -Wextra -pedantic -lm 

run : sim
	./sim $(num_sets) $(num_blocks) $(size_block) $(eviction_policy) $(trace_file) $(output_file)

clean : 
	rm -rf run sim.o sim