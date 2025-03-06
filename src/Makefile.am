# Autoconf/automake file

# General compiler/linker flags
AM_CFLAGS = ${CFLAGS} 
AM_LDFLAGS = ${LDFLAGS}

TARGETS = libuwsfbcvm.a libuwsfbcvm.so

all: $(TARGETS)

install:
	mkdir -p ${prefix}
	mkdir -p ${prefix}/lib
	mkdir -p ${prefix}/include
	cp libuwsfbcvm.so ${prefix}/lib
	cp libuwsfbcvm.a ${prefix}/lib
	cp uwsfbcvm.h ${prefix}/include

libuwsfbcvm.a: uwsfbcvm_static.o 
	$(AR) rcs $@ $^

libuwsfbcvm.so: uwsfbcvm.o
	$(CC) -shared $(AM_CFLAGS) -o libuwsfbcvm.so $^ $(AM_LDFLAGS)

uwsfbcvm.o: uwsfbcvm.c
	$(CC) -fPIC -DDYNAMIC_LIBRARY -o $@ -c $^ $(AM_CFLAGS)
	
uwsfbcvm_static.o: uwsfbcvm.c
	$(CC) -o $@ -c $^ $(AM_CFLAGS)
	
clean:
	rm -rf $(TARGETS)
	rm -rf *.o 

