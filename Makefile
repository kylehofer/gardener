TARGET=gardener
OUT_DIR=build

PKGCONFIG = $(shell which pkg-config)

# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall

PTHREAD=-pthread

SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst src/%.c, build/%.o, $(SOURCES))



CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

# linker  -export-dynamic -lX11 -ljpeg  -L/usr/local/lib/
LD=gcc
LFLAGS=-I/usr/include/modbus/
LDFLAGS=$(PTHREAD) -lmodbus

MKDIR_P = mkdir -p

all: ${OUT_DIR} $(OBJECTS)
	$(LD) -o $(OUT_DIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(LFLAGS) $(LFLAGS) 

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

$(OBJECTS): build/%.o : src/%.c
	$(CC) -c $< $(CCFLAGS) $(LFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f build/*.o build/*.c $(TARGET)
