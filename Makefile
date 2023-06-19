BIN_DIR = bin
LIBS_DIRS = ${patsubst libs/%,%,${wildcard libs/*} }
MYLIBS = mySimpleComputer myTerm myBigChars myInterface myReadKey myALU myCU mySignal

CFLAGS = -Wall ${addprefix -I libs/,${LIBS_DIRS}}
LDLIBS = ${addprefix -L libs/,${LIBS_DIRS}} ${addprefix -l,${LIBS_DIRS}}

MYSATP = ${patsubst ./project/assembler/%,%,${wildcard ./project/assembler/*.sa} }
MYSBTP = ${patsubst ./project/basic/%,%,${wildcard ./project/basic/*.sb} }

.PHONY: all build_dirs run format

all: build_dirs binary

binary: ${MYLIBS}
	${CC} $(CFLAGS) -o ${BIN_DIR}/binary project/main.c $(LDLIBS)

sa: project/commands.h
	$(CXX) $(CFLAGS) -I project/commands.h -o ${BIN_DIR}/sat project/assembler/sat.cpp $(LDLIBS)
	${MAKE} sat --ignore-errors --keep-going

sat: ${MYSATP}
	${info > SimpleAssembler_to_Machine > ${MYSATP}}
	${info >>> transfer is completed}

sb: project/commands.h
	$(CXX) -std=c++20 project/basic/sbt.cpp -o ${BIN_DIR}/sbt
	${MAKE} sbt --ignore-errors --keep-going

sbt: ${MYSBTP}
	${info > SimpleBasic_to_SimpleAssembler > ${MYSBTP}}
	${info >>> transfer is completed}

test: ${MYLIBS}
	${CC} $(CFLAGS) -o ${BIN_DIR}/testbch tests/bch.c $(LDLIBS)
	${CC} $(CFLAGS) -o ${BIN_DIR}/testif tests/iface.c $(LDLIBS)
	${CC} $(CFLAGS) -o ${BIN_DIR}/testrk tests/rk.c $(LDLIBS)
	./${BIN_DIR}/testrk;

${MYSATP}:
	./${BIN_DIR}/sat project/assembler/${notdir $@} resources/${notdir ${patsubst %.sa,%.o,$@}}

${MYSBTP}:
	./${BIN_DIR}/sbt project/basic/${notdir $@} project/assembler/${notdir ${patsubst %.sb,%.sa,$@}}

${MYLIBS}:
	${MAKE} -C libs/${notdir $@}

build_dirs:
	mkdir -p ${BIN_DIR}

run:
	./${BIN_DIR}/binary;

format:
	clang-format --style GNU -i --verbose project/main.c
	clang-format --style GNU -i --verbose libs/*/*.c
	clang-format --style GNU -i --verbose libs/*/*.h

clean:
	${RM} ./${BIN_DIR}/*
	${RM} $(shell find ./libs -name \*.a)
	${RM} $(shell find ./libs -name \*.o)
	${RM} $(shell find ./resources -name \*.o)