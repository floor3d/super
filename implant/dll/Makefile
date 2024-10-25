CC=x86_64-w64-mingw32-gcc
.PHONY: clean build 
NAME=system32
CFLAGS=-Wall -fPIC -shared -ffreestanding -s 
LDFLAGS= -lkernel32 -lpsapi
all: build 

build:
	$(CC) $(CFLAGS) $(NAME).c ../include/http.o -lwinhttp -lmsvcrt -lkernel32 -lpsapi -ladvapi32 -luser32 -e DllMain  $(LDFLAGS) -o $(NAME).dll

clean:
	-rm *.exe 
	-rm *.dll 
