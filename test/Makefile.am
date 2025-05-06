# Autoconf/automake file

bin_PROGRAMS = test_uwsfbcvm

# General compiler/linker flags
AM_CFLAGS = ${CFLAGS} -I${UCVM_SRC_PATH}/src/ucvm -I../src
AM_LDFLAGS = ${LDFLAGS} -L../src -luwsfbcvm -lm

objects = test_api.o
TARGETS = $(bin_PROGRAMS)

.PHONY = run_unit

all: $(bin_PROGRAMS)

############################################
# Executables
############################################
install:
	mkdir -p ${prefix}/test
	cp test_uwsfbcvm ${prefix}/test

test_uwsfbcvm: $(objects)
	$(CC) -o $@ $^ $(AM_LDFLAGS) 

run_unit : test_uwsfbcvm
	./test_uwsfbcvm
