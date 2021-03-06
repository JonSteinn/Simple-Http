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
SOCKET = $(PREFIX_NAME)server_socket
POLL = $(PREFIX_NAME)server_poll
UTIL = $(PREFIX_NAME)util
CLI_HANDLER = $(PREFIX_NAME)client_handler
RESPONSE = $(PREFIX_NAME)server_response
IO = $(PREFIX_NAME)server_io
S_CODE = $(PREFIX_NAME)status_codes
REQUEST = $(PREFIX_NAME)client_request
ROUTE = $(PREFIX_NAME)server_routes
STATIC = $(PREFIX_NAME)server_static_files
LOG = $(PREFIX_NAME)server_logging
PRINT = $(PREFIX_NAME)server_prints
# Combination
DEPENDENCIES = \
	$(APP).o \
	$(CONFIG).o \
	$(SOCKET).o \
	$(POLL).o \
	$(UTIL).o \
	$(CLI_HANDLER).o \
	$(RESPONSE).o \
	$(IO).o \
	$(S_CODE).o \
	$(REQUEST).o \
	$(ROUTE).o \
	$(STATIC).o \
	$(LOG).o \
	$(PRINT).o

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
$(call COMPILE,SOCKET)
$(call COMPILE,POLL)
$(call COMPILE,UTIL)
$(call COMPILE,CLI_HANDLER)
$(call COMPILE,RESPONSE)
$(call COMPILE,IO)
$(call COMPILE,S_CODE)
$(call COMPILE,REQUEST)
$(call COMPILE,ROUTE)
$(call COMPILE,STATIC)
$(call COMPILE,LOG)
$(call COMPILE,PRINT)

# Removal of .o files
clean:
	rm -f *.o
	rm -f simple_http/*.o
	rm -f simple_http/types/*.o

# Removal of executable
distclean: clean
	rm -f $(TARGET).exe
