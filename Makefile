CC = gcc
CXX = g++
DEBUG = gdb

STANDARD = -std=c11

OPTI = -O3

WARNINGS = -Wall -Wextra -Wpedantic -Werror

HEADERDIR = src/include
LIBDIR = src/lib

LIBC_CPP = -static-libstdc++ -static-libgcc
HIDEWINDOW = -mwindows

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

HEADERS = $(call rwildcard,src/include,*.h)
EXTERNAL_HEADERS = $(call rwildcard,src/include/external,*.h)
INTERNAL_HEADERS = $(filter-out $(EXTERNAL_HEADERS),$(HEADERS))
SOURCES = $(call rwildcard,src/c,*.c)
COMPILED = $(subst .c,.o,$(SOURCES)) 

QUIET = > nul 2>&1

LIB = -lws2_32

all: bin/client.exe bin/server.exe

bin/client.exe: client.o $(COMPILED)
	$(CC) client.o $(COMPILED) -o bin/client.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: client_debug
client_debug: client.c $(SOURCES)
	$(CC) -ggdb3 client.c $(SOURCES) -o bin/client_debug.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: debug_client
debug_client: client_debug
	$(DEBUG) bin/client_debug.exe

.PHONY: run_client
run_client: bin/client.exe
	./bin/client.exe

client.o: client.c
	$(CC) -c client.c -o client.o -I $(HEADERDIR) $(WARNING) $(STANDARD) $(OPTI)

bin/server.exe: server.o $(COMPILED)
	$(CC) server.o $(COMPILED) -o bin/server.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: server_debug
server_debug: server.c $(SOURCES)
	$(CC) -ggdb3 server.c $(SOURCES) -o bin/server_debug.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: debug_server
debug_server: server_debug
	$(DEBUG) bin/server_debug.exe

.PHONY: run_server
run_server: bin/server.exe
	./bin/server.exe
server.o: server.c
	$(CC) -c server.c -o server.o -I $(HEADERDIR) $(WARNING) $(STANDARD) $(OPTI)

src/c/%.o: src/c/%.c
	$(CC) -c $< -o $@ -I $(HEADERDIR) $(WARNINGS) $(STANDARD) $(OPTI)

.PHONY: clean
.SILENT: clean
clean:
	-del main.o $(QUIET)
	-del /S src\c\*.o $(QUIET)
	-del *.o $(QUIET)

.PHONY: cleanmore
.SILENT: cleanmore
cleanmore: clean
	-del bin\*.exe $(QUIET)
