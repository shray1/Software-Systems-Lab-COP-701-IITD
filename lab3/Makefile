all : library tests

matrix : library 
	cd tests && $(MAKE) matrix

boundedBuffer : library 
	cd tests && $(MAKE) boundedBuffer

run : library tests
	cd tests && $(MAKE) run

clean : 
	cd utl && $(MAKE) clean
	cd tests && $(MAKE) clean


library : 
	$(MAKE) -C utl

tests : library
	$(MAKE) -C tests

plot :  
	cd tests && $(MAKE) plot
