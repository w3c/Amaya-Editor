dnl
dnl Process this file with autoconf to produce a configure script.
dnl Autoconf script for Amaya and Thot.
dnl Daniel Veillard and Irene Vatton, 1997
dnl

dnl  ***************************************************************
dnl
dnl    GENERIC AUTOCONF STUFF : GUESS INFORMATIONS ON SYSTEM SETUP
dnl
dnl  ***************************************************************

AC_PREREQ(2.54)
AC_INIT(thotlib/internals/h/constpiv.h)
AC_REVISION

dnl
dnl Verify that configure is not being called in the Amaya directory
dnl
if test -d batch -a -f configure.in ; then
   echo 
   echo "****************************************************************"
   echo ERROR: You must invoke the configure script in a subidrectory
   echo under the Amaya tree.
   echo For example:
   echo "  cd Amaya ; mkdir obj ; cd obj ; ../configure [your options] "
   echo "****************************************************************"
   echo 
   exit 1
fi

dnl
dnl Checks for platform information
dnl

AC_CANONICAL_SYSTEM
AC_CONFIG_HEADER(config.h)

dnl
dnl Initializing Automake
dnl

AC_PROG_MAKE_SET
AC_ARG_PROGRAM

dnl
dnl Checks for programs.
dnl

AC_PROG_YACC
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PATH_PROG(CP, cp, /bin/cp, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(MV, mv, /bin/mv, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(RM, rm, /bin/rm, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(MKDIR, mkdir, /bin/mkdir, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(ECHO, echo, /bin/echo, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(CAT, cat, /bin/cat, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(CHMOD, chmod, /bin/chmod, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(SED, sed, /bin/sed, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(AR, ar, /usr/bin/ar, $PATH:/bin:/usr/bin:/usr/ucb/bin)
AC_PATH_PROG(DIFF, diff, /usr/bin/diff, $PATH:/bin:/usr/bin:/usr/ucb/bin)


dnl
dnl Checks for libraries.
dnl

AC_CHECK_LIB(socket, socket, )
AC_CHECK_LIB(inet, connect)
AC_CHECK_LIB(nsl, t_accept)
AC_CHECK_LIB(dl, dlopen)
#AC_PATH_X
#AC_PATH_XTRA

dnl
dnl Checks for header files.
dnl

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS(stdio.h)
AC_CHECK_HEADERS(sys/types.h types.h)
AC_CHECK_HEADERS(sys/unistd.h unistd.h)
AC_CHECK_HEADERS(sys/fcntl.h fcntl.h)
AC_CHECK_HEADERS(sys/limits.h limits.h)
AC_CHECK_HEADERS(sys/stat.h stat.h)
AC_CHECK_HEADERS(sys/file.h)
AC_CHECK_HEADERS(sys/ioctl.h)
AC_CHECK_HEADERS(sys/time.h time.h)
AC_CHECK_HEADERS(sys/systeminfo.h)
AC_CHECK_HEADERS(sys/string.h strings.h)
AC_CHECK_HEADERS(sys/syslog.h syslog.h)
AC_CHECK_HEADERS(sys/socket.h socket.h)

# SG: not used ? => do not compile on macosx
# AC_CHECK_HEADERS(appkit/appkit.h appkit.h)
# AC_CHECK_HEADERS(apppkit/apppkit.h apppkit.h)

AC_CHECK_HEADERS(netinet/in.h in.h)
AC_CHECK_HEADERS(dn.h)
AC_CHECK_HEADERS(sys/ipc.h)
AC_CHECK_HEADERS(net/errno.h sys/errno.h errno.h)
AC_CHECK_HEADERS(pwd.h)
AC_CHECK_HEADERS(grp.h)
AC_CHECK_HEADERS(arpa/inet.h inet.h)
AC_CHECK_HEADERS(netdb.h)
AC_CHECK_HEADERS(manifest.h)
AC_CHECK_HEADERS(bsdtypes.h)
AC_CHECK_HEADERS(stdefs.h)
AC_CHECK_HEADERS(bsdtime.h)
AC_CHECK_HEADERS(sys/select.h select.h)
AC_CHECK_HEADERS(dnetdb.h)
AC_CHECK_HEADERS(libc.h)
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(malloc.h)
AC_CHECK_HEADERS(memory.h)
AC_CHECK_HEADERS(unixlib.h)
AC_CHECK_HEADERS(ctype.h)
AC_CHECK_HEADERS(cursesX.h curses.h)
AC_CHECK_HEADERS(sys/resource.h resource.h)
AC_HEADER_DIRENT
AC_CHECK_HEADERS(dir.h direct.h) dnl AC_HEADER_DIRENT only checks for some..
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STAT
AC_CHECK_HEADERS(stdint.h)

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl

AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_TYPE_UID_T

dnl
dnl Checks for library functions.
dnl

AC_FUNC_ALLOCA
AC_TYPE_GETGROUPS
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(getcwd gethostname gettimeofday getwd mkdir mktime putenv select socket strdup strerror strftime strstr strtod strtol uname lstat strcasecmp strncasecmp strchr memcpy)

#
# Check for operating system (UNIX / WINDOWS / MACOSX )
# ---------->
# canonicalize the target OS
AMAYAOS="LINUX"
case $target_os in
  aix*)       AMAYAOS="LINUX" ;;
  cygwin*)    AMAYAOS="WINDOWS" ;;
  darwin*)    AMAYAOS="MACOSX" ;;
  freebsd2*)  AMAYAOS="FREEBSD" ;;
  freebsd*)   AMAYAOS="FREEBSD" ;;
  hpux*)      AMAYAOS="LINUX" ;;
  irix*)      AMAYAOS="LINUX" ;;
  linuxaout*) AMAYAOS="LINUX" ;;
  linux*)     AMAYAOS="LINUX" ;;
  mingw32*)   AMAYAOS="WINDOWS" ;;
  netbsd*)    AMAYAOS="LINUX" ;;
  openbsd*)   AMAYAOS="LINUX" ;;
  osf[12]*)   AMAYAOS="LINUX" ;;
  osf*)       AMAYAOS="LINUX" ;;
  solaris2*)  AMAYAOS="FREEBSD" ;;
  sunos4*)    AMAYAOS="FREEBSD" ;;
  ultrix*)    AMAYAOS="LINUX" ;;
esac
AC_SUBST(AMAYAOS)

if test "$AMAYAOS" = "WINDOWS" ; then
  AC_MSG_ERROR(Windows port do not use configure - not yet tested.)
fi

OS_OPTIONS="-D_UNIX" # default is UNIX
case $AMAYAOS in
  LINUX)       OS_OPTIONS="-D_UNIX" ;;
  WINDOWS)     OS_OPTIONS="-D_WINDOWS" ;; # not tested (cygwin or mingw)
  MACOSX)      OS_OPTIONS="-D_UNIX -D_MACOS" ;;
  FREEBSD)     OS_OPTIONS="-D_UNIX -D_FREEBSD" ;;
esac

dnl
dnl GL canvas is disabled by default in 8.7.x
dnl GL canvas is enabled by default in 9.x
dnl - 9.x needs : GL, GLX, GLU, freetype2, wxGLCanvas
dnl GL is for opengl
dnl GlX is for XWindow integration
dnl GLU is for tesselation algortihms (until we extract it from this lib)
dnl (tesselation is used for polygons with holes in it...)
dnl freetype2 is for text drawing (fonts ...)
dnl wxGLCanvas is for integration of opengl in wxWidgets (WX)

with_wx="yes"

AC_ARG_ENABLE([system-wx],
    AC_HELP_STRING([--enable-system-wx],[try the wxWidgets system library]),
    [case "${enableval}" in
      yes) with_system_wx="yes";;
      no)  with_system_wx="no" ;;
      *) AC_MSG_ERROR(bad value ${enableval} for --enable-system-wx) ;;
    esac],
    [with_system_wx="no"])



AC_ARG_WITH([gl],
   AC_HELP_STRING([--with-gl],[enable canvas with hardware OpenGL implementation]),
   [case "${withval}" in
	  yes) with_gl="yes" ;;
	  no)  with_gl="no" ;;
	  *) AC_MSG_ERROR(bad value ${withval} for --with-gl) ;;
	esac],
	[with_gl="no"])

# check for mesa library (OpenGL)
AC_ARG_WITH(MESA,              [  --with-mesa             enable canvas with software OpenGL implementation (default) ])
if test "$with_mesa" = "" ; then
  with_mesa="no"
fi


# mesa (software) gl implementation is default with wx (hardware gl implem can be choosen with --with-gl option)
if test "$AMAYAOS" = "MACOSX" ; then
  with_mesa="no"
  with_gl="yes"
else
  if test "$with_gl" = "no" ; then
    with_mesa="yes"
  fi
fi

# check if wxWidgets debug is enabled
AC_ARG_WITH(wxdebug,              [  --with-wxdebug          compiles wxWidgets with debuging options ])
if test "$with_wxdebug" = "yes" ; then
  with_wxdebug="yes"
else
  with_wxdebug="no"
fi

# it is possible to specify wxWidgets library path,
# usefull for developpers who want to compile amaya with different wxWidgets version
AC_ARG_WITH(wxsrcdir, [  --with-wxsrcdir=...     compiles Amaya with a specific wxWidgets sources path ])
if test "$with_wxsrcdir" = "" ; then
  with_wxsrcdir="../../wxWidgets"
fi


# setup the AMAYAGUI variable used to identify used GUI when 'make install' is called
AMAYAGUI="wx"
AC_SUBST(AMAYAGUI)


if test "$with_mesa" = "yes" ; then
  # --------- compile Mesa library (opengl) --------------- #
  # check that sources of Mesa exists
  if test ! -f ../../Mesa/Makefile ; then
    GL_BUILDDIR=""
    GL_INCLUDES="-I/usr/include"
    GL_LIBRARIES="-Wl,-rpath,usr/lib -L/usr/lib -lGL -lGLU"
    echo "---> Use system Mesa library"
  else
    OLDDIR=`pwd`
    GL_BUILDDIR="${OLDDIR}/Mesa"
    if test ! -d "$GL_BUILDDIR" ; then
      echo "---> Install Mesa library into ${GL_BUILDDIR}"
      mkdir -p ${GL_BUILDDIR}
      cp -r ../../Mesa/* ${GL_BUILDDIR}
      echo "---> Install Mesa library into ${GL_BUILDDIR} - DONE"
    fi
    # this is the flags used to build amaya with OpenGL
    GL_INCLUDES="-I${GL_BUILDDIR}/include"
    GL_LIBRARIES="-Wl,-rpath,${GL_BUILDDIR}/lib -L${GL_BUILDDIR}/lib -lGL -lGLU"
  fi
  GL_OPTIONS="-D_GL"
  AC_SUBST(GL_BUILDDIR)
  AC_SUBST(GL_INCLUDES)
  AC_SUBST(GL_LIBRARIES)
fi

if test "$with_system_wx" = "yes"; then
  AC_PATH_PROG(WXCONFIG, wx-config)
  if test -z "$WXCONFIG"; then
    with_system_wx="no"
    AC_MSG_WARN(wxWidgets system library not found, falling back to the builtin version.)
  fi
fi
  
if test "$with_system_wx" != "yes"; then
  # WX_SRCDIR is better to understand the code than with_wxsrcdir variable
  if test ! -f $with_wxsrcdir/Makefile.in ; then
    $with_wxsrcdir = ../../wxWidgets
  fi
  cd $with_wxsrcdir
  WX_SRCDIR=`pwd`
  cd -
  AC_SUBST(WX_SRCDIR)

  # check that sources of wxWidgets exists
  if test ! -f $WX_SRCDIR/Makefile.in ; then
    AC_MSG_ERROR(wxWidgets library sources not found, check --with-wxsrcdir is correct !)
  fi

  # --------- configure wxWidgets library --------------- #
  echo "---> Configuring wxWidgets library."
  OLDDIR=`pwd`
  # create the build directory (debug or release)
  if test $with_wxdebug = yes ; then
    WX_BUILDDIR="wxWidgets_DEBUG"
  else
    WX_BUILDDIR="wxWidgets_RELEASE"
  fi

  mkdir -p $WX_BUILDDIR
  cd $WX_BUILDDIR
  AC_SUBST(WX_BUILDDIR)

  # remove old configure cached options
  rm -f configarg.cache

  # setup configure options
  
  # disable unused wx features
  #   --disable-htmlhelp
  #   --disable-html
  #   --disable-docview
  #   --disable-mdi
  #   --disable-mdidoc
  #   --disable-sound
  #   --disable-richtext
  #   --disable-backtrace # to avoid the buggy popup when an assert occurs
  WXCONFIGURE_TUNNING_OPTION="--disable-printarch --disable-htmlhelp --disable-html --disable-richtext --disable-docview --disable-mdidoc --disable-sound --disable-backtrace"
  
  if test "$AMAYAOS" = "LINUX" ; then
    # LINUX
    #   --enable-gtk2           use GTK+ 2.0 if available (EXPERIMENTAL)
    #   --disable-shared        create only static library (.a)
    #   --enable-unicode        compile wxString with Unicode support
    #   --with-gtk              use GTK+
    #   --with-opengl           use OpenGL (or Mesa)
    WXCONFIGURE_OPTION="--build=$build_alias --host=$host_alias --target=$target_alias --with-opengl --with-gtk --enable-gtk2 --enable-unicode --enable-ipc --disable-shared  --with-libpng=builtin --with-libjpeg=builtin --with-libtiff=builtin  --with-libxpm=builtin $WXCONFIGURE_TUNNING_OPTION"
  fi
  if test "$AMAYAOS" = "MACOSX" ; then
    # MACOSX
    WXCONFIGURE_OPTION="--with-opengl --disable-shared --disable-tga --enable-unicode --with-libpng=builtin --with-libjpeg=builtin"
  fi
  if test $with_wxdebug = yes ; then
    WXCONFIGURE_OPTION="$WXCONFIGURE_OPTION --enable-debug"
    # SG: I've disabled other debug options because the lib has bugs when using these flags \
    # (maybe because these flags are not often used by wxWidgets developpers)
    # WXCONFIGURE_OPTION="$WXCONFIGURE_OPTION --enable-debug --enable-debug_cntxt --enable-debug_gdb --enable-debug_info --enable-debug_flag --enable-mem_tracing"
  else
    WXCONFIGURE_OPTION="$WXCONFIGURE_OPTION --enable-optimise"
  fi

  # launch the configure
  if test "$with_mesa" = "yes" ; then
    # use internal OpenGL (mesa) libraries
    $WX_SRCDIR/configure CFLAGS="-I${GL_BUILDDIR}/include" CXXFLAGS="-I${GL_BUILDDIR}/include" CPPFLAGS="-I${GL_BUILDDIR}/include" $WXCONFIGURE_OPTION
    #../$WX_SRCDIR/configure CFLAGS="-I${GL_BUILDDIR}/include" CXXFLAGS="-I${GL_BUILDDIR}/include" CPPFLAGS="-I${GL_BUILDDIR}/include" OPENGL_LIBS="-lGL -lGLU" $WXCONFIGURE_OPTION
    if test $? != 0 ; then
      AC_MSG_ERROR(A problem occurs durring wxWidgets library configuration. Please fix the problem and try again.)
    fi
  else
    # use OpenGL system libraries
    $WX_SRCDIR/configure $WXCONFIGURE_OPTION    
  fi
  cd $OLDDIR
  echo "---> Configuring wxWidgets library. - DONE"

  # --------- configuring wxWidgets paths --------------- #
  WXCONFIG=$WX_BUILDDIR/wx-config
  # check that wx-config exists
  if test ! -f $WXCONFIG ; then
    AC_MSG_ERROR(Cannot find wx-config. Please download wxWidgets (http://www.wxwidgets.org/))
  fi

  # setup include paths  
  # "-I$WX_SRCDIR/src/png -I$WX_SRCDIR/src/jpeg -I$WX_SRCDIR/src/tiff" is a temporary patch to fix incorrect generated paths by wx-config --cxxflags (remove it when wxWidgets will be fixed)
  WX_INCLUDES="-I$WX_SRCDIR/src/png -I$WX_SRCDIR/src/jpeg -I$WX_SRCDIR/src/tiff"
  BUILDWXWIDGETS="wxwidgets"
  AC_SUBST(BUILDWXWIDGETS)

fi #!$with_system_wx

WX_INCLUDES="$WX_INCLUDES `$WXCONFIG --cxxflags`"
AC_SUBST(WX_INCLUDES)

# setup libraries paths
WX_LIBRARIES=""
WX_LIBRARIES_EXTRA_PARAM=""
WXCONFIG_LIBS_PARAM="aui,xrc,adv,core,xml,net"
# GL is used ?
if test "$with_gl" = "yes" -o "$with_mesa" = "yes" ; then
  WXCONFIG_LIBS_PARAM="${WXCONFIG_LIBS_PARAM},gl"
  # SG: this is a mistery why wx-config doesn't add these needed option on macosx
  if test "$AMAYAOS" = "MACOSX" ; then
    WX_LIBRARIES_EXTRA_PARAM="-framework OpenGL -framework AGL"
  fi
fi
if test "$AMAYAOS" = "MACOSX" ; then
   WXCONFIG_LIBS_PARAM="${WXCONFIG_LIBS_PARAM},html"
fi

WX_LIBRARIES="`$WXCONFIG --libs $WXCONFIG_LIBS_PARAM` ${WX_LIBRARIES_EXTRA_PARAM}"
AC_SUBST(WX_LIBRARIES)
WX_LINKDEPS="`$WXCONFIG --linkdeps`"
AC_SUBST(WX_LINKDEPS)
WX_OPTIONS="-D_WX"
# --------- configuring wxWidgets paths - DONE --------------- #

if test "${GCC}" = "yes"; then
  CPP="${CPP} -x c"
fi

dnl  ***************************************************************
dnl
dnl    SPECIFIC STUFF : CONFIGURATION OPTIONS FOR AMAYA
dnl
dnl  ***************************************************************

# Do not care with libpng or libjpeg with wx version because we use the wxwidgets one.

# the wx version
AMAYAWX_MAJVER=11
AMAYAWX_MINVER=4
AMAYAWX_SUBVER=4
APP_MAJVER=$AMAYAWX_MAJVER
APP_MINVER=$AMAYAWX_MINVER
APP_SUBVER=$AMAYAWX_SUBVER

if test "$APP_SUBVER" = "0" ; then
  APP_VER=$APP_MAJVER.$APP_MINVER
else
  APP_VER=$APP_MAJVER.$APP_MINVER.$APP_SUBVER
fi

# generate a header file with the right version number, so it can be used on windows AND on unix
APPVERSION_H=../thotlib/internals/h/appversion.h
echo "#ifndef __APPVERSION_H__" > $APPVERSION_H
echo "#define __APPVERSION_H__" >> $APPVERSION_H
echo "#define APP_MAJVER $AMAYAWX_MAJVER" >> $APPVERSION_H
echo "#define APP_MINVER $AMAYAWX_MINVER" >> $APPVERSION_H
echo "#define APP_SUBVER $AMAYAWX_SUBVER" >> $APPVERSION_H
echo "#endif /* __APPVERSION_H__ */" >> $APPVERSION_H

dnl
dnl We use a special versoin of cextract patched for alignment
dnl to build the signatures of the modules functions located in f subdirs
dnl
CEXTRACT_VER="1.7"
CEXTRACT_FLAGS="+O"

# Check if debug is enable
AC_ARG_WITH(debug,              [  --with-debug            compiles Amaya with debuging options (-g) ])
if test "$with_debug" = "yes" ; then
  with_debug="yes"
  CFLAGS="-g"
  CXXFLAGS="-g"
else
  with_debug="no"
  CFLAGS="-O2"
  CXXFLAGS="-O2"
fi

if test "${CC}" = "gcc" ; then
  CFLAGS="${CFLAGS} -Wall"
  case "${host}" in
      *-*-irix5* | *-*irix6* )
         CFLAGS="${CFLAGS} -fsigned-char"
      ;;
  esac
fi

if test "$ac_cv_c_const" = "no" ; then
  CFLAGS="${CFLAGS} -DCONST= "
fi

# force C mode (-x c)
#CFLAGS="${CFLAGS} -Wall"
CFLAGS="${CFLAGS} -x c"

# C++ options (-x c++)
#CXXFLAGS="${CXXFLAGS} -Wall"
CXX_OPTIONS="-D__cplusplus"
CXXFLAGS="${CXXFLAGS} -Wall -x c++ ${CXX_OPTIONS}"

# setup LDFLAGS
# on macosx, specific flags must be set to avoid conflicts with allready installed libwww
# for exemple it's possible to install libwww with fink (fink install libwww)
# to avoid conflicts, we just force the linker to search for specified amaya path first
if test "$AMAYAOS" = "MACOSX" ; then
  LDFLAGS="-Wl,-search_paths_first"
else
  LDFLAGS=""
fi

dnl
dnl Check for availability of amaya sources
dnl

build_amaya="yes"

if test "$with_amaya" = "no" ; then
    build_amaya="no"
else
  if test ! -f $srcdir/amaya/Makefile.in ; then
    AC_MSG_WARN(Amaya browser/editor sources not found, disabling Amaya build !)
    build_amaya="no"
  fi
fi

dnl
dnl Check for additionnal packages
dnl use AC_CONFIG_SUBDIRS to launch configure in the subdir
dnl

if test "$with_gl" = "yes" -o "$with_mesa" = "yes" ; then

  # if with_gl is on then test system opengl library (GL & GLU)
  if test "$with_gl" = "yes" ; then
    if test "$AMAYAOS" = "LINUX" ; then
	# setup a list of standard paths int LDFLAGS
	# for AC_CHECK_LIB library existance checks
	LIBS_OLD=$LIBS
	LDFLAGS_OLD=$LDFLAGS
	LDFLAGS="-L/usr/X11R6/lib -L/usr/lib -L/usr/local/lib"
  	AC_MSG_CHECKING([OpenGL])
	for ac_dir in               		\
		/usr/include			\
		/usr/X11R6/include 		\
		/usr/local/include		\
		; \
	do  if test -r "$ac_dir/GL/gl.h" ; then
          gl_inc="yes"
          GL_INC_I="$ac_dir"
          if test "$ac_dir" != "/usr/include" ; then
            GL_INCLUDES="-I$ac_dir"
	  fi
          AC_CHECK_LIB(GL,glBegin,,AC_MSG_ERROR(Cannot find OpenGL library. Please install OpenGL.))
          AC_MSG_RESULT([yes])
       	  break	
        fi
	done
	
  	AC_MSG_CHECKING([GLU])	
	for ac_dir in               		\
		/usr/include		\
		/usr/X11R6/include		\
		/usr/local/include		\
		; \
	do  if test -r "$ac_dir/GL/glu.h" ; then
          glu_inc="yes"
          GL_INC_II="$ac_dir"
          if test "$ac_dir" != "/usr/include" ; then
            if test "$ac_dir" != "$GL_INC_I" ; then
              GL_INCLUDES="$GL_INCLUDES -I$ac_dir"
            fi
          fi
          # now test that glu version is >= 1.2
          AC_CHECK_LIB(GLU,gluNewTess,,AC_CHECK_LIB(glu32,gluNewTess,,AC_MSG_ERROR(Cannot find OpenGL GLU lib or existing version is < 1.2. Pease upgrade your GLU library.)))
          AC_MSG_RESULT([yes])
       	  break	
        fi
        done
	LDFLAGS=$LDFLAGS_OLD
	LIBS=$LIBS_OLD

    if test "$gl_inc" = "" ; then	        
      AC_MSG_WARN(Opengl headers (gl.h) not found !!)
      AC_MSG_WARN(Disabling opengl build !)
      with_gl="no"
      GL_OPTIONS=	
      GL_INCLUDES=	
      GL_LIBRARIES=
      exit 1;
    fi

    if test "$glu_inc" = "" ; then
      AC_MSG_WARN(Glu headers (glu.h) not found !!)
      AC_MSG_WARN(Disabling opengl build !)
  
      with_gl="no"
      GL_OPTIONS=	
      GL_INCLUDES=	
      GL_LIBRARIES=
      exit 1;
    fi
    fi # AMAYAOS = LINUX

    if test "$AMAYAOS" = "MACOSX" ; then
      GL_OPTIONS="-D_GL"	
      GL_INCLUDES="-I/System/Library/Frameworks/OpenGL.framework/Versions/Current/Headers/"	
      GL_LIBRARIES=""
      GL_BUILDDIR=""
    else
      GL_LIBRARIES="-L/usr/lib -L/usr/X11R6/lib -lGL -lGLU"
      GL_OPTIONS="-D_GL"
    fi

    AC_SUBST(GL_INCLUDES)
    AC_SUBST(GL_LIBRARIES)
  fi # if with_gl
	
  # ---------- Checking for freetype lib (fonts library)
  if test "$AMAYAOS" = "MACOSX" ; then
     # we use a static version of freetype lib for mac osx
     # ** search for includes (freetype.h)
     for ac_dir in               \
         /Developer/SDKs/MacOSX10.6.sdk/usr/X11R6/include/  \
         /Developer/SDKs/MacOSX10.6.sdk/usr/X11R6/include/freetype2/  \
         /Developer/SDKs/MacOSX10.5.sdk/usr/X11R6/include/  \
         /Developer/SDKs/MacOSX10.5.sdk/usr/X11R6/include/freetype2/  \
         /Developer/SDKs/MacOSX10.2.8.sdk/usr/X11R6/include/  \
         /Developer/SDKs/MacOSX10.2.8.sdk/usr/X11R6/include/freetype2/  \
         /Developer/SDKs/MacOSX10.3.0.sdk/usr/X11R6/include/  \
         /Developer/SDKs/MacOSX10.3.0.sdk/usr/X11R6/include/freetype2/  \
         /Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/include/  \
         /Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/include/freetype2/  \
         /Developer/SDKs/MacOSX10.3.9.sdk/usr/X11R6/include/  \
         /Developer/SDKs/MacOSX10.3.9.sdk/usr/X11R6/include/freetype2/  \
         /Developer/SDKs/MacOSX10.4.0.sdk/usr/X11R6/include/  \
         /Developer/SDKs/MacOSX10.4.0.sdk/usr/X11R6/include/freetype2/  \
         /opt/local/include/  \
         /opt/local/include/freetype2/  \
         /usr/X11R6/include/freetype2/freetype/  \
        ; \
         do
         if test -r "$ac_dir/ft2build.h"; then
            FREETYPE_INCLUDES="${FREETYPE_INCLUDES} -I$ac_dir"
         fi
         if test -r "$ac_dir/freetype/config/ftheader.h"; then
            FREETYPE_INCLUDES="${FREETYPE_INCLUDES} -I$ac_dir"
         fi
     done
     # ** search for binary (libfreetype.a)
     for ac_dir in               \
         /Developer/SDKs/MacOSX10.2.8.sdk/usr/X11R6/lib/  \
         /Developer/SDKs/MacOSX10.3.0.sdk/usr/X11R6/lib/  \
         /Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/lib/  \
         /Developer/SDKs/MacOSX10.3.9.sdk/usr/X11R6/lib/  \
         /Developer/SDKs/MacOSX10.4.0.sdk/usr/X11R6/lib/  \
         /opt/local/lib/  \
         /usr/X11R6/lib/  \
         ; \
         do
         if test -r "$ac_dir/libfreetype.a"; then
            FREETYPE_LIBRARIES="$ac_dir/libfreetype.a"
            break
         fi
     done
  else # "$AMAYAOS" = "MACOSX"
     if test ! -f $(which freetype-config) ; then
       AC_MSG_ERROR(Cannot find freetype library (font load/display routines). Please install it.)
     fi
     FREETYPE_INCLUDES=`freetype-config --cflags`
     FREETYPE_LIBRARIES=`freetype-config --libs`
  fi # "$AMAYAOS" = "MACOSX"
  AC_SUBST(FREETYPE_INCLUDES)
  AC_SUBST(FREETYPE_LIBRARIES)


  AC_MSG_CHECKING([gcc3])	
  for ac_dir in               		\
		/usr/lib 			\
		/usr/X11R6/lib 		\
		/usr/local/lib		\
		; \
  do  if test -r "$ac_dir/libstdc++.so.5" ; then
    stdglu_inc="yes"
    STDCPP_LIBRARIES="-lstdc++"
    STDCPP_INCLUDES=""
    AC_MSG_RESULT([yes])	
    break
  fi
  done

fi # if with_gl or with_mesa

AC_ARG_ENABLE([svg],
	AC_HELP_STRING([--disable-svg],[disable SVG support]),
	[case "${enableval}" in
	  yes) with_svg="yes" ;;
	  no)  with_svg="no" ;;
	  *) AC_MSG_ERROR(bad value ${enableval} for --disable-svg) ;;
	esac],
	[with_svg="yes"])

#################### LibWWW and WebDAV Support ################################

AC_ARG_ENABLE([system-libwww],
	AC_HELP_STRING([--enable-system-libwww],[try the libWWW system library]),
	[case "${enableval}" in
	  yes) with_system_libwww="yes" ;;
	  no)  with_system_libwww="no" ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-system-libwww]) ;;
	esac],
	[with_system_libwww="no"])

if test "$with_system_libwww" = "yes" ; then
    # check to see if we have a libWWW library
    AC_PATH_PROG([LIBWWW_CONFIG],[libwww-config])

    if test "$LIBWWW_CONFIG" = "" ; then
		AC_MSG_WARN(libwww system library not found, falling back to the builtin version.)
		with_system_libwww="no"
    fi
fi

dnl
dnl WebDAV support enabled by default
dnl
AC_ARG_WITH(dav,
  AC_HELP_STRING([--with-dav],[enable WebDAV support (default)]),
  [case "${withval}" in
	  yes) with_dav="yes" ;;
	  no)  with_dav="no" ;;
	  *) AC_MSG_ERROR(bad value ${withval} for --with-dav) ;;
	esac],
	[with_dav="yes"])

# If DAV is enabled, check if the source code is present
if test ! -f $srcdir/davlib/Makefile.in ; then
	if test "$with_dav" = "yes" ; then
	    AC_MSG_WARN(WebDAV makefile not found, disabling WebDAV build)
	    with_dav="no"
	fi
fi

if test "$with_dav" = "yes" ; then
   WITHDAV="--with-dav"
else
   WITHDAV="--without-dav"
fi

if test "$with_system_libwww" != "yes" ; then
  # configure libwww
  CURRENT_PATH="`pwd`"
  mkdir libwww
  cd libwww && \
    $CURRENT_PATH/../../libwww/configure \
    --build=$build_alias --host=$host_alias --target=$target_alias \
    --disable-shared \
    $WITHDAV \
    --with-zlib --with-ssl && \
    echo "libwww is configured" ; \
    echo "patching wwwconf.h because appkit.h do not compile on macos, and it seems that appkit.h is not used in amaya" ; \
    cp wwwconf.h wwwconf.h.orig ; \
    sed -e "s/\(\#define HAVE_APPKIT_APPKIT_H 1\)/\/* \1 *\//" \
			wwwconf.h.orig > wwwconf.h ; \
    echo "wwwconf.h patched !" ; \
  cd ..
  chmod +x libwww/libwww-config
  BUILDLIBWWW="libwww"
  LIBWWW_CONFIG="libwww/libwww-config"
  LIBWWW_LDFLAGS="-L$CURRENT_PATH/libwww/Library/src/.libs -lwwwxml -lwwwzip -lwwwinit -lwwwapp -lwwwhtml -lwwwtelnet -lwwwnews -lwwwhttp -lwwwmime -lwwwgopher -lwwwftp -lwwwfile -lwwwdir -lwwwcache -lwwwstream -lwwwmux -lwwwtrans -lwwwcore -lwwwutils -lwwwdav \
  -L$CURRENT_PATH/libwww/Library/src/SSL/.libs -lwwwssl\
  -L$CURRENT_PATH/libwww/modules/md5/.libs -lmd5 \
  -L$CURRENT_PATH/libwww/modules/expat/.libs -lexpat \
  -L/usr/lib -lssl -lcrypto"
  LIBWWW_CFLAGS="-I$CURRENT_PATH/../../libwww/Library/src -I$CURRENT_PATH/libwww -I$CURRENT_PATH/../../libwww/modules/expat/lib -I$CURRENT_PATH/../../libwww/Library/src/SSL"
else
  LIBWWW_CFLAGS="`$LIBWWW_CONFIG --cflags`"
  LIBWWW_LDFLAGS="`$LIBWWW_CONFIG --libs`"

  if test "$with_dav" = "yes" ; then
    AC_CHECK_LIB([wwwdav],
				 [toto],
				 [LIBWWW_LDFLAGS="$LIBWWW_LDFLAGS -lwwwdav"],
				 [AC_MSG_WARN([libwwwdav is not available, disabling WebDAV]); with_dav="no"])
  fi
fi

if test "$with_dav" = "yes" ; then
   DAV_OPTIONS=-DDAV
   DAVDIR="davlib"
else
   with_dav="no"
   DAV_OPTIONS=""
   DAVDIR=""
fi

AC_SUBST(BUILDLIBWWW)

AC_SUBST(LIBWWW_CFLAGS)
AC_SUBST(LIBWWW_LDFLAGS)

AC_SUBST(DAVDIR)
AC_SUBST(LIBDAV)
AC_SUBST(WWWDAV)

############# end of LibWWW and WebDAV Support ################################

##########################Annotations Support####################
dnl
dnl annotations are enabled by default
dnl
AC_ARG_ENABLE([annot],
	AC_HELP_STRING([--disable-annot],[disable Annotations support]),
	[case "${enableval}" in
	  yes) with_annot="yes" ;;
	  no)  with_annot="no" ;;
	  *) AC_MSG_ERROR(bad value ${enableval} for --disable-annot) ;;
	esac],
	[with_annot="yes"])


#annot need Raptor
if test "$with_annot" = "yes" ; then
	with_raptor="yes"
else
	with_raptor="no"
fi

AC_ARG_ENABLE([system-raptor],
	AC_HELP_STRING([--enable-system-raptor],[try the raptor RDF system library]),
	[case "${enableval}" in
	  yes) with_system_raptor="yes";;
	  no)  with_system_raptor="no" ;;
	  *) AC_MSG_ERROR(bad value ${enableval} for --enable-system-raptor) ;;
	esac],
	[with_system_raptor="no"])

#If Raptor is needed and we are asked
#to use the system library, check if is present
if test "$with_raptor" = "yes" -a "$with_system_raptor" = "yes" ; then
    # check to see if we have a raptor library
    AC_PATH_PROG([RAPTOR_CONFIG],[raptor-config])

    if test "$RAPTOR_CONFIG" = "" ; then
      AC_MSG_WARN(Not using system-provided Raptor library)
      with_system_raptor="no"
    fi
fi

#if we use the builtin library,
#check it is present
if test "$with_raptor" = "yes" -a "$with_system_raptor" != "yes" \
	-a ! -d "$srcdir/../redland/raptor" ; then
	AC_MSG_WARN(Raptor source dir not found !!)
	AC_MSG_WARN(Disabling raptor build !)
	with_raptor="no"
fi

#At this point, we know everything about Raptor,
#so either set the variables we will need later,
#or disable Annotations
if test "$with_raptor" = "yes" ; then
	if test "$with_system_raptor" != "yes" ; then
		AMAYA_ANNOT_OPTIONS="$AMAYA_ANNOT_OPTIONS -I\$(THOTDIR)/../redland/raptor "
		AMAYA_RAPTOR_LIBS="../raptor/.libs/libraptor.a"
		AMAYA_RAPTOR_INCLUDES="-I\$(THOTDIR)/../redland/raptor"
		BUILDRAPTOR="raptor"
		CURRENTPATH="`pwd`"
		mkdir $BUILDRAPTOR
		cd $BUILDRAPTOR &&
			CC="$CC" CXX="$CXX" \
			CPPFLAGS="-I$CURRENTPATH/../../libwww/modules/expat/lib" \
			LDFLAGS="-L$CURRENTPATH/libwww/modules/expat -L$CURRENTPATH/libwww/modules/expat/.libs" \
			../../../redland/raptor/configure \
			--build=$build_alias --host=$host_alias --target=$target_alias \
			--disable-shared \
			--with-xml-parser=expat \
			--with-www=none
			$ECHO "Raptor is configured"
			$ECHO "Patching the raptor_config.h file to avoid including glib2-0"
			$CP raptor_config.h raptor_config.h.orig
			$SED "s/\(\#define HAVE_G_UTF8_NORMALIZE 1\)/\/* \1 *\//" \
				raptor_config.h.orig > raptor_config.h
			$RM raptor_config.h.orig
			$ECHO "Raptor is patched"
		cd ..
	else
		raptor_includes=`$RAPTOR_CONFIG --cflags`
		raptor_libs="-lraptor"
		BUILDRAPTOR=""
 		AMAYA_ANNOT_OPTIONS="$AMAYA_ANNOT_OPTIONS"
		AMAYA_ANNOT_INCLUDES="$raptor_includes -I\$(THOTDIR)/thotlib/include"
		AMAYA_ANNOT_EXTRA_LIBS="$raptor_libs"
		AMAYA_RAPTOR_INCLUDES="$raptor_includes"
       fi
else
	with_annot="no"
fi

ANNOTLIB_COMPILE_BM=['#']

AC_SUBST(BUILDRAPTOR)

AC_SUBST(AMAYA_ANNOT_OPTIONS)
AC_SUBST(AMAYA_ANNOT_INCLUDES)
AC_SUBST(AMAYA_ANNOT_EXTRA_LIBS)

AC_SUBST(ANNOTLIB_COMPILE_BM)
AC_SUBST(AMAYA_RAPTOR_LIBS)
AC_SUBST(AMAYA_RAPTOR_INCLUDES)

###################End of Annotations Support####################


dnl
dnl The templates support is enabled by default
dnl
AC_ARG_ENABLE([templates],
	AC_HELP_STRING([--disable-templates],[disable experimental templates support]),
	[case "${enableval}" in
	  yes) with_templates="yes" ;;
	  no)  with_templates="no" ;;
	  *) AC_MSG_ERROR(bad value ${enableval} for --disable-templates) ;;
	esac],
	[with_templates="yes"])

if test "$with_templates" = yes -a "$with_wx" != "yes"; then
    AC_MSG_WARN(Templates are only available in WX version !!)
    AC_MSG_WARN(Disabling templates)
    with_templates="no"
fi

if test "$with_templates" = "yes" ; then
    TEMPLATES_OPTIONS="-DTEMPLATES"
else
    TEMPLATES_OPTIONS=""
fi


dnl
dnl Generic XML support enabled by default
dnl
AC_ARG_ENABLE([generic-xml],
	AC_HELP_STRING([--disable-generic-xml],[disable Generic XML support]),
	[case "${enableval}" in
	  yes) with_generic_xml="yes" ;;
	  no)  with_generic_xml="no" ;;
	  *) AC_MSG_ERROR(bad value ${enableval} for --disable-generic-xml) ;;
	esac],
	[with_generic_xml="yes"])

if test "$with_generic_xml" = "yes" ; then
 GENERIC_XML_OPTIONS="-DXML_GENERIC"
else
 GENERIC_XML_OPTIONS=""
fi


if test "$build_amaya" = "yes" ; then
    if test ! -f $srcdir/amaya/MathML.S ; then
	if test "$with_math" = "yes" ; then
	    AC_MSG_WARN(MathML sources not found !!)
	    AC_MSG_WARN(Disabling Math build !)
	    with_math="no"
	    MATH_OPTIONS=
	fi
    fi

    if test ! -f $srcdir/amaya/SVG.S ; then
	if test "$with_svg" = "yes" ; then
	    AC_MSG_WARN(SVG sources not found !!)
	    AC_MSG_WARN(Disabling SVG build !)
	    with_svg="no"
	    SVG_OPTIONS=
	fi
    fi

fi

dnl
dnl The options for each library or binary
dnl
THOT_OPTIONS="$OS_OPTIONS $GL_OPTIONS $WX_OPTIONS"

AMAYA_OPTIONS="$OS_OPTIONS $MATH_OPTIONS $SVG_OPTIONS $GENERIC_XML_OPTIONS $GL_OPTIONS $WX_OPTIONS $TEMPLATES_OPTIONS"

if test "$with_debug" = "yes" ; then
  AMAYA_OPTIONS="$AMAYA_OPTIONS -DAMAYA_DEBUG"
fi

if test "$with_wx" = "yes" ; then
  IMGLIBS=""
else
  IMGLIBS="-ljpeg -lpng -lz"
fi

EXTRA_INCLUDES=

# check for malloc.h
# on macosx, this file is not at the right place
if test "$AMAYAOS" = "MACOSX" ; then
for ac_dir in               \
  /Developer/SDKs/MacOSX10.6.sdk/usr/include/objc/   \
  /Developer/SDKs/MacOSX10.6.sdk/usr/include/sys/    \
  /Developer/SDKs/MacOSX10.5.sdk/usr/include/objc/   \
  /Developer/SDKs/MacOSX10.5.sdk/usr/include/sys/    \
  /Developer/SDKs/MacOSX10.2.8.sdk/usr/include/objc/   \
  /Developer/SDKs/MacOSX10.2.8.sdk/usr/include/sys/    \
  /Developer/SDKs/MacOSX10.3.0.sdk/usr/include/objc/   \
  /Developer/SDKs/MacOSX10.3.0.sdk/usr/include/sys/    \
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include/objc/   \
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include/sys/    \
  /Developer/SDKs/MacOSX10.3.9.sdk/usr/include/objc/   \
  /Developer/SDKs/MacOSX10.3.9.sdk/usr/include/sys/    \
  ; \
do
  if test -r "$ac_dir/malloc.h"; then
    echo "found malloc.h"
    EXTRA_INCLUDES="${EXTRA_INCLUDES} -I$ac_dir"
    break
  fi
done
else # "$AMAYAOS" = "MACOSX"
for ac_dir in               \
  /usr/include/malloc/      \
  /sw/include/malloc/       \
  ; \
do
  if test -r "$ac_dir/malloc.h"; then
    echo "found malloc.h"
    EXTRA_INCLUDES="${EXTRA_INCLUDES} -I$ac_dir"
    break
  fi
done
fi # "$AMAYAOS" = "MACOSX"

dnl
dnl Set up the PATHs needed
dnl

API="\$(THOTDIR)/thotlib/include"
VAR="\$(THOTDIR)/thotlib/internals/var"
INCL="\$(THOTDIR)/thotlib/internals/h"
FUNC="\$(THOTDIR)/thotlib/internals/f"

dnl
dnl Set up the include variables.
dnl

THOTINCLUDES="-I${API} -I${VAR} -I${INCL} -I${FUNC} ${JPEGINCL} ${PNGINCL}"
APIINCLUDES=-I${API}
 
if test "${VPATHOPT}" = "" ; then
	VPATHOPT="VPATH"
fi

AMAYA_INSTALLDIR="Amaya"

dnl
dnl Exports the current values of the variables.
dnl

AC_SUBST(AMAYA_INSTALLDIR)

AC_SUBST(APP_MAJVER)
AC_SUBST(APP_MINVER)
AC_SUBST(APP_SUBVER)
AC_SUBST(APP_VER)
AC_SUBST(CEXTRACT_VER)
AC_SUBST(CEXTRACT_FLAGS)

AC_SUBST(THOTINCLUDES)
AC_SUBST(APIINCLUDES)
AC_SUBST(VPATHOPT)

AC_SUBST(THOT_OPTIONS)
AC_SUBST(AMAYA_OPTIONS)
AC_SUBST(IMGLIBS)
AC_SUBST(EXTRA_INCLUDES)

AC_SUBST(MAKE_LIBWWW_RDF_PARSER)
AC_SUBST(LIBWWW_RDF_PARSER)
#AC_SUBST(ANNOTLIB_COMPILE_BM)
AC_SUBST(AMAYA_RAPTOR_LIBS)
AC_SUBST(AMAYA_RAPTOR_INCLUDES)

AC_SUBST(SUBDIRS)

dnl
dnl Include the option Makefile.xxx fragment according to the configure options
dnl

www_frag=$srcdir/amaya/Makefile.libwww
AC_SUBST_FILE(www_frag)

if test "$with_math" = "no" ; then
    math_frag=/dev/null
else
    math_frag=$srcdir/amaya/Makefile.math
fi
AC_SUBST_FILE(math_frag)

if test "$with_svg" = "no" ; then
    svg_frag=/dev/null
else
    svg_frag=$srcdir/amaya/Makefile.svg
fi
AC_SUBST_FILE(svg_frag)

if test "$with_annot" = "yes" ; then
    annot_frag=$srcdir/annotlib/Makefile.annot
else
    annot_frag=/dev/null
fi
AC_SUBST_FILE(annot_frag)

if test "$with_dav" = "yes" ; then
    dav_frag=$srcdir/davlib/Makefile.dav
else
    dav_frag=/dev/null
fi
AC_SUBST_FILE(dav_frag)

if test "$with_templates" = "yes" ; then
    templates_frag=$srcdir/amaya/Makefile.templates
else
    templates_frag=/dev/null
fi
AC_SUBST_FILE(templates_frag)

dnl  ***************************************************************
dnl
dnl      FINAL STEP : PRODUCE ALL THE MAKEFILES AND CONFIG FILES
dnl
dnl  ***************************************************************

EXTRA_MAKEFILE_IN=
make_output="Makefile Options.orig:Options.in tools/Makefile \
       tools/mkdep/Makefile \
       tools/cextract-1.7/Makefile thotlib/Makefile \
       thotlib/include/Makefile batch/Makefile \
       script_install:../batch/script_install.in \
       script_install_gnomekde:../batch/script_install_gnomekde.in \
       script_install_macosxbundle:../batch/script_install_macosxbundle.in \
       script_uninstall:../batch/script_uninstall.in"

if test "$build_amaya" = "yes" ; then
    make_output="$make_output amaya/Makefile"
    EXTRA_MAKEFILE_IN="$srcdir/amaya/Makefile.in \
    $srcdir/amaya/Makefile.libwww"

    if test "$with_annot" = "yes" ; then
       make_output="$make_output annotlib/Makefile"
       EXTRA_MAKEFILE_IN="$srcdir/annotlib/Makefile.in"
    fi

    if test "$with_dav" = "yes" ; then
       make_output="$make_output davlib/Makefile"
       EXTRA_MAKEFILE_IN="$srcdir/davlib/Makefile.in"
    fi

fi

dnl
dnl Generate the script
dnl
if test "$AMAYAOS" != "MACOSX" ; then
    if test ! -d bin ; then
      mkdir -p bin
    fi
    make_output="$make_output bin/amaya:../batch/amaya.in"
fi

AC_SUBST(EXTRA_MAKEFILE_IN)
AC_OUTPUT($make_output , [
dnl
dnl This fragment of shell code is executed after the
dnl Makefile(s) dump ...
dnl

echo timestamp > stamp-h

dnl
dnl Check for an upgrade in the Options numbers
dnl
if test -f Options ; then
   oldversion=`grep Version Options`
   newversion=`grep Version Options.orig`
   if test "$oldversion" != "$newversion" ; then
       echo WARNING : Options files have changed, old options saved to Options.old
       cp Options Options.old
       cp Options.orig Options
       exit 1
   fi
fi
if test -f script_install ; then
   chmod a+x script_install
fi
if test -f script_install_gnomekde ; then
   chmod a+x script_install_gnomekde
fi
if test -f script_install_macosxbundle ; then
   chmod a+x script_install_macosxbundle
fi
if test -f script_uninstall ; then
   chmod a+x script_uninstall
fi
if test -f bin/amaya ; then
   chmod 755 bin/amaya
    echo WARNING : `ls -l bin/amaya`
fi
])

dnl
if test "$build_amaya" = "yes" ; then
    echo Amaya $AMAYA_VER configured
    echo Amaya configured with libWWW \(local source\)
    if test "$with_math" = "yes" ; then
	echo Amaya configured with Math support
    fi
    if test "$with_svg" = "yes" ; then
	echo Amaya configured with SVG support
    fi
    if test "$with_annot" = "yes" ; then
	echo Amaya configured with Annotations support
      if test "$with_system_raptor" = "yes" ; then
 	echo Amaya configured with the RaptorF library \(system\)
      else
	echo Amaya configured with the Raptor library \(local source\)
      fi
    fi
    if test "$with_templates" = "yes" ; then
	echo Amaya configured with templates support
    fi
    if test "$with_generic_xml" = "yes" ; then
	echo Amaya configured with generic XML support
    fi
    echo Amaya configured with internationalization support
    if test "$with_gl" = "yes" ; then
	echo Amaya configured with OpenGL Canvas Rendering \(System library\) \(experimental\)
    fi
    if test "$with_mesa" = "yes" ; then
	echo Amaya configured with OpenGL Canvas Rendering \(MesaLib\)
    fi
    if test "$with_dav" = "yes" ; then
	echo Amaya configured with WebDAV support
    fi
fi

if test "$with_wx" = "yes" ; then
  if test "$with_wxdebug" = "yes" ; then
    echo "Amaya configured with wxWidgets {srcdir=$WX_SRCDIR} (debug)"
  else
    echo "Amaya configured with wxWidgets {srcdir=$WX_SRCDIR}"
  fi
fi


echo ""
echo "Now type \"make\" (GNU make preferably) to build the binaries"
echo ""

