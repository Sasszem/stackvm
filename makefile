
all: tests asm
.PRECIOUS: build/%.o	

build/%.o: src/%.cpp dirs
	g++ -c -o $@ $< 

build/%.o: tests/%.cpp dirs
	g++ -c -o $@ $<

bin/%: build/%.o build/VM.o dirs
	g++ -o $@ build/VM.o $<

dirs:
	mkdir -p build
	mkdir -p bin

clean:
	rm -rf bin
	rm -rf build

tests: bin/test_static bin/test_open

asm: bin/test.bin bin/rot.bin bin/string.bin

bin/%.bin: asm/%.asm
	python compiler.py $< $@
