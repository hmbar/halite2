CXX=g++
RM=rm -f
CPPFLAGS=-g -c -I../hlt -I../ -I./ -std=c++11 -Wno-psabi 
LDFLAGS=-g

BIN=hernan
ODIR=../obj
BDIR=../bin
OBJS_COMMON=hlt/hlt_in.o hlt/map.o 
OBJS_HERNAN=hmb/halite2/planet.o hmb/halite2/ship.o hmb/halite2/round_manager.o
OBJS=$(BIN).o $(OBJS_COMMON) $(addprefix $(BIN)/, $(OBJS_HERNAN))
SRCS=$(OBJS:.o=.cpp)

all: $(BIN)

$(BIN): $(addprefix $(ODIR)/, $(OBJS)) 
	@mkdir -p $(BDIR)
	$(CXX) $(LDFLAGS) $? -o $(BDIR)/$@

$(addprefix $(ODIR)/, $(OBJS_COMMON)): 
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $(subst .o,.cpp,$(subst $(ODIR),..,$@))

$(addprefix $(ODIR)/$(BIN)/, $(OBJS_HERNAN)): 
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $(subst .o,.cpp,$(subst $(ODIR)/$(BIN)/,,$@))

$(ODIR)/%.o: %.cpp 
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $*.cpp

.PHONY: clean all

clean:
	$(RM) $(addprefix $(ODIR)/, $(OBJS))
	$(RM) $(BDIR)/$(BIN)
 
