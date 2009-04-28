#
#   THE Nebula TEKDEMOS (Main Program)
#
#   tekdemos.tcl
#
#   upd.:   01-Apr-2000 floh    added fog.tcl and burningbanana.tcl
#           18-Mai-2000 bernd   spherical landscape renderer added, new interface
#           18-Feb-2002 wgold   added physics demo
#
#   (C) 1999-2000 RADON LABS Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

global nomads_home
set    demorunning  "none"

#--------------------------------------------------------------------
#
#   Shows the TCL source of the current running Nebula demo.
#   This is pure TK code.
#
#--------------------------------------------------------------------
proc viewsource { datei } {
    if ![winfo exists .code] {
       	toplevel    .code
	    frame       .code.frame
    	pack        .code.frame -expand yes -fill both -padx 1 -pady 1
    	text .code.text -height 20 -wrap word\
    	    -xscrollcommand ".code.xscroll set" \
    	    -yscrollcommand ".code.yscroll set" \
    	    -setgrid 1 -highlightthickness 0 -pady 2 -padx 3
    	scrollbar .code.xscroll -command ".code.text xview" -highlightthickness 0 -orient horizontal
    	scrollbar .code.yscroll -command ".code.text yview" -highlightthickness 0 -orient vertical

    	grid .code.text -in .code.frame -padx 1 -pady 1 -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
    	grid .code.yscroll -in .code.frame -padx 1 -pady 1 -row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
    	grid rowconfig    .code.frame 0 -weight 1 -minsize 0
    	grid columnconfig .code.frame 0 -weight 1 -minsize 0
    }

    wm title     .code "Nebula Source: $datei"
    wm iconname  .code "Nebula Source: $datei"
    wm deiconify .code
    set id [ open $datei "r" ]
    .code.text delete 1.0 end
    .code.text insert 1.0 [read $id]
    .code.text mark set insert 1.0
    close $id
    
return
}

#--------------------------------------------------------------------
#   load_script
#   Laedt ein neues Tekdemo-Script. Vorher wird /usr/scene
#   aufgeraeumt. Nach dem Laden wird die Kamera neu positioniert.
#--------------------------------------------------------------------
proc load_script { script brightness } {
global nomads_home
global demorunning
global bg

    foreach job [after info] { after cancel $job }

    sel /
    delete /usr/scene
    new n3dnode /usr/scene
    /sys/servers/console.unwatch
    /sys/servers/time.starttime
    /sys/servers/shadow.setcastshadows false

    # Eventuelle TK-Subfenster killen
    destroy .r
    destroy .ped2
    update

    # Eventuellen TIMELOCK korrigieren
    /sys/servers/time.lockdeltat 0.0

    /sys/servers/gfx.setclearcolor $brightness $brightness $brightness $brightness
    set bg $brightness

    source $nomads_home/data/tekdemos/$script
    set    demorunning $nomads_home/data/tekdemos/$script

    # Standard-Kameraposition setzen
    orig
}

#--------------------------------------------------------------------
#   create_window
#   Erzeugt das Tk-Frontend.
#--------------------------------------------------------------------
proc create_window { } {
global 3ddevice
global nomads_home
global bg

    set platform [get_platform]

    wm title     .     "Nebula Tekdemos" 
    wm geometry  .     214x150+580+1
    wm resizable .     0 0

    frame .f0 -relief sunken -borderwidth 1
    pack  .f0 -side top -expand 1

    # D3D/OpenGL-Switcher
    menubutton .f0.mb -width 213   -text $3ddevice -textvariable 3ddevice -indicatoron 1 -menu .f0.mb.m1 -relief raised -direction below
    menu .f0.mb.m1 -tearoff 0               

    if {($platform=="win9x") || ($platform=="win2k")} {
        .f0.mb.m1 add radiobutton -label " Direct3D V8 " -variable 3ddevice -command { tod3d8 
            /sys/servers/gfx.setclearcolor $bg $bg $bg $bg }
    }
    .f0.mb.m1 add radiobutton -label " OpenGL " -variable 3ddevice -command { togl 
                /sys/servers/gfx.setclearcolor $bg $bg $bg $bg 
    }
    pack .f0.mb -side top -fill x

            
    button   .f0.ba   -height 1  -text "Current Source"  -command { viewsource $demorunning }
    pack     .f0.ba  -side right -fill x -expand 1

    button   .f0.bb   -text "Toggle Console"   -command { /sys/servers/console.toggle }
    pack     .f0.bb  -side right -fill x -expand 1

    frame     .f1 -bd 2  -relief sunken 
    pack      .f1 -expand yes -fill both 

    label     .f1.label -text "Select Demo:"
    pack      .f1.label -side left -expand yes -fill x 

    # Demo-Selector
    set demo "Physics Demo"
    menubutton .f1.mb -width 213   -text $demo -textvariable demo -indicatoron 1 -menu .f1.mb.m1 -relief raised -direction below
    menu .f1.mb.m1 -tearoff 0    

    .f1.mb.m1 add radiobutton -label "Mesh Interplation"       -variable demo -command { load_script meshanim.tcl    0.5 }
    .f1.mb.m1 add radiobutton -label "Mesh Mixing"             -variable demo -command { load_script meshmix.tcl     0.5 }
    .f1.mb.m1 add radiobutton -label "Skin Deformation"        -variable demo -command { load_script boneanim.tcl    0.5 }
    .f1.mb.m1 add radiobutton -label "Hierarchical Animation"  -variable demo -command { load_script newplanets.tcl  0   }
#    .f1.mb.m1 add radiobutton -label "Animation Blending"     -variable demo -command { load_script weightdemo.tcl  0.5 }
#    .f1.mb.m1 add radiobutton -label "MipMapping"             -variable demo -command { load_script mipmap.tcl      0.5 }
#    .f1.mb.m1 add radiobutton -label "Material Ed (old)"      -variable demo -command { load_script materialed.tcl  0   }
    .f1.mb.m1 add radiobutton -label "Animated Light"          -variable demo -command { load_script lightanim.tcl   0.2 }
    .f1.mb.m1 add radiobutton -label "Flip Flop (Texture)"               -variable demo -command { load_script flipflop.tcl    0.5 }
    .f1.mb.m1 add radiobutton -label "Shaders"                 -variable demo -command { load_script shadernode.tcl  0   }
    .f1.mb.m1 add radiobutton -label "Projected Textures"      -variable demo -command { load_script projectedsmoke.tcl  0   }
    .f1.mb.m1 add radiobutton -label "Static Mesh Emitter"     -variable demo -command { load_script staticmeshemitter.tcl  0   }
    .f1.mb.m1 add radiobutton -label "Alpha Test"              -variable demo -command { load_script alphatest.tcl  0   }
    .f1.mb.m1 add radiobutton -label "Shadows (needs 32 bit desktop!)"  -variable demo -command { load_script shadow.tcl 0
                                                                                                  /usr/camera.tz 50
                                                                                                }
    .f1.mb.m1 add radiobutton -label "Billboards + Sprites"    -variable demo -command { load_script billboards.tcl  0 
                                                                                         /usr/lookat.txyz   0    0    0
                                                                                         /usr/lookat.rxyz  -5   25   0
                                                                                         /usr/camera.tz     25
                                                                                       }
    .f1.mb.m1 add radiobutton -label "Particle Editor"          -variable demo -command { load_script particle.tcl  0.0 
                                                                                         /usr/lookat.txyz   0   0   0
                                                                                         /usr/lookat.rxyz -45   0   0
                                                                                         /usr/camera.tz    10
                                                                                       }
    .f1.mb.m1 add radiobutton -label "Exponential Fog"          -variable demo -command { load_script fog.tcl  0.0 
                                                                                         /usr/lookat.txyz   0   5   0
                                                                                         /usr/lookat.rxyz   0   0   0
                                                                                         /usr/camera.tz     0
                                                                                       }
    .f1.mb.m1 add radiobutton -label "Mesh Particle Emitter"    -variable demo -command { load_script meshemitter.tcl  0.0
                                                                                         /usr/lookat.txyz   0   0   0
                                                                                         /usr/lookat.rxyz   0   0   0
                                                                                         /usr/camera.tz     5
                                                                                       }
    .f1.mb.m1 add radiobutton -label "Spherical Terrain"        -variable demo -command { load_script sphericalTerrain.tcl  0.0
                                                                                         /usr/lookat.txyz   0   5   0
                                                                                         /usr/lookat.rxyz   0   0   0
                                                                                         /usr/camera.tz     20
                                                                                       }
    .f1.mb.m1 add radiobutton -label "Flat Terrain"        -variable demo -command { load_script flatTerrain.tcl  0.0
                                                                                         /usr/lookat.txyz   0   5   0
                                                                                         /usr/lookat.rxyz   0   0   0
                                                                                         /usr/camera.tz     20
                                                                                       }
    .f1.mb.m1 add radiobutton -label "Physics Demo"        -variable demo -command { load_script nodephysics.tcl  0.5
                                                                                         /usr/lookat.txyz   0   50   0
                                                                                         /usr/lookat.rxyz   0   0   0
                                                                                         /usr/camera.tz     20
                                                                                       }
    pack .f1.mb -side left -fill x
}

#--------------------------------------------------------------------
#   main
#   HIER GEHTS LOS
#--------------------------------------------------------------------

if  [ catch { set test $3ddevice } err ] {
    set 3ddevice " OpenGL "
}    

# NomadsHome setzen
set     nomads_home    [/sys/share/assigns/home.gets]
set     home           $nomads_home

# Libs laden
source $nomads_home/bin/nsh.tcl
source $nomads_home/bin/observerlib2.tcl 
source $nomads_home/bin/loadtk.tcl

cd $nomads_home/data/tekdemos

# observer-Objekt initialisieren
init_observer

# Window erzeugen
create_window

# Default Demo
load_script nodephysics.tcl 0.5
/usr/lookat.txyz   0   50   0
/usr/lookat.rxyz   0   0   0
/usr/camera.tz     20

# und los....
/observer.setsleep 0.02
/observer.setgrid  false
/observer.start

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
