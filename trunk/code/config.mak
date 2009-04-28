#---------------------------------------------------------------------
#   config.mak
#   ==========
#   Frontend for configuration options, customize to your needs...
#
#   *** THESE SETTINGS ONLY APPLY WHEN BUILDING FROM THE COMMAND LINE ***
#
#   30-Apr-99   floh    created
#   20-Aug-99   floh    translated to English
#   13-Feb-00   floh    removed support for non-glibc systems
#   22-Mar-00   floh    added 'Linux' as platform recognition pattern
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# N_PLATFORM = __LINUX__, __WIN32__
# Automatically try to determine the platform we are running
# on. WinNT and Linux offer the 'OSTYPE' env var, Win9x not.
#---------------------------------------------------------------------
N_PLATFORM = __WIN32__
ifeq ($(OSTYPE),linux)
  N_PLATFORM = __LINUX__
endif 
ifeq ($(OSTYPE),linux-gnu)
  N_PLATFORM = __LINUX__
endif
ifeq ($(OSTYPE),Linux)
  N_PLATFORM = __LINUX__
endif
ifeq ($(OSTYPE),darwin)
  N_PLATFORM = __MACOSX__
endif

#---------------------------------------------------------------------
# N_COMPILER = __VC__, __GNUC__
# Based on N_PLATFORM, set the compiler type macro.
#---------------------------------------------------------------------
ifeq ($(N_PLATFORM),__WIN32__)
  N_COMPILER = __VC__
else 
  N_COMPILER = __GNUC__
endif

#---------------------------------------------------------------------
# N_TARGETDIR
# Defines target directory for binaries
# Append 'd' to N_TARGETDIR in debug builds.
#---------------------------------------------------------------------
ifeq ($(wildcard .release),.release)
  N_TDAPPEND = 
else
  N_TDAPPEND = d
endif
ifeq ($(N_PLATFORM),__WIN32__)
  N_TARGETDIR = $(NOMADS_HOME)/bin/win32$(N_TDAPPEND)/
endif
ifeq ($(N_PLATFORM),__LINUX__)
  N_TARGETDIR = $(NOMADS_HOME)/bin/linux$(N_TDAPPEND)/
endif
ifeq ($(N_PLATFORM),__MACOSX__)
  N_TARGETDIR = $(NOMADS_HOME)/bin/macosx$(N_TDAPPEND)/
endif

#---------------------------------------------------------------------
# N_USE_STLPORT
# Define whether or not STLPort should be used on Linux and Mac OS X
# platforms.  This should be enabled for gcc 2.9x and disabled for
# gcc 3.x (as gcc 3.x has a suitable STL implementation).
#---------------------------------------------------------------------
#
ifeq ($(N_PLATFORM),__WIN32__)
  N_USE_STLPORT = false
endif
ifeq ($(N_PLATFORM),__LINUX__)
  N_USE_STLPORT = true
endif

ifeq ($(N_PLATFORM),__MACOSX__)
  N_USE_STLPORT = false
endif

#---------------------------------------------------------------------
# N_STLPORT_HOME
# Defines include directory for STLport
#---------------------------------------------------------------------
#
ifeq ($(N_PLATFORM),__LINUX__)
  N_STLPORT_HOME = /usr/include/stlport
endif
ifeq ($(N_PLATFORM),__MACOSX__)
  N_STLPORT_HOME = /usr/include/stlport
endif

#---------------------------------------------------------------------
# N_STLPORT_CFLAGS
# Defines CFLAGS to use with STLport.  If you want to use STLport's 
# I/O streams, set the value of this flag to an empty string, although
# you'll then need to modify each link line to include -lstlport.
#---------------------------------------------------------------------
ifeq ($(N_PLATFORM),__LINUX__)
  N_STLPORT_CFLAGS = -D_STLP_NO_OWN_IOSTREAMS
endif
ifeq ($(N_PLATFORM),__MACOSX__)
  N_STLPORT_CFLAGS = -D_STLP_NO_OWN_IOSTREAMS
endif

#---------------------------------------------------------------------
# N_PYTHON_INCLUDES
# Defines include directory for Python
#---------------------------------------------------------------------
ifeq ($(N_PLATFORM),__LINUX__)
  N_PYTHON_INCLUDES = /usr/include/python2.2/
endif

ifeq ($(N_PLATFORM),__MACOSX__)
  N_PYTHON_INCLUDES = /usr/include/python2.2/
endif

#---------------------------------------------------------------------
# N_PYTHON_LIBS
# Defines lib directory for Python
#---------------------------------------------------------------------
ifeq ($(N_PLATFORM),__LINUX__)
  N_PYTHON_LIBS = /usr/lib/python2.2/config/
endif

#---------------------------------------------------------------------
# N_NOTHREADS = true/false (default=false, except on OS X)
# Set to 'true' when you want to compile without multithreading
# support. This is mainly for debugging reasons. Please note
# that all sockets based code uses multithreading and will
# not function when multithreading is turned off.
#---------------------------------------------------------------------
#
ifeq ($(N_PLATFORM),__MACOSX__)
  N_NOTHREADS = true
else
  N_NOTHREADS = false
endif

#---------------------------------------------------------------------
# N_MEMMANAGER = true/false
# Turn on/off Nebula's own memory manager. The mem manager optimizes
# small block allocations, and offers some debugging support
# (bounds and mem leak checking).
# GCC doesn't like it to overload the global 'new' operator for
# some reason.
#---------------------------------------------------------------------
#
ifeq ($(N_COMPILER),__GNUC__)
  N_MEMMANAGER = false
else
  N_MEMMANAGER = true
endif

#---------------------------------------------------------------------
# N_OPTIMIZEFLAGS
# Defines additional flags when building the release version.
#---------------------------------------------------------------------
#
ifeq ($(N_COMPILER),__GNUC__)
#  N_OPTIMIZEFLAGS = -O3 -m486 -ffast-math
  N_OPTIMIZEFLAGS = -O3 -mpentium -ffast-math -fomit-frame-pointer
endif 
ifeq ($(N_COMPILER),__VC__)
  N_OPTIMIZEFLAGS = /G6 /Ob2 /Og /Oi /Os /Oy /MD
endif  

#---------------------------------------------------------------------
# N_ASSEMBLY
# Turns on/off additional assembly output.
#---------------------------------------------------------------------
#
N_ASSEMBLY = false

#---------------------------------------------------------------------
# EOF
#---------------------------------------------------------------------
