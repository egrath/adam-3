CC      = cc
CFLAGS  = -c -std=c99 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -pedantic-errors -g -D_GNU_SOURCE
LDFLAGS = 

# debugging and profiling
DEBUG	= 0
PROFILE = 0

SRCDIR  = .
BLDDIR  = build

OBJS    = $(addprefix $(BLDDIR)/, main.o adm3a.o commandline.o renderer.o config.o helper.o)
EXE     = $(BLDDIR)/adam-3

# build debug?
ifeq ($(DEBUG), 1)
	CFLAGS    += -DDEBUG
endif

ifeq ($(PROFILE), 1)
	CFLAGS    += -DPROFILE
endif

# operating system specific compiler flags
# we keep all the options separate, as we want to have
# more control over compile flags and object files
# just in case we need it later on
UNAME   = $(shell uname -s)
ifeq ($(UNAME), Darwin)
	CFLAGS    += $(shell pkg-config --cflags sdl3) 
	LDFLAGS   += $(shell pkg-config --libs sdl3)
	OBJS      += $(addprefix $(BLDDIR)/, pty_unix.o)
endif

ifeq ($(UNAME), Haiku)
	CFLAGS    += $(shell pkg-config --cflags sdl3)
	LDFLAGS   += $(shell pkg-config --libs sdl3)
	OBJS      += $(addprefix $(BLDDIR)/, pty_unix.o)

endif

ifeq ($(UNAME), Linux)
	CFLAGS    += $(shell pkg-config --cflags sdl3)
	LDFLAGS   += $(shell pkg-config --libs sdl3)
	OBJS      += $(addprefix $(BLDDIR)/, pty_unix.o)
endif

ifneq ($(findstring MINGW64,$(UNAME)),)
	CFLAGS    += $(shell pkg-config --cflags sdl3)
	LDFLAGS   += $(shell pkg-config --libs sdl3) -mconsole
	OBJS      += $(addprefix $(BLDDIR)/, pty_windows.o)
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
