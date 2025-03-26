# GNU Automake config


# Autoconf/automake file

# General compiler/linker flags
AM_CFLAGS = ${CFLAGS}
AM_FCFLAGS = ${FCFLAGS}
AM_LDFLAGS = ${LDFLAGS}

all:
if WITH_UWSFBCVM_LARGEDATA_DIR
	echo "USING WITH_UWSFBCVM_LARGEDATA_DIR in uwsfbcvm"
else
	if [ ! -d "uwsfbcvm" ]; then \
		./make_data_files.py; \
	fi
endif

install:
if WITH_UWSFBCVM_LARGEDATA_DIR
	if [ ! -d "${prefix}/data/uwsfbcvm" ]; then \
		$(MKDIR_P) ${prefix}/data; \
		$(LN_S) -f $(UWSFBCVM_LARGEDATA_DIR) ${prefix}/data/uwsfbcvm; \
		echo "USING WITH_UWSFBCVM_LARGEDATA_DIR in uwsfbcvm"; \
	fi
	cp config ${prefix}/data
else
	if [ ! -d "${prefix}/data/uwsfbcvm" ]; then \
		$(MKDIR_P) ${prefix}/data; \
		mv uwsfbcvm ${prefix}/data; \
		$(LN_S) -f ${prefix}/data/uwsfbcvm .; \
	fi
	cp config ${prefix}/data
endif

clean:
	echo "clean"

