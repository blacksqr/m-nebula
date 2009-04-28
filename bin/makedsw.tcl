#--------------------------------------------------------------------
#   makedsw.tcl
#   ===========
#   Functions to generate Visual Studio Workspace files.
#   Is included into makemake.tcl.
#
#   22-Jan-01   floh    added header file support to ALL target types
#   04-Jan-2000 floh    changed tclsh82 to tclsh84
#   28-Mar-2000 floh    edited optimizing compiler options
#
#   (C) 2000 A.Weissflog
#--------------------------------------------------------------------

set release_cpp_flags "/nologo /vmb /vms /Gy /GF /MD /W4 /GX- /G6 /Ob2 /Og /Oi /Os /Oy /FD /c /D_NEBULA_DEVICE_ /DWIN32 /DNDEBUG /D__NEBULA_MEM_MANAGER__ /DNT_PLUGIN /I..\\inc /I..\\inc\\stlport /Tp"
set debug_cpp_flags   "/nologo /vmb /vms /MDd /W4 /GX- /Gm /FD /ZI /Od /GZ /c /D_NEBULA_DEVICE_ /DWIN32 /D_DEBUG /D__NEBULA_MEM_MANAGER__ /DNT_PLUGIN /I..\\inc /I..\\inc\\stlport /Tp"
set release_dswlibpath "/libpath:..\\..\\bin\\win32 /libpath:..\\lib\\win32_vc_i386"
set debug_dswlibpath "/libpath:..\\..\\bin\\win32d /libpath:..\\lib\\win32_vc_i386"

#--------------------------------------------------------------------
#   get_dirmod
#   Return list of module indices, sorted by the module's directory.
#   09-Mar-00
#--------------------------------------------------------------------
proc get_dirmod {name} {
    global tar
    global num_tars
    global mod
    global num_mods

    set t [findtargetbyname $name]

    set dirmod_list ""
    for {set i 0} {$i < [llength $tar($t,mods)]} {incr i} {
        set m [findmodbyname [lindex $tar($t,mods) $i]]
        set dm "$mod($m,dir) $m"
        lappend dirmod_list $dm
    }

    # puts "Before sort: $dirmod_list"
    set dirmod_list [lsort -index 0 $dirmod_list]
    # puts "After sort: $dirmod_list"

    set mod_list ""
    for {set i 0} {$i < [llength $dirmod_list]} {incr i} {
        lappend mod_list [lindex [lindex $dirmod_list $i] 1]
    }
    # puts "Result: $mod_list"
    return $mod_list
}
        
#--------------------------------------------------------------------
#   gen_package_dsp
#   Generate dsp file for a package target definition. 
#   07-Mar-00   floh    created
#   20-Mar-00   floh    + added post build step to create
#                         package .toc files
#--------------------------------------------------------------------
proc gen_package_dsp {name} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_cpp_flags
    global debug_cpp_flags
    global release_dswlibpath
	global debug_dswlibpath

    puts "-> gen_package_dsp $name"

    set i [findtargetbyname $name]

    # sort modules by directory
    set smod_list [get_dirmod $name]

    # create list of link libs (win32 libs, dependency libs)
    set libs ""
    for {set j 0} {$j < [llength $tar($i,libs_win32)]} {incr j} {
        addtolist libs "[lindex $tar($i,libs_win32) $j]"
    }

    # write .dsp file
    set cid [open $nomads_home/code/vstudio/$name.dsp w]

    puts $cid "# Microsoft Developer Studio Project File - Name=\"$name\" - Package Owner=<4>"
    puts $cid "# Microsoft Developer Studio Generated Build File, Format Version 6.00"
    puts $cid "# ** NICHT BEARBEITEN **"
    puts $cid ""
    puts $cid "# TARGTYPE \"Win32 (x86) Dynamic-Link Library\" 0x0102"
    puts $cid ""
    puts $cid "CFG=$name - Win32 Debug"
    puts $cid "!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE"
    puts $cid "!MESSAGE verwenden Sie den Befehl \"Makefile exportieren\" und führen Sie den Befehl"
    puts $cid "!MESSAGE" 
    puts $cid "!MESSAGE NMAKE /f \"$name.mak\"."
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben"
    puts $cid "!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE NMAKE /f \"$name.mak\" CFG=\"$name - Win32 Debug\""
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Für die Konfiguration stehen zur Auswahl:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE \"$name - Win32 Release\" (basierend auf  \"Win32 (x86) Dynamic-Link Library\")"
    puts $cid "!MESSAGE \"$name - Win32 Debug\" (basierend auf  \"Win32 (x86) Dynamic-Link Library\")"
    puts $cid "!MESSAGE "
    puts $cid ""
    puts $cid "# Begin Project"
    puts $cid "# PROP AllowPerConfigDependencies 0"
    puts $cid "# PROP Scc_ProjName \"\""
    puts $cid "# PROP Scc_LocalPath \"\""
    puts $cid "CPP=cl.exe"
    puts $cid "MTL=midl.exe"
    puts $cid "RSC=rc.exe"
    puts $cid ""
    puts $cid "!IF  \"\$(CFG)\" == \"$name - Win32 Release\""
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries 0"
    puts $cid "# PROP BASE Output_Dir \"..\\..\\bin\\win32\""
    puts $cid "# PROP BASE Intermediate_Dir \"Release\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries 0"
    puts $cid "# PROP Output_Dir \"..\\..\\bin\\win32\""
    puts $cid "# PROP Intermediate_Dir \"Release\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $release_cpp_flags"
    puts $cid "# ADD CPP $release_cpp_flags"
    puts $cid "# ADD BASE MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD BASE RSC /l 0x409 /d \"NDEBUG\""
    puts $cid "# ADD RSC /l 0x409 /d \"NDEBUG\""
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"
    puts $cid "LINK32=link.exe"
    puts $cid "# ADD BASE LINK32 $libs /nologo /dll /machine:I386 /libpath:\"Release\" $release_dswlibpath"
    puts $cid "# ADD LINK32 $libs /nologo /dll /machine:I386 /libpath:\"Release\" $release_dswlibpath"
    puts $cid "# Begin Special Build Tool"
    puts $cid "SOURCE=\"\$(InputPath)\""
    puts $cid "PostBuild_Desc=Generating TOC file $name.toc"
    puts $cid "PostBuild_Cmds=tclsh ..\\src\\gentocfile.tcl $name win32 release"
    puts $cid "# End Special Build Tool"
    puts $cid ""
    puts $cid "!ELSEIF  \"\$(CFG)\" == \"$name - Win32 Debug\""
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries 1"
    puts $cid "# PROP BASE Output_Dir \"..\\..\\bin\\win32d\""
    puts $cid "# PROP BASE Intermediate_Dir \"Debug\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries 1"
    puts $cid "# PROP Output_Dir \"..\\..\\bin\\win32d\""
    puts $cid "# PROP Intermediate_Dir \"Debug\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $debug_cpp_flags"
    puts $cid "# ADD CPP $debug_cpp_flags"
    puts $cid "# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD BASE RSC /l 0x409 /d \"_DEBUG\""
    puts $cid "# ADD RSC /l 0x409 /d \"_DEBUG\""
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"
    puts $cid "LINK32=link.exe"
    puts $cid "# ADD BASE LINK32 $libs /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:\"Debug\" $debug_dswlibpath"
    puts $cid "# ADD LINK32 $libs /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:\"Debug\" $debug_dswlibpath"
    puts $cid "# Begin Special Build Tool"
    puts $cid "SOURCE=\"\$(InputPath)\""
    puts $cid "PostBuild_Desc=Generating TOC file $name.toc"
    puts $cid "PostBuild_Cmds=tclsh ..\\src\\gentocfile.tcl $name win32 debug"
    puts $cid "# End Special Build Tool"
    puts $cid ""
    puts $cid "!ENDIF"
    puts $cid ""
    puts $cid "# Begin Target"
    puts $cid ""
    puts $cid "# Name \"$name - Win32 Release\""
    puts $cid "# Name \"$name - Win32 Debug\""

    # generate a group for each module directory, and below it a group for each
    # module in that directory
    set cur_dir "none"
    for {set k 0} {$k < [llength $smod_list]} {incr k} {
        set m  [lindex $smod_list $k]

        # check if we need to start a new directory group
        if {$mod($m,dir) != $cur_dir} {
            # end current directory group
            if {$cur_dir != "none"} {
                puts $cid "# End Group"
            }
            # start a new group
            puts $cid "# Begin Group \"$mod($m,dir)\""
            puts $cid ""
            puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
            set cur_dir $mod($m,dir)
        }

        # create a sub group for each module and add files
        puts $cid "# Begin Group \"$mod($m,name)\""
        puts $cid ""
        puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            set mod_name $mod($m,name)
            set more_syms " /D \"N_INIT=n_init_$mod_name\" /D \"N_FINI=n_fini_$mod_name\" /D \"N_NEW=n_new_$mod_name\" /D \"N_VERSION=n_version_$mod_name\" /D \"N_INITCMDS=n_initcmds_$mod_name\""
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\[lindex $mod($m,srcs) $j]"
            puts $cid ""
            puts $cid "!IF  \"\$(CFG)\" == \"$name - Win32 Release\""
            puts $cid ""
            puts $cid "# ADD CPP $more_syms"
            puts $cid ""
            puts $cid "!ELSEIF \"\$(CFG)\" == \"$name - Win32 Debug\""
            puts $cid ""
            puts $cid "# ADD CPP $more_syms"
            puts $cid ""
            puts $cid "!ENDIF" 
            puts $cid ""
            puts $cid "# End Source File"
        }

        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\inc\\[lindex $mod($m,hdrs) $j]"
            puts $cid "# End Source File"
        }            

        if {$mod($m,version) != ""} {
            # add version source file
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\$mod($m,dir)\\$mod($m,version).cc"
            puts $cid "# End Source File"
        }
        puts $cid "# End Group"
    }
    puts $cid "# End Group"

    # Source code group (only 1 source file, package.cc)
    puts $cid "# Begin Group \"pkg_$name\""
    puts $cid ""
    puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
    puts $cid "# Begin Source File"
    puts $cid ""
    puts $cid "SOURCE=..\\src\\packages\\pkg_$name.cc"
    puts $cid "# End Source File"
    puts $cid "# End Group"

    # resource file group
    if {[llength $tar($i,rsrc_win32)] > 0} {
        puts $cid "# Begin Group \"resources\""
        puts $cid ""
        puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
        for {set j 0} {$j < [llength $tar($i,rsrc_win32)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\[lindex $tar($i,rsrc_win32) $j].rc"
    puts $cid "# End Source File"
        }
    puts $cid "# End Group"
    }

    # Header file group
#    puts $cid "# Begin Group \"Header-Dateien\""
#    puts $cid ""
#    puts $cid "# PROP Default_Filter \"h;hpp;hxx;hm;inl\""
#    puts $cid "# End Group"

    # End Of File
    puts $cid "# End Target"
    puts $cid "# End Project"
    
    close $cid
}

#--------------------------------------------------------------------
#   gen_dll_dsp
#   Generate dsp file for a dll target definition. 
#   07-Mar-00   floh    created
#--------------------------------------------------------------------
proc gen_dll_dsp {name} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_cpp_flags
    global debug_cpp_flags
    global release_dswlibpath
	global debug_dswlibpath

    puts "-> gen_dll_dsp $name"

    set i [findtargetbyname $name]

    # create list of link libs (win32 libs)
    set libs ""
    for {set j 0} {$j < [llength $tar($i,libs_win32)]} {incr j} {
        addtolist libs "[lindex $tar($i,libs_win32) $j]"
    }

    # write .dsp file
    set cid [open $nomads_home/code/vstudio/$name.dsp w]

    puts $cid "# Microsoft Developer Studio Project File - Name=\"$name\" - Package Owner=<4>"
    puts $cid "# Microsoft Developer Studio Generated Build File, Format Version 6.00"
    puts $cid "# ** NICHT BEARBEITEN **"
    puts $cid ""
    puts $cid "# TARGTYPE \"Win32 (x86) Dynamic-Link Library\" 0x0102"
    puts $cid ""
    puts $cid "CFG=$name - Win32 Debug"
    puts $cid "!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE"
    puts $cid "!MESSAGE verwenden Sie den Befehl \"Makefile exportieren\" und führen Sie den Befehl"
    puts $cid "!MESSAGE" 
    puts $cid "!MESSAGE NMAKE /f \"$name.mak\"."
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben"
    puts $cid "!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE NMAKE /f \"$name.mak\" CFG=\"$name - Win32 Debug\""
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Für die Konfiguration stehen zur Auswahl:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE \"$name - Win32 Release\" (basierend auf  \"Win32 (x86) Dynamic-Link Library\")"
    puts $cid "!MESSAGE \"$name - Win32 Debug\" (basierend auf  \"Win32 (x86) Dynamic-Link Library\")"
    puts $cid "!MESSAGE "
    puts $cid ""
    puts $cid "# Begin Project"
    puts $cid "# PROP AllowPerConfigDependencies 0"
    puts $cid "# PROP Scc_ProjName \"\""
    puts $cid "# PROP Scc_LocalPath \"\""
    puts $cid "CPP=cl.exe"
    puts $cid "MTL=midl.exe"
    puts $cid "RSC=rc.exe"
    puts $cid ""
    puts $cid "!IF  \"\$(CFG)\" == \"$name - Win32 Release\""
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries 0"
    puts $cid "# PROP BASE Output_Dir \"..\\..\\bin\\win32\""
    puts $cid "# PROP BASE Intermediate_Dir \"Release\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries 0"
    puts $cid "# PROP Output_Dir \"..\\..\\bin\\win32\""
    puts $cid "# PROP Intermediate_Dir \"Release\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $release_cpp_flags"
    puts $cid "# ADD CPP $release_cpp_flags"
    puts $cid "# ADD BASE MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD BASE RSC /l 0x409 /d \"NDEBUG\""
    puts $cid "# ADD RSC /l 0x409 /d \"NDEBUG\""
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"
    puts $cid "LINK32=link.exe"
    puts $cid "# ADD BASE LINK32 $libs /nologo /dll /machine:I386 /libpath:\"Release\" $release_dswlibpath"
    puts $cid "# ADD LINK32 $libs /nologo /dll /machine:I386 /libpath:\"Release\" $release_dswlibpath"
    puts $cid ""
    puts $cid "!ELSEIF  \"\$(CFG)\" == \"$name - Win32 Debug\""
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries 1"
    puts $cid "# PROP BASE Output_Dir \"..\\..\\bin\\win32d\""
    puts $cid "# PROP BASE Intermediate_Dir \"Debug\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries 1"
    puts $cid "# PROP Output_Dir \"..\\..\\bin\\win32d\""
    puts $cid "# PROP Intermediate_Dir \"Debug\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $debug_cpp_flags"
    puts $cid "# ADD CPP $debug_cpp_flags"
    puts $cid "# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD BASE RSC /l 0x409 /d \"_DEBUG\""
    puts $cid "# ADD RSC /l 0x409 /d \"_DEBUG\""
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"
    puts $cid "LINK32=link.exe"
    puts $cid "# ADD BASE LINK32 $libs /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:\"Debug\" $debug_dswlibpath"
    puts $cid "# ADD LINK32 $libs /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:\"Debug\" $debug_dswlibpath"
    puts $cid ""
    puts $cid "!ENDIF"
    puts $cid ""
    puts $cid "# Begin Target"
    puts $cid ""
    puts $cid "# Name \"$name - Win32 Release\""
    puts $cid "# Name \"$name - Win32 Debug\""

    # Source code group
    puts $cid "# Begin Group \"Source Files\""
    puts $cid ""
    puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
    for {set k 0} {$k < [llength $tar($i,mods)]} {incr k} {
        set m [findmodbyname [lindex $tar($i,mods) $k]]        
        # add regular sources
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\[lindex $mod($m,srcs) $j]"
            puts $cid "# End Source File"
        }
        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\inc\\[lindex $mod($m,hdrs) $j]"
            puts $cid "# End Source File"
        }            
        if {$mod($m,version) != ""} {
            # add version source file
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\$mod($m,dir)\\$mod($m,version).cc"
            puts $cid "# End Source File"
        }
    }
    puts $cid "# End Group"

    # resource file group
    if {[llength $tar($i,rsrc_win32)] > 0} {
        puts $cid "# Begin Group \"resources\""
        puts $cid ""
        puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
        for {set j 0} {$j < [llength $tar($i,rsrc_win32)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\[lindex $tar($i,rsrc_win32) $j].rc"
            puts $cid "# End Source File"
        }
        puts $cid "# End Group"
    }

    # Header file group
#    puts $cid "# Begin Group \"Header-Dateien\""
#    puts $cid ""
#    puts $cid "# PROP Default_Filter \"h;hpp;hxx;hm;inl\""
#    puts $cid "# End Group"

    # End Of File
    puts $cid "# End Target"
    puts $cid "# End Project"
    
    close $cid
}

#--------------------------------------------------------------------
#   gen_mll_dsp
#   Generate dsp file for a mll target definition. 
#   07-Nov-00   floh    created
#--------------------------------------------------------------------
proc gen_mll_dsp {name} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_cpp_flags
    global debug_cpp_flags
    global release_dswlibpath
	global debug_dswlibpath

    puts "-> gen_dll_dsp $name"

    set i [findtargetbyname $name]

    # create list of link libs (win32 libs)
    set libs ""
    for {set j 0} {$j < [llength $tar($i,libs_win32)]} {incr j} {
        addtolist libs "[lindex $tar($i,libs_win32) $j]"
    }

    # write .dsp file
    set cid [open $nomads_home/code/vstudio/$name.dsp w]

    puts $cid "# Microsoft Developer Studio Project File - Name=\"$name\" - Package Owner=<4>"
    puts $cid "# Microsoft Developer Studio Generated Build File, Format Version 6.00"
    puts $cid "# ** NICHT BEARBEITEN **"
    puts $cid ""
    puts $cid "# TARGTYPE \"Win32 (x86) Dynamic-Link Library\" 0x0102"
    puts $cid ""
    puts $cid "CFG=$name - Win32 Debug"
    puts $cid "!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE"
    puts $cid "!MESSAGE verwenden Sie den Befehl \"Makefile exportieren\" und führen Sie den Befehl"
    puts $cid "!MESSAGE" 
    puts $cid "!MESSAGE NMAKE /f \"$name.mak\"."
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben"
    puts $cid "!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE NMAKE /f \"$name.mak\" CFG=\"$name - Win32 Debug\""
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Für die Konfiguration stehen zur Auswahl:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE \"$name - Win32 Release\" (basierend auf  \"Win32 (x86) Dynamic-Link Library\")"
    puts $cid "!MESSAGE \"$name - Win32 Debug\" (basierend auf  \"Win32 (x86) Dynamic-Link Library\")"
    puts $cid "!MESSAGE "
    puts $cid ""
    puts $cid "# Begin Project"
    puts $cid "# PROP AllowPerConfigDependencies 0"
    puts $cid "# PROP Scc_ProjName \"\""
    puts $cid "# PROP Scc_LocalPath \"\""
    puts $cid "CPP=cl.exe"
    puts $cid "MTL=midl.exe"
    puts $cid "RSC=rc.exe"
    puts $cid ""
    puts $cid "!IF  \"\$(CFG)\" == \"$name - Win32 Release\""
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries 0"
    puts $cid "# PROP BASE Output_Dir \"..\\..\\bin\\win32\""
    puts $cid "# PROP BASE Intermediate_Dir \"Release\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries 0"
    puts $cid "# PROP Output_Dir \"..\\..\\bin\\win32\""
    puts $cid "# PROP Intermediate_Dir \"Release\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $release_cpp_flags"
    puts $cid "# ADD CPP $release_cpp_flags"
    puts $cid "# ADD BASE MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD BASE RSC /l 0x409 /d \"NDEBUG\""
    puts $cid "# ADD RSC /l 0x409 /d \"NDEBUG\""
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"
    puts $cid "LINK32=link.exe"
    puts $cid "# ADD BASE LINK32 $libs /nologo /dll /machine:I386 /libpath:\"Release\" $release_dswlibpath /export:initializePlugin /export:uninitializePlugin"
    puts $cid "# ADD LINK32 $libs /nologo /dll /machine:I386 /libpath:\"Release\" $release_dswlibpath /export:initializePlugin /export:uninitializePlugin"
    puts $cid ""
    puts $cid "!ELSEIF  \"\$(CFG)\" == \"$name - Win32 Debug\""
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries 1"
    puts $cid "# PROP BASE Output_Dir \"..\\..\\bin\\win32d\""
    puts $cid "# PROP BASE Intermediate_Dir \"Debug\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries 1"
    puts $cid "# PROP Output_Dir \"..\\..\\bin\\win32d\""
    puts $cid "# PROP Intermediate_Dir \"Debug\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $debug_cpp_flags"
    puts $cid "# ADD CPP $debug_cpp_flags"
    puts $cid "# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD BASE RSC /l 0x409 /d \"_DEBUG\""
    puts $cid "# ADD RSC /l 0x409 /d \"_DEBUG\""
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"
    puts $cid "LINK32=link.exe"
    puts $cid "# ADD BASE LINK32 $libs /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:\"Debug\" $debug_dswlibpath /export:initializePlugin /export:uninitializePlugin"
    puts $cid "# ADD LINK32 $libs /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:\"Debug\" $debug_dswlibpath /export:initializePlugin /export:uninitializePlugin"
    puts $cid ""
    puts $cid "!ENDIF"
    puts $cid ""
    puts $cid "# Begin Target"
    puts $cid ""
    puts $cid "# Name \"$name - Win32 Release\""
    puts $cid "# Name \"$name - Win32 Debug\""

    # Source code group
    puts $cid "# Begin Group \"Source Files\""
    puts $cid ""
    puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
    for {set k 0} {$k < [llength $tar($i,mods)]} {incr k} {
        set m [findmodbyname [lindex $tar($i,mods) $k]]        
        # add regular sources
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\[lindex $mod($m,srcs) $j]"
            puts $cid "# End Source File"
        }
        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\inc\\[lindex $mod($m,hdrs) $j]"
            puts $cid "# End Source File"
        }            
    }
    puts $cid "# End Group"

    # resource file group
    if {[llength $tar($i,rsrc_win32)] > 0} {
        puts $cid "# Begin Group \"resources\""
        puts $cid ""
        puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
        for {set j 0} {$j < [llength $tar($i,rsrc_win32)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\[lindex $tar($i,rsrc_win32) $j].rc"
            puts $cid "# End Source File"
        }
        puts $cid "# End Group"
    }

    # Header file group
#    puts $cid "# Begin Group \"Header-Dateien\""
#    puts $cid ""
#    puts $cid "# PROP Default_Filter \"h;hpp;hxx;hm;inl\""
#    puts $cid "# End Group"

    # End Of File
    puts $cid "# End Target"
    puts $cid "# End Project"
    
    close $cid
}

#--------------------------------------------------------------------
#   gen_exe_dsp
#   Generate dsp file for an executable target definition. 
#   08-Mar-00   floh    created
#--------------------------------------------------------------------
proc gen_exe_dsp {name} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_cpp_flags
    global debug_cpp_flags
    global release_dswlibpath
	global debug_dswlibpath

    puts "-> gen_exe_dsp $name"

    set i [findtargetbyname $name]

    # create list of link libs (win32 libs)
    set libs ""
    for {set j 0} {$j < [llength $tar($i,libs_win32)]} {incr j} {
        addtolist libs "[lindex $tar($i,libs_win32) $j]"
    }

    # write .dsp file
    set cid [open $nomads_home/code/vstudio/$name.dsp w]

    puts $cid "# Microsoft Developer Studio Project File - Name=\"$name\" - Package Owner=<4>"
    puts $cid "# Microsoft Developer Studio Generated Build File, Format Version 6.00"
    puts $cid "# ** NICHT BEARBEITEN **"
    puts $cid ""
    puts $cid "# TARGTYPE \"Win32 (x86) Application\" 0x0101"
    puts $cid ""
    puts $cid "CFG=$name - Win32 Debug"
    puts $cid "!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE"
    puts $cid "!MESSAGE verwenden Sie den Befehl \"Makefile exportieren\" und führen Sie den Befehl"
    puts $cid "!MESSAGE" 
    puts $cid "!MESSAGE NMAKE /f \"$name.mak\"."
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben"
    puts $cid "!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE NMAKE /f \"$name.mak\" CFG=\"$name - Win32 Debug\""
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Für die Konfiguration stehen zur Auswahl:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE \"$name - Win32 Release\" (basierend auf  \"Win32 (x86) Dynamic-Link Library\")"
    puts $cid "!MESSAGE \"$name - Win32 Debug\" (basierend auf  \"Win32 (x86) Dynamic-Link Library\")"
    puts $cid "!MESSAGE "
    puts $cid ""
    puts $cid "# Begin Project"
    puts $cid "# PROP AllowPerConfigDependencies 0"
    puts $cid "# PROP Scc_ProjName \"\""
    puts $cid "# PROP Scc_LocalPath \"\""
    puts $cid "CPP=cl.exe"
    puts $cid "MTL=midl.exe"
    puts $cid "RSC=rc.exe"
    puts $cid ""
    puts $cid "!IF  \"\$(CFG)\" == \"$name - Win32 Release\""
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries 0"
    puts $cid "# PROP BASE Output_Dir \"..\\..\\bin\\win32\""
    puts $cid "# PROP BASE Intermediate_Dir \"Release\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries 0"
    puts $cid "# PROP Output_Dir \"..\\..\\bin\\win32\""
    puts $cid "# PROP Intermediate_Dir \"Release\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $release_cpp_flags"
    puts $cid "# ADD CPP $release_cpp_flags"
    puts $cid "# ADD BASE MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD BASE RSC /l 0x409 /d \"NDEBUG\""
    puts $cid "# ADD RSC /l 0x409 /d \"NDEBUG\""
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"
    puts $cid "LINK32=link.exe"
    puts $cid "# ADD BASE LINK32 $libs /nologo /machine:I386 /libpath:\"Release\" $release_dswlibpath"
    puts $cid "# ADD LINK32 $libs /nologo /libpath:\"Release\" $release_dswlibpath"
    puts $cid ""
    puts $cid "!ELSEIF  \"\$(CFG)\" == \"$name - Win32 Debug\""
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries 1"
    puts $cid "# PROP BASE Output_Dir \"..\\..\\bin\\win32d\""
    puts $cid "# PROP BASE Intermediate_Dir \"Debug\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries 1"
    puts $cid "# PROP Output_Dir \"..\\..\\bin\\win32d\""
    puts $cid "# PROP Intermediate_Dir \"Debug\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $debug_cpp_flags"
    puts $cid "# ADD CPP $debug_cpp_flags"
    puts $cid "# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32"
    puts $cid "# ADD BASE RSC /l 0x409 /d \"_DEBUG\""
    puts $cid "# ADD RSC /l 0x409 /d \"_DEBUG\""
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"
    puts $cid "LINK32=link.exe"
    puts $cid "# ADD BASE LINK32 $libs /nologo /debug /machine:I386 /pdbtype:sept /libpath:\"Debug\" $debug_dswlibpath"
    puts $cid "# ADD LINK32 $libs /nologo /debug /machine:I386 /pdbtype:sept /libpath:\"Debug\" $debug_dswlibpath"
    puts $cid ""
    puts $cid "!ENDIF"
    puts $cid ""
    puts $cid "# Begin Target"
    puts $cid ""
    puts $cid "# Name \"$name - Win32 Release\""
    puts $cid "# Name \"$name - Win32 Debug\""

    # Source code group
    puts $cid "# Begin Group \"Source Files\""
    puts $cid ""
    puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
    for {set k 0} {$k < [llength $tar($i,mods)]} {incr k} {
        set m [findmodbyname [lindex $tar($i,mods) $k]]        
        # add regular sources
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\[lindex $mod($m,srcs) $j]"
            puts $cid "# End Source File"
        }
        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\inc\\[lindex $mod($m,hdrs) $j]"
            puts $cid "# End Source File"
        }            
    }
    puts $cid "# End Group"

    # resource file group
    if {[llength $tar($i,rsrc_win32)] > 0} {
        puts $cid "# Begin Group \"resources\""
        puts $cid ""
        puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
        for {set j 0} {$j < [llength $tar($i,rsrc_win32)]} {incr j} {
            puts $cid "# Begin Source File"
            puts $cid ""
            puts $cid "SOURCE=..\\src\\[lindex $tar($i,rsrc_win32) $j].rc"
            puts $cid "# End Source File"
        }
        puts $cid "# End Group"
    }

    # Header file group
#    puts $cid "# Begin Group \"Header-Dateien\""
#    puts $cid ""
#    puts $cid "# PROP Default_Filter \"h;hpp;hxx;hm;inl\""
#    puts $cid "# End Group"

    # End Of File
    puts $cid "# End Target"
    puts $cid "# End Project"
    
    close $cid
}

#--------------------------------------------------------------------
#   gen_workspace
#   09-Mar-00   floh    created
#   20-Mar-00   floh    no longer generate .toc files
#--------------------------------------------------------------------
proc gen_workspace {name list_of_targets} {
    global tar
    global num_tars
    global nomads_home

    puts "Generating vstudio workspace file $name.dsw..."

    # for each target, generate a dsp file, and if its a package target,
    # a .toc and package.cc file
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        set t [findtargetbyname [lindex $list_of_targets $i]]

        # ignore any non-win32-packages
        if {(($tar($t,platform)=="all") || ($tar($t,platform)=="win32"))} {
            if {$tar($t,type) == "package"} {
                gen_package_dsp $tar($t,name)
            } elseif {$tar($t,type) == "dll"} {
                gen_dll_dsp $tar($t,name)
            } elseif {$tar($t,type) == "exe"} {
                gen_exe_dsp $tar($t,name)
            } elseif {$tar($t,type) == "mll"} {
                gen_mll_dsp $tar($t,name)
            } else {
                puts "ERROR: UNKNOWN TARGET TYPE '$tar($t,type)'"
                exit
            }
            if {$tar($t,type) == "package"} {
                genpacksrc $tar($t,name)
            }
        }
    }

    # write .dsw file
    set cid [open $nomads_home/code/vstudio/$name.dsw w]

    puts $cid "Microsoft Developer Studio Workspace File, Format Version 6.00"
    puts $cid "# WARNUNG: DIESE ARBEITSBEREICHSDATEI DARF NICHT BEARBEITET ODER GELÖSCHT WERDEN!"
    puts $cid ""

    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        set t [findtargetbyname [lindex $list_of_targets $i]]

        # ignore any non-win32-packages
        if {(($tar($t,platform)=="all") || ($tar($t,platform)=="win32"))} {

            puts $cid "###############################################################################"
            puts $cid ""
            puts $cid "Project: \"$tar($t,name)\"=.\\$tar($t,name).dsp - Package Owner=<4>"
            puts $cid ""
            puts $cid "Package=<5>"
            puts $cid "\{\{\{"
            puts $cid "\}\}\}"
            puts $cid ""
            puts $cid "Package=<4>"
            puts $cid "\{\{\{"
            for {set k 0} {$k < [llength $tar($t,depends)]} {incr k} {
                set dtar [lindex $tar($t,depends) $k]
                # check if depend target exists as win32 target
                set d [findtargetbyname $dtar]
                if {(($tar($d,platform)=="win32") || ($tar($d,platform)=="all"))} {
                    puts $cid "    Begin Project Dependency"
                    puts $cid "    Project_Dep_Name [lindex $tar($t,depends) $k]"
                    puts $cid "    End Project Dependency"
                }
            }
            puts $cid "\}\}\}"
            puts $cid ""
        }
    }
    puts $cid "###############################################################################"
    puts $cid ""
    puts $cid "Global:"
    puts $cid ""
    puts $cid "Package=<5>"
    puts $cid "\{\{\{"
    puts $cid "\}\}\}"
    puts $cid ""
    puts $cid "Package=<3>"
    puts $cid "\{\{\{"
    puts $cid "\}\}\}"
    puts $cid ""
    puts $cid "###############################################################################"

    close $cid

    puts "...done generating $name.dsw."
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

