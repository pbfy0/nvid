DEBUG = FALSE
GCC = nspire-gcc
AS = nspire-as
GXX=nspire-g++
LD = nspire-ld-bflt
GCCFLAGS = -Wall -W -marm -Werror=missing-prototypes -Werror=missing-declarations -Werror=implicit-function-declaration
LDFLAGS = -L. -lvpx
ifeq ($(DEBUG),FALSE)
	GCCFLAGS += -O3
else
	GCCFLAGS += -Og -g
	LDFLAGS += --debug
endif
EXE = nvid.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $(DISTDIR)/$@ $(LDFLAGS)

clean:
	rm -f *.o *.elf *.gdb
	rm -f $(DISTDIR)/$(EXE)
