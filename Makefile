MIX = mix
ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS = -g -O3 -ansi -pedantic -Wall -Wextra -Wno-unused-parameter -Ideps/hoedown/src -I$(ERLANG_PATH)

HOEDOWN_OBJS=\
	deps/hoedown/src/html.o \
	deps/hoedown/src/html_smartypants.o \
	deps/hoedown/src/html_blocks.o \
	deps/hoedown/src/buffer.o \
	deps/hoedown/src/autolink.o \
	deps/hoedown/src/stack.o \
	deps/hoedown/src/escape.o \
	deps/hoedown/src/markdown.o

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC

	ifeq ($(shell uname),Darwin)
		LDFLAGS += -dynamiclib -undefined dynamic_lookup
	endif
endif

.PHONY: all markdown clean

all: markdown

markdown:
	$(MIX) compile

priv/markdown.so: src/markdown.c
	$(MAKE) -C deps/hoedown libhoedown.a
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $(HOEDOWN_OBJS) src/markdown.c

clean:
	$(MIX) clean
	$(MAKE) -C deps/hoedown clean
	$(RM) deps/hoedown/libhoedown.a
	$(RM) priv/markdown.so
