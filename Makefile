CXX=g++
RM=rm -f
CPPFLAGS=-g -c -Ihlt -I. -std=c++11 -Wno-psabi 
LDFLAGS=-g

ODIR=obj
BDIR=bin
OBJS_COMMON=$(ODIR)/hlt_in.o $(ODIR)/map.o
OBJS=$(ODIR)/hernan.o $(ODIR)/nomoves.o $(ODIR)/default.o 

all: hernan nomoves default

dirs:
	@mkdir -p $(ODIR)
	@mkdir -p $(BDIR)

$(OBJS_COMMON): dirs 
	$(CXX) $(CPPFLAGS) hlt/$(notdir $(basename $@)).cpp -o $(ODIR)/$(notdir $(basename $@)).o

$(OBJS): dirs 
	$(CXX) $(CPPFLAGS) $(notdir $(basename $@))/$(notdir $(basename $@)).cpp -o $(ODIR)/$(notdir $(basename $@)).o


%: dirs $(ODIR)/%.o $(OBJS_COMMON)
	$(CXX) $(LDFLAGS) -o $(BDIR)/$@ $(ODIR)/$@.o $(OBJS_COMMON) 

.PHONY: clean

clean:
	$(RM) $(OBJS)
	$(RM) $(OBJS_COMMON)

distclean: clean

help:
	@echo 'Makefile help:'
	@echo '  "make hernan" to generate hernan binary'
	@echo '  "make nomoves" to generate a binary with no moves'
	@echo '  "make default" to generate the default halite2 binary'
	@echo '  "make" or "make all" to generate all above binaries'


