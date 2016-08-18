dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.
dnl

AC_PREREQ(2.61)
AC_INIT([mres], [0.1], [Timothy.P.Brown@noaa.gov])
AC_CONFIG_AUX_DIR([build-aux])

# Initialise automake
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Define our M4 macro directory
AC_CONFIG_MACRO_DIR([m4])

# Check there is a source file
AC_CONFIG_SRCDIR([src/main.c])
# Put our generated config header in the source directory
AC_CONFIG_HEADERS([src/config.h])

# Checks for compiler support/falgs
AC_PROG_CC_C99
AC_C_RESTRICT

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h string.h unistd.h])

# Checks for functions and libraries
AC_CHECK_FUNCS(memset strrchr uname getprogname \
               program_invocation_short_name)
AC_FUNC_MALLOC

# Check for HDF5 support
AX_LIB_HDF5()
AM_CONDITIONAL([HAVE_HDF5], [test "x$with_hdf5" = "xyes"])
if test "x$with_hdf5" = "xyes"; then
	CC=${HDF5_CC}
fi

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 ])


AC_OUTPUT
echo
echo
echo "========================================================================"
echo "$PACKAGE_NAME              version $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "Installation prefix        : $prefix"
echo "C compiler                 : $ax_cv_c_compiler_vendor $CC"
echo "C command                  : $CC $CFLAGS"
echo "Linker                     : $LD $LDFLAGS $LIBS"
echo "HDF5 support               : $with_hdf5"
echo


