#--------------------------------------------------------------------
#   particleed2.tcl
#   ===============
#   New and improved!
#
#   particleed2 works on a Nebula hierarchie like this:
#
#   new n3dnode psys
#       new nspriterender sr
#           new npointemitter pe
#       new ntexarraynode tex
#       new nshadernode shd
#
#   11-Jan-01   floh    created
#
#   (C) 2001 RadonLabs GmbH -- A.Weissflog
#--------------------------------------------------------------------

global nomads_home
global home
source $home/bin/keyframeed.tcl

#--------------------------------------------------------------------
#   Global Variables:
#
#   pe2_showgrid        - turn on/off grid
#   pe2_showtriangles   - turn on/off triangle count
#   pe2_freezetime      - stop time
#
#   pe2_start_time      - start emitting on local timeline 
#   pe2_stop_time       - stop emitting on local timeline
#   pe2_repeat_time     - repeat after on local timeline
#   pe2_lifetime        - lifetime of particel
#   pe2_freq            - emitter frequency
#   pe2_speed           - emission speed
#   pe2_accel_x         - global acceleration vector
#   pe2_accel_y
#   pe2_accel_z
#   pe2_innercone       - inner emitter cone limit
#   pe2_outercone       - outer emitter cone limit
#   pe2_spin            - particle spin
#   pe2_spinaccel       - particle spin acceleration
#   pe2_numkeys         - number of keys in particle keyframe array
#   pe2_keys            - particle keyframe array
#
#       pe2_keys(x,size)
#       pe2_keys(x,bounce)
#       pe2_keys(x,red)
#       pe2_keys(x,green)
#       pe2_keys(x,blue)
#       pe2_keys(x,alpha)
#--------------------------------------------------------------------

set pe2_showgrid      0
set pe2_showtriangles 0
set pe2_freezetime    0
set pe2_lockfeedback  0

#--------------------------------------------------------------------
#   proc update_variables_from_psys
#
#   Update global variables from default particle system
#   hierarchie as described by proc new_psys
#   
#   11-Jan-01   floh    created
#--------------------------------------------------------------------
proc update_variables_from_psys {} {
global pe2_start_time
global pe2_stop_time
global pe2_repeat_time
global pe2_lifetime
global pe2_freq
global pe2_speed
global pe2_accel_x
global pe2_accel_y
global pe2_accel_z
global pe2_innercone
global pe2_outercone
global pe2_spin
global pe2_spinaccel
global pe2_numkeys
global pe2_keys
global pe2_lockfeedback

    # prevent gui from updating object
    set pe2_lockfeedback 1

    set cwd [psel]
    sel /usr/scene
    
    set times [psys/sr/pe.gettimes]
    set pe2_start_time  [lindex $times 0]
    set pe2_stop_time   [lindex $times 1]
    set pe2_repeat_time [lindex $times 2]
    set pe2_lifetime    [psys/sr/pe.getlifetime]
    set pe2_freq        [psys/sr/pe.getfreq]
    set pe2_speed       [psys/sr/pe.getspeed]
    
    set accel [psys/sr/pe.getaccel]
    set pe2_accel_x [lindex $accel 0]
    set pe2_accel_y [lindex $accel 1]
    set pe2_accel_z [lindex $accel 2]
    
    # get sprite render object attributes
    set pe2_innercone [psys/sr/pe.getinnercone]
    set pe2_outercone [psys/sr/pe.getoutercone]
    
    set pe2_spin      [psys/sr.getspin]
    set pe2_spinaccel [psys/sr.getspinaccel]
    set pe2_numkeys   [psys/sr.getnumkeys]
    
    for {set i 0} {$i < $pe2_numkeys} {incr i} {
        set curkey [psys/sr.getkey $i]
        set pe2_keys($i,size)   [lindex $curkey 0]
        set pe2_keys($i,bounce) [lindex $curkey 1]
        set pe2_keys($i,red)    [lindex $curkey 2]
        set pe2_keys($i,green)  [lindex $curkey 3]
        set pe2_keys($i,blue)   [lindex $curkey 4]
        set pe2_keys($i,alpha)  [lindex $curkey 5]
    }

    # update the keyframe editor
    update_keyeditor_keys

    # allow gui to update object
    set pe2_lockfeedback 0
}               

#--------------------------------------------------------------------
#   proc new_psys
#
#   Create a new default particle system (complete with 3dnode, shader 
#   and texture) under /usr/scene/psys, and update global
#   variables.
#
#   11-Jan-01   floh    created
#--------------------------------------------------------------------
proc new_psys {} {

    set cwd [psel]
    sel /usr/scene
    
    # if exists, delete previous particle system
    if {[exists psys]} {
        delete psys
    }
    
    # create new default particle system
    new n3dnode psys
        sel psys
        
        # the particle system object itself   
        new nspriterender sr
            sel sr
            new npointemitter pe
                sel pe
                .settimes 0 0 0
                .setlifetime 1
                .setfreq 100
                .setspeed 5
                .setaccel 0 0 0
                .setinnercone 0
                .setoutercone 0
            sel ..
            .setemitter pe
            .setspin 0
            .setspinaccel 0
            .beginkeys 6
            .setkey 0 0.5 0.0 1.0 1.0 1.0 1.0
            .setkey 1 0.5 0.0 1.0 1.0 1.0 1.0
            .setkey 2 0.5 0.0 1.0 1.0 1.0 1.0
            .setkey 3 0.5 0.0 1.0 1.0 1.0 1.0
            .setkey 4 0.5 0.0 1.0 1.0 1.0 1.0
            .setkey 5 0.5 0.0 1.0 1.0 1.0 1.0
            .endkeys
        sel ..
        
        # the default shader (additive transparent)
        new nshadernode shd
            sel shd
#            .setminlod 0.000000
            .setrenderpri 0
            .setnumstages 1
            .setcolorop 0 "mul tex prev"
            .setalphaop 0 "nop"
            .setconst 0 0.000000 0.000000 0.000000 0.000000
            .begintunit 0
            .setaddress "wrap" "wrap"
            .setminmagfilter "linear_mipmap_nearest" "linear"
            .settexcoordsrc "uv0"
            .setenabletransform false
            .txyz 0.000000 0.000000 0.000000
            .rxyz 0.000000 0.000000 0.000000
            .sxyz 1.000000 1.000000 1.000000
            .endtunit
            .setdiffuse 0.000000 0.000000 0.000000 0.000000
            .setemissive 0.000000 0.000000 0.000000 0.000000
            .setambient 0.000000 0.000000 0.000000 0.000000
            .setlightenable false
            .setalphaenable true
            .setzwriteenable false
            .setfogenable false
            .setalphablend "one" "one"
            .setzfunc "lessequal"
            .setcullmode "ccw"
            .setcolormaterial "material"
        sel ..
        
        # the default texture
        new ntexarraynode tex
            sel tex
        sel ..
    sel ..
    
    # update the global variables
    update_variables_from_psys
    
    # update the keyframe editor
    ke_refresh

    sel $cwd
}

#--------------------------------------------------------------------
#   proc load_psys
#   Load particle system defined by filename.
#   19-Jan-01   floh    created
#--------------------------------------------------------------------
proc load_psys {file} {

    set cwd [psel]

    # delete current particle system if exists
    sel /usr/scene
    if {[exists psys]} {
        delete psys
    }
    get $file
    
    # rename particle system object to psys
    set psysname_with_ext [file tail $file]
    set psysname [file rootname $psysname_with_ext]
    
    /usr/scene/$psysname.setname psys

    # update the global variables
    update_variables_from_psys

    sel $cwd
}

#--------------------------------------------------------------------
#   proc save_psys_dialog
#   Create file save dialog and save current particle system.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
proc save_psys_dialog {} {
    
    # create a file save dialog
    set types {
        {"Nebula particle system" .psys}
    }
    set file [tk_getSaveFile -filetypes $types -initialfile unnamed -defaultextension .n -title "Save particle system"]

    if {$file != ""} {
        /usr/scene/psys.saveas $file
        
        # Nebula appends a .n, thus we rename the file to *.psys
        file rename -force $file.n $file
        
    } else {
        puts "Cancelled"
    }
}

#--------------------------------------------------------------------
#   proc load_psys_dialog
#   Create file open dialog and load selected particle system.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
proc load_psys_dialog {} {
    
    # create a file load dialog
    set types {
        {"Nebula particle system" .psys}
    }
    set file [tk_getOpenFile -filetypes $types -title "Load particle system"]
    
    if {$file != ""} {

        # load particle system
        load_psys $file
        
        # update the keyframe editor
        ke_refresh
    } else {
        puts "Cancelled"
    }
}

#--------------------------------------------------------------------
#   proc load_shader
#   Load a shader serialized object file.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
proc load_shader {} {

    # create a file load dialog
    set types {
        {"Nebula shader" .shd}
    }
    set file [tk_getOpenFile -filetypes $types -title "Load shader"]

    if {$file != ""} {
        set cwd [psel]
        sel /usr/scene/psys
        
        delete shd
        get $file
        
        # rename shader to "shd"
        for {set o [/usr/scene/psys.gethead]} {$o != "null"} {set o [$o.getsucc]} {
            if {[$o.isa nshadernode]} {
                $o.setname shd
                break
            }
        }
        sel $cwd

    } else {
        puts "Cancelled"
    }
}

#--------------------------------------------------------------------
#   proc load_tex
#   Define a texture image for the ntexarraynode object.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
proc load_tex {stage} {
    
    # create a file load dialog
    set types {
        {"Bmp file" .bmp}
    }
    set file [tk_getOpenFile -filetypes $types -title "Load texture map for stage $stage"]
    
    if {$file != ""} {
        set curtexalpha [/usr/scene/psys/tex.gettexture $stage]
        set curalpha [lindex $curtexalpha 1]
        /usr/scene/psys/tex.settexture $stage $file $curalpha
    } else {
        puts "Cancelled"
    }
}

#--------------------------------------------------------------------
#   proc load_alpha
#   Define a texture image for the ntexarraynode object.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
proc load_alpha {stage} {
    
    # create a file load dialog
    set types {
        {"Bmp file" .bmp}
    }
    set file [tk_getOpenFile -filetypes $types -title "Load alpha map for stage $stage"]

    if {$file != ""} {
        set curtexalpha [/usr/scene/psys/tex.gettexture $stage]
        set curtex [lindex $curtexalpha 0]
        /usr/scene/psys/tex.settexture $stage $curtex $file
    } else {
        puts "Cancelled"
    }
}

#--------------------------------------------------------------------
#   proc pe2_updkeys_callback
#   Called by keyframe editor whenever a key value changes.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_updkeys_callback {channel key value} {
global pe2_keys

    if {$channel == 0} {
        # red channel
        set pe2_keys($key,red) $value
    } elseif {$channel == 1} {
        # green channel
        set pe2_keys($key,green) $value
    } elseif {$channel == 2} {
        # blue channel
        set pe2_keys($key,blue) $value
    } elseif {$channel == 3} {
        # alpha channel
        set pe2_keys($key,alpha) $value
    } elseif {$channel == 4} {
        # bounce channel
        set pe2_keys($key,bounce) $value
    } elseif {$channel == 5} {
        # size channel
        set pe2_keys($key,size) $value
    }

    # set the new key in the sprite renderer
    /usr/scene/psys/sr.setkey $key \
                              $pe2_keys($key,size) \
                              $pe2_keys($key,bounce) \
                              $pe2_keys($key,red) \
                              $pe2_keys($key,green) \
                              $pe2_keys($key,blue) \
                              $pe2_keys($key,alpha)
}

#--------------------------------------------------------------------
#   proc pe2_update_times
#   Validate time variables and update emitter object.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_update_times {} {
global pe2_start_time
global pe2_stop_time
global pe2_repeat_time

    if {$pe2_stop_time < $pe2_start_time} {
        set pe2_stop_time $pe2_start_time
    }
    if {$pe2_repeat_time < $pe2_stop_time} {
        set pe2_repeat_time $pe2_stop_time
    }
    
    /usr/scene/psys/sr/pe.settimes $pe2_start_time $pe2_stop_time $pe2_repeat_time
}

#--------------------------------------------------------------------
#   proc pe2_reset_times
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_reset_times {} {
global pe2_start_time
global pe2_stop_time
global pe2_repeat_time

    set pe2_start_time 0.0
    set pe2_stop_time  0.0
    set pe2_repeat_time 0.0
    pe2_update_times
}

#--------------------------------------------------------------------
#   proc pe2_update_lifetime_freq
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_update_lifetime_freq {} {
global pe2_lifetime
global pe2_freq

    /usr/scene/psys/sr/pe.setlifetime $pe2_lifetime
    /usr/scene/psys/sr/pe.setfreq $pe2_freq
}

#--------------------------------------------------------------------
#   proc pe2_reset_lifetime_freq
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_reset_lifetime_freq {} {
global pe2_lifetime
global pe2_freq
    set pe2_lifetime 1.0
    set pe2_freq     100.0
    
    pe2_update_lifetime_freq
}

#--------------------------------------------------------------------
#   proc pe2_update_accel
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_update_accel {} {
global pe2_accel_x
global pe2_accel_y
global pe2_accel_z

    /usr/scene/psys/sr/pe.setaccel $pe2_accel_x $pe2_accel_y $pe2_accel_z
}

#--------------------------------------------------------------------
#   proc pe2_reset_accel
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_reset_accel {} {
global pe2_accel_x
global pe2_accel_y
global pe2_accel_z

    set pe2_accel_x 0.0
    set pe2_accel_y 0.0
    set pe2_accel_z 0.0
    pe2_update_accel
}

#--------------------------------------------------------------------
#   proc pe2_update_cones
#   Update inner and outer cone of object.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_update_cones {} {
global pe2_innercone
global pe2_outercone

    if {$pe2_innercone < 0.0} {
        set pe2_innercone 0.0
    } elseif {$pe2_innercone > 180.0} {
        set pe2_innercone 180.0
    }
    if {$pe2_outercone < 0.0} {
        set pe2_outercone 0.0
    } elseif {$pe2_outercone > 180.0} {
        set pe2_outercone 180.0
    }
    /usr/scene/psys/sr/pe.setinnercone $pe2_innercone
    /usr/scene/psys/sr/pe.setoutercone $pe2_outercone
}

#--------------------------------------------------------------------
#   proc pe2_reset_cones
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_reset_cones {} {
global pe2_innercone
global pe2_outercone

    set pe2_innercone 0.0
    set pe2_outercone 0.0
    pe2_update_cones
}

#--------------------------------------------------------------------
#   proc pe2_update_spin
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_update_spin {} {
global pe2_spin
global pe2_spinaccel

    /usr/scene/psys/sr.setspin $pe2_spin
    /usr/scene/psys/sr.setspinaccel $pe2_spinaccel
}

#--------------------------------------------------------------------
#   proc pe2_reset_spin
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_reset_spin {} {
global pe2_spin
global pe2_spinaccel

    set pe2_spin 0.0
    set pe2_spinaccel 0.0
    pe2_update_spin
}

#--------------------------------------------------------------------
#   proc pe2_update_speed
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_update_speed {} {
global pe2_speed

    /usr/scene/psys/sr/pe.setspeed $pe2_speed
}

#--------------------------------------------------------------------
#   proc pe2_toggletime
#   Freeze/unfreeze time based on pe2_freezetime state.
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_toggletime {} {
global pe2_freezetime

    if {$pe2_freezetime} {
        /sys/servers/time.stoptime
    } else {
        /sys/servers/time.starttime
    }
}

#--------------------------------------------------------------------
#   proc pe2_togglegrid
#   Toggle grid based on pe2_showgrid state.
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_togglegrid {} {
global pe2_showgrid

    if {$pe2_showgrid} {
        /observer.setgrid true
    } else {
        /observer.setgrid false
    }
}

#--------------------------------------------------------------------
#   proc pe2_toggletriangles
#   Toggle triangle counter based on pe2_showtriangles
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_toggletriangles {} {
global pe2_showtriangles

    if {$pe2_showtriangles} {
        /sys/servers/console.watch gfx_triangles
    } else {
        /sys/servers/console.unwatch
    }
}

#====================================================================
#   GUI STUFF
#====================================================================

#--------------------------------------------------------------------
#   proc validate_cmd [newcontent] [updcommand]
#
#   Called by the entry widget for input validation and to 
#   call back update routines which update the Nebula objects.
#
#   Strategy:
#   - reject new content if it contains illegal characters
#   - call provided update routine
#
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc validate_cmd {newcontent updcallback} {
global pe2_lockfeedback
    # do nothing if gui may not update object
    if {$pe2_lockfeedback} {
        return 1
    }

    # see if it is a valid floating point number
    
    if {[string is double $newcontent]} {
        $updcallback
        return 1
    } else {
        puts "INVALID INPUT"
        return 1
    }
}

#--------------------------------------------------------------------
#   proc pe2_focus_next
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_focus_next {cur} {
    
    focus [tk_focusNext $cur]
}

#--------------------------------------------------------------------
#   proc pe2_setup_focus
#   Setup focus handling for entry widgets when pressing return.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_setup_focus {entry_list} {
    
    for {set i 0} {$i < [llength $entry_list]} {incr i} {
        set cur [lindex $entry_list $i]
        bind $cur <Return> "pe2_focus_next $cur"
    }
}

#--------------------------------------------------------------------
#   proc pe2_enable_texture_buttons
#   Enable/disable texture loader buttons based on
#   how many stages the current shader has.
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_enable_texture_buttons {w} {
    
    set numstages [/usr/scene/psys/shd.getnumstages]

    $w.ls.r2.tex0 configure -state normal
    $w.ls.r2.tex1 configure -state normal
    $w.ls.r2.tex2 configure -state normal
    $w.ls.r2.tex3 configure -state normal
    if {$numstages < 4} {
        $w.ls.r2.tex3 configure -state disabled
    }
    if {$numstages < 3} {
        $w.ls.r2.tex2 configure -state disabled
    }
    if {$numstages < 2} {
        $w.ls.r2.tex1 configure -state disabled
    }
    if {$numstages < 1} {
        $w.ls.r2.tex0 configure -state disabled
    }
        
    $w.ls.r3.alpha0 configure -state normal
    $w.ls.r3.alpha1 configure -state normal
    $w.ls.r3.alpha2 configure -state normal
    $w.ls.r3.alpha3 configure -state normal
    if {$numstages < 4} {
        $w.ls.r3.alpha3 configure -state disabled
    }
    if {$numstages < 3} {
        $w.ls.r3.alpha2 configure -state disabled
    }
    if {$numstages < 2} {
        $w.ls.r3.alpha1 configure -state disabled
    }
    if {$numstages < 1} {
        $w.ls.r3.alpha0 configure -state disabled
    }
}

#--------------------------------------------------------------------
#   proc gen_loadsave_frame
#
#   Generate frame object with buttons for 
#
#   New
#   Save
#   Load
#   Load Shader
#   Load Texture 1..4
#   Load Alpha   1..4
#
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
proc gen_loadsave_frame {w} {

    frame $w.ls -relief groove -borderwidth 2
    pack $w.ls -side bottom -fill x

    # row1: New Save Load Shader
    frame $w.ls.r1
    pack $w.ls.r1 -side top -fill x -pady 2
    button $w.ls.r1.new    -text "New"          -command "new_psys"
    button $w.ls.r1.save   -text "Save..."      -command "save_psys_dialog"
    button $w.ls.r1.load   -text "Load..."      -command "load_psys_dialog"
    button $w.ls.r1.shader -text "Shader..."    -command "load_shader"
    pack $w.ls.r1.new $w.ls.r1.save $w.ls.r1.load $w.ls.r1.shader -side left -expand 1 -padx 2 -fill x

    # row2: Tex0 Tex1 Tex2 Tex3
    frame $w.ls.r2
    pack $w.ls.r2 -side top -fill x -pady 2
    button $w.ls.r2.tex0 -text "Tex0..."        -command "load_tex 0"
    button $w.ls.r2.tex1 -text "Tex1..."        -command "load_tex 1"
    button $w.ls.r2.tex2 -text "Tex2..."        -command "load_tex 2"
    button $w.ls.r2.tex3 -text "Tex3..."        -command "load_tex 3"
    pack $w.ls.r2.tex0 $w.ls.r2.tex1 $w.ls.r2.tex2 $w.ls.r2.tex3 -side left -expand 1 -padx 2 -fill x

    # row3: Alpha0 Alpha1 Alpha2 Alpha3
    frame $w.ls.r3
    pack $w.ls.r3 -side top -fill x -pady 2
    button $w.ls.r3.alpha0 -text "Alpha0..."    -command "load_alpha 0"
    button $w.ls.r3.alpha1 -text "Alpha1..."    -command "load_alpha 1"
    button $w.ls.r3.alpha2 -text "Alpha2..."    -command "load_alpha 2"
    button $w.ls.r3.alpha3 -text "Alpha3..."    -command "load_alpha 3"
    pack $w.ls.r3.alpha0 $w.ls.r3.alpha1 $w.ls.r3.alpha2 $w.ls.r3.alpha3 -side left -expand 1 -padx 2 -fill x

    pe2_enable_texture_buttons $w
}

#--------------------------------------------------------------------
#   proc gen_control_frame
#   
#   Create a frame with buttons and sliders for emitter
#   and some renderer attributes.
#
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
proc gen_control_frame {w} {
global pe2_start_time
global pe2_stop_time
global pe2_repeat_time
global pe2_lifetime
global pe2_freq
global pe2_speed
global pe2_accel_x
global pe2_accel_y
global pe2_accel_z
global pe2_innercone
global pe2_outercone
global pe2_spin
global pe2_spinaccel
global pe2_freezetime
global pe2_showgrid
global pe2_showtriangles

    frame $w.c -relief groove -borderwidth 2
    pack $w.c -side top -fill x
        
    # freezetime, showgrid, showtriangles
    set f $w.c.f0
    frame $f
    pack $f -side top -fill x -pady 1
    
    checkbutton $f.cb1 -text "freeze"   -indicatoron 1 -variable pe2_freezetime    -command pe2_toggletime
    checkbutton $f.cb2 -text "grid"     -indicatoron 1 -variable pe2_showgrid      -command pe2_togglegrid
    checkbutton $f.cb3 -text "tricount" -indicatoron 1 -variable pe2_showtriangles -command pe2_toggletriangles
    pack $f.cb1 $f.cb2 $f.cb3 -side left -padx 2

    # start/stop/repeat times
    set f $w.c.f1
    frame $f
    pack $f -side top -fill x -pady 1
    
    label $f.l -text "start/stop/repeat"
    pack $f.l -side left -padx 2
    
    entry $f.e1 -width 8 -textvariable pe2_start_time   -validate focusout -vcmd "validate_cmd %P pe2_update_times"
    entry $f.e2 -width 8 -textvariable pe2_stop_time    -validate focusout -vcmd "validate_cmd %P pe2_update_times"
    entry $f.e3 -width 8 -textvariable pe2_repeat_time  -validate focusout -vcmd "validate_cmd %P pe2_update_times"
    button $f.b1 -text "Reset" -command pe2_reset_times
    pack $f.b1 $f.e3 $f.e2 $f.e1 -side right -padx 2
    pe2_setup_focus "$f.e1 $f.e2 $f.e3"
        
    # lifetime/freq/speed
    set f $w.c.f2
    frame $f
    pack $f -side top -fill x -pady 1
    
    label $f.l -text "lifetime/freq/speed"
    pack $f.l -side left -padx 2
    
    entry $f.e1 -width 8 -textvariable pe2_lifetime     -validate focusout -vcmd "validate_cmd %P pe2_update_lifetime_freq"
    entry $f.e2 -width 8 -textvariable pe2_freq         -validate focusout -vcmd "validate_cmd %P pe2_update_lifetime_freq"
    entry $f.e3 -width 8 -textvariable pe2_speed        -validate focusout -vcmd "validate_cmd %P pe2_update_speed"
    button $f.b1 -text "Reset" -command "pe2_reset_lifetime_freq"
    pack $f.b1 $f.e3 $f.e2 $f.e1 -side right -padx 2
    pe2_setup_focus "$f.e1 $f.e2 $f.e3"

    # accel
    set f $w.c.f3
    frame $f
    pack $f -side top -fill x -pady 1
    
    label $f.l -text "global accel"
    pack $f.l -side left -padx 2
    
    entry $f.e1 -width 8 -textvariable pe2_accel_x -validate focusout -vcmd "validate_cmd %P pe2_update_accel"
    entry $f.e2 -width 8 -textvariable pe2_accel_y -validate focusout -vcmd "validate_cmd %P pe2_update_accel"
    entry $f.e3 -width 8 -textvariable pe2_accel_z -validate focusout -vcmd "validate_cmd %P pe2_update_accel"
    button $f.b1 -text "Reset" -command pe2_reset_accel
    pack $f.b1 $f.e3 $f.e2 $f.e1 -side right -padx 2
    pe2_setup_focus "$f.e1 $f.e2 $f.e3"
    
    # inner/outer cone
    set f $w.c.f4
    frame $f
    pack $f -side top -fill x -pady 1
    
    label $f.l -text "inner/outer cone"
    pack $f.l -side left -padx 2
    
    entry $f.e1 -width 8 -textvariable pe2_innercone -validate focusout -vcmd "validate_cmd %P pe2_update_cones"
    entry $f.e2 -width 8 -textvariable pe2_outercone -validate focusout -vcmd "validate_cmd %P pe2_update_cones"
    button $f.b1 -text "Reset" -command pe2_reset_cones
    pack $f.b1 $f.e2 $f.e1 -side right -padx 2
    pe2_setup_focus "$f.e1 $f.e2"
    
    # spin/spin accel
    set f $w.c.f5
    frame $f
    pack $f -side top -fill x -pady 1
    
    label $f.l -text "spin/spin accel"
    pack $f.l -side left -padx 2
    
    entry $f.e1 -width 8 -textvariable pe2_spin      -validate focusout -vcmd "validate_cmd %P pe2_update_spin"
    entry $f.e2 -width 8 -textvariable pe2_spinaccel -validate focusout -vcmd "validate_cmd %P pe2_update_spin"
    button $f.b1 -text "Reset" -command pe2_reset_spin
    pack $f.b1 $f.e2 $f.e1 -side right -padx 2
    pe2_setup_focus "$f.e1 $f.e2"
}

#--------------------------------------------------------------------
#   proc update_keyeditor_keys
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc update_keyeditor_keys {} {
global ke_num_channels
global ke_chn_array
global pe2_numkeys
global pe2_keys

    # configure the keyframe editor
    set ke_num_channels 6
    
    # red
    set ke_chn_array(0,name)        "red"
    set ke_chn_array(0,color)       "red"
    set ke_chn_array(0,minval)      0.0
    set ke_chn_array(0,maxval)      1.0
    set ke_chn_array(0,callback)    pe2_updkeys_callback
    set ke_chn_array(0,numkeys)     $pe2_numkeys
    for {set i 0} {$i < $pe2_numkeys} {incr i} {
        set ke_chn_array(0,$i) $pe2_keys($i,red)
    }
    
    # green
    set ke_chn_array(1,name)        "green"
    set ke_chn_array(1,color)       "green"
    set ke_chn_array(1,minval)      0.0
    set ke_chn_array(1,maxval)      1.0
    set ke_chn_array(1,callback)    pe2_updkeys_callback
    set ke_chn_array(1,numkeys)     $pe2_numkeys
    for {set i 0} {$i < $pe2_numkeys} {incr i} {
        set ke_chn_array(1,$i) $pe2_keys($i,green)
    }
    
    # blue
    set ke_chn_array(2,name)        "blue"
    set ke_chn_array(2,color)       "blue"
    set ke_chn_array(2,minval)      0.0
    set ke_chn_array(2,maxval)      1.0
    set ke_chn_array(2,callback)    pe2_updkeys_callback
    set ke_chn_array(2,numkeys)     $pe2_numkeys
    for {set i 0} {$i < $pe2_numkeys} {incr i} {
        set ke_chn_array(2,$i) $pe2_keys($i,green)
    }

    # alpha
    set ke_chn_array(3,name)        "alpha"
    set ke_chn_array(3,color)       "DarkGrey"
    set ke_chn_array(3,minval)      0.0
    set ke_chn_array(3,maxval)      1.0
    set ke_chn_array(3,callback)    pe2_updkeys_callback
    set ke_chn_array(3,numkeys)     $pe2_numkeys
    for {set i 0} {$i < $pe2_numkeys} {incr i} {
        set ke_chn_array(3,$i) $pe2_keys($i,alpha)
    }
    
    # bounce
    set ke_chn_array(4,name)        "bounce"
    set ke_chn_array(4,color)       "salmon"
    set ke_chn_array(4,minval)      0.0
    set ke_chn_array(4,maxval)      2.0
    set ke_chn_array(4,callback)    pe2_updkeys_callback
    set ke_chn_array(4,numkeys)     $pe2_numkeys
    for {set i 0} {$i < $pe2_numkeys} {incr i} {
        set ke_chn_array(4,$i) $pe2_keys($i,bounce)
    }
    
    # size
    set ke_chn_array(5,name)        "size"
    set ke_chn_array(5,color)       "turquoise2"
    set ke_chn_array(5,minval)      0.0
    set ke_chn_array(5,maxval)      5.0
    set ke_chn_array(5,callback)    pe2_updkeys_callback
    set ke_chn_array(5,numkeys)     $pe2_numkeys
    for {set i 0} {$i < $pe2_numkeys} {incr i} {
        set ke_chn_array(5,$i) $pe2_keys($i,size)
    }
}

#--------------------------------------------------------------------
#   proc gen_key_editor
#   Generate the keyframe editor.
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
proc gen_key_editor {w} {
global ke_num_channels
global ke_chn_array
global pe2_numkeys
global pe2_keys
    
    frame $w.kf -relief groove -borderwidth 2
    pack $w.kf -side top -fill x

    update_keyeditor_keys

    ke_create $w.kf
}

#--------------------------------------------------------------------
#   proc gen_window
#   Generate the entire window.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
proc gen_window {w x y} {

    toplevel $w
    wm title $w "Particle Ed2"
    wm resizable $w 0 0
    wm geometry $w +$x+$y

    frame $w.all
    pack $w.all -side top

    gen_control_frame  $w.all
    gen_key_editor     $w.all
    gen_loadsave_frame $w.all
}

#--------------------------------------------------------------------
#   proc pe2_open_new
#   Open the particle ed, expects a name for the toplevel
#   window, and a x/y position on screen. Creates a new 
#   default particle system.
#   
#   19-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_open_new {win x y} {
    
    # create a default particle system
    new_psys

    # create the particle ed window
    gen_window $win $x $y
}

#--------------------------------------------------------------------
#   proc pe2_open_load
#   Open the particle ed, expects a name for the toplevel
#   window, and a x/y position on screen. Load an existing particle
#   system.
#   19-Jan-01   floh    created
#--------------------------------------------------------------------
proc pe2_open_load {win x y file} {
    
    # create a default particle system
    load_psys $file

    # create the particle ed window
    gen_window $win $x $y
}

#--------------------------------------------------------------------
#   main
#   The main window.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------

# comment out the following return to get standalone editor
return

# load tclnebula extension
# load tclnebula
load tk85

set nomads_home [/sys/share/assigns/home.gets]
set home        $nomads_home

# Libs laden
source $home/bin/nsh.tcl
source $home/bin/observerlib.tcl

# initialize Nebula nobserver object 
init_observer

# create a default particle system
new_psys

# create the particle ed window
gen_window .w 580 0

# go!
/sys/servers/gfx.setclearcolor 0 0 0 1
/observer.setsleep 0.02
/observer.setgrid  false
/observer.start

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------






        
        
            
            
            
    
                

