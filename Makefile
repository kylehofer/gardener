all: src tests

src:
	make -C ./src

tests:
	make -C ./tests

# run_tests:


clean:
	make clean -C ./src
	make clean -C ./tests

.PHONY: all tests clean src