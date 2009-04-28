#--------------------------------------------------------------------
#   makesln.tcl
#   ===========
#   Functions to generate Visual Studio .NET solution files.
#   Is included into makemake.tcl.
#
#   UUIDGEN.exe must be in PATH!
#   (\Program Files\Microsoft Visual Studio .NET\Common7\Tools\Bin)
#
#   29-Jan-2002 stki	created
#	20-Feb-2002 stki	+ added fix for missing _ver files
#
#--------------------------------------------------------------------

set release_preprocessor_defs "_NEBULA_DEVICE_;WIN32;NDEBUG;__NEBULA_MEM_MANAGER__;NT_PLUGIN;__WIN32__;_CRT_SECURE_NO_DEPRECATE;NO_STLPORT;__USE_SSE__"
set debug_preprocessor_defs "_NEBULA_DEVICE_;WIN32;_DEBUG;__NEBULA_MEM_MANAGER__;NT_PLUGIN;__WIN32__;_CRT_SECURE_NO_DEPRECATE;NO_STLPORT;__USE_SSE__"
set release_libpath "..\\..\\bin\\win32,..\\lib\\win32_vc_i386"
set debug_libpath "..\\..\\bin\\win32d,..\\lib\\win32_vc_i386"
set incpath "..\\inc"

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
#   gen_package_vcproj
#   Generate vcproj file for a package target definition.
#   29-Jan-02   stki	created
#--------------------------------------------------------------------
proc gen_package_vcproj {name} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_preprocessor_defs
    global debug_preprocessor_defs
    global release_libpath
    global debug_libpath
    global incpath

    puts "-> gen_package_vcproj $name"

    set i [findtargetbyname $name]

    # sort modules by directory
    set smod_list [get_dirmod $name]

    # create list of link libs (win32 libs, dependency libs)
    set libs ""
    for {set j 0} {$j < [llength $tar($i,libs_win32)]} {incr j} {
        addtolist libs "[lindex $tar($i,libs_win32) $j]"
    }

    # write .vcproj file
    set cid [open $nomads_home/code/vstudio/$name.vcproj w]

    puts $cid "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>"
    puts $cid "<VisualStudioProject"
    puts $cid "	ProjectType=\"Visual C++\""
    puts $cid "	Version=\"7.00\""
    puts $cid "	Name=\"$name\""
    puts $cid "	SccProjectName=\"\""
    puts $cid "	SccLocalPath=\"\">"
    puts $cid "	<Platforms>"
    puts $cid "		<Platform"
    puts $cid "			Name=\"Win32\"/>"
    puts $cid "	</Platforms>"
    puts $cid "	<Configurations>"
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Debug|Win32\""
    puts $cid "			OutputDirectory=\".\\..\\..\\bin\\win32d\""
    puts $cid "			IntermediateDirectory=\".\\Debug\""
    puts $cid "			ConfigurationType=\"2\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"false\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"0\""
    puts $cid "				AdditionalIncludeDirectories=\"$incpath\""
    puts $cid "				PreprocessorDefinitions=\"$debug_preprocessor_defs\""
    puts $cid "				BasicRuntimeChecks=\"3\""
    puts $cid "				RuntimeLibrary=\"3\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				PrecompiledHeaderFile=\".\\Debug/$name.pch\""
    puts $cid "				AssemblerListingLocation=\".\\Debug/\""
    puts $cid "				ObjectFile=\".\\Debug/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Debug/\""
    puts $cid "				WarningLevel=\"4\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				DebugInformationFormat=\"4\""	
    puts $cid "				CompileAs=\"2\""
	puts $cid "				DisableSpecificWarnings=\"4706;4127\""
	puts $cid "				/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalOptions=\"/MACHINE:IX86\""
    puts $cid "				AdditionalDependencies=\"$libs\""
    puts $cid "				OutputFile=\".\\..\\..\\bin\\win32d/$name.dll\""
    puts $cid "				LinkIncremental=\"2\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				AdditionalLibraryDirectories=\"Debug,$debug_libpath\""
    puts $cid "				GenerateDebugInformation=\"true\""
    puts $cid "				ProgramDatabaseFile=\".\\..\\..\\bin\\win32d/$name.pdb\""
    puts $cid "				ImportLibrary=\".\\..\\..\\bin\\win32d/$name.lib\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCMIDLTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				MkTypLibCompatible=\"true\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				TargetEnvironment=\"1\""
    puts $cid "				TypeLibraryName=\".\\..\\..\\bin\\win32d/$name.tlb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPostBuildEventTool\""
    puts $cid "				Description=\"Generating TOC file $name.toc\""
    puts $cid "				CommandLine=\"tclsh ..\\src\\gentocfile.tcl $name win32 debug\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreLinkEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebServiceProxyGeneratorTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebDeploymentTool\"/>"
    puts $cid "		</Configuration>"
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Release|Win32\""
    puts $cid "			OutputDirectory=\".\\..\\..\\bin\\win32\""
    puts $cid "			IntermediateDirectory=\".\\Release\""
    puts $cid "			ConfigurationType=\"2\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"false\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"4\""
    puts $cid "				GlobalOptimizations=\"true\""
    puts $cid "				InlineFunctionExpansion=\"2\""
    puts $cid "				EnableIntrinsicFunctions=\"true\""
    puts $cid "				FavorSizeOrSpeed=\"1\""
    puts $cid "				OmitFramePointers=\"true\""
    puts $cid "				OptimizeForProcessor=\"2\""
    puts $cid "				AdditionalIncludeDirectories=\"$incpath\""
    puts $cid "				PreprocessorDefinitions=\"$release_preprocessor_defs\""
    puts $cid "				StringPooling=\"true\""		
    puts $cid "				RuntimeLibrary=\"2\""
    puts $cid "				EnableFunctionLevelLinking=\"true\""
	puts $cid "				EnableEnhancedInstructionSet=\"2\""
	puts $cid "				RuntimeTypeInfo=\"false\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				PrecompiledHeaderFile=\".\\Release/$name.pch\""
    puts $cid "				AssemblerListingLocation=\".\\Release/\""
    puts $cid "				ObjectFile=\".\\Release/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Release/\""
    puts $cid "				WarningLevel=\"4\""
    puts $cid "				SuppressStartupBanner=\"true\""	
    puts $cid "				CompileAs=\"2\""
	puts $cid "				DisableSpecificWarnings=\"4706;4127\""
	puts $cid "				/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalOptions=\"/MACHINE:IX86\""
    puts $cid "				AdditionalDependencies=\"$libs\""
    puts $cid "				OutputFile=\".\\..\\..\\bin\\win32/$name.dll\""
    puts $cid "				LinkIncremental=\"1\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				AdditionalLibraryDirectories=\"Release,$release_libpath\""
    puts $cid "				ProgramDatabaseFile=\".\\..\\..\\bin\\win32/$name.pdb\""
    puts $cid "				ImportLibrary=\".\\..\\..\\bin\\win32/$name.lib\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCMIDLTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				MkTypLibCompatible=\"true\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				TargetEnvironment=\"1\""
    puts $cid "				TypeLibraryName=\".\\..\\..\\bin\\win32/$name.tlb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPostBuildEventTool\""
    puts $cid "				Description=\"Generating TOC file $name.toc\""
    puts $cid "				CommandLine=\"tclsh ..\\src\\gentocfile.tcl $name win32 release\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreLinkEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebServiceProxyGeneratorTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebDeploymentTool\"/>"
    puts $cid "		</Configuration>"
    puts $cid "	</Configurations>    "

    puts $cid "	<Files>"

    # generate a group for each module directory, and below it a group for each
    # module in that directory
    set cur_dir "none"
    for {set k 0} {$k < [llength $smod_list]} {incr k} {
        set m  [lindex $smod_list $k]

        # check if we need to start a new directory group
        if {$mod($m,dir) != $cur_dir} {
            # end current directory group
            if {$cur_dir != "none"} {
                puts $cid "		</Filter>"
            }
            # start a new group
            puts $cid "		<Filter"
            puts $cid "			Name=\"$mod($m,dir)\""
            puts $cid "			Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"
            set cur_dir $mod($m,dir)
        }

        # create a sub group for each module and add files

        puts $cid "			<Filter"
        puts $cid "				Name=\"$mod($m,name)\""
        puts $cid "				Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            set mod_name $mod($m,name)
            set more_syms "N_STATIC;N_INIT=n_init_$mod_name;N_FINI=n_fini_$mod_name;N_NEW=n_new_$mod_name;N_VERSION=n_version_$mod_name;N_INITCMDS=n_initcmds_$mod_name"
            puts $cid "				<File"
            puts $cid "					RelativePath=\"..\\src\\$mod($m,dir)\\[lindex $mod($m,files) $j].cc\">"
            puts $cid "					<FileConfiguration"
            puts $cid "						Name=\"Debug|Win32\">"
            puts $cid "						<Tool"
            puts $cid "							Name=\"VCCLCompilerTool\""
            puts $cid "							PreprocessorDefinitions=\"$more_syms\"/>"
            puts $cid "					</FileConfiguration>"
            puts $cid "					<FileConfiguration"
            puts $cid "						Name=\"Release|Win32\">"
            puts $cid "						<Tool"
            puts $cid "							Name=\"VCCLCompilerTool\""
            puts $cid "							PreprocessorDefinitions=\"$more_syms\"/>"
            puts $cid "					</FileConfiguration>"
            puts $cid "				</File>"
        }

        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "				<File"
            puts $cid "					RelativePath=\"..\\inc\\$mod($m,dir)\\[lindex $mod($m,headers) $j].h\">"
            puts $cid "				</File>"
        }

        if {$mod($m,version) != ""} {
        	# add version source file
        	puts $cid "				<File"
        	puts $cid "					RelativePath=\"..\\src\\$mod($m,dir)\\$mod($m,version).cc\">"
        	puts $cid "				</File>"
        	}
     	puts $cid "			</Filter>"
    }
    puts $cid "		</Filter>"

    # Source code group (only 1 source file, package.cc)
    puts $cid "		<Filter"
    puts $cid "			Name=\"pkg_$name\""
    puts $cid "			Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"
    puts $cid "			<File"
    puts $cid "				RelativePath=\"..\\src\\packages\\pkg_$name.cc\">"
    puts $cid "			</File>"
    puts $cid "		</Filter>    "

    # End Of File
    puts $cid "	</Files>"
    puts $cid "	<Globals>"
    puts $cid "	</Globals>"
    puts $cid "</VisualStudioProject>"
    
    close $cid
}

#--------------------------------------------------------------------
#   gen_dll_vcproj
#   Generate vcproj file for a dll or mll target definition.
#   29-Jan-02	stki    created
#--------------------------------------------------------------------
proc gen_dll_vcproj {name extension} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_preprocessor_defs
    global debug_preprocessor_defs
    global release_libpath
    global debug_libpath
    global incpath

    puts "-> gen_dll_vcproj $name"

    set i [findtargetbyname $name]

    # create list of link libs (win32 libs)
    set libs ""
    for {set j 0} {$j < [llength $tar($i,libs_win32)]} {incr j} {
        addtolist libs "[lindex $tar($i,libs_win32) $j]"
    }

    # write .dsp file
    set cid [open $nomads_home/code/vstudio/$name.vcproj w]
    
    puts $cid "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>"
    puts $cid "<VisualStudioProject"
    puts $cid "	ProjectType=\"Visual C++\""
    puts $cid "	Version=\"7.00\""
    puts $cid "	Name=\"$name\""
    puts $cid "	SccProjectName=\"\""
    puts $cid "	SccLocalPath=\"\">"
    puts $cid "	<Platforms>"
    puts $cid "		<Platform"
    puts $cid "			Name=\"Win32\"/>"
    puts $cid "	</Platforms>"
    puts $cid "	<Configurations>"
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Debug|Win32\""
    puts $cid "			OutputDirectory=\".\\..\\..\\bin\\win32d\""
    puts $cid "			IntermediateDirectory=\".\\Debug\""
    puts $cid "			ConfigurationType=\"2\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"false\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"0\""
    puts $cid "				AdditionalIncludeDirectories=\"$incpath\""
    puts $cid "				PreprocessorDefinitions=\"$debug_preprocessor_defs\""
    puts $cid "				BasicRuntimeChecks=\"3\""
    puts $cid "				RuntimeLibrary=\"3\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				PrecompiledHeaderFile=\".\\Debug/$name.pch\""
    puts $cid "				AssemblerListingLocation=\".\\Debug/\""
    puts $cid "				ObjectFile=\".\\Debug/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Debug/\""
    puts $cid "				WarningLevel=\"3\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				DebugInformationFormat=\"4\""
    puts $cid "				CompileAs=\"2\""
	puts $cid "				DisableSpecificWarnings=\"4706;4127\""
	puts $cid "				/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalOptions=\"/MACHINE:IX86\""
    puts $cid "				AdditionalDependencies=\"$libs\""
    puts $cid "				OutputFile=\".\\..\\..\\bin\\win32d/$name.dll\""
    puts $cid "				LinkIncremental=\"2\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				AdditionalLibraryDirectories=\"Debug,$debug_libpath\""
    puts $cid "				GenerateDebugInformation=\"true\""
    puts $cid "				ProgramDatabaseFile=\".\\..\\..\\bin\\win32d/$name.pdb\""
    puts $cid "				ImportLibrary=\".\\..\\..\\bin\\win32d/$name.lib\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCMIDLTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				MkTypLibCompatible=\"true\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				TargetEnvironment=\"1\""
    puts $cid "				TypeLibraryName=\".\\..\\..\\bin\\win32d/$name.tlb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPostBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreLinkEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebServiceProxyGeneratorTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebDeploymentTool\"/>"
    puts $cid "		</Configuration>"
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Release|Win32\""
    puts $cid "			OutputDirectory=\".\\..\\..\\bin\\win32\""
    puts $cid "			IntermediateDirectory=\".\\Release\""
    puts $cid "			ConfigurationType=\"2\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"false\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"4\""
    puts $cid "				GlobalOptimizations=\"true\""
    puts $cid "				InlineFunctionExpansion=\"2\""
    puts $cid "				EnableIntrinsicFunctions=\"true\""
    puts $cid "				FavorSizeOrSpeed=\"1\""
    puts $cid "				OmitFramePointers=\"true\""
    puts $cid "				OptimizeForProcessor=\"2\""
    puts $cid "				AdditionalIncludeDirectories=\"$incpath\""
    puts $cid "				PreprocessorDefinitions=\"$release_preprocessor_defs\""
    puts $cid "				StringPooling=\"true\""	
    puts $cid "				RuntimeLibrary=\"2\""
    puts $cid "				EnableFunctionLevelLinking=\"true\""
	puts $cid "				EnableEnhancedInstructionSet=\"2\""
	puts $cid "				RuntimeTypeInfo=\"false\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				PrecompiledHeaderFile=\".\\Release/$name.pch\""
    puts $cid "				AssemblerListingLocation=\".\\Release/\""
    puts $cid "				ObjectFile=\".\\Release/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Release/\""
    puts $cid "				WarningLevel=\"4\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				CompileAs=\"2\""
	puts $cid "				DisableSpecificWarnings=\"4706;4127\""
	puts $cid "				/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalOptions=\"/MACHINE:IX86\""
    puts $cid "				AdditionalDependencies=\"$libs\""
    puts $cid "				OutputFile=\".\\..\\..\\bin\\win32/$name.$extension\""
    puts $cid "				LinkIncremental=\"1\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				AdditionalLibraryDirectories=\"Release,$release_libpath\""
    puts $cid "				ProgramDatabaseFile=\".\\..\\..\\bin\\win32/$name.pdb\""
    puts $cid "				ImportLibrary=\".\\..\\..\\bin\\win32/$name.lib\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCMIDLTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				MkTypLibCompatible=\"true\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				TargetEnvironment=\"1\""
    puts $cid "				TypeLibraryName=\".\\..\\..\\bin\\win32/$name.tlb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPostBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreLinkEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebServiceProxyGeneratorTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebDeploymentTool\"/>"
    puts $cid "		</Configuration>"
    puts $cid "	</Configurations>    "

    # Source code group
    puts $cid "	<Files>"
    puts $cid "		<Filter"
    puts $cid "			Name=\"Source Files\""
    puts $cid "			Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"    
    for {set k 0} {$k < [llength $tar($i,mods)]} {incr k} {
        set m [findmodbyname [lindex $tar($i,mods) $k]]
        # add regular sources
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            puts $cid "			<File"
            puts $cid "				RelativePath=\"..\\src\\$mod($m,dir)\\[lindex $mod($m,files) $j].cc\">"
            puts $cid "			</File>"
        }
        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "			<File"
            puts $cid "				RelativePath=\"..\\inc\\$mod($m,dir)\\[lindex $mod($m,headers) $j].h\">"
            puts $cid "			</File>"        
        }
        if {$mod($m,version) != ""} {        
        	# add version source file
        	puts $cid "			<File"
        	puts $cid "				RelativePath=\"..\\src\\$mod($m,dir)\\$mod($m,version).cc\">"
        	puts $cid "			</File>"        
        }
    }
    puts $cid "		</Filter>"

    # End Of File
    puts $cid "	</Files>"
    puts $cid "	<Globals>"
    puts $cid "	</Globals>"
    puts $cid "</VisualStudioProject>"

    close $cid
}


#--------------------------------------------------------------------
#   gen_exe_vcproj
#   Generate vcproj file for an executable target definition.
#   29-Jan-02	stki    created
#--------------------------------------------------------------------
proc gen_exe_vcproj {name} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_preprocessor_defs
    global debug_preprocessor_defs
    global release_libpath
    global debug_libpath
    global incpath

    puts "-> gen_exe_vcproj $name"

    set i [findtargetbyname $name]

    # create list of link libs (win32 libs)
    set libs ""
    for {set j 0} {$j < [llength $tar($i,libs_win32)]} {incr j} {
        addtolist libs "[lindex $tar($i,libs_win32) $j]"
    }

    # write .dsp file
    set cid [open $nomads_home/code/vstudio/$name.vcproj w]
    
    puts $cid "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>"
    puts $cid "<VisualStudioProject"
    puts $cid "	ProjectType=\"Visual C++\""
    puts $cid "	Version=\"7.00\""
    puts $cid "	Name=\"$name\""
    puts $cid "	SccProjectName=\"\""
    puts $cid "	SccLocalPath=\"\">"
    puts $cid "	<Platforms>"
    puts $cid "		<Platform"
    puts $cid "			Name=\"Win32\"/>"
    puts $cid "	</Platforms>"
    puts $cid "	<Configurations>"
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Release|Win32\""
    puts $cid "			OutputDirectory=\".\\..\\..\\bin\\win32\""
    puts $cid "			IntermediateDirectory=\".\\Release\""
    puts $cid "			ConfigurationType=\"1\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"false\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"4\""
    puts $cid "				GlobalOptimizations=\"true\""
    puts $cid "				InlineFunctionExpansion=\"2\""
    puts $cid "				EnableIntrinsicFunctions=\"true\""
    puts $cid "				FavorSizeOrSpeed=\"1\""
    puts $cid "				OmitFramePointers=\"true\""
    puts $cid "				OptimizeForProcessor=\"2\""
    puts $cid "				AdditionalIncludeDirectories=\"$incpath\""
    puts $cid "				PreprocessorDefinitions=\"_WINDLL;$release_preprocessor_defs\""
    puts $cid "				StringPooling=\"true\""	
    puts $cid "				RuntimeLibrary=\"2\""
    puts $cid "				EnableFunctionLevelLinking=\"true\""
	puts $cid "				EnableEnhancedInstructionSet=\"2\""
	puts $cid "				RuntimeTypeInfo=\"false\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				PrecompiledHeaderFile=\".\\Release/$name.pch\""
    puts $cid "				AssemblerListingLocation=\".\\Release/\""
    puts $cid "				ObjectFile=\".\\Release/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Release/\""
    puts $cid "				WarningLevel=\"4\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				CompileAs=\"2\""
	puts $cid "				DisableSpecificWarnings=\"4706;4127\""
	puts $cid "				/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalDependencies=\"$libs\""
    puts $cid "				OutputFile=\".\\..\\..\\bin\\win32/$name.exe\""
    puts $cid "				LinkIncremental=\"1\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				AdditionalLibraryDirectories=\"Release,$release_libpath\""
    puts $cid "				ProgramDatabaseFile=\".\\..\\..\\bin\\win32/$name.pdb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCMIDLTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				MkTypLibCompatible=\"true\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				TargetEnvironment=\"1\""
    puts $cid "				TypeLibraryName=\".\\..\\..\\bin\\win32/$name.tlb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPostBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreLinkEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebServiceProxyGeneratorTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebDeploymentTool\"/>"
    puts $cid "		</Configuration>"
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Debug|Win32\""
    puts $cid "			OutputDirectory=\".\\..\\..\\bin\\win32d\""
    puts $cid "			IntermediateDirectory=\".\\Debug\""
    puts $cid "			ConfigurationType=\"1\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"false\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"0\""
    puts $cid "				AdditionalIncludeDirectories=\"$incpath\""
    puts $cid "				PreprocessorDefinitions=\"_WINDLL;$debug_preprocessor_defs\""
    puts $cid "				BasicRuntimeChecks=\"3\""
    puts $cid "				RuntimeLibrary=\"3\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				PrecompiledHeaderFile=\".\\Debug/$name.pch\""
    puts $cid "				AssemblerListingLocation=\".\\Debug/\""
    puts $cid "				ObjectFile=\".\\Debug/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Debug/\""
    puts $cid "				WarningLevel=\"4\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				DebugInformationFormat=\"4\""
    puts $cid "				CompileAs=\"2\""
	puts $cid "				DisableSpecificWarnings=\"4706;4127\""
	puts $cid "				/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalOptions=\"/MACHINE:IX86\""
    puts $cid "				AdditionalDependencies=\"$libs\""
    puts $cid "				OutputFile=\".\\..\\..\\bin\\win32d/$name.exe\""
    puts $cid "				LinkIncremental=\"2\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				AdditionalLibraryDirectories=\"Debug,$debug_libpath\""
    puts $cid "				GenerateDebugInformation=\"true\""
    puts $cid "				ProgramDatabaseFile=\".\\..\\..\\bin\\win32d/$name.pdb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCMIDLTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				MkTypLibCompatible=\"true\""
    puts $cid "				SuppressStartupBanner=\"true\""
    puts $cid "				TargetEnvironment=\"1\""
    puts $cid "				TypeLibraryName=\".\\..\\..\\bin\\win32d/$name.tlb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPostBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreBuildEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCPreLinkEventTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebServiceProxyGeneratorTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCWebDeploymentTool\"/>"
    puts $cid "		</Configuration>"
    puts $cid "	</Configurations>"


    # Source code group
    puts $cid "	<Files>"
    puts $cid "		<Filter"
    puts $cid "			Name=\"Source Files\""
    puts $cid "			Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"
    for {set k 0} {$k < [llength $tar($i,mods)]} {incr k} {
        set m [findmodbyname [lindex $tar($i,mods) $k]]
        # add regular sources
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            puts $cid "			<File"
            puts $cid "				RelativePath=\"..\\src\\$mod($m,dir)\\[lindex $mod($m,files) $j].cc\">"
            puts $cid "			</File>"
        }
        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "			<File"
            puts $cid "				RelativePath=\"..\\inc\\$mod($m,dir)\\[lindex $mod($m,headers) $j].h\">"
            puts $cid "			</File>"
        }
    }
    puts $cid "		</Filter>"

    # End Of File
    puts $cid "	</Files>"
    puts $cid "	<Globals>"
    puts $cid "	</Globals>"
    puts $cid "</VisualStudioProject>"

    close $cid
}

#--------------------------------------------------------------------
#   gen_solution
#   29-Jan-02	stki    created
#--------------------------------------------------------------------
proc gen_solution {name list_of_targets} {
    global tar
    global num_tars
    global nomads_home

    # Check for uuidgen command
    if { [catch {exec uuidgen}] } {
        puts "Unable to generate Visual Studio .NET solution"
        puts "Ensure that uuidgen.exe is in your path."
        return
    }
    puts "Generating Visual Studio .NET solution file $name.sln..."

    # for each target, generate a vcproj file, and if its a package target,
    # a .toc and package.cc file
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        set t [findtargetbyname [lindex $list_of_targets $i]]

        # ignore any non-win32-packages
        if {(($tar($t,platform)=="all") || ($tar($t,platform)=="win32"))} {
            if {$tar($t,type) == "package"} {
                gen_package_vcproj $tar($t,name)
            } elseif {$tar($t,type) == "dll"} {
                gen_dll_vcproj $tar($t,name) dll
            } elseif {$tar($t,type) == "exe"} {
                gen_exe_vcproj $tar($t,name)
            } elseif {$tar($t,type) == "mll"} {
                gen_dll_vcproj $tar($t,name) mll
            } else {
                puts "ERROR: UNKNOWN TARGET TYPE '$tar($t,type)'"
                exit
            }
            if {$tar($t,type) == "package"} {
                genpacksrc $tar($t,name)
            }
        }
    }

    # write .sln file
    set cid [open $nomads_home/code/vstudio/$name.sln w]

    puts $cid "Microsoft Visual Studio Solution File, Format Version 7.00"

    # project list
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        set t [findtargetbyname [lindex $list_of_targets $i]]

        # ignore any non-win32-packages
        if {(($tar($t,platform)=="all") || ($tar($t,platform)=="win32"))} {

	    # generate UUID for project
	    set tar($t,uuid) [string toupper [exec uuidgen]]
	    
	    puts $cid "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"$tar($t,name)\", \"$tar($t,name).vcproj\", \"{$tar($t,uuid)}\""
	    puts $cid "EndProject"
	}
    }
    
    puts $cid "Global"
    puts $cid "	GlobalSection(SolutionConfiguration) = preSolution"
    puts $cid "		ConfigName.0 = Debug"
    puts $cid "		ConfigName.1 = Release"
    puts $cid "	EndGlobalSection"
    puts $cid "	GlobalSection(ProjectDependencies) = postSolution    "
    
    # project dependencies
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        set t [findtargetbyname [lindex $list_of_targets $i]]

        # ignore any non-win32-packages
        if {(($tar($t,platform)=="all") || ($tar($t,platform)=="win32"))} {
            set depcount 0
            for {set k 0} {$k < [llength $tar($t,depends)]} {incr k} {
                set dtar [lindex $tar($t,depends) $k]
                # check if depend target exists as win32 target
                set d [findtargetbyname $dtar]
                if {(($tar($d,platform)=="win32") || ($tar($d,platform)=="all"))} {
                    puts $cid "		{$tar($t,uuid)}.$depcount = {$tar($d,uuid)}"
                    incr depcount
                }
            }
        }
    }
    
    puts $cid "	EndGlobalSection"
    puts $cid "	GlobalSection(ProjectConfiguration) = postSolution"
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        set t [findtargetbyname [lindex $list_of_targets $i]]

        # ignore any non-win32-packages
        if {(($tar($t,platform)=="all") || ($tar($t,platform)=="win32"))} {
            puts $cid "		{$tar($t,uuid)}.Debug.ActiveCfg = Debug|Win32"
            puts $cid "		{$tar($t,uuid)}.Debug.Build.0 = Debug|Win32"
            puts $cid "		{$tar($t,uuid)}.Release.ActiveCfg = Release|Win32"
            puts $cid "		{$tar($t,uuid)}.Release.Build.0 = Release|Win32"
        }
    }
    
    puts $cid "	EndGlobalSection"
    puts $cid "	GlobalSection(ExtensibilityGlobals) = postSolution"
    puts $cid "	EndGlobalSection"
    puts $cid "	GlobalSection(ExtensibilityAddIns) = postSolution"
    puts $cid "	EndGlobalSection"
    puts $cid "EndGlobal"

    close $cid

    puts "...done generating $name.sln."
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

