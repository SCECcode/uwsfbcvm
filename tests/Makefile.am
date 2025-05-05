# Autoconf/automake file

bin_PROGRAMS = test_uwsfbcvm

# General compiler/linker flags
AM_CFLAGS = ${CFLAGS} -I${UCVM_SRC_PATH}/src/ucvm
AM_LDFLAGS = ${LDFLAGS}

objects = test_api.o
TARGETS = $(bin_PROGRAMS)

all: $(bin_PROGRAMS)

install:
	mkdir -p ${prefix}/tests
	cp test_uwsfbcvm ${prefix}/tests

test_uwsfbcvm$(EXEEXT): $(objects)
	$(CC) -o $@ $^ $(AM_CFLAGS) -L../src -luwsfbcvm $(AM_LDFLAGS)

run_unit : test_uwsfbcvm
	./test_uwsfbcvm

$(objects): %.o: %.c
	$(CC) -o $@ -c $^ $(AM_CFLAGS) -I../src/ $(AM_CFLAGS)
