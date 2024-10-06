all: bin

clean:
	rm -rf ./bin

bin:
	mkdir -p ./bins
	
	x86_64-w64-mingw32-gcc -Wall -fpic -c include/http3.c -nostdlib -o ./include/http.o -lwinhttp
	x86_64-w64-mingw32-gcc -Os -flto -mconsole -o ./bins/implant.exe implant.c process.c include/http.o -lwinhttp
