all: bin

clean:
	rm -rf ./bin

bin:
	mkdir -p ./bins
	x86_64-w64-mingw32-gcc -Os -flto -mconsole                      -o ./bins/implant.exe     implant.c process.c
