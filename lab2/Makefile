CC=gcc
INCLUDE=-I./include/
LDLIB := -lm -pthread
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
BINARY := ./bin/pi

all : $(BINARY)

$(BINARY) : $(OBJS) $(LIBS)
	@[ -d $(BINDIR) ] || mkdir $(BINDIR)
	$(CC) $(INCLUDE) $(OBJS) $(LDLIB) -o $(BINARY)

$(LIBS) : $(SRCS) $(INCS)
	@[ -d $(LIBDIR) ] || mkdir $(LIBDIR)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	$(CC) -c $< -o $@

run : $(BINARY)
	@echo "Generating Pi Values."
	@$(BINDIR)/pi 2 40000000


plot : 
	gnuplot $(SCRIPTS)/accuracyTerms.plot
	gnuplot $(SCRIPTS)/time.plot
	gnuplot $(SCRIPTS)/leibnizTime.plot
	gnuplot $(SCRIPTS)/wallisTime.plot

clean : 
	@echo "Cleaning files."
	@rm -rf $(LIBDIR)
	@rm -rf $(BINDIR)
	@rm -rf $(OBJDIR)
	@echo "Files cleaned."