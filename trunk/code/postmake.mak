#--------------------------------------------------------------------
# postmake.mak
# ============
# General targets for all makefiles.
# 13-Dec-99  floh  new target: 'noassert'
#--------------------------------------------------------------------
N_CLEANOBJS = "*/*$(OBJ)" 

ifeq ($(N_PLATFORM),__WIN32__)
  N_CLEANLIBS = "$(N_TARGETDIR)*.lib"
endif
ifeq ($(N_PLATFORM),__LINUX__)
  N_CLEANLIBS = "$(N_TARGETDIR)*.a"
endif
ifeq ($(N_PLATFORM),__MACOSX__)
  N_CLEANLIBS = "$(N_TARGETDIR)*.a"
endif

#--------------------------------------------------------------------
clean:
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" rm $(N_CLEANOBJS)
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" rm $(N_CLEANLIBS)
#--------------------------------------------------------------------
killdotfiles:
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" rm .debug
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" rm .release
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" rm .profile
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" rm .noassert
#--------------------------------------------------------------------
release: killdotfiles clean
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" echo .release x
#--------------------------------------------------------------------
debug: killdotfiles clean
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" echo .debug x
#--------------------------------------------------------------------
noassert: release
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" echo .noassert x
#--------------------------------------------------------------------
profile:
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" rm .profile
	-$(TCL) "$(NOMADS_HOME)/bin/fileutil.tcl" echo .profile x
#--------------------------------------------------------------------
