#--------------------------------------------------------------------
#   makemake.tcl
#   ============
#   Tcl procs to convert Nebula package definitions into Makefiles
#   and Visual Studio workspace files.
#
#   Nebula package files define 'modules' and 'targets'. A module
#   is a collection of source files that belong together.
#   A target is a collection of modules, and specifies how the
#   modules should be compiled into a binary. This can be a
#   generic dll, a class package dll, or an executable.
#
#   (C) 2000 A.Weissflog
#
#   17-May-00   floh    added support for header files
#   07-Nov-00   floh    new target type 'mll' for maya plugins
#   01-Feb-01   floh    support for setdoxycfg keyword
#   14-Feb-01   leaf    added code to generate VS.NET build files,
#                       contributed by Stefan Kirsch.
#	18-Feb-02	leaf	now loads .pak files automatically
#--------------------------------------------------------------------

global includePath

source "$nomads_home/bin/makedepend.tcl"
source "$nomads_home/bin/nsh.tcl"

#--------------------------------------------------------------------
#   structs
#   =======
#   mod(i,name)         - name of module
#   mod(i,dir)          - directory where source files reside
#   mod(i,files)        - list of source files for module 'name'
#   mod(i,headers)      - list of header files for module 'name'
#   mod(i,version)      - version file for module 'name', can be empty
#   mod(i,objs)         - object file names for win32 (generated)
#   mod(i,srcs)         - source file names for win32 (generated)
#   mod(i,hdrs)         - header file names
#   mod(i,deps,src)     - dependencies for 'src' (generated)
#   mod(i,type)         - "main" -> this module contains the main function
#
#   tar(i,name)         - name of target
#   tar(i,platform)     - one of 'all|win32|unix|macosx'
#   tar(i,type)         - one of 'dll|package|exe|mll|lib'
#   tar(i,mods)         - module list for target 'name'
#   tar(i,depends)      - list of other targets, which this target depends on
#   tar(i,libs_win32)   - win32 specific link libs
#   tar(i,libs_unix)    - unix specific link libs
#   tar(i,libs_macosx)  - macosx specific link libs
#   tar(i,frameworks_macosx)  - macosx specific frameworks
#   tar(i,rsrc_win32)   - win32 specific resource files
#   tar(i,copyright)    - target copyright string
#   tar(i,doxycfg)      - location of Doxygen config file
#
#   'plat' may be one of 'win32', 'unix', 'macosx', 'all'
#   'tartype' may be one of 'release', 'debug', 'profile', 'noassert'
#--------------------------------------------------------------------

global mod
global tar
global num_mods
global num_tars
global workspaces
global ws_name

set num_mods 0
set num_tars 0
set ws_name nebula

set platform [get_platform]

#--------------------------------------------------------------------
#   procs
#   =====
#   beginmodule $mod_name
#       setdir $subdir
#       setfiles   $file $file $file...
#       setheaders $file $file $file...
#       setver $file
#   endmodule
#   begintarget $name
#       setmods $module $module $module...
#       setdepend $target $target...
#       setlibs_win32 $lib $lib...
#       setlibs_unix  $lib $lib...
#       setlibs_macosx $lib $lib...
#       setframeworks_macosx $lib $lib...
#   endtarget
#--------------------------------------------------------------------

#====================================================================
#   UTILITY PROCEDURES
#====================================================================

#--------------------------------------------------------------------
#   cvs
#   Minimal tcl shell around cvs.
#--------------------------------------------------------------------
proc cvs { cmd dir pattern } {
    global platform
    
    set err "ok"
    set cwd [pwd]
    cd $dir
    set files [glob -nocomplain $pattern]

    if {$files != ""} {
        set sh "sh"
        set sh_arg "-c"
        if {($platform=="win2k") || ($platform=="winnt")} {
            # replace slashes by backslash in 'dir'
            set dir [string map {/ \\} $dir]
            set sh "cmd.exe"
            set sh_arg "/c"
            set sh_delim "&&"
        } elseif {$platform=="win9x"} {
            set dir [string map {/ \\} $dir]
            set sh "command.com"
            set sh_arg "/c"
            set sh_delim "&&"
        } elseif {($platform=="unix") || ($platform=="macosx")} {
            set sh "sh"
            set sh_arg "-c"
            set sh_delim ";"
        }
        after 500
        switch $cmd {
            addascii { catch { exec $sh $sh_arg "cd $dir $sh_delim cvs add $files"  } err }
            commit   { catch { exec $sh $sh_arg "cd $dir $sh_delim cvs commit -m bla $files" } err }
            edit     { catch { exec $sh $sh_arg "cd $dir $sh_delim cvs edit $files" } err }
        }
        puts "-> cvs $cmd $files = $err"
    } else {
        puts "-> ignoring cvs on empty list"
    }
    cd $cwd
}

#--------------------------------------------------------------------
#   make_cvs_edit
#   Puts all affected files into 'cvs edit' state.
#   11-Mar-00   floh    created
#   20-Mar-00   floh    no longer affect .toc files
#--------------------------------------------------------------------
proc make_cvs_edit {path} {
    global nomads_home
    set cwd [pwd]
    cvs edit $path/src/packages *.cc
    cvs edit $path/src          *.mak
    cvs edit $path/vstudio      *.dsp
    cvs edit $path/vstudio      *.dsw
    cd $cwd
}

#--------------------------------------------------------------------
#   make_cvs_add
#   Add any new files.
#   11-Mar-00   floh    created
#   20-Mar-00   floh    + no longer affect .toc files
#                       + no longer commit changes, just add
#--------------------------------------------------------------------
proc make_cvs_add {path} {
    global nomads_home
    set cwd [pwd]
    cvs addascii $path/src/packages *.cc
    cvs addascii $path/src          *.mak
    cvs addascii $path/vstudio      *.dsp
    cvs addascii $path/vstudio      *.dsw
    cd $cwd
}

#--------------------------------------------------------------------
#   addtolist $var $list
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc addtolist {var list} {
    upvar $var v
    for {set i 0} {$i < [llength $list]} {incr i} {
        append v [lindex $list $i] " "
    }
}

#--------------------------------------------------------------------
#   findmodbyname $name
#   Find module index by name.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc findmodbyname {name} {
    global mod
    global num_mods
    for {set i 0} {$i < $num_mods} {incr i} {
        if {$name == $mod($i,name)} {
            return $i
        }
    }
    puts "ERROR: module '$name' not defined!"
    exit
}

#--------------------------------------------------------------------
#   findtargetbyname $name
#   Find target index by name.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc findtargetbyname {name} {
    global tar
    global num_tars
    for {set i 0} {$i < $num_tars} {incr i} {
        if {$name == $tar($i,name)} {
            return $i
        }
    }
    puts "ERROR: target '$name' not defined!"
    exit
}

#--------------------------------------------------------------------
#   gen_filelists $module
#   Generate source and object file name lists for a given module.
#   04-Mar-00   floh    created
#   11-Mar-00   floh    + generate dependencies for files
#   20-Mar-00   floh    + global variable 'global_gendeps' used to
#                         turn dependency-generation on/off
#   17-May-00   floh    + support for header files
#--------------------------------------------------------------------
proc gen_filelists {module} {
    global mod
    global num_mods
    global global_gendeps
    global includePath 

    set i [findmodbyname $module]

    set num_files [llength $mod($i,files)]

    # generate object file lists
    set mod($i,objs) ""
    for {set j 0} {$j < $num_files} {incr j} {
        lappend mod($i,objs) "$mod($i,dir)/[lindex $mod($i,files) $j]\$(OBJ)"
    }

    # generate source file lists and dependencies
    set mod($i,srcs) ""   
    for {set j 0} {$j < $num_files} {incr j} {
        set cur_src "$mod($i,dir)/[lindex $mod($i,files) $j].cc"
        lappend mod($i,srcs) $cur_src
        if {$global_gendeps} {
            puts "-> dependencies for $cur_src"
            set tmp_dep_list [get_depends $cur_src $includePath ""]
            set dep_list ""
            for {set k 0} {$k < [llength $tmp_dep_list]} {incr k} {
                append dep_list "../inc/[lindex $tmp_dep_list $k]" " "
            }
            set mod($i,deps,$cur_src) $dep_list
        }
    }

    # append header files to 'hdrs'
    set mod($i,hdrs) ""
    set num_headers [llength $mod($i,headers)]
    for {set j 0} {$j < $num_headers} {incr j} {
        set cur_hdr "$mod($i,dir)/[lindex $mod($i,headers) $j].h"
        lappend mod($i,hdrs) $cur_hdr
    }
}

#====================================================================
#   .PAK FILE PARSING PROCEDURES
#====================================================================

#--------------------------------------------------------------------
#	workspace $name
#	Sets workspace for target(s)
#	18-Feb-02	leaf	created
#--------------------------------------------------------------------
proc workspace {name} {
	global ws_name
	set ws_name $name
}

#--------------------------------------------------------------------
#   beginmodule $name
#   Start definition of a module.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc beginmodule {name} {
    global num_mods
    global mod
    set mod($num_mods,name)         $name
    set mod($num_mods,dir)          ""
    set mod($num_mods,files)        ""
    set mod($num_mods,headers)      ""
    set mod($num_mods,version)      ""
    set mod($num_mods,type)         ""
}

#--------------------------------------------------------------------
#   setdir $subdir
#   Specify path where module files live.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setdir {subdir} {
    global mod
    global num_mods
    set mod($num_mods,dir) $subdir
}

#--------------------------------------------------------------------
#   setfiles $files
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setfiles {file_list} {
    global mod
    global num_mods
    addtolist mod($num_mods,files) $file_list
}

#--------------------------------------------------------------------
#   setmodtype type
#   24-Jul-02   floh    created
#--------------------------------------------------------------------
proc setmodtype {type} {
    global mod
    global num_mods
    set mod($num_mods,type) $type
}    

#--------------------------------------------------------------------
#   setheaders $files
#   17-May-00   floh    created
#--------------------------------------------------------------------
proc setheaders {file_list} {
    global mod
    global num_mods
    addtolist mod($num_mods,headers) $file_list
}

#--------------------------------------------------------------------
#   setversion $version_file
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setversion {version_file} {
    global mod
    global num_mods
    set mod($num_mods,version) $version_file
}

#--------------------------------------------------------------------
#   endmodule
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc endmodule { } {
    global mod
    global num_mods

    # generate file lists for module
    set name $mod($num_mods,name)
    incr num_mods
    gen_filelists $name
}

#--------------------------------------------------------------------
#   dumpmodules
#   Print data of all defined modules.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc dumpmodules { } {
    global mod
    global num_mods
    puts "\n\n======================================================="
    puts " MODULES"
    puts " "
    for {set i 0} {$i < $num_mods} {incr i} {
        puts "-------------------------------------------------------"
        puts " module:      $mod($i,name)"
        puts " dir:         $mod($i,dir)"
        puts " files:       $mod($i,files)"
        puts " headers:     $mod($i,headers)"
        puts " version: $mod($i,version)"
    }
}

#--------------------------------------------------------------------
#   begintarget $name
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc begintarget {name} {
    global num_tars
    global tar
    global workspaces
    global ws_name
    
    set tar($num_tars,name)         $name
    set tar($num_tars,platform)     "all"
    set tar($num_tars,type)         ""
    set tar($num_tars,mods)         ""
    set tar($num_tars,depends)      ""
    set tar($num_tars,libs_win32)   ""
    set tar($num_tars,libs_unix)    ""
    set tar($num_tars,libs_macosx)  ""
    set tar($num_tars,frameworks_macosx)  ""
    set tar($num_tars,rsrc_win32)   ""
    set tar($num_tars,copyright)    "(C) 2000 A.Weissflog"
    set tar($num_tars,doxycfg)      ""
    set tar($num_tars,doxytitle)    ""
    
    # add target to workspace
    lappend workspaces($ws_name) $name
}

#--------------------------------------------------------------------
#   setplatform $platform
#   09-Mar-00   floh    created
#--------------------------------------------------------------------
proc setplatform {platform} {
    global num_tars
    global tar
    set tar($num_tars,platform) $platform
}

#--------------------------------------------------------------------
#   settype $type
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc settype {type} {
    global num_tars
    global tar
    set tar($num_tars,type) $type
}

#--------------------------------------------------------------------
#   setmods $modules
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setmods {mod_list} {
    global num_tars
    global tar
    set tar($num_tars,mods) ""
    for {set i 0} {$i < [llength $mod_list]} {incr i} {
        lappend tar($num_tars,mods) [lindex $mod_list $i] 
    }
}

#--------------------------------------------------------------------
#   setdepends $depends
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setdepends {depends} {
    global num_tars
    global tar
    global workspaces
    global ws_name
    set tar($num_tars,depends) $depends
    
    # add each depend target to workspace
    foreach dt $depends {
    	lappend workspaces($ws_name) $dt
    }
}

#--------------------------------------------------------------------
#   setdoxycfg $filename
#   01-Feb-01   floh    created
#--------------------------------------------------------------------
proc setdoxycfg {filename} {
    global num_tars
    global tar
    set tar($num_tars,doxycfg)   $filename
}

#--------------------------------------------------------------------
#   setlibs_win32 $libs
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setlibs_win32 {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_win32) $libs
}

#--------------------------------------------------------------------
#   setlibs_unix $libs
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setlibs_unix {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_unix) $libs
}

#--------------------------------------------------------------------
#   setlibs_macosx $libs
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setlibs_macosx {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_macosx) $libs
}

#--------------------------------------------------------------------
#   setframeworks_macosx $libs
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setframeworks_macosx {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,frameworks_macosx) $libs
}

#--------------------------------------------------------------------
#   setrsrc_win32 $libs
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setrsrc_win32 {file_list} {
    global num_tars
    global tar
    addtolist tar($num_tars,rsrc_win32) $file_list
}

#--------------------------------------------------------------------
#   endtarget
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc endtarget { } {
    global num_tars
    incr num_tars
}

#--------------------------------------------------------------------
#   dumptargets
#   Print data of all defined targets.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc dumptargets { } {
    global tar
    global num_tars
    puts "\n\n======================================================="
    puts " TARGETS"
    puts " "
    for {set i 0} {$i < $num_tars} {incr i} {
        puts "-------------------------------------------------------"
        puts " target:      $tar($i,name)"
        puts " type:        $tar($i,type)"
        puts " mods:        $tar($i,mods)"
        puts " depends:     $tar($i,depends)"
        puts " libs_win32:  $tar($i,libs_win32)"
        puts " libs_unix:   $tar($i,libs_unix)"
        puts " libs_macosx: $tar($i,libs_macosx)"
        puts " frameworks_macosx: $tar($i,frameworks_macosx)"
        puts " rsrc_win32:  $tar($i,rsrc_win32)"
        puts " copyright:   $tar($i,copyright)"
    }
}

#====================================================================
#   PACKAGE HELPER FUNCTIONS
#====================================================================

#--------------------------------------------------------------------
#   gentocfile
#   Generate TOC files for a package target. 
#--------------------------------------------------------------------
proc gentocfile {tarname outdir} {
    global tar
    global nomads_home
    set i [findtargetbyname $tarname]

    puts "-> generating $nomads_home/bin/$outdir/$tarname.toc"
    set cid [open $nomads_home/bin/$outdir/$tarname.toc w]
    puts $cid "#--------------------------------------------------------------------"
    puts $cid "# /bin/$outdir/$tarname.toc"
    puts $cid "# MACHINE GENERATED, DON'T EDIT!"
    puts $cid "# [clock format [clock seconds]]"
    puts $cid "#--------------------------------------------------------------------"
    puts $cid "\$$tarname"
    for {set j 0} {$j < [llength $tar($i,mods)]} {incr j} {
        puts $cid "%[lindex $tar($i,mods) $j]"
    }
    close $cid
}


#--------------------------------------------------------------------
#   genpacksrc
#   Generate package.cc file for a package target.
#   06-Mar-00   floh    created
#   24-Jul-02   floh    + added support for module types
#--------------------------------------------------------------------
proc genpacksrc {tarname} {
    global tar
    global mod

    set i [findtargetbyname $tarname]

    puts "-> generating packages/pkg_$tarname.cc"
    set cid [open "packages/pkg_$tarname.cc" w]
    puts $cid "//----------------------------------------------------------"
    puts $cid "// pkg_$tarname.cc"
    puts $cid "// MACHINE GENERATED, DON'T EDIT!"
    puts $cid "//----------------------------------------------------------"
    puts $cid {#include "kernel/ntypes.h"}
    puts $cid {#include "kernel/nkernelserver.h"}
    for {set j 0} {$j < [llength $tar($i,mods)]} {incr j} {
        set curMod [lindex $tar($i,mods) $j]
        set modIndex [findmodbyname $curMod]

        # don't create a package entry for lib modules
        if {!(($mod($modIndex,type) == "cpplib") || ($mod($modIndex,type) == "clib"))} {    
            set initproc "n_init_$curMod"
            set finiproc "n_fini_$curMod"
            set newproc  "n_new_$curMod"
            set versproc "n_version_$curMod"
            puts $cid "extern \"C\" bool $initproc (nClass *, nKernelServer *);"
            puts $cid "extern \"C\" void $finiproc (void);"
            puts $cid "extern \"C\" void *$newproc (void);"
            puts $cid "extern \"C\" char *$versproc (void);"
        }
    }
    puts $cid "#ifndef __MACOSX__"
    puts $cid "extern \"C\" void N_EXPORT n_addmodules(nKernelServer *);"
    puts $cid "extern \"C\" void N_EXPORT n_remmodules(nKernelServer *);"
    puts $cid "void N_EXPORT n_addmodules(nKernelServer *ks)"
    puts $cid "#else"
    puts $cid "extern \"C\" void N_EXPORT ${tarname}_addmodules(nKernelServer *);"
    puts $cid "extern \"C\" void N_EXPORT ${tarname}_remmodules(nKernelServer *);"
    puts $cid "void N_EXPORT ${tarname}_addmodules(nKernelServer *ks)"
    puts $cid "#endif"
    puts $cid "\{"
    for {set j 0} {$j < [llength $tar($i,mods)]} {incr j} {
        set curMod [lindex $tar($i,mods) $j]
        set modIndex [findmodbyname $curMod]
        
        # don't create a package entry for modules containing the main function
        if {!(($mod($modIndex,type) == "cpplib") || ($mod($modIndex,type) == "clib"))} {    
            set initproc "n_init_$curMod"
            set finiproc "n_fini_$curMod"
            set newproc  "n_new_$curMod"
            puts $cid "    ks->AddModule(\"$curMod\",$initproc,$finiproc,$newproc);"
        }
    }
    puts $cid "\}"
    puts $cid "#ifndef __MACOSX__"
    puts $cid "void N_EXPORT n_remmodules(nKernelServer *)"
    puts $cid "#else"
    puts $cid "void N_EXPORT ${tarname}_remmodules(nKernelServer *)"
    puts $cid "#endif"
    puts $cid "\{"
    puts $cid "\}"
    puts $cid "//----------------------------------------------------------"
    puts $cid "// EOF"
    puts $cid "//----------------------------------------------------------"
    close $cid
    puts "-> generating packages/${tarname}_bundle.cc"
    set cid [open "packages/${tarname}_bundle.cc" w]
    puts $cid "//----------------------------------------------------------"
    puts $cid "// ${tarname}_bundle.cc"
    puts $cid "// MACHINE GENERATED, DON'T EDIT!"
    puts $cid "//----------------------------------------------------------"
    puts $cid "#include \"kernel/nkernelserver.h\""
    puts $cid ""
    puts $cid "extern \"C\" void N_EXPORT ${tarname}_addmodules(nKernelServer *);"
    puts $cid "extern \"C\" void N_EXPORT ${tarname}_remmodules(nKernelServer *);"
    puts $cid ""
    puts $cid "extern \"C\" void N_EXPORT n_addmodules(nKernelServer *);"
    puts $cid "extern \"C\" void N_EXPORT n_remmodules(nKernelServer *);"
    puts $cid ""
    puts $cid "void N_EXPORT n_addmodules(nKernelServer *ks)"
    puts $cid "\{"
    puts $cid "    ${tarname}_addmodules(ks);"
    puts $cid "\}"
    puts $cid ""
    puts $cid "void N_EXPORT n_remmodules(nKernelServer *ks)"
    puts $cid "\{"
    puts $cid "    ${tarname}_remmodules(ks);"
    puts $cid "\}"
    puts $cid "//----------------------------------------------------------"
    puts $cid "// EOF"
    puts $cid "//----------------------------------------------------------"
    close $cid
}

#====================================================================
#   Include generator functions for GNU Make and Visual Studio
#   Workspace files.
#====================================================================
source $nomads_home/bin/makeunix.tcl
source $nomads_home/bin/makesln2005.tcl
source $nomads_home/bin/makedsw.tcl

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------



