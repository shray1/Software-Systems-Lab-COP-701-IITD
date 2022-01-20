OBJDIR :=./obj
BINDIR :=./bin
LIBDIR :=./lib

all : ./lib/libmlpClassifier.so ./bin/mlp


./bin/mlp : ./obj/mlp.o
	@[ -d $(BINDIR) ] || mkdir $(BINDIR)
	gcc -o ./bin/mlp -g -I ./include/ ./obj/mlp.o  -L ./lib/ -lmlpClassifier -lm


./obj/mlp.o : ./src/test.c ./lib/libmlpClassifier.so
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)/lib/
	gcc -c ./src/test.c -g -I ./include/ -o ./obj/mlp.o -L ./lib/ -lmlpClassifier -lm

./lib/libmlpClassifier.so : ./obj/mlpClassifier.o  
	@[ -d $(LIBDIR) ] || mkdir $(LIBDIR)
	gcc -o ./lib/libmlpClassifier.so ./obj/mlpClassifier.o  -shared -g

./obj/mlpClassifier.o : ./src/mlpClassifier.c ./include/mlpClassifier.h
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	gcc -c ./src/mlpClassifier.c -I ./include/ -Wall -fpic -o ./obj/mlpClassifier.o -g 

clean : 
	rm -rf obj/
	rm -rf lib/
	rm -rf bin/
