TARGET=GardenLibraryTests
OUT_DIR=../build/tests
SRC_OUT_DIR=../build/src
TEST_DIR=.
SRC_DIR=../src
INCLUDE_DIR=../include
TEST_INCLUDE_DIR=.
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

TEST_OBJECTS=$(patsubst ${TEST_DIR}/%.cpp, $(OUT_DIR)/%.o, $(wildcard ${TEST_DIR}/*.cpp))
LIBRARIES=../include/
SRC_OBJECTS= $(filter-out ${SRC_OUT_DIR}/main.o, $(wildcard ${SRC_OUT_DIR}/*.o))

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

# linker  -export-dynamic -lX11 -ljpeg  -L/usr/local/lib/
LD=g++
LFLAGS=-I/usr/include/modbus/ -I${INCLUDE_DIR} -I${TEST_INCLUDE_DIR}
LDFLAGS=$(PTHREAD) -lmodbus /usr/lib/x86_64-linux-gnu/libgtest.a /usr/lib/x86_64-linux-gnu/libgtest_main.a

MKDIR_P = mkdir -p

all: src ${OUT_DIR} ${TEST_OBJECTS}
	$(LD) -o $(OUT_DIR)/$(TARGET) ${TEST_OBJECTS} ${SRC_OBJECTS} $(LDFLAGS) $(LFLAGS) 
	$(OUT_DIR)/$(TARGET)

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

$(TEST_OBJECTS): ${OUT_DIR}/%.o : ${TEST_DIR}/%.cpp
	$(CC) -c $< $(CCFLAGS) $(LFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f ${OUT_DIR}/*.o ${OUT_DIR}/*.cpp $(TARGET)

src:
	$(MAKE) -C ../src

.PHONY: src