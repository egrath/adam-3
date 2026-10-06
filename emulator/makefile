CC      = cc
CFLAGS  = -c -std=c99 -Wall -Wextra -Wno-unused-function -pedantic-errors -g -DDEBUG
LDFLAGS = 

SRCDIR  = .
BLDDIR  = build

OBJS    = $(addprefix $(BLDDIR)/, main.o adm3a.o pty_unix.o commandline.o)
EXE     = $(BLDDIR)/adam-3

# operating system specific compiler flags
UNAME   = $(shell uname -s)
ifeq ($(UNAME), Darwin)
	CFLAGS    += $(shell pkg-config --cflags sdl3)
	LDFLAGS   += $(shell pkg-config --libs sdl3)
endif

# default target
.PHONY: default
default: $(EXE);

$(EXE): $(OBJS)
		@mkdir -p $(BLDDIR)
		$(CC) $^ -o $@ $(LDFLAGS)

$(BLDDIR)/%.o: %.c
		@mkdir -p $(BLDDIR)
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf $(EXE) $(OBJS)
