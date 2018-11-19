# Compilation variables
CC = gcc
CFLAGS = -std=c11 -D_XOPEN_SOURCE=700 -O2 -Wall -Wextra -Wformat=2 $(shell pkg-config --cflags glib-2.0)
OBJFLAGS = -c
LDLIBS = $(shell pkg-config --libs glib-2.0)
TARGET = main

# Name prefixes for dependencies
LIB_DIR = simple_http/
PREFIX_NAME = $(LIB_DIR)simple_http_

# Dependencies
APP = $(PREFIX_NAME)app
CONFIG = $(PREFIX_NAME)server_config
DEPENDENCIES = $(APP).o $(CONFIG).o

# Compile function with required flasg
define COMPILE
$(1).o: $(1).c
	$(CC) $($(1)).c $(CFLAGS) $(LDLIBS) $(OBJFLAGS)
endef

.DEFAULT: all
.PHONY: all
all: $(TARGET)

# Executable
$(TARGET): $(TARGET).o $(DEPENDENCIES)
	$(CC) $(TARGET).o $(DEPENDENCIES) -o $(TARGET).exe $(LDLIBS)

# Compilation
$(call COMPILE,TARGET)
$(call COMPILE,APP)
$(call COMPILE,CONFIG)

# Removal of .o files
clean:
	rm -f *.o
	rm -f simple_http/*.o
	rm -f simple_http/types/*.o

# Removal of executable
distclean: clean
	rm -f $(TARGET).exe
