# GNU Autoconf config

AC_INIT(uwsfbcvm, 1.0)
AC_CONFIG_AUX_DIR([./aux-config])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_RANLIB
AC_PROG_MKDIR_P
AC_PROG_LN_S

# Determine MPI/CC compiler
: ${CFLAGS=""}
AC_PROG_CC

# Check for GNU compiler
if test "x$ac_compiler_gnu" = "xno"; then
AC_MSG_ERROR(["GNU C compiler or MPI wrapper based on GNU is required. Please check your programming environment."])
fi

##check optional large data path 
##UWSFBCVM_LARGETDATA_DIR=$CVM_LARGETDATA_DIR/model/uwsfbcvm
if test x"$CVM_LARGEDATA_DIR" != x; then
   # test directory existence
   UWSFBCVM_LARGEDATA_DIR=$CVM_LARGEDATA_DIR/model/uwsfbcvm
# In docker container building.. this is nto accessible  yet
   AM_CONDITIONAL([WITH_UWSFBCVM_LARGEDATA_DIR], true)
   AC_SUBST(UWSFBCVM_LARGEDATA_DIR)
else
   AM_CONDITIONAL(WITH_UWSFBCVM_LARGEDATA_DIR, false)
fi

CFLAGS="$CFLAGS"
LDFLAGS="$LDFLAGS -lm"

AC_CONFIG_FILES([Makefile
                data/Makefile
                src/Makefile
                tests/Makefile])

AC_OUTPUT
