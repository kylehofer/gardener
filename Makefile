
EXCLUDE=--exclude='build/' --exclude='.git/' --exclude='temp/' --exclude='.envrc' --exclude='.pio' --exclude='.vscode' 

copy:
	 sshpass -p $(PASSWORD) rsync -rav ${EXCLUDE} -e ssh ./ $(USER)@$(HOST):$(TARGET)
