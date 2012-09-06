# Process this file with autoconf to produce a configure script.
m4_include([ocaml.m4])
AC_PREREQ(2.59)
AC_INIT(camlimages, 3.2.0, use-omake-to-be-supported@nowhere.com)
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

# Check ocaml
AC_PROG_OCAML([3.08])
AC_PROG_OCAML_TOOL(OCAMLMKLIB, ocamlmklib)
AC_SUBST(OCAMLLIB)

# Check versions to build
AC_ARG_ENABLE(
    [native-library],
    AC_HELP_STRING(
	[--enable-native-library],
	[build native version of library (default)]
    ),
    [case "$enableval" in
	yes) enable_native=$enableval;;
	no)  enable_native=$enableval;;
	*)   AC_MSG_ERROR([bad value $enableval for --enable-native-library]);;
    esac],
    [enable_native=yes]
)
AC_MSG_CHECKING([wether to build native library])
build_native=no
if test -n "$OCAMLOPT" && test "$enable_native" = yes; then
    AC_MSG_RESULT([yes])
    build_native=yes
else
    AC_MSG_RESULT([no])
    build_native=no
fi
AM_CONDITIONAL([BUILD_NATIVE], [test "$build_native" = yes])

AC_ARG_ENABLE(
    [bytecode-library],
    AC_HELP_STRING(
	[--enable-bytecode-library],
	[build bytecode version of library (default)]
    ),
    [case "$enableval" in
	yes) enable_bytecode=$enableval;;
	no)  enable_bytecode=$enableval;;
	*)   AC_MSG_ERROR([bad value $enableval for --enable-bytecode-library]);;
    esac],
    [enable_bytecode=yes]
)
AC_MSG_CHECKING([wether to build bytecode library])
build_bytecode=no
if test -n "$OCAMLC" && test "$enable_bytecode" = yes; then
    AC_MSG_RESULT([yes])
    build_bytecode=yes
else
    AC_MSG_RESULT([no])
    build_bytecode=no
fi
AM_CONDITIONAL([BUILD_BYTECODE], [test "$build_bytecode" = yes])

if test "$build_native" = no && test "$build_bytecode" = no; then
    AC_MSG_ERROR([neither native nor bytecode library selected])
fi

# Checks other programs
AC_PROG_CC

# Checks word size
AC_CHECK_SIZEOF(long)
WORD_SIZE=$ac_cv_sizeof_long
AC_SUBST(WORD_SIZE)

# Checks X libraries
AC_PATH_XTRA

# Checks lablgtk support
SUPPORT_LABLGTK="false"
LABLGTKDIR=

AC_PATH_PROG(PATH_OCAMLFIND, ocamlfind, no)

if test "x$PATH_OCAMLFIND" = "x"; then
  with_ocamlfind_default="no"
else
  with_ocamlfind_default="yes"
fi

AC_ARG_WITH(
    ocamlfind,
    AC_HELP_STRING(
	[--with-ocamlfind],
	[enable ocamlfind support (default: depends on ocamlfind)]
    ),
    [case "$withval" in
	yes) with_ocamlfind=yes;;
	no)  with_ocamlfind=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-ocamlfind]);;
    esac],
    [with_ocamlfind=$with_ocamlfind_default]
)	

if test "$with_ocamlfind" = "no"; then
  OCAMLSITELIBDIR=$OCAMLLIB/camlimages
else
  OCAMLSITELIBDIR=$OCAMLLIB/site-lib/camlimages
fi
AC_SUBST(OCAMLSITELIBDIR)

AC_ARG_WITH(
    lablgtk,
    AC_HELP_STRING(
	[--with-lablgtk],
	[enable lablgtk support (default: yes)]
    ),
    [case "$withval" in
	yes) with_lablgtk=yes;;
	no)  with_lablgtk=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-lablgtk]);;
    esac],
    [with_lablgtk=yes]
)

AC_MSG_CHECKING([lablgtk])

if test "$with_lablgtk" = "no"; then
    AC_MSG_RESULT(disabled)
else
    AC_ARG_WITH(
	lablgtk-dir,
	AC_HELP_STRING(
	    [--with-lablgtk-dir=dir],
	    [defines lablgtk installation directory]
	),
	[if test -d "$withval"; then
	    lablgtkdir=$withval
	else
	    AC_MSG_ERROR([invalid value $withval for --with-lablgtk-dir])
	fi],
	[lablgtkdir="$OCAMLLIB/lablgtk"]
    )

    if test -f "$lablgtkdir/gtk.ml"; then
	AC_MSG_RESULT([found at $lablgtkdir])

	AC_MSG_CHECKING([for lablgtk version])
	if test \
	    -f "$lablgtkdir/gBin.mli" && \
	    ! `grep image: "$lablgtkdir/gdk.mli" > /dev/null`; \
	then
	    AC_MSG_RESULT([ok])
	    SUPPORT_LABLGTK="true"
	    LABLGTKDIR="$lablgtkdir"
	else
	    AC_MSG_RESULT([too old])
	fi 
    else
	AC_MSG_RESULT([not found])
    fi

fi
AM_CONDITIONAL(HAVE_LABLGTK, test x$SUPPORT_LABLGTK = xtrue)
AC_SUBST(LABLGTKDIR)

# Checks lablgtk2 support
SUPPORT_LABLGTK2="false"
LABLGTK2DIR=

AC_ARG_WITH(
    [lablgtk2],
    [AC_HELP_STRING(
	[--with-lablgtk2],
	[enable lablgtk2 support (default: yes)]
    )],
    [case "$withval" in
	yes) with_lablgtk2=yes;;
	no)  with_lablgtk2=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-lablgtk2]);;
    esac],
    [with_lablgtk2=yes]
)

AC_MSG_CHECKING([lablgtk2])

if test "$with_lablgtk2" = "no"; then
    AC_MSG_RESULT(disabled)
else
    if test "$SUPPORT_LABLGTK" = "true"; then
	AC_MSG_RESULT([incompatible with lablgtk])
    else
	AC_ARG_WITH(
	    lablgtk2-dir,
	    AC_HELP_STRING(
		[--with-lablgtk2-dir=dir],
		[defines lablgtk2 installation directory]
	    ),
	    [if test -d "$withval"; then
		lablgtkdir2=$withval
	    else
		AC_MSG_ERROR([invalid value $withval for --with-lablgtk2-dir])
	    fi],
	    [lablgtk2dir="$OCAMLLIB/lablgtk2"]
	)

	if test -f "$lablgtk2dir/gtk.ml"; then
	    AC_MSG_RESULT([found at $lablgtk2dir])
	    SUPPORT_LABLGTK2="true"
	    LABLGTK2DIR="$lablgtk2dir"
	else
	    AC_MSG_RESULT(not found)
	fi
    fi
fi
AM_CONDITIONAL(HAVE_LABLGTK2, test x$SUPPORT_LABLGTK2 = xtrue)
AC_SUBST(LABLGTK2DIR)

# Checks gif support
SUPPORT_GIF="false"
LIBGIF=""

AC_ARG_WITH(
    [gif],
    [AC_HELP_STRING(
	[--with-gif],
	[enable gif support (default: yes)]
    )],
    [case "$withval" in
	yes) with_gif=yes;;
	no)  with_gif=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-gif]);;
    esac],
    [with_gif=yes]
)

if test "$with_gif" = "no" ; then
    AC_MSG_RESULT([disabled])
else
    AC_CHECK_HEADER(
	[gif_lib.h],
	[AC_CHECK_LIB(
	    [gif],
	    [DGifOpenFileName], 
	    [LIBGIF="-lgif"],
	    [AC_CHECK_LIB(
		[ungif],
		[DGifOpenFileName], 
		[LIBGIF="-lungif"]
	    )]
	)]
    )

    if test -n "$LIBGIF"; then
	SUPPORT_GIF="true"

	# Checks old gif bug
	AC_MSG_CHECKING([if gif bug is fixed])
	save_LDFLAGS="$LDFLAGS"
	LDFLAGS="$LDFLAGS $LIBGIF"
	AC_TRY_RUN(
	    [
#include <stdio.h>
#include <gif_lib.h>
main()
{
    EGifSetGifVersion("89a");
    return 0;
}
	    ],
	    [AC_DEFINE([GIFLIB_BUG_FIXED], [1], [Define if the bug in gif library has been fixed.])
	    AC_MSG_RESULT([yes])],
	    [AC_MSG_WARN([buggy gif library, consider rebuilding it with  gcc option -fwritable-strings])
	    AC_MSG_RESULT([no])]
	)
	LDFLAGS=$save_LDFLAGS
    fi
fi
AC_SUBST(LIBGIF)
AC_SUBST(SUPPORT_GIF)
AM_CONDITIONAL(HAVE_GIF, test x$SUPPORT_GIF = xtrue)

# Checks png support
SUPPORT_PNG="false"
LIBPNG=""

AC_ARG_WITH(
    png,
    AC_HELP_STRING(
	[--with-png],
	[enable png support (default: yes)]
    ),
    [case "$withval" in
	yes) with_png=yes;;
	no)  with_png=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-png]);;
    esac],
    [with_png=yes]
)

if test "$with_png" = "no" ; then
    AC_MSG_RESULT([disabled])
else
    AC_CHECK_HEADER(
	[png.h],
	[AC_CHECK_LIB(
	    [png],
	    [png_create_read_struct], 
	    [LIBPNG="-lpng -lz"
	    SUPPORT_PNG="true"],
	    [],
	    [-lz]
	)]
    )
fi
AC_SUBST(LIBPNG)
AC_SUBST(SUPPORT_PNG)
AM_CONDITIONAL(HAVE_PNG, test x$SUPPORT_PNG = xtrue)

# Checks jpeg support
SUPPORT_JPEG="false"
LIBJPEG=""

AC_ARG_WITH(
    [jpeg],
    [AC_HELP_STRING(
	[--with-jpeg],
	[enable jpeg support (default: yes)]
    )],
    [case "$withval" in
	yes) with_jpeg=yes;;
	no)  with_jpeg=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-jpeg]);;
    esac],
    [with_jpeg=yes]
)

if test "$with_jpeg" = "no" ; then
    AC_MSG_RESULT([disabled])
else
    AC_CHECK_HEADER(
	[jpeglib.h],
	[AC_CHECK_LIB(
	    [jpeg],
	    [jpeg_read_header], 
	    [LIBJPEG="-ljpeg"
	    SUPPORT_JPEG="true"]
	)]
    )
fi
AC_SUBST(LIBJPEG)
AC_SUBST(SUPPORT_JPEG)
AM_CONDITIONAL(HAVE_JPEG, test x$SUPPORT_JPEG = xtrue)

# Checks tiff support
SUPPORT_TIFF="false"
LIBTIFF=""

AC_ARG_WITH(
    [tiff],
    [AC_HELP_STRING(
	[--with-tiff],
	[enable tiff support (default: yes)]
    )],
    [case "$withval" in
	yes) with_tiff=yes;;
	no)  with_tiff=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-tiff]);;
    esac],
    [with_tiff=yes]
)

if test "$with_tiff" = "no" ; then
    AC_MSG_RESULT([disabled])
else
    AC_CHECK_HEADER(
	[tiffio.h],
	[AC_CHECK_LIB(
	    [tiff],
	    [TIFFOpen], 
	    [LIBTIFF="-ltiff -lz -ljpeg"
	    SUPPORT_TIFF="true"],
	    [],
	    [-lz -ljpeg]
	)]
    )
fi
AC_SUBST(LIBTIFF)
AC_SUBST(SUPPORT_TIFF)
AM_CONDITIONAL(HAVE_TIFF, test x$SUPPORT_TIFF = xtrue)

# Checks xpm support
SUPPORT_XPM="false"
LIBXPM=""

AC_ARG_WITH(
    [xpm],
    [AC_HELP_STRING(
	[--with-xpm],
	[enable xpm support (default: yes)]
    )],
    [case "$withval" in
	yes) with_xpm=yes;;
	no)  with_xpm=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-xpm]);;
    esac],
    [with_xpm=yes]
)

if test "$with_xpm" = "no" ; then
    AC_MSG_RESULT([disabled])
else
    save_CFLAGS="$CFLAGS"
    save_LDFLAGS="$LDFLAGS"
    CFLAGS="$CFLAGS $X_CFLAGS"
    LDFLAGS="$LDFLAGS $X_LIBS"
    AC_CHECK_HEADER(
	[X11/xpm.h],
	[AC_CHECK_LIB(
	    [Xpm],
	    [XpmReadFileToXpmImage], 
	    [INCXPM="$X_CFLAGS"
	    LIBXPM="$X_LIBS -lXpm $X_PRE_LIBS -lX11 $X_EXTRA_LIBS"
	    SUPPORT_XPM="true"],
	    [],
	    [$X_PRE_LIBS -lX11 $X_EXTRA_LIBS]
	)]
    )
    CFLAGS="$save_CFLAGS"
    LDFLAGS="$save_LDFLAGS"
fi
AC_SUBST(LIBXPM)
AC_SUBST(INCXPM)
AC_SUBST(SUPPORT_XPM)
AM_CONDITIONAL(HAVE_XPM, test x$SUPPORT_XPM = xtrue)

# Checks freetype support
SUPPORT_FREETYPE="false"
LIBFREETYPE=""
INCFREETYPE=""

AC_ARG_WITH(
    [freetype],
    [AC_HELP_STRING(
	[--with-freetype],
	[enable freetype support (default: yes)]
    )],
    [case "$withval" in
	yes) with_freetype=yes;;
	no)  with_freetype=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-freetype]);;
    esac],
    [with_freetype=yes]
)

if test "$with_freetype" = "no" ; then
    AC_MSG_RESULT([disabled])
else
    AC_PATH_PROG(FREETYPE_CONFIG, freetype-config)
    if test -n "$FREETYPE_CONFIG"; then
	LIBFREETYPE=`freetype-config --libs`
	INCFREETYPE=`freetype-config --cflags`
	SUPPORT_FREETYPE="true"
    fi
fi
AC_SUBST(INCFREETYPE)
AC_SUBST(LIBFREETYPE)
AC_SUBST(SUPPORT_FREETYPE)
AM_CONDITIONAL(HAVE_FREETYPE, test x$SUPPORT_FREETYPE = xtrue)
 
# Checks gs support
SUPPORT_PS="false"

AC_ARG_WITH(
    [gs],
    [AC_HELP_STRING(
	[--with-gs],
	[enable gs support (default: yes)]
    )],
    [case "$withval" in
	yes) with_gs=yes;;
	no)  with_gs=no;;
	*)   AC_MSG_ERROR([bad value $withval for --with-gs]);;
    esac],
    [with_gs=yes]
)

if test "$with_gs" = "no" ; then
    AC_MSG_RESULT([disabled])
else
    AC_PATH_PROG(GS, gs) 
    SUPPORT_PS="false"
    if test -n "$GS"; then
	SUPPORT_PS="true"
    fi
fi
AC_SUBST(SUPPORT_PS)
AM_CONDITIONAL(HAVE_PS, test x$SUPPORT_PS = xtrue)

# Checks rgb.txt
PATH_RGB_TXT="none"
if test -f "$x_libraries/X11/rgb.txt" ; then
    PATH_RGB_TXT="$x_libraries/X11/rgb.txt"
fi
AC_SUBST(PATH_RGB_TXT)

# compute camlimages dependencies
REQUIRES=""

if test "$SUPPORT_LABLGTK" = "true"; then
    REQUIRES="camlimages.lablgtk"
fi

if test "$SUPPORT_LABLGTK2" = "true"; then
    REQUIRES="camlimages.lablgtk2"
fi

REQUIRES="$REQUIRES graphics"
AC_SUBST(REQUIRES)

# Create files
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([src/META
		 Makefile
		 src/Makefile
	         doc/Makefile
		 test/Makefile
		 examples/Makefile
		 examples/converter/Makefile
		 examples/crop/Makefile
		 examples/edgedetect/Makefile
		 examples/gifanim/Makefile
		 examples/imgstat/Makefile
		 examples/liv/Makefile
		 examples/monochrome/Makefile
		 examples/normalize/Makefile
		 examples/resize/Makefile
		 examples/tiffps/Makefile
		 examples/ttfimg/Makefile])
AC_OUTPUT()

cat <<EOF
Configuration summary

Language:            $OCAMLVERSION
Bytecode compiler:   $OCAMLC
Native compiler:     $OCAMLOPT
OCamlfind:	     $with_ocamlfind
Installed in:        $OCAMLSITELIBDIR
Lablgtk support:     $SUPPORT_LABLGTK
Lablgtk2 support:    $SUPPORT_LABLGTK2
Freetype support:    $SUPPORT_FREETYPE
PNG support:         $SUPPORT_PNG
PS support:          $SUPPORT_PS
GIF support:         $SUPPORT_GIF
JPEG support:        $SUPPORT_JPEG
TIFF support:        $SUPPORT_TIFF
XPM support:         $SUPPORT_XPM
EOF
