# Autoconf/automake file

#bin_PROGRAMS = test_sfbcvm
bin_PROGRAMS = 

# General compiler/linker flags
AM_CFLAGS = ${CFLAGS} -I${UCVM_SRC_PATH}/src/ucvm
AM_LDFLAGS = ${LDFLAGS}

objects = test_api.o
TARGETS = $(bin_PROGRAMS)

all: $(bin_PROGRAMS)

install:
	mkdir -p ${prefix}/tests
#	cp test_sfbcvm ${prefix}/tests

test_sfbcvm$(EXEEXT): $(objects)
	$(CC) -o $@ $^ $(AM_CFLAGS) -L../src -lsfbcvm $(AM_LDFLAGS)

$(objects): %.o: %.c
	$(CC) -o $@ -c $^ $(AM_CFLAGS) -I../src/ $(AM_CFLAGS)
