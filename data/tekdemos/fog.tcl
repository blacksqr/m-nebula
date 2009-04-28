#
#   fog.tcl
#
#   NEBULA tek demos showing various fogging effects.
#
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

#--------------------------------------------------------------------
#   gen_lib
#   Generate a lib of shadernodes, 
#--------------------------------------------------------------------
proc gen_lib {} {

    set cwd [psel]

    if {[exists /usr/lib]} {
        sel /usr
        delete lib
    }

    sel /usr
    new nroot lib
    sel lib
    new nshadernode shader
        sel shader
        .setnumstages 1
        .setcolorop 0 "mul tex prev"
    
        .begintunit 0
            .setaddress         wrap   wrap
            .setminmagfilter    linear linear
            .settexcoordsrc     uv0
            .setenabletransform false
        .endtunit

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false
        .setfogenable true
    sel ..

    sel $cwd
}

#--------------------------------------------------------------------
#   Generate named a textured rotating cube at position (x,y,z).
#--------------------------------------------------------------------
proc gen_cube { name x y z } {
    new n3dnode $name
        sel $name
        .txyz $x $y $z

        # generate vbuffer, texture and material
        new nmeshnode mesh
        mesh.setfilename meshes/torus1.n3d

        # linknode to actual shadernode, shared
        # shadernodes will speed the whole thing up
        new nlinknode shader
        shader.settarget /usr/lib/shader

        new ntexarraynode tex
            sel tex
            .settexture 0 bmp/marble.bmp none
        sel ..
        
        # generate a rotation interpolator
        new nipol ip
            sel ip
            .connect rxyz
            .addkey3f 0 0 0 0
            .addkey3f 2 360 360 360
        sel ..
    sel ..
}

#--------------------------------------------------------------------
#   Generate a grid of rotating cubes...
#--------------------------------------------------------------------
proc gen_grid { name y } {
    new n3dnode $name
        sel $name
        .ty $y
        .ry -30
#        for {set x -40} {$x <= +40} {incr x 20} {
            for {set z -150} {$z < 10} {incr z 10} {
                gen_cube cube_($z) -10 0 $z
            }
#        }
    sel ..
}

#--------------------------------------------------------------------
#   Define one ambient and 1 directional light.
#--------------------------------------------------------------------
proc gen_lights { } {
    sel /usr/scene
    new n3dnode dlight
    	sel dlight

    	new n3dnode light1
    		sel light1
    		new nlightnode light
    			sel light
    		    .setattenuation 1 0 0 
    			.settype  directional
    			.setcolor 0.7 0.2 0.7 1
    		sel ..	
    	sel ..

        new nlightnode amb
            sel amb
    		.settype  ambient
            .setcolor 0.3 0.3 0.3 1
        sel ..

    sel ..
}

#--------------------------------------------------------------------
#   A few small helper functions to set fog and bg colors from
#   Tk scalers.
#--------------------------------------------------------------------
proc set_red { c } {
    set color [/usr/scene/fog.getcolor]
    set r $c
    set g [lindex $color 1]
    set b [lindex $color 2]
    set a [lindex $color 3]
    /usr/scene/fog.setcolor $r $g $b $a
    /sys/servers/gfx.setclearcolor $r $g $b $a
}

proc set_green { c } {
    set color [/usr/scene/fog.getcolor]
    set r [lindex $color 0]
    set g $c
    set b [lindex $color 2]
    set a [lindex $color 3]
    /usr/scene/fog.setcolor $r $g $b $a
    /sys/servers/gfx.setclearcolor $r $g $b $a
}

proc set_blue { c } {
    set color [/usr/scene/fog.getcolor]
    set r [lindex $color 0]
    set g [lindex $color 1]
    set b $c
    set a [lindex $color 3]
    /usr/scene/fog.setcolor $r $g $b $a
    /sys/servers/gfx.setclearcolor $r $g $b $a
}

#--------------------------------------------------------------------
#   main
#   Execution starts here.
#--------------------------------------------------------------------
sel /usr/scene

# setup lighting
gen_lights

# generate 2 grids of rotating cubes
gen_lib

gen_grid grid1 -10
gen_grid grid2 0
gen_grid grid3 +10

# generate a fog node
new nfognode fog
    sel fog
    .setmode exp
    .setcolor 0.5 0.5 0.5 1.0
    .setdensity 0.01
sel ..

#--------------------------------------------------------------------
#   Create a small tk window with scales for density and fog color.
#--------------------------------------------------------------------
toplevel     .r
wm title     .r     "Exponential Fog Controls"
wm geometry  .r     571x121+1+420
wm resizable .r     0 0

global f_density
global f_red
global f_green
global f_blue

set f_density [/usr/scene/fog.getdensity]
set f_color   [/sys/servers/gfx.getclearcolor]
set f_red     [lindex $f_color 0]
set f_green   [lindex $f_color 1]
set f_blue    [lindex $f_color 2]

frame  .r.a
pack   .r.a -side top -expand 1
scale  .r.a.dens -length 550 -orient horizontal -from 0.00001 -to 0.10 -showvalue 0 \
        -label "" -variable f_density -resolution 0.001 \
   	    -command { /usr/scene/fog.setdensity }
label  .r.a.t1 -text "Density" -width 10
pack   .r.a.t1 .r.a.dens   -side left -expand 1

frame .r.b
pack  .r.b -side top -expand 1
scale .r.b.red -length 550 -orient horizontal -from 0.0 -to 1.0 -showvalue 0 \
        -label "" -variable f_red -resolution 0.01 \
   	    -command { set_red  }
label .r.b.t1  -text "Red" -width 10
pack  .r.b.t1 .r.b.red -side left -expand 1

frame .r.c
pack  .r.c -side top -expand 1
scale .r.c.green -length 550 -orient horizontal -from 0.0 -to 1.0 -showvalue 0 \
        -label "" -variable f_green -resolution 0.01 \
   	    -command { set_green  }
label .r.c.t1  -text "Green" -width 10
pack  .r.c.t1  .r.c.green -side left -expand 1

frame .r.d
pack  .r.d -side top -expand 1
scale .r.d.blue -length 550 -orient horizontal -from 0.0 -to 1.0 -showvalue 0 \
        -label "" -variable f_blue -resolution 0.01 \
   	    -command { set_blue  }
label .r.d.t1   -text "Blue" -width 10
pack  .r.d.t1 .r.d.blue -side left -expand 1



#--------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------
        



        
