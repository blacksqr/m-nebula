#--------------------------------------------------------------------
#   makeunix.tcl
#   ============
#   Functions to generate GNU Make files. Is included in makemake.tcl.
#   (C) 2000 A.Weissflog
#
#   01-Feb-2001     floh    added Doxygen support
#--------------------------------------------------------------------

global src_list 

#--------------------------------------------------------------------
#   gen_module_make
#   Write the statements to compile the files that define a module
#   into the makefile.
#   04-Mar-00   floh    created
#   11-Mar-00   floh    + generate depend list for each source file
#   12-Mar-00   floh    + ignore redundant sources across multiple
#                         modules
#--------------------------------------------------------------------
proc gen_module_make {name type cid} {
    global mod
    global num_mods
    global src_list
    
    # get array index of module
    set i [findmodbyname $name]

    # write source file statements...
    for {set j 0} {$j < [llength $mod($i,objs)]} {incr j} {
        set cur_obj [lindex $mod($i,objs) $j]
        set cur_src [lindex $mod($i,srcs) $j]
        # ignore redundant sources
        if {[lsearch $src_list $cur_src] == -1} {
            lappend src_list $cur_src
            puts $cid "$cur_obj : $cur_src $mod($i,deps,$cur_src)" 
            set more_syms ""
            if {$type == "package"} {
                set more_syms "\$(SYM_OPT)N_INIT=n_init_$name \$(SYM_OPT)N_FINI=n_fini_$name \$(SYM_OPT)N_NEW=n_new_$name \$(SYM_OPT)N_VERSION=n_version_$name \$(SYM_OPT)N_INITCMDS=n_initcmds_$name"
            }
            puts $cid "\t\$(CC) \$(CC_OPT)\$< \$(OBJ_OPT)\$@ \$(NOLINK_OPT) \$(CFLAGS) $more_syms"
        }
    }

    # write version file statement
    if {$mod($i,version) != ""} {
        # create fake .obj name to force compilation! 
        set cur_file "$mod($i,dir)/$mod($i,version)"
        set cur_obj "$cur_file\$(OBJ)"
        set cur_src "$cur_file.cc"
        puts $cid "$cur_obj : $cur_src" 
        puts $cid "\t\$(CC) \$(CC_OPT)\$< \$(OBJ_OPT)\$@ \$(NOLINK_OPT) \$(CFLAGS)"
    }
}

#--------------------------------------------------------------------
#   gen_doxydoc
#   Write statements for compiling Doxygen documentation into
#   make file.
#   01-Feb-2001     floh    created
#   13-Jan-2002     leaf    changed to use gendox.tcl
#--------------------------------------------------------------------
proc gen_doxydoc {cid tar_index} {
    global tar
    
    # was the setdoxycfg keyword present in the target definition?
    if {$tar($tar_index,doxycfg) != ""} {
        
        # for each entry in the doxycfg list...
        for {set i 0} {$i < [llength $tar($tar_index,doxycfg)]} {incr i} {
            set curcfg [lindex $tar($tar_index,doxycfg) $i]
            puts $cid "\t-\$(TCL) \"\$(NOMADS_HOME)/bin/gendox.tcl\" $curcfg"
        }
    }
}

#--------------------------------------------------------------------
#   gen_target_make
#   Generate *.mak file for given target.
#
#   TODO:
#   - handle platform specific targets (ndxinputserver...)
#   - handle rsrc files
#   - cvs handling
#   12-Mar-00   floh    added target dependencies to Make dependency
#                       list
#   20-Mar-00   floh    + generate package toc file in build process
#   07-Nov-00   floh    + mll target type
#--------------------------------------------------------------------
proc gen_target_make {name cid} {
    global mod
    global num_mods
    global tar
    global num_tars

    set i [findtargetbyname $name]

    # handle platform specific targets
    if {$tar($i,platform) != "all"} {
        if {$tar($i,platform) == "win32"} {
            puts $cid "ifeq (\$(N_PLATFORM),__WIN32__)"
        } elseif {$tar($i,platform) == "unix"} {
            puts $cid "ifeq (\$(N_PLATFORM),__LINUX__)"
        } elseif {$tar($i,platform) == "macosx"} {
            puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        }
    }

    # write modules
    for {set j 0} {$j < [llength $tar($i,mods)]} {incr j} {
        gen_module_make [lindex $tar($i,mods) $j] $tar($i,type) $cid
    }

    # collect all source files that make up the target
    set files ""
    for {set j 0} {$j < [llength $tar($i,mods)]} {incr j} {
        set m [findmodbyname [lindex $tar($i,mods) $j]]
        addtolist files $mod($m,objs)
        if {($tar($i,type) != "exe") && ($tar($i,type) != "mll")} {
            if {$mod($m,version) != ""} {
                addtolist files "$mod($m,dir)/$mod($m,version)\$(OBJ)"
            }
        }
        if {$tar($i,type) == "package"} {
            addtolist files "packages/pkg_$name\$(OBJ)"
        }
        }

    # handle resource files
    set rsrc_files ""
    if {[llength $tar($i,rsrc_win32)] > 0} {
	    puts $cid "ifeq (\$(N_PLATFORM),__WIN32__)"
	    for {set j 0} {$j < [llength $tar($i,rsrc_win32)]} {incr j} {
	        set curRsrc [lindex $tar($i,rsrc_win32) $j]
	        set rsrcSrc ""
	        set rsrcDst ""
	        append rsrcSrc "$curRsrc" ".rc"
	        append rsrcDst "$curRsrc" ".res"
	        puts $cid "$rsrcDst : $rsrcSrc"
	        addtolist rsrc_files $rsrcDst
	    }
	    puts $cid "endif"
	}

    # generate list of link libs for that target
    set libs_win32 ""
    for {set j 0} {$j < [llength $tar($i,libs_win32)]} {incr j} {
        addtolist libs_win32 "\$(LIB_OPT)[lindex $tar($i,libs_win32) $j]"
    }
    set libs_unix ""
    for {set j 0} {$j < [llength $tar($i,libs_unix)]} {incr j} {
        addtolist libs_unix "\$(LIB_OPT)[lindex $tar($i,libs_unix) $j]"
    }
    set libs_macosx ""
    for {set j 0} {$j < [llength $tar($i,libs_macosx)]} {incr j} {
        addtolist libs_macosx "\$(LIB_OPT)[lindex $tar($i,libs_macosx) $j]"
    }
    set frameworks_macosx ""
    for {set j 0} {$j < [llength $tar($i,frameworks_macosx)]} {incr j} {
        addtolist frameworks_macosx "-framework [lindex $tar($i,frameworks_macosx) $j]"
    }

    # generate list of target dependencies for that target
    set dep_win32  ""
    set dep_unix   ""
    set dep_macosx ""
    for {set j 0} {$j < [llength $tar($i,depends)]} {incr j} {
        set dtar [lindex $tar($i,depends) $j]
        set d [findtargetbyname $dtar]
        if {$tar($d,platform)=="all"} {
            addtolist dep_win32  $dtar
            addtolist dep_unix   $dtar
            addtolist dep_macosx $dtar
        } elseif {$tar($d,platform)=="win32"} {
            addtolist dep_win32 $dtar
        } elseif {$tar($d,platform)=="unix"} {
            addtolist dep_unix $dtar
        } elseif {$tar($d,platform)=="macosx"} {
            addtolist dep_macosx $dtar
        }
    }

    # package specific stuff
    if {$tar($i,type) == "package"} {
        # write compile statement for package.cc file into Makefile
        puts $cid "packages/pkg_$name\$(OBJ) : packages/pkg_$name.cc" 
        puts $cid "\t\$(CC) \$(CC_OPT)\$< \$(OBJ_OPT)\$@ \$(NOLINK_OPT) \$(CFLAGS)"
        puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        puts $cid "packages/${name}_bundle\$(OBJ) : packages/${name}_bundle.cc" 
        puts $cid "\t\$(CC) \$(CC_OPT)\$< \$(OBJ_OPT)\$@ \$(NOLINK_OPT) \$(CFLAGS)"
        puts $cid "endif"
    }

    # compile target itself
    if {$tar($i,type) == "dll"} {
        # FIXME: Libs!
        set t "\$(DLL_PRE)$name\$(DLL_POST)"
        set tarpath "\$(N_TARGETDIR)$t"
        puts $cid "ifeq (\$(N_COMPILER),__VC__)"
        puts $cid "$name : $dep_win32 $tarpath"
        puts $cid "$tarpath : $files $rsrc_files"
        puts $cid "\t\$(LD) \$^ /OUT:\$@ \$(LFLAGS) \$(LIBDIR) \$(LIBS) $libs_win32 /DLL"
        puts $cid "else"
        puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        set t2 "\$(DLL_PRE)$name.dylib"
        set tarpath2 "\$(N_TARGETDIR)$t2"
        puts $cid "$name : $dep_macosx $tarpath2"
        puts $cid "$tarpath2 : $files"
        puts $cid "\t\$(CC) -dynamiclib -compatibility_version 0.1 -current_version 0.1.0 \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) $libs_macosx $frameworks_macosx "
        puts $cid "else"
        puts $cid "$name : $dep_unix $tarpath"
        puts $cid "$tarpath : $files"
        puts $cid "\t\$(CC) \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) $libs_unix -shared"
        puts $cid "endif"
        puts $cid "endif"
    } elseif {$tar($i,type) == "package"} {
        # FIXME: Libs!
        set t "\$(DLL_PRE)$name\$(DLL_POST)"
        set tarpath "\$(N_TARGETDIR)$t"
        puts $cid "ifeq (\$(N_COMPILER),__VC__)"
        puts $cid "$name : $dep_win32 $tarpath"
        puts $cid "$tarpath : $files $rsrc_files"
        puts $cid "\t\$(LD) \$^ /OUT:\$@ \$(LFLAGS) \$(LIBDIR) \$(LIBS) $libs_win32 /DLL"
        puts $cid "\t-\$(TCL) gentocfile.tcl $tar($i,name) win32 \$(TAR_TYPE)"
        puts $cid "else"
        puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        set t2 "\$(DLL_PRE)$name.dylib"
        set tarpath2 "\$(N_TARGETDIR)$t2"
        puts $cid "$name : $dep_macosx $tarpath2 $tarpath"
        puts $cid "$tarpath2 : $files"
        puts $cid "\t\$(CC) -dynamiclib -compatibility_version 0.1 -current_version 0.1.0 \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) $libs_macosx $frameworks_macosx"
        puts $cid "$tarpath : packages/${name}_bundle\$(OBJ)"
        puts $cid "\t\$(CC) \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) \$(LIB_OPT)$name $libs_macosx $frameworks_macosx -bundle -flat_namespace"
        puts $cid "\t-\$(TCL) gentocfile.tcl $tar($i,name) macosx \$(TAR_TYPE)"
        puts $cid "else"
        puts $cid "$name : $dep_unix $tarpath"
        puts $cid "$tarpath : $files"
        puts $cid "\t\$(CC) \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) $libs_unix -shared"
        puts $cid "\t-\$(TCL) gentocfile.tcl $tar($i,name) linux \$(TAR_TYPE)"
        puts $cid "endif"
        puts $cid "endif"
    } elseif {$tar($i,type) == "exe"} {
        set t "$name\$(EXE)"
        set tarpath "\$(N_TARGETDIR)$t"
        puts $cid "ifeq (\$(N_COMPILER),__VC__)"
        puts $cid "$name : $dep_win32 $tarpath"
        puts $cid "$tarpath : $files $rsrc_files"
        puts $cid "\t\$(LD) \$^ /OUT:\$@ \$(LFLAGS) \$(LIBDIR) \$(LIBS) $libs_win32"
        puts $cid "else"
        puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        puts $cid "$name : $dep_macosx $tarpath"
        puts $cid "$tarpath : $files"
        puts $cid "\t\$(CC) \$^ -o \$@ \$(LIBDIR) \$(LIBS) $libs_macosx -flat_namespace"
        puts $cid "else"
        puts $cid "$name : $dep_unix $tarpath"
        puts $cid "$tarpath : $files"
        puts $cid "\t\$(CC) \$^ -o \$@ \$(LIBDIR) \$(LIBS) $libs_unix"
        puts $cid "endif"
        puts $cid "\tcp \$@ \$(N_TARGETDIR)$name"
        puts $cid "endif"
    } elseif {$tar($i,type) == "mll"} {
        set t "\$(DLL_PRE)$name.mll"
        set tarpath "\$(N_TARGETDIR)$t"
        puts $cid "ifeq (\$(N_COMPILER),__VC__)"
        puts $cid "$name : $dep_win32 $tarpath"
        puts $cid "$tarpath : $files $rsrc_files"
        puts $cid "\t\$(LD) \$^ /OUT:\$@ \$(LFLAGS) \$(LIBDIR) \$(LIBS) $libs_win32 /DLL /export:initializePlugin /export:uninitializePlugin"
        puts $cid "endif"
    }

    # handle platform specific targets
    if {$tar($i,platform) != "all"} {
        puts $cid "endif"
    }
}

#--------------------------------------------------------------------
#   gen_makefile
#   Generate a Makefile for a list of targets.
#   12-Mar-00   floh    code to ignore redundant sources across
#                       modules within identical makefile
#   01-Feb-01   floh    added 'doc' target
#--------------------------------------------------------------------
proc gen_makefile {name list_of_targets} {
    global tar
    global num_tars
    global nomads_home
    global src_list

    puts "Generating makefile $name.mak..."

    # clear global list of sources (for redundancy check) 
    set src_list ""

    # for each package target, generate a package.cc file
    # and a platform specific global dependency list
    set tarlist_win32 ""
    set tarlist_unix  ""
    set tarlist_macosx ""
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        set t [findtargetbyname [lindex $list_of_targets $i]]
        if {$tar($t,type) == "package"} {
            genpacksrc $tar($t,name)
        }
        if {$tar($t,platform)=="all"} {
            addtolist tarlist_win32 $tar($t,name)
            addtolist tarlist_unix $tar($t,name)
            addtolist tarlist_macosx $tar($t,name)
        } elseif {$tar($t,platform)=="win32"} {
            addtolist tarlist_win32 $tar($t,name)
        } elseif {$tar($t,platform)=="unix"} {
            addtolist tarlist_unix $tar($t,name)
        } elseif {$tar($t,platform)=="macosx"} {
            addtolist tarlist_macosx $tar($t,name)
        } else {
            puts "ERROR: UNKNOWN PLATFORM $tar($t,platform) IN TARGET $tar($t,name)!"
            exit
        }
    }

    set cid [open $nomads_home/code/src/$name.mak w]

    puts $cid "#--------------------------------------------------------------------"
    puts $cid "#    $name.mak"
    puts $cid "#    MACHINE GENERATED, DON'T EDIT!"
    puts $cid "#--------------------------------------------------------------------"
    puts $cid ""
    puts $cid "ifeq (\$(N_PLATFORM),__WIN32__)"
    puts $cid "$name: $tarlist_win32"
    puts $cid "endif"
    puts $cid "ifeq (\$(N_PLATFORM),__LINUX__)"
    puts $cid "$name: $tarlist_unix"
    puts $cid "endif"
    puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
    puts $cid "$name: $tarlist_macosx"
    puts $cid "endif"
    puts $cid ""
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        gen_target_make [lindex $list_of_targets $i] $cid
    }
    puts $cid ""
    puts $cid "doc:"
    for {set i 0} {$i < $num_tars} {incr i} {
        gen_doxydoc $cid $i
    }
    close $cid

    puts "...done generating $name.mak."
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

