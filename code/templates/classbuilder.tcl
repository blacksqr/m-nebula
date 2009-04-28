#--------------------------------------------------------------------
#   classbuilder.tcl
#	----------------
#   Generates basic class files.
#	Use as a starting point for a new class.
#
#	The files created are:
#	nebula/code/inc/$subdir/$nclassname.h
#	nebula/code/src/$subdir/$nclassname_main.cc, $nclassname_cmds.cc
#	nebula/code/src/$packagename.pak
#
#	This an early version, and as such is not very robust. But it saves
#	time cutting&pasting.
#
#   (C)	2002	leaf
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   Toggle State of .pak group
#--------------------------------------------------------------------
proc togglepak {state} {
	if {$state} {
		.f1.f.deps config -state normal
		.f1.f.libs config -state normal
		.f1.tar config -state normal
	} else {
		.f1.f.deps config -state disabled
		.f1.f.libs config -state disabled
		.f1.tar config -state disabled
	}
}

#--------------------------------------------------------------------
#   Build Interface
#--------------------------------------------------------------------
wm resizable . 0 0
wm title . "Class Builder"
frame .f0 -bd 4
pack .f0 -side top -expand 1 -fill both

entry .cln -textvariable classname -bd 1
.cln insert 0 "nClassName"
label .clnl -text "Class Name" -anchor w -padx 4

entry .scln -textvariable superclassname -bd 1
.scln insert 0 "nSuperclassName"
label .sclnl -text "Superclass Name" -anchor w -padx 4

entry .dir -textvariable directory -bd 1
.dir insert 0 "package"
label .dirl -text "Directory/Package" -anchor w -padx 4

entry .aut -textvariable author -bd 1 
.aut insert 0 "user"
label .autl -text "Short Author Name" -anchor w -padx 4

checkbutton .scrpt -text "Scriptable" -variable scriptable -anchor w

frame .f1 -bd 1 -relief raised

grid .cln .clnl -sticky ew -in .f0 -pady 1
grid .scln .sclnl -sticky ew -in .f0 -pady 1
grid .dir .dirl -sticky ew -in .f0 -pady 1
grid .aut .autl -sticky ew -in .f0 -pady 1
grid x .scrpt -sticky ew -in .f0
grid .f1 -sticky news -in .f0 -columnspan 2

checkbutton .f1.enable -text "Make .pak File" -variable pakenable -command {togglepak $pakenable} -anchor w
menubutton .f1.tar -text "Target Type" -textvariable target -menu .f1.tar.ml -indicatoron 1  -bd 1
menu .f1.tar.ml -tearoff 0
.f1.tar.ml add radiobutton -label " Package " -variable target
.f1.tar.ml add radiobutton -label " Executable " -variable target
.f1.tar.ml add radiobutton -label " DLL " -variable target
.f1.tar config -state disabled
frame .f1.f -bd 4
pack .f1.f -side bottom -fill both -expand yes
pack .f1.enable -side left -anchor w
pack .f1.tar -side right -padx 5

entry .f1.f.deps -textvariable depends -bd 1 
.f1.f.deps insert 0 "nkernel nnebula"
.f1.f.deps config -state disabled
label .f1.f.depsl -text "Depends" -anchor w 
entry .f1.f.libs -textvariable libs -bd 1 
.f1.f.libs insert 0 "nkernel nnebula"
.f1.f.libs config -state disabled
label .f1.f.libsl -text "Libraries" -anchor w

grid .f1.f.deps .f1.f.depsl -sticky ew -pady 1 
grid .f1.f.libs .f1.f.libsl -sticky ew -pady 1
grid columnconfigure .f1.f 0 -weight 1

frame .f2
pack .f2 -side bottom 
button .f2.go -text "Ok" -command {writeall} -width 10 -bd 1
button .f2.ex -text "Cancel" -command {exit} -width 10 -bd 1
grid .f2.go .f2.ex -pady 4 -padx 4

#--------------------------------------------------------------------
#   Write the class files
#--------------------------------------------------------------------
proc writeall {} {
	global classname
	global superclassname
	global directory
	global scriptable
	global author
	global pakenable
	global libs
	global depends
	global target
	
	set nClassName $classname
	set nclassname [string tolower $classname]
	set N_CLASSNAME_H N_[string toupper [string range $classname 1 [string length $classname]]]_H
	set nSuperClassName $superclassname
	set nsuperclassname [string tolower $superclassname]
	set N_SUPERCLASSNAME_H N_[string toupper [string range $superclassname 1 [string length $superclassname]]]_H
	if {$scriptable} {
		set classmacro nNebulaScriptClass
	} else {
		set classmacro nNebulaClass
	}
	
	set subdir $directory
	set superdir [lindex [file split [glob -directory ../inc */$nsuperclassname.h]] 2]
	set year [clock format [clock seconds] -format %Y]
	set date [clock format [clock seconds] -format %d-%b-%Y]
	set user $author
	set npackagename $directory
	set win32libs {}
	set unixlibs {}
	foreach {lib} $libs {
		lappend win32libs $lib.lib
		lappend unixlibs $lib
	}
	set targettype [string tolower [string trim $target]]
	
	source classtemplates.tcl
	
	if [file exists [file join .. src $subdir]] {} {
		file mkdir [file join .. src $subdir]
	}
	if [file exists [file join .. inc $subdir]] {} {
		file mkdir [file join .. inc $subdir]
	}
	set outfile [open [file join .. src $subdir $nclassname\_main.cc] w]
	puts $outfile $main
	close $outfile
	set outfile [open [file join .. inc $subdir $nclassname\.h] w]
	puts $outfile $header
	close $outfile
	set outfile [open [file join .. src $subdir $nclassname\_cmds.cc] w]
	puts $outfile $cmds
	close $outfile
	
	if {$pakenable} {
		set outfile [open [file join .. src $npackagename.pak] w]
		puts $outfile $pakfile
		close $outfile
	}
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------





