#--------------------------------------------------------------------
#   keyframeed.tcl
#   --------------
#   Implements a universal keyframe editor inside a Tk frame.
#
#   Configuration:
#
#   Initialize the following global variables and call 
#   ke_create()
#
#   ke_num_channels        - number of channels
#   ke_chn_array           - config for each channel:
#       ke_chn_array(chn,name)       - name of channel
#       ke_chn_array(chn,color)      - color (a Tcl color, like DarkGreen)
#       ke_chn_array(chn,minval)     - minimum value
#       ke_chn_array(chn,maxval)     - maximum value
#       ke_chn_array(chn,numkeys)    - number of keys
#       ke_chn_array(chn,0)          - initial key values
#       ke_chn_array(chn,1)
#                 ...
#       ke_chn_array(chn,callback)   - name of tcl procedure to callback if key changes
#
#   Callback procedure:
#   Each channel requires a callback procedure which is called whenever one
#   of the key changes. The procedure will be called with the keynumber
#   and its new value.
#
#   16-Jan-01   floh    created
#--------------------------------------------------------------------

global ke_num_channels
global ke_chn_array
global ke_xoffset
global ke_yoffset
global ke_parentframe

set ke_xoffset 4
set ke_yoffset -3
set ke_parentframe "none"

#--------------------------------------------------------------------
#   proc ke_mousedown
#   Called when mouse is pressed down over a curve point.
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
proc ke_mousedown { canvas channel key w h x y } {
global ke_chn_array
    
    $canvas dtag selected
    $canvas addtag selected withtag current
    $canvas raise current

    # store the current y position, so that we can undo the
    # operation
    set ke_chn_array($channel,$key,last_y) $y
}

#--------------------------------------------------------------------
#   proc ke_mouserelease
#   Called when mouse is release from a point curve.
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
proc ke_mouserelease { canvas channel key w h x y } {
global ke_chn_array
    $canvas dtag selected
    
    # call key update callback function
    $ke_chn_array($channel,callback) $channel $key $ke_chn_array($channel,$key)
    
    # redraw the whole canvas
    ke_refreshcanvas $canvas
}

#--------------------------------------------------------------------
#   proc ke_mousemove
#   Called back during drag and drop. Update the internal keys
#   array and calls back the channels callback procedure.
#   
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
proc ke_mousemove { canvas channel key w h x y } {
global ke_chn_array
global ke_yoffset

    set y [expr ($y - $ke_yoffset)]

    # move the selected point to its new position
    set my [expr $y-$ke_chn_array($channel,$key,last_y)]
    $canvas move selected 0 $my
    set ke_chn_array($channel,$key,last_y) $y
   
    # compute the new key value
    set maxval $ke_chn_array($channel,maxval)
    set minval $ke_chn_array($channel,minval)
    set dval   [expr ($maxval - $minval)]
    set newval [expr (((($h-$y) * $dval) / $h) + $minval)]
    if {$newval < $minval} {
        set newval $minval
    } elseif {$newval > $maxval} {
        set newval $maxval
    }

    # set new key value
    set ke_chn_array($channel,$key) $newval
    
    # FIXME: call callback procedure
}

#--------------------------------------------------------------------
#   proc ke_channelcurve
#   Generate the curve inside the canvas for one channel, and
#   setup drag and drop.
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
proc ke_channelcurve { canvas channel w h } {
global ke_chn_array
global ke_xoffset
global ke_yoffset

    set numkeys $ke_chn_array($channel,numkeys)
    set xstep   [expr ($w / ($ke_chn_array($channel,numkeys) - 1))]
    set yscale  [expr ($ke_chn_array($channel,maxval) - $ke_chn_array($channel,minval))]
    
    # draw lines
    for {set i 0} {$i < [expr ($numkeys-1)]} {incr i} {
        set nexti [expr ($i+1)]
        set x0 [expr (($i * $xstep) + $ke_xoffset)]
        set x1 [expr (($nexti * $xstep) + $ke_xoffset)]
        set y0 [expr ($h - ((($h * ($ke_chn_array($channel,$i) - $ke_chn_array($channel,minval))) / $yscale) + $ke_yoffset))]
        set y1 [expr ($h - ((($h * ($ke_chn_array($channel,$nexti) - $ke_chn_array($channel,minval))) / $yscale) + $ke_yoffset))]
        $canvas create line $x0 $y0 $x1 $y1 -fill $ke_chn_array($channel,color) -width 2
    }
    
    # draw points
    for {set i 0} {$i < $numkeys} {incr i} {
        set x [expr (($i * $xstep) + $ke_xoffset)]
        set y [expr ($h - ((($h * ($ke_chn_array($channel,$i) - $ke_chn_array($channel,minval))) / $yscale) + $ke_yoffset))]
        set x0 [expr ($x + 3)]
        set x1 [expr ($x - 3)]
        set y0 [expr ($y + 3)]
        set y1 [expr ($y - 3)]
        set item [$canvas create rectangle $x0 $y0 $x1 $y1 -width 1 -outline black -fill $ke_chn_array($channel,color)]
        
        # setup drag & drop
        $canvas bind $item <1>                  "ke_mousedown    $canvas $channel $i $w $h %x %y"
        $canvas bind $item <B1-Motion>          "ke_mousemove    $canvas $channel $i $w $h %x %y"
        $canvas bind $item <ButtonRelease-1>    "ke_mouserelease $canvas $channel $i $w $h %x %y"
    }
}

#--------------------------------------------------------------------
#   proc ke_refreshcanvas
#   Refresh the whole canvas.
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
proc ke_refreshcanvas {canvas} {
global ke_chn_array
global ke_num_channels

    if {[winfo exists $canvas]} {
        destroy $canvas
    }

    set c_width  300
    set c_height 120

    canvas $canvas -relief sunken -bg grey -width $c_width -height $c_height \
                   -borderwidth 1 -xscrollincrement 1 -yscrollincrement 1
    pack $canvas -side left -expand 1 -fill y

    # render curve for each channel
    for {set i 0} {$i < $ke_num_channels} {incr i} {
        if {$ke_chn_array($i,visible)} {
            ke_channelcurve $canvas $i $c_width $c_height
        }
    }
}

#--------------------------------------------------------------------
#   proc ke_create_frames
#   Create frames to embed canvas, and for visibility checkbuttons.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
proc ke_create_frames {parentframe} {
global ke_chn_array
global ke_num_channels
    
    # create frame for canvas
    frame $parentframe.cf
    pack $parentframe.cf -side left -fill x
    
    # create canvas
    ke_refreshcanvas $parentframe.cf.canvas

    # create frame for checkbuttons
    frame $parentframe.cb
    pack $parentframe.cb -side right -fill x
    
    # create checkbuttons for channel visibility
    for {set i 0} {$i < $ke_num_channels} {incr i} {
        checkbutton $parentframe.cb.$i \
                    -text $ke_chn_array($i,name) \
                    -indicatoron 1 \
                    -anchor w \
                    -variable ke_chn_array($i,visible) \
                    -bg $ke_chn_array($i,color) \
                    -command "ke_refreshcanvas $parentframe.cf.canvas"
        pack $parentframe.cb.$i -side top -fill x -expand 1
    }
}

#--------------------------------------------------------------------
#   proc ke_create
#   Create the keyframe editor Tk frame object inside a given frame.
#   Don't forget to initialize the global config variables 
#   as described in the head of this file.
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
proc ke_create {parentframe} {
global ke_num_channels
global ke_chn_array
global ke_parentframe

    set ke_parentframe $parentframe

    # by default, all channels are visible
    for {set i 0} {$i < $ke_num_channels} {incr i} {
        set ke_chn_array($i,visible) 1
    }
    ke_create_frames $parentframe
}

#--------------------------------------------------------------------
#   proc ke_refresh
#   Call whenever keyvalues have changed externally.
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
proc ke_refresh {} {
global ke_parentframe
    if {$ke_parentframe != "none"} {
        ke_refreshcanvas $ke_parentframe.cf.canvas
    }
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

