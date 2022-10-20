TARGET=gardener
OUT_DIR=build
INSTALL_DIR=/usr/bin

PKGCONFIG = $(shell which pkg-config)

# compiler
CC=g++
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall

PTHREAD=-pthread

C_OBJECTS=$(patsubst src/%.c, build/%.o, $(wildcard src/*.c))
CPP_OBJECTS=$(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

# linker  -export-dynamic -lX11 -ljpeg  -L/usr/local/lib/
LD=g++
LFLAGS=-I/usr/include/modbus/ -Iinclude/
LDFLAGS=$(PTHREAD) -lmodbus

MKDIR_P = mkdir -p

all: ${OUT_DIR} $(C_OBJECTS) $(CPP_OBJECTS)
	$(LD) -o $(OUT_DIR)/$(TARGET) $(C_OBJECTS) $(CPP_OBJECTS) $(LDFLAGS) $(LFLAGS) $(LFLAGS) 

install:
	cp $(OUT_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

$(C_OBJECTS): build/%.o : src/%.c
	$(CC) -c $< $(CCFLAGS) $(LFLAGS) $(LDFLAGS) -o $@

$(CPP_OBJECTS): build/%.o : src/%.cpp
	$(CC) -c $< $(CCFLAGS) $(LFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f build/*.o build/*.c build/*.cpp $(TARGET)
