OBJDIR :=./obj
BINDIR :=./bin
LIBDIR :=./lib

all : ./lib/libmyOwnThread.so

./lib/libmyOwnThread.so : ./obj/queue.o ./obj/thread.o 
	@[ -d $(LIBDIR) ] || mkdir $(LIBDIR)
	gcc -o ./lib/libmyOwnThread.so ./obj/thread.o ./obj/queue.o -shared -g

./obj/queue.o : ./src/queue.c ./header/queue.h
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	gcc -c ./src/queue.c -I ./header/ -Wall -fpic -o ./obj/queue.o -g 

./obj/thread.o : ./src/thread.c ./header/thread.h ./header/queue.h 
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	gcc -c ./src/thread.c -I ./header/ -Wall -fpic -o ./obj/thread.o -g 


clean : 
	rm -rf obj/
	rm -rf lib/



