OBJDIR :=./obj
BINDIR :=./bin
LIBDIR :=./lib
SCRIPTS :=./scripts/
all : boundedBuffer matrix test1 test2 test3 test4

run : matrix boundedBuffer
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/ ./bin/matrix
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/ ./bin/boundedBuffer

plot :
	gnuplot $(SCRIPTS)/time.plot

matrix : ./bin/matrix 
	
boundedBuffer : ./bin/boundedBuffer

test1 : ./bin/test1

test2 : ./bin/test2

test3 : ./bin/test3

test4 : ./bin/test4



./bin/matrix : ./lib/libmyOwnThread.so ./src/mm.c ./include/thread.h
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/
	gcc -c ./src/mm.c -I ./include -o ./obj/mm.o -g 
	gcc -o ./bin/matrix ./obj/mm.o -I ./include -lmyOwnThread -L ./lib -g  -lpthread

./bin/boundedBuffer : ./lib/libmyOwnThread.so ./obj/boundedBuffer.o 
	@[ -d $(BINDIR) ] || mkdir $(BINDIR) 
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/
	gcc -o ./bin/boundedBuffer ./obj/boundedBuffer.o -I ./include/ -lmyOwnThread -L ./lib -g

./obj/boundedBuffer.o  : ./src/boundedBuffer.c ./include/thread.h ./include/queue.h
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	gcc -c ./src/boundedBuffer.c -I ./include -o ./obj/boundedBuffer.o -g 

./lib/libmyOwnThread.so : 
	rm -rf ./lib/
	cp -r ../utl/lib ./

./bin/test1 : ./lib/libmyOwnThread.so ./src/test1.c ./include/thread.h
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	@export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/
	gcc -c ./src/test1.c -I ./include -o ./obj/test1.o -g 
	gcc -o ./bin/test1 ./obj/test1.o -I ./include -lmyOwnThread -L ./lib -g

./bin/test2 : ./lib/libmyOwnThread.so ./src/test2.c ./include/thread.h
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	@export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/
	gcc -c ./src/test2.c -I ./include -o ./obj/test2.o -g 
	gcc -o ./bin/test2 ./obj/test2.o -I ./include -lmyOwnThread -L ./lib -g

./bin/test3 : ./lib/libmyOwnThread.so ./src/test3.c ./include/thread.h
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	@export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/
	gcc -c ./src/test3.c -I ./include -o ./obj/test3.o -g 
	gcc -o ./bin/test3 ./obj/test3.o -I ./include -lmyOwnThread -L ./lib -g

./bin/test4 : ./lib/libmyOwnThread.so ./src/test4.c ./include/thread.h
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	@export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/
	gcc -c ./src/test4.c -I ./include -o ./obj/test4.o -g 
	gcc -o ./bin/test4 ./obj/test4.o -I ./include -lmyOwnThread -L ./lib -g


clean : 
	rm -rf ./obj/
	rm -rf ./lib/
	rm -rf ./bin/
