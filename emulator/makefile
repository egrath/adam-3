CC      = cc
CFLAGS  = -c -std=c99 -Wall -Wextra -Wno-unused-function -pedantic-errors -g -DDEBUG
LDFLAGS = -lSDL3

SRCDIR  = .
BLDDIR  = build

OBJS    = $(addprefix $(BLDDIR)/, main.o adm3a.o pty_unix.o)
EXE     = $(BLDDIR)/adam

$(EXE): $(OBJS)
		@mkdir -p $(BLDDIR)
		$(CC) $^ -o $@ $(LDFLAGS)

$(BLDDIR)/%.o: %.c
		@mkdir -p $(BLDDIR)
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf $(EXE) $(OBJS)
