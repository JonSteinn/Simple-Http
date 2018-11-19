CC = gcc
CFLAGS = -std=c11 -D_XOPEN_SOURCE=700 -O2 -Wall -Wextra -Wformat=2 $(shell pkg-config --cflags glib-2.0)
OBJFLAGS = -c
LDLIBS = $(shell pkg-config --libs glib-2.0)
TARGET = main

LIB_DIR = simple_http
TEST = $(LIB_DIR)/test

DEPENDENCIES = $(TEST).o

define COMPILE
$(1).o: $(1).c
	$(CC) $($(1)).c $(CFLAGS) $(LDLIBS) $(OBJFLAGS)
endef

.DEFAULT: all
.PHONY: all
all: $(TARGET)

$(TARGET): $(TARGET).o $(DEPENDENCIES)
	$(CC) $(TARGET).o $(DEPENDENCIES) -o $(TARGET) $(LDLIBS)

$(call COMPILE,TARGET)
$(call COMPILE,TEST)

clean:
	rm -f *.o

distclean: clean
	rm -f $(TARGET)
