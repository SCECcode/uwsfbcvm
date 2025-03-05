# Autoconf/automake file

# General compiler/linker flags
AM_CFLAGS = ${CFLAGS} 
AM_LDFLAGS = ${LDFLAGS}

TARGETS = libsfbcvm.a libsfbcvm.so

all: $(TARGETS)

install:
	mkdir -p ${prefix}
	mkdir -p ${prefix}/lib
	mkdir -p ${prefix}/include
	cp libsfbcvm.so ${prefix}/lib
	cp libsfbcvm.a ${prefix}/lib
	cp sfbcvm.h ${prefix}/include

libsfbcvm.a: sfbcvm_static.o 
	$(AR) rcs $@ $^

libsfbcvm.so: sfbcvm.o
	$(CC) -shared $(AM_CFLAGS) -o libsfbcvm.so $^ $(AM_LDFLAGS)

sfbcvm.o: sfbcvm.c
	$(CC) -fPIC -DDYNAMIC_LIBRARY -o $@ -c $^ $(AM_CFLAGS)
	
sfbcvm_static.o: sfbcvm.c
	$(CC) -o $@ -c $^ $(AM_CFLAGS)
	
clean:
	rm -rf $(TARGETS)
	rm -rf *.o 

