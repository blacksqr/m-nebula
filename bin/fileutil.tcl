#--------------------------------------------------------------------
#   fileutil.tcl
#   Diverse Fileutils zum Compilieren, um von den Unix-Tools
#   fuer Windows frei zu sein.
#
#   tclsh fileutil.tcl rm [pattern]
#   tclsh fileutil.tcl cp [from] [to]
#   tclsh fileutil.tcl echo [file] [text]
#
#   (C) 1999 A.Weissflog
#--------------------------------------------------------------------

proc fileutil_rm {pat} {
    set files [glob -nocomplain -- $pat]
    for {set i 0} {$i < [llength $files]} {incr i} {
        set f [lindex $files $i]
        puts "rm $f"
        file delete $f
    }
}

proc fileutil_cp {from to} {
    puts "cp $from $to"
    file copy -force -- $from $to
}

proc fileutil_echo {file text} {
    set cid [open $file w]
    puts $cid $text
    close $cid
}

#--------------------------------------------------------------------
#   main
#--------------------------------------------------------------------
set cmd [lindex $argv 0]
switch $cmd {
    rm { fileutil_rm [lindex $argv 1] }
    cp { fileutil_cp [lindex $argv 1] [lindex $argv 2] }
    echo { fileutil_echo [lindex $argv 1] [lindex $argv 2] }
    default { puts "unknown fileutil command: $cmd" }
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------