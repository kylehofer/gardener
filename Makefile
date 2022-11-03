all: src tests

src:
	make -C ./src

tests:
	make -C ./tests

copy:
	 sshpass -p $(PASSWORD) rsync -rav -e ssh --exclude='build/' --exclude='.git/' --exclude='temp/' ./ $(USER)@$(HOST):$(TARGET)

clean:
	make clean -C ./src
	make clean -C ./tests

.PHONY: all tests clean src