#---------------------------------------------------------------------
# code/make.mak
# =============
# Build rules for building one of the module types.
# Don't edit unless you know what you're doing.
# (C) 1999 A.Weissflog
#---------------------------------------------------------------------

# get TAR(GET) name depending on module type
ifeq ($(MOD_TYPE),class)
  CFLAGS += $(SYM_OPT)N_STATIC $(SYM_OPT)N_INIT=n_init_$(MODULE) $(SYM_OPT)N_FINI=n_fini_$(MODULE) $(SYM_OPT)N_NEW=n_new_$(MODULE) $(SYM_OPT)N_VERSION=n_version_$(MODULE) $(SYM_OPT)N_INITCMDS=n_initcmds_$(MODULE)
  T = $(LIB_PRE)$(MODULE)$(LIB_POST)
  TAR = $(N_TARGETDIR)$(T)
endif

ifeq ($(MOD_TYPE),dll)
  T = $(DLL_PRE)$(MODULE)$(DLL_POST)
  TAR = $(N_TARGETDIR)$(T)
endif

ifeq ($(MOD_TYPE),exe)
  T = $(MODULE)$(EXE)
  TAR = $(N_TARGETDIR)$(T)
endif

ifeq ($(MOD_TYPE),local_exe)
  T = $(MODULE)$(EXE)
  TAR = $(T)
endif

ifeq ($(MOD_TYPE),pack)
  T = $(DLL_PRE)$(MODULE)$(DLL_POST)
  TAR = $(N_TARGETDIR)$(T)
endif

SRCS = $(addsuffix .cc,$(FROM))
OBJS = $(addsuffix $(OBJ),$(FROM))

NEB_LIBS.0 = $(addprefix $(LIB_OPT),$(NEB_LIBS))
NEB_LIBS.1 = $(addsuffix $(LIB_POST),$(NEB_LIBS.0))
SYS_LIBS.0 = $(addprefix $(LIB_OPT),$(SYS_LIBS))
SYS_LIBS.1 = $(addsuffix $(LIB_POST),$(SYS_LIBS.0))

LIBS += $(NEB_LIBS.1)
LIBS += $(SYS_LIBS.1)

VERS_TARGET = $(VERSION)$(OBJ)

#---------------------------------------------------------------------
ifeq ($(N_COMPILER),__VC__)

  ifeq ($(MOD_TYPE),dll)
$(TAR) : $(OBJS) $(RESOURCE)
	$(CC) /Tp$(VERSION).cc $(NOLINK_OPT) $(CFLAGS)
	$(LD) $(VERS_TARGET) $^ /OUT:$@ $(LFLAGS) $(LIBDIR) $(LIBS) /DLL
  endif

  ifeq ($(MOD_TYPE),exe)
$(TAR) : $(OBJS) $(RESOURCE)
	$(LD) $^ /OUT:$@ $(LFLAGS) $(LIBDIR) $(LIBS)
  endif

  ifeq ($(MOD_TYPE),local_exe)
$(TAR) : $(OBJS) $(RESOURCE)
	$(LD) $^ /OUT:$@ $(LFLAGS) $(LIBDIR) $(LIBS)
  endif

  ifeq ($(MOD_TYPE),class)
$(TAR) : $(OBJS) $(RESOURCE)
	$(CC) /Tp$(VERSION).cc $(NOLINK_OPT) $(CFLAGS)
	$(AR) $(VERS_TARGET) $^ /OUT:$@
  endif

  ifeq ($(MOD_TYPE),pack)
$(TAR) : $(OBJS) $(RESOURCE)
	$(LD) $^ /OUT:$@ $(LFLAGS) $(LIBDIR) $(LIBS) /DLL
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" cp $(MODULE).toc $(N_TARGETDIR)
  endif

#--- END WIN32 ---
else
#--- START LINUX ---

  ifeq ($(MOD_TYPE),dll)
$(TAR) : $(OBJS)
	$(CC) $(VERSION).cc $(NOLINK_OPT) $(CFLAGS)
	$(CC) $(VERS_TARGET) $^ -o $@ -shared $(CFLAGS) $(LIBDIR) $(LIBS)
  endif

  ifeq ($(MOD_TYPE),exe)
$(TAR): $(OBJS)
	$(CC) $^ -o $@ $(LIBDIR) $(LIBS)
	cp $@ $(N_TARGETDIR)$(MODULE)
  endif

  ifeq ($(MOD_TYPE),local_exe)
$(MODULE): $(OBJS)
	$(CC) $^ -o $@ $(LIBDIR) $(LIBS)
  endif

  ifeq ($(MOD_TYPE),class)
$(TAR) : $(OBJS)
	$(CC) $(VERSION).cc $(NOLINK_OPT) $(CFLAGS)
	$(AR) -rvcs $@.a $(VERS_TARGET) $^
  endif

  ifeq ($(MOD_TYPE),pack)
$(TAR) : $(OBJS)
	$(CC) $^ -o $@ -shared $(CFLAGS) $(LIBDIR) $(LIBS)
	cp $(MODULE).toc $(N_TARGETDIR)
  endif
endif
#---------------------------------------------------------------------
