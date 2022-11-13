all: src tests

src:
	make -C ./src

tests: src
	make -C ./tests

clean:
	make clean -C ./src
	make clean -C ./tests

.PHONY: all tests clean src