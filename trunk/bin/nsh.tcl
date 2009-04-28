#!/bin/sh
#
#	/nomads/bin/nsh.tcl
#
#	Stellt verschiedene Standard-Tools und Spracherweiterungen 
#	für die NebulaSHell und für die Standard-Nebula-Scripts
#	zur Verfügung.
#	Mit nsh_help bekommt eine Übersicht über die implementierten Befehle.
#
#   *** ACHTUNG !!! Alle DEMO-spezifischen Befehle habe ich 
#   nach /nomads/scripts/commands.tcl
#   verlegt. (Flieger, Samml, etc.)
#
#
#	created:	Okt-1998
#	upd.:		25-May-99
#               21-Jun-99   floh    "<error>" durch "null" ersetzt
#               22-Jun-99   bernd   Help gekillt
#               15-Feb-00   floh    + new proc get_platform()
#
#	(c) 1998-1999 Bernd Beyreuther
#



# set         home                $env(NOMADS_HOME)
# source      $home/bin/assigns.tcl


#--------------------------------------------------------------------
#   get_platform()
#   Detect platform and return one of the following strings:
#   win9x   -> Windows 95 and 98
#   winnt   -> Windows NT 4
#   win2k   -> Windows 2000
#   unix    -> Unix variant
#   unknown -> unknown
#--------------------------------------------------------------------
proc get_platform {} {
    global tcl_platform
    set p "unknown"
    if {$tcl_platform(platform) == "windows"} {
        if {$tcl_platform(os) == "Windows NT"} {
            if {$tcl_platform(osVersion) >= 5.0} {
                set p "win2k"
                puts "Windows 2000 detected"
            } else {
                set p "winnt"
                puts "Windows NT detected"
            }
        } else {
            set p "win9x"
            puts "Windows9x detected"
        }
    } elseif {$tcl_platform(platform) == "unix"} {
        if {$tcl_platform(os) == "Darwin"} {
            set p "macosx"
            puts "Mac OS X detected"
        } else {
            set p "unix"
            puts "Unix detected"
        }
    } else {
        set p "unknown"
        puts "Unknown platform detected"
    }
    return $p
}

# -------------------------------------------------------------------
proc st {} {
	sel /usr/scene/std
return
}

# -------------------------------------------------------------------
proc sc {} {
	sel /usr/scene
return
}

# -------------------------------------------------------------------
proc roty {} {
	catch { delete roty  } err
	catch { delete roty2 } err
	new nipol roty
		sel roty
		.connect ry
		.addkey1f 0 0
		.addkey1f 3 360
	sel ..
return
}

# -------------------------------------------------------------------
proc roty2 {} {
	catch { delete roty  } err
	catch { delete roty2 } err
	new nipol roty2
		sel roty2
		.connect ry
		.addkey1f 0 360
		.addkey1f 3 0
	sel ..
return
}

# -------------------------------------------------------------------
proc rotx {} {
	new nipol rotx
		sel rotx
		.connect rx
		.addkey1f 0 0
		.addkey1f 5 360
	sel ..
return
}

# -------------------------------------------------------------------
proc rotz {} {
	new nipol rotz
		sel rotz
		.connect rz
		.addkey1f 0 0
		.addkey1f 5 360
	sel ..
return
}

# -------------------------------------------------------------------
proc stop {} {
	catch { delete rotx  } err
	catch { delete roty  } err
	catch { delete roty2 } err
	catch { delete rotz  } err
	.rxyz 0 0 0
return
}

# -------------------------------------------------------------------
proc gfx {} {
	connect gfxserv
	sel /
return
}

# -------------------------------------------------------------------
proc l {} {
	sel /
	sel usr
	sel scene
	sel dlight
return
}

# -------------------------------------------------------------------
proc ndir {} {
    set cur_dir [psel]
    set result ""
    set x [.gethead]
    while {$x != "null"} {
        sel $x
        lappend result [.getname]
        set x [.getsucc]
    }
    sel $cur_dir
return $result
}

# -------------------------------------------------------------------
proc new_objects {class name number} {
    for {set i 0} {$i < $number} {incr i 1} {
        new $class $name$i
    }
return
}

# -------------------------------------------------------------------
proc ndir2 {} {
	set dirinhalt [ndir]
	foreach eintrag $dirinhalt {
		puts stdout $eintrag
	}
return
}

# -------------------------------------------------------------------
proc tree {} {
    set cur_dir [psel]
    set result ""
    set x [.gethead]
    while {$x != "null"} {
        sel $x
        puts stdout  [.getfullname]
	tree
        set x [.getsucc]
    }
    sel $cur_dir
return
}

# -------------------------------------------------------------------
proc licht {} {
global home
    set current [getfullname]
    sel /usr/scene/dlight/light1
    if {[ .getactive ] == "true"} {
        puts stdout "licht aus"
        .setactive false
        sel /usr/scene/dlight/light2
        .setactive false
    } else {
        puts stdout "- LICHT AN -"
        .setactive true
        sel /usr/scene/dlight/light2
        .setactive true
    }
    sel $current
return
}

# -------------------------------------------------------------------
proc  zeit {} {
    set current [getfullname]
    sel /sys/servers/time

    # ask two times with a break (!), because otherwise gettime will
    # return same value on fast machines
    set time1 [.gettime]
    after 10
    set time2 [.gettime]
    if  {$time1 == $time2} {
        .starttime
    } else {
        .stoptime
    }
    sel $current
return
}

# -------------------------------------------------------------------
proc viewlod {} {
    /sys/servers/console.watch sg*
return    
}

# -------------------------------------------------------------------
proc init_loddisp {} {
    new nenv       /sys/var/sg_lod
    refresh_loddisp
return    
}

# -------------------------------------------------------------------
#   13-Nov-00   floh    standard-vbuf Name jetzt 'mesh'
#
# -------------------------------------------------------------------
proc refresh_loddisp {} {
    set obj ""

    if {[exists /usr/scene/std]} {
#        /sys/var/sg_lod.setf [ /usr/scene/std.getcurrentlod ]
#        after 100 refresh_loddisp
    }
}
