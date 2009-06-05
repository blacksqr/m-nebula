#---------------------------------------------------------------------
#   nomads/code/defines.mak
#   -----------------------
#   Basic Nebula build definitions, please see config.mak
#   for configurating the build process first before making
#   changes to this file.
#
#   20-Aug-99    floh    translated to English
#   05-Jan-01    floh    moved to tclsh84
#   01-Feb-01    floh    added DOCTOOL variable
#
#   (C) 1998 Andre Weissflog.
#---------------------------------------------------------------------

include $(NOMADS_HOME)/code/config.mak

#---------------------------------------------------------------------
ifeq ($(N_COMPILER),__VC__)
# VisualC unter Win32
  RC          = rc
  CC          = cl
  TCL         = tclsh
  AR          = lib
  LD          = link
  DOCTOOL     = doxygen
  IPATH_OPT   = /I
  LPATH_OPT   = /LIBPATH:
  TAR_OPT     = 
  LIB_OPT     =  
  EXE         = .exe
  OBJ         = .obj
  DLL_PRE     = 
  DLL_POST    = .dll
  LIB_PRE     =
  LIB_POST    = .lib
  CC_OPT      = /Tp
  OBJ_OPT     = /Fo
  OUT_OPT     = /Fe
  NOLINK_OPT  = /c
  SYM_OPT     = /D
endif

ifeq ($(N_COMPILER),__GNUC__)
# GCC unter Linux
  ifeq ($(N_PLATFORM),__MACOSX__)
    CC        = c++
  else
    CC        = g++
  endif
  TCL         = tclsh
  AR          = ar
  LD          = ld
  DOCTOOL     = doxygen	
  IPATH_OPT   = -I
  LPATH_OPT   = -L
  TAR_OPT     = -o
  LIB_OPT     = -l
  EXE         = .exe
  OBJ         = .o
  DLL_PRE     = lib
  DLL_POST    = .so
  LIB_PRE     = lib
  LIB_POST    =
  CC_OPT      =
  OBJ_OPT     = -o
  OUT_OPT     = -o
  NOLINK_OPT  = -c
  SYM_OPT     = -D
endif	

#---------------------------------------------------------------------
ifeq ($(N_COMPILER),__VC__)
  INCDIR = $(IPATH_OPT)"$(NOMADS_HOME)/code/inc"
  LIBDIR = $(LPATH_OPT)"$(N_TARGETDIR)" $(LPATH_OPT)"$(NOMADS_HOME)/code/lib/win32_vc_i386"
  NEB_LIBS =
  SYS_LIBS = 
  CFLAGS = $(INCDIR) /vmb /vms /W3 /GF /Gy /YX /FD /nologo /D"WIN32" /D"_WINDOWS" /D"NT_PLUGIN"

#  CFLAGS = $(INCDIR) /vmb /vms /W3 /GF /nologo /D"WIN32" /D"_WINDOWS" /D"NT_PLUGIN"
  LFLAGS =  /NOLOGO

  ifeq ($(N_MEMMANAGER),true)
    CFLAGS += /D"__NEBULA_MEM_MANAGER__"
  endif
  ifeq ($(N_NOTHREADS),true)
    CFLAGS += /D"__NEBULA_NO_THREADS__"
  endif
  ifeq ($(N_ASSEMBLY),true)
    CFLAGS += /FAs
  endif

endif

ifeq ($(N_COMPILER),__GNUC__)
  INCDIR += $(IPATH_OPT)$(NOMADS_HOME)/code/inc
  INCDIR += $(IPATH_OPT)$(N_PYTHON_INCLUDES)
  LIBDIR += $(LPATH_OPT)$(N_TARGETDIR)
  ifneq (X$(N_PYTHON_LIBS),X)
    LIBDIR += $(LPATH_OPT)$(N_PYTHON_LIBS)
  endif
  ifeq ($(N_USE_STLPORT),true)
    INCDIR += $(IPATH_OPT)$(N_STLPORT_HOME)
  endif
  ifeq ($(N_PLATFORM),__LINUX__)
    LIBDIR += $(LPATH_OPT)/usr/X11R6/lib
  endif
  ifeq ($(N_PLATFORM),__MACOSX__)
    OSX_CFLAGS = -fno-common
  endif
  NEB_LIBS = 
  SYS_LIBS = m
  CFLAGS += $(INCDIR) -fPIC -D$(N_PLATFORM) -W -Wall -Wno-multichar -Wno-reorder
  ifeq ($(N_USE_STLPORT),true)
    CFLAGS += $(N_STLPORT_CFLAGS)
  endif
  CFLAGS += $(OSX_CFLAGS)
  ifeq ($(N_MEMMANAGER),true)
    CFLAGS += -D__NEBULA_MEM_MANAGER__
  endif
    
  ifeq ($(N_NOTHREADS),false) 
    CFLAGS += -D_REENTRANT
    LIBS   += -lpthread
  else
    CFLAGS += -D__NEBULA_NO_THREADS__
  endif

endif

TAR_TYPE = $(wildcard .release)
PROF_TYPE = $(wildcard .profile)
NO_ASSERT = $(wildcard .noassert)
ifeq ($(TAR_TYPE),.release)
	TAR_TYPE = release
else
	TAR_TYPE = debug
endif

#---------------------------------------------------------------------
ifeq ($(TAR_TYPE),release)
  CFLAGS += $(N_OPTIMIZEFLAGS)
  ifeq ($(N_COMPILER),__VC__)	
    LFLAGS += /OPT:REF /INCREMENTAL:NO
  endif
else 
  ifeq ($(N_COMPILER),__VC__)
    CFLAGS += /Zi /D_DEBUG /MDd
    LFLAGS += /DEBUG /INCREMENTAL:YES
  else 	
    CFLAGS += -g -D_DEBUG
#    LIBS += -lefence
  endif
endif
ifeq ($(PROF_TYPE),.profile)
  ifeq ($(N_COMPILER),__VC__)
    CFLAGS += $(N_OPTIMIZEFLAGS) /Zi
  endif
endif
ifeq ($(NO_ASSERT),.noassert)
  CFLAGS += $(SYM_OPT)__NEBULA_NO_ASSERT__  
endif

#---------------------------------------------------------------------
%$(OBJ) : %.cc
	$(CC) $(CC_OPT) $< $(OBJ_OPT)$@ $(NOLINK_OPT) $(CFLAGS)

ifeq ($(N_COMPILER),__VC__)
  %.res : %.rc
	$(RC) $<
endif
#---------------------------------------------------------------------
