dnl Process this file with autoconf to produce a configure script.

AC_INIT

AM_INIT_AUTOMAKE(libopenmodeller, 0.4.3)

AC_PREFIX_PROGRAM(gcc)

AC_PROG_CXX

AC_LANG([C++])

AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_INSTALL

AC_PROG_LIBTOOL

AC_CHECK_HEADERS([float.h] [values.h] [jni.h])

dnl ---------------------------------------------------------------------------
dnl Check for Windows hosts
dnl ---------------------------------------------------------------------------

ac_windows=0 
ac_apple=0 
case "${host}" in 
  *-*-cygwin* | *-*-mingw32*) 
    ac_windows=1;; 
	*-*darwin*)
	  ac_apple=1;;
  *) 
	  ;;
esac 

AM_CONDITIONAL([ISWINDOWS], [test "$ac_windows" = "1"]) 
AM_CONDITIONAL([ISAPPLE], [test "$ac_apple" = "1"]) 


dnl ---------------------------------------------------------------------------
dnl Add any global compiler flags here
dnl ---------------------------------------------------------------------------

dnl CPPFLAGS="$CPPFLAGS -Wall -Werror"

dnl ---------------------------------------------------------------------------
dnl X11
dnl 
dnl Macro: AC_PATH_X
dnl      Try to locate the X Window System include files and libraries.  If
dnl      the user gave the command line options `--x-includes=DIR' and
dnl      `--x-libraries=DIR', use those directories.  If either or both
dnl      were not given, get the missing values by running `xmkmf' on a
dnl      trivial `Imakefile' and examining the `Makefile' that it produces.
dnl      If that fails (such as if `xmkmf' is not present), look for the
dnl      files in several directories where they often reside.  If either
dnl      method is successful, set the shell variables `x_includes' and
dnl      `x_libraries' to their locations, unless they are in directories
dnl      the compiler searches by default.
dnl 
dnl      If both methods fail, or the user gave the command line option
dnl      `--without-x', set the shell variable `no_x' to `yes'; otherwise
dnl      set it to the empty string.
dnl 
dnl ---------------------------------------------------------------------------
AC_PATH_X([X11], [X11/Xlib.h], [XOpenDisplay(NULL)])
AC_MSG_CHECKING([for X11 headers])
if test "$x_includes" != "" -a "$x_includes" != "NONE"; then
  X11_CFLAGS="-I$x_includes"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi
AC_MSG_CHECKING([for X11 libraries])
if test "$x_libraries" != "" -a "$x_libraries" != "NONE"; then
  X11_LIB="-L$x_libraries -lX11"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi
AC_SUBST(X11_LIB)
AC_SUBST(X11_CFLAGS)
AM_CONDITIONAL([HAVE_X11], [test "$X11_LIB" != ""])

dnl ---------------------------------------------------------------------------
dnl GDAL/OGR
dnl ---------------------------------------------------------------------------
AC_ARG_WITH(gdal,
  AC_HELP_STRING([--with-gdal=path],
                 [Full path to 'gdal-config' script, e.g. '--with-gdal=/usr/local/bin/gdal-config']))
                 
AC_MSG_CHECKING([for GDAL])
if test x"$with_gdal" = "xyes" -o x"$with_gdal" = "x" ; then
  GDAL_CONFIG=`which gdal-config`
  if test ! -x "$GDAL_CONFIG" ; then
    AC_MSG_ERROR(["Cannot find gdal-config, try to use --with-gdal option"])
  fi
  AC_MSG_RESULT([yes])
elif test x"$with_gdal" = "xno" ; then
  ac_gdalogr=no
  AC_MSG_RESULT([no])
else
  if test -x "$with_gdal/gdal-config" ; then
    GDAL_CONFIG="$with_gdal/gdal-config"
    AC_MSG_RESULT([yes])
  else
    AC_MSG_ERROR("Cannot find $with_gdal/gdal-config")
  fi
fi

if test x"$ac_gdalogr" != "xno" ; then
  AC_MSG_CHECKING([for OGR in GDAL])
  if test x`$GDAL_CONFIG --ogr-enabled` = "xno" ; then
    AC_MSG_ERROR([GDAL must be compiled with OGR support and currently is not.])
  fi
  AC_MSG_RESULT(yes)
  ac_gdalogr=yes

  GDAL_LIB="`$GDAL_CONFIG --libs`"
  GDAL_CFLAGS="`$GDAL_CONFIG --cflags`"

  AC_SUBST(GDAL_LIB)
  AC_SUBST(GDAL_CFLAGS)
else
  AC_ERROR([GDAL with OGR support is needed to build openmodeller])
fi

dnl ---------------------------------------------------------------------------
dnl Expat - XML parser
dnl ---------------------------------------------------------------------------
AC_ARG_WITH([expat],AC_HELP_STRING([--with-expat],[where to find Expat]),[with_expat=$withval],[with_expat=/usr])
if test x$with_expat = xno; then
	AC_MSG_ERROR("we require Expat.")
fi
expat_cflags=$CFLAGS
expat_ldflags=$LDFLAGS
expat_libs=$LIBS
expat_cxxflags=$CXXFLAGS
EXPAT_CFLAGS=''
EXPAT_LDFLAGS=''
EXPAT_LIBS=''
have_expat=no
expat_dirs='/usr /usr/local'
if test x$with_expat != x/usr; then
	expat_dirs="$with_expat $expat_dirs"
fi
for dir in $expat_dirs; do
	expat_okay=''
	for expat_dir in include include/expat; do
		AC_MSG_NOTICE([looking for expat.h in $dir/$expat_dir])
		CFLAGS="${expat_cflags} -I$dir/${expat_dir}"
		LDFLAGS="${expat_ldflags} -L$dir/lib"
		AC_CHECK_HEADER([expat.h],[
if test x$with_expat != x/usr; then
	EXPAT_CFLAGS="-I$dir/$expat_dir"
	EXPAT_LDFLAGS="-L$dir/lib"
fi
expat_okay='x'
have_expat=yes
break
],)
	done
	if test x$expat_okay = xx; then
		break
	fi
done
for expat_libname in -1 ''; do
	CFLAGS="${expat_cflags} $EXPAT_CFLAGS"
	CXXFLAGS="${expat_cflags} $EXPAT_CXXFLAGS"
	LDFLAGS="${expat_ldflags} $EXPAT_LDFLAGS"
	LIBS="${expat_libs} -lexpat${expat_libname}"
	AC_MSG_CHECKING([whether linking with expat${expat_libname} works])
	AC_TRY_LINK( [
#include <expat.h>
], [
	XML_Parser foo( XML_ParserCreate( "UTF-8" ) );
	XML_ParserFree( foo );
], [
EXPAT_LIBS="-lexpat$expat_libname"
AC_MSG_RESULT([yes])
break
], )
done
CFLAGS=$expat_cflags
CXXFLAGS=$expat_cxxflags
LDFLAGS=$expat_ldflags
LIBS=$expat_libs
AC_SUBST(have_expat)
AC_SUBST(EXPAT_CFLAGS)
AC_SUBST(EXPAT_LDFLAGS)
AC_SUBST(EXPAT_LIBS)

dnl ---------------------------------------------------------------------------
dnl Proj4
dnl ---------------------------------------------------------------------------
dnl AC_CHECK_LIB(proj, pj_init, [], [AC_MSG_ERROR("Cannot find Proj4 library")])

AC_ARG_WITH(proj,
    AC_HELP_STRING([--with-proj=DIR],
      [ Proj4 installation directory, e.g. '--with-proj=/usr/local']),
    [
      if test x"$with_proj" = "x" ; then
        AC_MSG_CHECKING([for Proj4 library in $with_proj/lib])
        PROJ_LIB="-L$with_proj/lib -lproj"
        if test -d "$with_proj/lib" ; then
          projlibdir="$with_proj/lib"
        fi
      else
        AC_MSG_CHECKING([for Proj4 library in $with_projlib])
        PROJ_LIB="-L$with_projlib -lproj"
        if test -d "$with_projlib" ; then
          projlibdir="$with_projlib"
        fi
      fi

      if test x"$projlibdir" = "x" ; then
         AC_MSG_ERROR([*** directory $projlibdir does not exist.])
      else
        LDFLAGS="$LDFLAGS $PROJ_LIB"
        CPPFLAGS="$CPPFLAGS -I$with_proj/include"
        AC_MSG_RESULT(["$with_proj exists"])

        AC_CHECK_HEADERS(proj_api.h,
          [PROJ_INC="-I$with_proj/include"], 
           [AC_MSG_ERROR([*** Proj4 headers not found.])])

        AC_CHECK_LIB(proj,pj_is_latlong, 
            [have_proj_lib=yes],
            [AC_MSG_ERROR([*** Proj4 library not found.])])
      fi
    ],
  
    [
     AC_CHECK_HEADERS(proj_api.h,[PROJ_INC=""],
       [AC_MSG_ERROR([*** Proj4 headers not found.])])
    ]
    AC_CHECK_LIB(proj,pj_is_latlong,
    [
     have_proj_lib=yes
     PROJ_LIB="-lproj"
    ],
    [
     AC_MSG_ERROR([Proj4 is required and was not found.])
    ])
 )

AC_ARG_WITH(projlib,AC_HELP_STRING([--with-projlib=DIR],
    [ Proj4 library directory, optional, use when library is in a different directory to that derived by appending /lib to --with-projdir ]),[],[])

AC_SUBST(PROJ_INC)
AC_SUBST(PROJ_LIB)

dnl ---------------------------------------------------------------------------
dnl gsl
dnl ---------------------------------------------------------------------------
AC_ARG_WITH(gsl,
  AC_HELP_STRING([--with-gsl=path],
                 [Full path to 'gsl-config' script, e.g. '--with-gsl=/usr/local/bin/gsl-config']))
                 
AC_MSG_CHECKING([for GSL])
GSL_CONFIG=`which gsl-config 2>/dev/null`
if test x"$with_gsl" = "xno" ; then
  ac_gsl=no
  AC_MSG_RESULT([no])
elif test -x "$GSL_CONFIG" ; then
  AC_MSG_RESULT([yes])
elif test x"$with_gsl" = "xyes" ; then
  if test ! -x "$GSL_CONFIG" ; then
    AC_MSG_ERROR(["Cannot find gsl-config, try to use --with-gsl option"])
  fi
  AC_MSG_RESULT([yes])
elif test ! x"$with_gsl" = "x" ; then 
  if test -x "$with_gsl/gsl-config" ; then
    GSL_CONFIG="$with_gsl/gsl-config"
    AC_MSG_RESULT([yes])
  else
    AC_MSG_ERROR("Cannot find $with_gsl/gsl-config")
  fi
else
  ac_gsl=no
  AC_MSG_RESULT([no])
fi

if test x"$ac_gsl" != "xno" ; then
  GSL_LIBS="`$GSL_CONFIG --libs`"
  GSL_CFLAGS="`$GSL_CONFIG --cflags`"

  AC_SUBST(GSL_LIBS)
  AC_SUBST(GSL_CFLAGS)
else
  echo "------"
  echo "The GNU Scientific Library is needed to build the CSM - Kaiser-Gutman and "
  echo "CSM - Broken Stick algorithms."
  echo "------"
fi
AM_CONDITIONAL([HAVE_GSL], [test "$GSL_LIBS" != ""])

dnl ---------------------------------------------------------------------------
dnl SOAP interface
dnl ---------------------------------------------------------------------------
AC_MSG_CHECKING([if SOAP server is enabled])

AC_ARG_ENABLE(soap,
  AC_HELP_STRING([--enable-soap],
                 [The SOAP interface is an optional feature. You need to explicitly enable it if you want to compile the SOAP server by using '--enable-soap']),
  [
   case "${enableval}" in
    yes)
      build_soap=yes
      ;;
    no)
      build_soap=no
      ;;
    *)
      AC_MSG_ERROR([bad value ${enableval} for --enable-soap])
      ;;
   esac
  ],
  [
    build_soap=no
  ]
)
AC_MSG_RESULT([$build_soap])

AM_CONDITIONAL([BUILD_SOAP], [test x$build_soap = xyes])

dnl ---------------------------------------------------------------------------
dnl SWIG interface
dnl ---------------------------------------------------------------------------

dnl python

AC_MSG_CHECKING([if SWIG python interface is enabled])

AC_ARG_ENABLE(python,
  AC_HELP_STRING([--enable-python],
                 [The SWIG python interface is an optional feature. If you want to enable it use '--enable-python']),
  [
   case "${enableval}" in
    yes)
      build_python=yes
      AC_MSG_RESULT([$build_python])
      AM_PATH_PYTHON
      AM_CHECK_PYTHON_HEADERS([PYSUBDIR=python],[])
      AC_SUBST(PYSUBDIR)
      ;;
    no)
      build_python=no
      AC_MSG_RESULT([$build_python])
      ;;
    *)
      AC_MSG_ERROR([bad value ${enableval} for --enable-python])
      ;;
   esac
  ],
  [
    build_python=no
    AC_MSG_RESULT([$build_python])
  ]
)

AM_CONDITIONAL([BUILD_PYTHON], [test x$build_python = xyes])

dnl java

AC_MSG_CHECKING([if SWIG java interface is enabled])

AC_ARG_ENABLE(java,
  AC_HELP_STRING([--enable-java],
                 [The SWIG java interface is an optional feature. If you want to enable it use '--enable-java']),
  [
   case "${enableval}" in
    yes)
      if test "y$ac_cv_header_jni_h" == "yyes"; then
        build_java=yes
      else
        AC_MSG_ERROR([java is enabled, but jni.h is not available])
      fi
      ;;
    no)
      build_java=no
      ;;
    *)
      AC_MSG_ERROR([bad value ${enableval} for --enable-java])
      ;;
   esac
  ],
  [
    build_java=no
  ]
)
AC_MSG_RESULT([$build_java])

AM_CONDITIONAL([BUILD_JAVA], [test x$build_java = xyes])

dnl SWIG

AM_CONDITIONAL([BUILD_SWIG], [test x$build_python = xyes -o x$build_java = xyes])

dnl ---------------------------------------------------------------------------
dnl TerraLib
dnl ---------------------------------------------------------------------------

AC_MSG_CHECKING([if TerraLib interface is enabled])

AC_ARG_ENABLE(terralib,
  AC_HELP_STRING([--enable-terralib],
                 [The TerraLib is an optional feature. If you want to enable it use '--enable-terralib']),
  [
   case "${enableval}" in
    yes)
      build_terralib=yes
      AC_MSG_RESULT([$build_terralib])
      ;;
    no)
      build_terralib=no
      AC_MSG_RESULT([$build_terralib])
      ;;
    *)
      AC_MSG_ERROR([bad value ${enableval} for --enable-terralib])
      ;;
   esac
  ],
  [
    build_terralib=no
    AC_MSG_RESULT([$build_terralib])
  ]
)

AM_CONDITIONAL([BUILD_TERRALIB], [test x$build_terralib = xyes])

dnl ---------------------------------------------------------------------------
dnl  SQLite
dnl ---------------------------------------------------------------------------

AC_ARG_WITH(sqlite3,
    AC_HELP_STRING([--with-sqlite3=DIR],
        [ sqlite3 installation directory, e.g. '--with-sqlite3=/usr/local']),
    [
    AC_MSG_CHECKING([for Sqlite3 library in $with_sqlite3])
        if test -d "$with_sqlite3/lib"; then
          LDFLAGS="$LDFLAGS -L$with_sqlite3/lib"
          CPPFLAGS="$CPPFLAGS -I$with_sqlite3/include"
          AC_MSG_RESULT(["$with_sqlite3 exists"])
          AC_CHECK_HEADERS(sqlite3.h,
                [SQLITE3_INC="-I$with_sqlite3/include"],
                [
                AC_MSG_ERROR([SQLite3 headers not found.])
                ])
          AC_CHECK_LIB(sqlite3,sqlite3_open,
                [
                have_sqlite3_lib=yes
                SQLITE3_LIB="-L$with_sqlite3/lib -lsqlite3"
                ],
                [
                AC_MSG_ERROR([SQLite3 library not found.])
                ])
                else
                AC_MSG_ERROR([Directory $with_sqlite3 does not exist.])
        fi
    ],
    [
    AC_CHECK_HEADERS(sqlite3.h,[SQLITE3_INC=""],
        [
        AC_MSG_WARN([SQLite3 headers not found.])
        ])
    AC_CHECK_LIB(sqlite3,sqlite3_open,
        [
        have_sqlite3_lib=yes
        SQLITE3_LIB="-lsqlite3"
        ],
        [
        AC_MSG_WARN([SQLite3 library not found.])
        ])
    ]
    )

AC_SUBST(SQLITE3_INC)
AC_SUBST(SQLITE3_LIB)

AM_CONDITIONAL([HAVE_SQLITE3], [test x$have_sqlite3_lib = xyes])


dnl ---------------------------------------------------------------------------

dnl 	data/Makefile

AC_CONFIG_FILES([
	Makefile
	test/Makefile
	test/CppUnitLite/Makefile
	test/library/Makefile
	src/Makefile
	src/openmodeller/Makefile
	src/console/Makefile
	src/soap/Makefile
	src/swig/Makefile
	src/swig/python/Makefile
	src/swig/java/Makefile
	src/algorithms/Makefile
	src/algorithms/best_subsets/Makefile
	src/algorithms/garp/Makefile
	src/algorithms/garp/test/Makefile
	src/algorithms/garp/garp_best_subsets/Makefile
	src/algorithms/dg_garp/Makefile
	libopenmodeller.spec
])

AC_OUTPUT
