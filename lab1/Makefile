CC=gcc
INCLUDE=-I./include/
SRCDIR :=./src
INCDIR :=./include
OBJDIR :=./obj
BINDIR :=./bin
LIBDIR :=./lib
DATADIR :=./data
SCRIPTS :=./scripts
SRCS := $(shell ls $(SRCDIR)/*.c)
#INCS := $(shell ls $(INCDIR)/*.h)
OBJS := $(patsubst %.c,%.o,$(SRCS))
OBJS := $(subst $(SRCDIR),$(OBJDIR),$(OBJS))
BINARY := ./bin/mm

all : plotData 
	@echo "Generating Result Graphs."
	gnuplot $(SCRIPTS)/function.plot
	gnuplot $(SCRIPTS)/largeValues.plot
	gnuplot $(SCRIPTS)/blockSize.plot
	

plotData : $(BINARY) 
	@[ -d $(DATADIR) ] || mkdir $(DATADIR)
	@echo "Computing Matrix Multiplications..."
	@echo "This will take a while..."
	@$(BINDIR)/mm  "fromMakefile"

$(BINARY) : $(OBJS) $(LIBS)
	@[ -d $(BINDIR) ] || mkdir $(BINDIR)
	$(CC) $(INCLUDE) $(OBJS) -o $(BINARY)

$(LIBS) : $(SRCS) $(INCS)
	@[ -d $(LIBDIR) ] || mkdir $(LIBDIR)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	$(CC) -c $< -o $@

results : $(DATADIR)/blockMultiplication.txt $(DATADIR)/simpleMultiplication.txt $(DATADIR)/largeArrayBlockMultiplication.txt $(DATADIR)/largeArraySimpleMultiplication.txt $(DATADIR)/blockSizeVariation.txt
	@echo "Generating Result Graphs."
	gnuplot $(SCRIPTS)/function.plot
	gnuplot $(SCRIPTS)/largeValues.plot
	gnuplot $(SCRIPTS)/blockSize.plot

clean : 
	@echo "Cleaning files."
	@rm -rf $(LIBDIR)
	@rm -rf $(BINDIR)
	@rm -rf $(DATADIR)
	@rm -rf $(OBJDIR)
	@echo "Files cleaned."

debug: 
	@echo $(OBJS)
	@echo $(SRCS)

