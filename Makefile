MIX = mix
ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)

ifeq (, $(shell find . -name deps))
	HOEDOWN_PATH = ../hoedown
else
	HOEDOWN_PATH = deps/hoedown
endif

CFLAGS = -g -O3 -ansi -pedantic -Wall -Wextra -Wno-unused-parameter -I$(HOEDOWN_PATH)/src -I$(ERLANG_PATH)

HOEDOWN_OBJS=\
	$(HOEDOWN_PATH)/src/html.o \
	$(HOEDOWN_PATH)/src/html_smartypants.o \
	$(HOEDOWN_PATH)/src/html_blocks.o \
	$(HOEDOWN_PATH)/src/buffer.o \
	$(HOEDOWN_PATH)/src/autolink.o \
	$(HOEDOWN_PATH)/src/stack.o \
	$(HOEDOWN_PATH)/src/escape.o \
	$(HOEDOWN_PATH)/src/markdown.o

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
	$(MAKE) -C $(HOEDOWN_PATH)
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $(HOEDOWN_OBJS) src/markdown.c

clean:
	$(MIX) clean
	$(MAKE) -C $(HOEDOWN_PATH) clean
	$(RM) priv/markdown.so
