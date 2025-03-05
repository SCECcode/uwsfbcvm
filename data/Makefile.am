# GNU Automake config


# Autoconf/automake file

# General compiler/linker flags
AM_CFLAGS = ${CFLAGS}
AM_FCFLAGS = ${FCFLAGS}
AM_LDFLAGS = ${LDFLAGS}

all:
if WITH_SFBCVM_LARGEDATA_DIR
	echo "USING WITH_SFBCVM_LARGEDATA_DIR in sfbcvm"
else
	./make_data_files.py
endif

install:
if WITH_SFBCVM_LARGEDATA_DIR
	if [ ! -d "${prefix}/data/sfbcvm" ]; then \
		$(MKDIR_P) ${prefix}/data; \
		$(LN_S) -f $(SFBCVM_LARGEDATA_DIR) ${prefix}/data/sfbcvm; \
		echo "USING WITH_SFBCVM_LARGEDATA_DIR in sfbcvm"; \
	fi
	cp config ${prefix}/data
else
	if [ ! -d "${prefix}/data/sfbcvm" ]; then \
		$(MKDIR_P) ${prefix}/data; \
		mv sfbcvm ${prefix}/data; \
		$(LN_S) -f ${prefix}/data/sfbcvm .; \
	fi
	cp config ${prefix}/data
endif

clean:
	echo "clean"

