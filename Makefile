BDIR=bin
TARGETS=nomoves default hernan
RM=rm -f

.PHONY: clean $(TARGETS)

all: $(TARGETS)

$(TARGETS):
	@make -C $@


clean:
	$(RM) $(addprefix $(BDIR)/, $(TARGETS))

distclean: clean

help:
	@echo 'Makefile help:'
	@echo '  "make hernan" to generate hernan binary'
	@echo '  "make nomoves" to generate a binary with no moves'
	@echo '  "make default" to generate the default halite2 binary'
	@echo '  "make" or "make all" to generate all above binaries'


