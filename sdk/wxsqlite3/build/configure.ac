# ======================================================================================
# Author: Francesco Montorsi
# RCS-ID: $Id: configure.ac,v 1.10 2005/11/20 14:45:54 utelle Exp $
#
# A basic "configure.ac" for a wxCode component.
# See 
#    http://www.gnu.org/software/autoconf/manual/autoconf-2.57/html_mono/autoconf.html
# for more info about autoconf and the autoconf macros.
# ======================================================================================



# Init this configure script with basic info about the component
# (DON'T PUT ANYTHING BEFORE AC_INIT, JUST COMMENTS)
AC_INIT([wxSqlite3], [1.0], [mymail@myserver.com])

# ENABLES/DISABLES THE DEBUG MODE FOR THIS CONFIGURE SCRIPT
#DBG_CONFIGURE=1

# you need the wxCode/build/bakefiles in your local repository to generate a configure
# script from this configure.ac file since wxcode.m4 is required....
m4_include(../../../build/autoconf/wxcode.m4)
AM_WXCODE_INIT




## CONFIGURE OPTIONS
##
## Before starting checks, declare the options of this configure script
## Here you should use the AC_ARG_ENABLE and AC_ARG_WITH macros
## to add to this configure script the --enable-XXX and/or --with-XXX
## options required. If you did not use any <option> tag in your
## component's bakefile, then you can leave this section as is
#####################################################################

AM_OPTIONS_WXCONFIG             # don't touch this macro
AM_OPTIONS_WXPRESETS            # don't touch this macro

AC_ARG_WITH([sqlite3_prefix],
                AC_HELP_STRING([--with-sqlite3-prefix],
                        [Prefix where sqlite3 is installed (optional; default is ./sqlite3)]),, 
                [with_sqlite3_prefix=./sqlite3])
SQLITE3_DIR=$with_sqlite3_prefix
AC_SUBST(SQLITE3_DIR)



## CONFIGURE CHECKS
##
## Here you should use the AC_CHECK_LIB, AC_COMPILE_IFELSE, 
## AC_LINK_IFELSE, etc macros to check that the libraries required
## by your component exist on the host machine and match your
## required options (version, build settings, etc)
#####################################################################

AM_WXCODE_CHECKS([2.6.0], [])

AM_WXCODE_ARG_ENABLE([dynamic-load], [Enables the SQLite dynamic load support], [no])
AC_MSG_CHECKING([for the --enable-dynamic-load option])
if test "$enable_dynamic_load" = "yes"; then
    USE_DYNAMIC_SQLITE3_LOAD=1
    AC_MSG_RESULT([yes])
else
    USE_DYNAMIC_SQLITE3_LOAD=0
    AC_MSG_RESULT([no])
fi

AM_WXCODE_ARG_ENABLE([metadata], [Enables the wxSQLite3 meta data support (SQLite needs to be compiled with meta data support)], [no])
AC_MSG_CHECKING([for the --enable-metadata option])
if test "$enable_metadata" = "yes"; then
    HAVE_METADATA=1
    AC_MSG_RESULT([yes])
else
    HAVE_METADATA=0
    AC_MSG_RESULT([no])
fi

AM_WXCODE_ARG_ENABLE([codec], [Enables the wxSQLite3 CODEC support (SQLite needs to be compiled with CODEC support)], [no])
AC_MSG_CHECKING([for the --enable-codec option])
if test "$enable_codec" = "yes"; then
    HAVE_CODEC=1
    AC_MSG_RESULT([yes])
else
    HAVE_CODEC=0
    AC_MSG_RESULT([no])
fi

AM_WXCODE_ARG_ENABLE([load-extension], [Enables the wxSQLite3 loadable extension support (SQLite needs to be compiled with loadable extension support)], [no])
AC_MSG_CHECKING([for the --enable-load-extension option])
if test "$enable_load_extension" = "yes"; then
    HAVE_LOAD_EXTENSION=1
    AC_MSG_RESULT([yes])
else
    HAVE_LOAD_EXTENSION=0
    AC_MSG_RESULT([no])
fi

# end in a nice way the configure script
AM_WXCODE_END_PART1
if test "$USE_DYNAMIC_SQLITE3_LOAD" = "1"; then
echo "  - dynamic load enabled"
else
echo "  - dynamic load disabled"
fi
if test "$HAVE_METADATA" = "1"; then
echo "  - meta data support enabled"
else
echo "  - meta data support disabled"
fi
if test "$HAVE_CODEC" = "1"; then
echo "  - CODEC support enabled"
else
echo "  - CODEC support disabled"
fi
if test "$HAVE_LOAD_EXTENSION" = "1"; then
echo "  - loadable extension support enabled"
else
echo "  - loadable extension support disabled"
fi
AM_WXCODE_END_PART2


