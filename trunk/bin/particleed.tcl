#--------------------------------------------------------------------
#   particleed.tcl
#   ==============
#   Rudimentary particle system editor.
#   (C) 2000 A.Weissflog
#--------------------------------------------------------------------

global pe_time
global pe_freq
global pe_speed
global pe_accel
global pe_spin
global pe_spinacc
global pe_innercone
global pe_outercone
global pe_gaccel_x
global pe_gaccel_y
global pe_gaccel_z
global pe_visualize

#--------------------------------------------------------------------
#   pe_keys is an array of this structure:
#   pe_keys($i,size)    -> current size
#   pe_keys($i,bounce)  -> current bounce
#   pe_keys($i,red)     -> current red
#   pe_keys($i,green)   -> current green
#   pe_keys($i,blue)    -> current blue
#   pe_keys($i,alpha)   -> current alpha
#--------------------------------------------------------------------
global pe_numkeys
global pe_keys

#--------------------------------------------------------------------
#   pe_state is an array of the following structure:
#   pe_state(shown,size|bounce|red|green|blue|alpha)    -> canvas curve is shown
#   pe_state(maxval,size...) -> maximum value
#--------------------------------------------------------------------
global pe_state

set pe_state(shown,size)   1
set pe_state(shown,bounce) 1
set pe_state(shown,red)    1
set pe_state(shown,green)  1
set pe_state(shown,blue)   1
#set pe_state(shown,alpha)  1

set pe_state(maxval,size)   10.0
set pe_state(maxval,bounce) 2.0
set pe_state(maxval,red)    1.0
set pe_state(maxval,green)  1.0
set pe_state(maxval,blue)   1.0
#set pe_state(maxval,alpha)  1.0

set home [/sys/share/assigns/home.gets]
#source $home/bin/loadtk.tcl

#--------------------------------------------------------------------
#   updatekeysfromobject
#   Update pe_numkeys and pe_keys from object.
#--------------------------------------------------------------------
proc pe_updatekeysfromobject { obj } {
    global pe_numkeys
    global pe_keys
    set pe_numkeys [$obj.getnumkeys]
    for {set i 0} {$i < $pe_numkeys} {incr i} {
        set k [$obj.getkey $i]
        set pe_keys($i,size)   [lindex $k 0]
        set pe_keys($i,bounce) [lindex $k 1]
        set pe_keys($i,red)    [lindex $k 2]
        set pe_keys($i,green)  [lindex $k 3]
        set pe_keys($i,blue)   [lindex $k 4]
        set pe_keys($i,alpha)  [lindex $k 5]
    }
}

#--------------------------------------------------------------------
#   updateobjectfromkeys
#   Update pe_numkeys and pe_keys from object.
#--------------------------------------------------------------------
proc pe_updateobjectfromkeys { obj } {
    global pe_numkeys
    global pe_keys
    for {set i 0} {$i < $pe_numkeys} {incr i} {
        $obj.setkey $i $pe_keys($i,size) $pe_keys($i,bounce) $pe_keys($i,red) $pe_keys($i,green) $pe_keys($i,blue) $pe_keys($i,alpha)
    }
}

#--------------------------------------------------------------------
#   pe_setattr
#   Set object attribute by invoking command on it.
#--------------------------------------------------------------------
proc pe_setattr {obj cmd val} {
    $obj.$cmd $val
}

#--------------------------------------------------------------------
#   pe_updgaccel
#   Update global acceleration vector.
#--------------------------------------------------------------------
proc pe_updgaccel {obj val} {
    global pe_gaccel_x
    global pe_gaccel_y
    global pe_gaccel_z
    $obj.setglobaccel $pe_gaccel_x $pe_gaccel_y $pe_gaccel_z
}



#--------------------------------------------------------------------
#   pe_getobject
#   Reads the values from the object and inits the window
#--------------------------------------------------------------------
proc pe_getobject { obj } {
    global pe_time
    global pe_freq
    global pe_speed
    global pe_accel
    global pe_spin
    global pe_spinacc
    global pe_innercone
    global pe_outercone
    global pe_gaccel_x
    global pe_gaccel_y
    global pe_gaccel_z
    global pe_visualize

    set pe_time      1.0
    set pe_freq      100.0
    set pe_speed     1.0
    set pe_accel     0.0
    set pe_spin      0.0
    set pe_spinacc   0.0
    set pe_innercone 0.0
    set pe_outercone 0.0
    set pe_gaccel_x  0.0
    set pe_gaccel_y  0.0
    set pe_gaccel_z  0.0
    set pe_visualize 0

    set pe_time         [format %.1f [$obj.getlifetime       ]]
    set pe_freq         [format %.1f [$obj.getfreq           ]]
    set pe_speed        [format %.1f [$obj.getspeed          ]]
    set pe_accel        [format %.1f [$obj.getaccel          ]]
    set pe_spin         [format %.1f [$obj.getspin           ]]
    set pe_spinacc      [format %.1f [$obj.getspinaccel      ]]
    set pe_innercone    [format %.1f [$obj.getinnercone      ]]
    set pe_outercone    [format %.1f [$obj.getoutercone      ]]
    set accel           [$obj.getglobaccel]
    set pe_gaccel_x     [format %.1f [lindex $accel 0 ]]
    set pe_gaccel_y     [format %.1f [lindex $accel 1 ]]
    set pe_gaccel_z     [format %.1f [lindex $accel 2 ]]
    set pe_visualize    [$obj.getvisualize]
return
}


#--------------------------------------------------------------------
#   pe_initobject
#   Initialize start values and init object
#--------------------------------------------------------------------
proc pe_initobject { obj } {
    global pe_time
    global pe_freq
    global pe_speed
    global pe_accel
    global pe_spin
    global pe_spinacc
    global pe_innercone
    global pe_outercone
    global pe_gaccel_x
    global pe_gaccel_y
    global pe_gaccel_z
    global pe_visualize

    set pe_time      1.0
    set pe_freq      100.0
    set pe_speed     1.0
    set pe_accel     0.0
    set pe_spin      0.0
    set pe_spinacc   0.0
    set pe_innercone 0.0
    set pe_outercone 0.0
    set pe_gaccel_x  0.0
    set pe_gaccel_y  0.0
    set pe_gaccel_z  0.0
    set pe_visualize 0

    $obj.setlifetime    $pe_time
    $obj.setfreq        $pe_freq
    $obj.setspeed       $pe_speed
    $obj.setaccel       $pe_accel
    $obj.setspin        $pe_spin
    $obj.setspinaccel   $pe_spinacc
    $obj.setinnercone   $pe_innercone
    $obj.setoutercone   $pe_outercone
    $obj.setglobaccel   $pe_gaccel_x $pe_gaccel_y $pe_gaccel_z
    $obj.setvisualize false
}

#--------------------------------------------------------------------
#   pe_attrscale
#   Create a attribute scaler with additional numerical input.
#--------------------------------------------------------------------
proc pe_attrscale { win label var min_val max_val res obj cmd } {
    frame .$win.f_$var
    pack  .$win.f_$var -side top -expand 1
    label .$win.f_$var.label_$var -width 5 -justify left -text $label
    scale .$win.f_$var.scale_$var -length 90 -orient horizontal\
          -from $min_val -to $max_val -resolution $res -showvalue no\
          -variable $var -command "pe_setattr $obj $cmd"
    label .$win.f_$var.num_$var -width 3 -justify left -textvariable $var
    pack  .$win.f_$var.label_$var .$win.f_$var.scale_$var .$win.f_$var.num_$var -side left -expand 1
}

#--------------------------------------------------------------------
#   pe_procscale
#   Create a scaler that just calls procedure
#--------------------------------------------------------------------
proc pe_procscale { win label var min_val max_val res obj proc } {
    frame .$win.f_$var
    pack  .$win.f_$var -side top -expand 1
    label .$win.f_$var.label_$var -width 5 -justify left -text $label
    scale .$win.f_$var.scale_$var -length 90 -orient horizontal\
          -from $min_val -to $max_val -resolution $res -showvalue no\
          -variable $var -command "pe_updgaccel $obj"
    label .$win.f_$var.num_$var -width 3 -justify left -textvariable $var
    pack  .$win.f_$var.label_$var .$win.f_$var.scale_$var .$win.f_$var.num_$var -side left -expand 1
}

#--------------------------------------------------------------------
#   pe_down
#   Called back whenever mouse is pressed over curve point
#--------------------------------------------------------------------
proc pe_down { obj canvas entry index w h x y } {
    global pe_keys
    $canvas dtag selected
    $canvas addtag selected withtag current
    $canvas raise current
    set pe_keys($index,$entry,last_y) $y
}

#--------------------------------------------------------------------
#   pe_release
#   Called back whenever mouse is pressed over curve point
#--------------------------------------------------------------------
proc pe_release { obj canvas entry index w h x y } {
    global pe_keys
    $canvas dtag selected
    pe_refreshcanvas $obj $canvas
}

#--------------------------------------------------------------------
#   pe_move
#   Called back during drag and drop move...
#--------------------------------------------------------------------
proc pe_move { obj canvas entry index w h x y } {
    global pe_keys
    global pe_state

    set my [expr $y-$pe_keys($index,$entry,last_y)]
    $canvas move selected 0 $my
    set pe_keys($index,$entry,last_y) $y

    # update object key
    set newval [expr (($y*$pe_state(maxval,$entry))/$h)]
    if {$newval < 0.0} { set newval 0.0 }
    if {$newval > $pe_state(maxval,$entry)} { set newval $pe_state(maxval,$entry) }
    set pe_keys($index,$entry) $newval
    pe_updateobjectfromkeys $obj
}

#--------------------------------------------------------------------
#   pe_curve
#   Create connected line segments inside canvas, following
#   a keyframe definition:
#
#   canvas -> name of canvas object
#   scale  -> maximum value for Y scaling
#   entry  -> entry in pe_keys "size", "bounce"...
#   color  -> line color
#--------------------------------------------------------------------
proc pe_curve { obj canvas w h entry color } {
    global pe_numkeys
    global pe_keys
    global pe_state
    set scale $pe_state(maxval,$entry)

    set dist_x [expr ($w / ($pe_numkeys-1))]

    # draw lines
    for {set i 0} {$i < [expr ($pe_numkeys-1)]} {incr i} {
        set next_i [expr ($i+1)]
        set x0 [expr ($i * $dist_x)]
        set y0 [expr (($h * $pe_keys($i,$entry))/$scale)]
        set x1 [expr ($next_i * $dist_x)]
        set y1 [expr (($h * $pe_keys($next_i,$entry))/$scale)]
        $canvas create line $x0 $y0 $x1 $y1 -fill $color
    }

    # draw points
    for {set i 0} {$i < $pe_numkeys} {incr i} {
        set x [expr ($i * $dist_x)]
        set y [expr (($h * $pe_keys($i,$entry))/$scale)]
        set x0 [expr $x-4]
        set y0 [expr $y-4]
        set x1 [expr $x+4]
        set y1 [expr $y+4]
        set pe_keys($i,$entry,posx) $x
        set pe_keys($i,$entry,posy) $y
        set item [$canvas create rectangle $x0 $y0 $x1 $y1 -width 1 -outline black -fill $color]

        # setup drag and drop
        $canvas bind $item <1>                  "pe_down    $obj $canvas $entry $i $w $h %x %y"
        $canvas bind $item <B1-Motion>          "pe_move    $obj $canvas $entry $i $w $h %x %y"
        $canvas bind $item <ButtonRelease-1>    "pe_release $obj $canvas $entry $i $w $h %x %y"
    }
}

#--------------------------------------------------------------------
#   pe_refreshcanvas
#--------------------------------------------------------------------
proc pe_refreshcanvas { obj canvas } {
    global pe_state
    
    if {[winfo exists $canvas]} {
        destroy $canvas
    }
    set c_width  250
    set c_height 110
    canvas $canvas -relief sunken   -bg white -width $c_width
    pack $canvas -side left -expand 1 -fill y
    if {$pe_state(shown,size)}   { pe_curve $obj $canvas $c_width $c_height size   black }
    if {$pe_state(shown,bounce)} { pe_curve $obj $canvas $c_width $c_height bounce yellow }
    if {$pe_state(shown,red)}    { pe_curve $obj $canvas $c_width $c_height red    red }
    if {$pe_state(shown,green)}  { pe_curve $obj $canvas $c_width $c_height green  green }
    if {$pe_state(shown,blue)}   { pe_curve $obj $canvas $c_width $c_height blue   blue }
    # --------------------------------------------------------------------------------------
    # In the Tekdemo only additive material is used - so i can remove the alpha slider here
    #    if {$pe_state(shown,alpha)}  { pe_curve $obj $canvas $c_width $c_height alpha  grey }
}

#--------------------------------------------------------------------
#   pe_createcanvas
#   Create Tk canvas to display keyframe curves.
#   pe_numkeys -> must contain number of keyframes
#   pe_keys    -> must contain complete keyframe definitions
#--------------------------------------------------------------------
proc pe_createcanvas { win obj } {
    global pe_numkeys
    global pe_keys
    global pe_state

    set c .$win.f_plot.f_canvas.c

    frame .$win.f_plot
    pack  .$win.f_plot -side top -expand 1
    frame .$win.f_plot.f_canvas
    frame .$win.f_plot.f_check
    pack  .$win.f_plot.f_canvas .$win.f_plot.f_check -side left -expand 1
    checkbutton .$win.f_plot.f_check.c_size    -anchor w -text "size"   -variable pe_state(shown,size)   -command "pe_refreshcanvas $obj $c"
    checkbutton .$win.f_plot.f_check.c_bounce  -anchor w -text "bounc"  -variable pe_state(shown,bounce) -command "pe_refreshcanvas $obj $c"
    checkbutton .$win.f_plot.f_check.c_red     -anchor w -text "red"    -variable pe_state(shown,red)    -command "pe_refreshcanvas $obj $c"
    checkbutton .$win.f_plot.f_check.c_green   -anchor w -text "green"  -variable pe_state(shown,green)  -command "pe_refreshcanvas $obj $c"
    checkbutton .$win.f_plot.f_check.c_blue    -anchor w -text "blue"   -variable pe_state(shown,blue)   -command "pe_refreshcanvas $obj $c"
    # --------------------------------------------------------------------------------------
    # In the Tekdemo only additive material is used - so i can remove the alpha slider here
    #    checkbutton .$win.f_plot.f_check.c_alpha  -text "alpha"  -variable pe_state(shown,alpha)  -command "pe_refreshcanvas $obj $c"
    #
    # pack .$win.f_plot.f_check.c_size .$win.f_plot.f_check.c_bounce .$win.f_plot.f_check.c_red\
    #     .$win.f_plot.f_check.c_green .$win.f_plot.f_check.c_blue .$win.f_plot.f_check.c_alpha\
    #     -side top -expand 1 -anchor w

    pack .$win.f_plot.f_check.c_size .$win.f_plot.f_check.c_bounce .$win.f_plot.f_check.c_red\
         .$win.f_plot.f_check.c_green .$win.f_plot.f_check.c_blue\
         -side top -expand 1 -anchor w -fill x

   
global canvas
set canvas $c

    pe_refreshcanvas $obj $c
}

#--------------------------------------------------------------------
#   pe_setvisualize
#--------------------------------------------------------------------
proc pe_setvisualize { obj } {
    global pe_visualize   
    if {$pe_visualize} {
        $obj.setvisualize true
    } else {
        $obj.setvisualize false
    }
}

#--------------------------------------------------------------------
#   pe_createwindow
#   Create the Tk window, expects name of window to create and
#   name of Nebula object it is bound to.
#   upd.:   08-Feb-2000 resized to std-demo-window-dimension
#--------------------------------------------------------------------
proc pe_createwindow { win obj } {
global pe_visualize   
global canvas

    toplevel     .$win
    wm title     .$win     "Particle Editor" 
    wm geometry  .$win     793x126+1+420
    wm resizable .$win     0 0

    set     path $win.frame1
    frame .$path -relief sunken -borderwidth 1 
    pack  .$path -side left -expand 1 -fill y

    set     subpath $path.subframe1
    frame .$subpath -relief sunken -borderwidth 1 
    pack  .$subpath -side top -expand 1 -fill x

    checkbutton .$subpath.vis -anchor w -text "Wire" -variable pe_visualize -command "pe_setvisualize $obj"
    pack .$subpath.vis -anchor w -side left -expand 1

    # ---------------------------------------------------------
    # Select from all particle-Systems in /psys
    # Only if subdirectory exists.
    # ---------------------------------------------------------
    if [file exists psys] {
        set psys "fire.n"
        menubutton .$subpath.mb1 -textvariable psys \
        -indicatoron 1			    \
        -menu .$subpath.mb1.rnu		\
            -relief raised -bd 2	\
            -highlightthickness 2	\
            -direction flush
            menu .$subpath.mb1.rnu -tearoff 0
    
            cd psys        
        	foreach psys [lsort -ascii [glob -nocomplain -- *.{n}]] {
                .$subpath.mb1.rnu add radiobutton 			\
                -label    $psys           		            \
                -variable psys              				\
                -command {
                    set cur [psel]
                    sel /usr/scene/particle
                    delete p
                    get psys/$psys
                    set shortname   [ file tail [ file rootname $psys ]]
                    $shortname.setname p         
                    pe_getobject            [p.getfullname]
                    pe_updatekeysfromobject [p.getfullname]
                    pe_refreshcanvas        [p.getfullname] $canvas
                    update
                    sel $cur
                } 
            }
            cd ..
        set psys "fire.n"
        pack   .$subpath.mb1  -side left -expand 1 -fill x
    } 


    set     subpath $path.subframe2
    frame .$subpath -relief sunken -borderwidth 1 
    pack  .$subpath -side top -expand 1 -fill y
   
    pe_attrscale $subpath "time"        "pe_time"      0 100  0.1 $obj "setlifetime"
    pe_attrscale $subpath "freq"        "pe_freq"      0 1000 0.1 $obj "setfreq"
    pe_attrscale $subpath "speed"       "pe_speed"     -20 20 0.1 $obj "setspeed"

    set     path $win.frame2
    frame .$path -relief sunken -borderwidth 1 
    pack  .$path -side left -expand 1 -fill y

    pe_attrscale $path "accel"       "pe_accel"     -20 20 0.1 $obj "setaccel"
    pe_attrscale $path "spin"        "pe_spin"      -1000  +1000 1  $obj "setspin"
    pe_attrscale $path "spinacc"     "pe_spinacc"   -200   +200  1  $obj "setspinaccel"
    pe_attrscale $path "cone 1"      "pe_innercone" 0 180  1   $obj "setinnercone"

    set     path $win.frame3
    frame .$path -relief sunken -borderwidth 1 
    pack  .$path -side left -expand 1 -fill y

    pe_attrscale $path "cone2"    "pe_outercone" 0 180  1   $obj "setoutercone"
    pe_procscale $path "acc x"    "pe_gaccel_x" -20 +20 0.1 $obj "pe_updgaccel"
    pe_procscale $path "acc y"    "pe_gaccel_y" -20 +20 0.1 $obj "pe_updgaccel"
    pe_procscale $path "acc z"    "pe_gaccel_z" -20 +20 0.1 $obj "pe_updgaccel"

    set     path $win.frame4
    frame .$path -relief sunken -borderwidth 1
    pack  .$path -side left -expand 1  -fill y

    pe_createcanvas $path $obj
}

#--------------------------------------------------------------------
#   pe_open
#   Open a new particle editor window and initialize
#   the Nebula object belonging to it.
#   upd.:   08-Feb-2000 Statt initobject werden jetzt erst die Werte
#                       des Objektes geladen.    
#--------------------------------------------------------------------
proc pe_open { win obj } {
    global pe_numkeys
    global pe_keys
    global pe_size_shown
    global pe_bounce_shown
    global pe_red_shown
    global pe_green_shown
    global pe_blue_shown
    global pe_alpha_shown

    set pe_size_shown   0
    set pe_bounce_shown 0
    set pe_red_shown    0
    set pe_green_shown  0
    set pe_blue_shown   0
    set pe_alpha_shown  0

    pe_getobject  $obj
    pe_updatekeysfromobject $obj
    pe_createwindow $win $obj
}

#--------------------------------------------------------------------
#   pe_close
#   Destroy a particle editor window
#--------------------------------------------------------------------
proc pe_close { win } {
    destroy .$win
}
    
