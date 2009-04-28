#
#   meshmix.tcl
#
#   Demonstrate weighted mesh mixing.
#
#   (C) COPYRIGHT 2001 RADONLABS GMBH
#

proc environ_shader {} {
    new nshadernode sn
        sel sn

        .setnumstages 2
        .setcolorop 0 "mul  tex prev"
        .setcolorop 1 "ipol tex prev const.a"

        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz0 2 2 2
        
        .begintunit 1
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc spheremap
            .setenabletransform true
        .endtunit

        .setconst0 0.2 0.2 0.2 0.2

        .setlightenable true
        .setdiffuse  0.5 0.5 0.5 1.0
        .setemissive 1 0 0 0
        .setambient  0 0 0 0
        .setalphaenable false

        # mix red/green emission based on weights
        new nmixer mix
            sel mix
            .beginsources 2
            .setsource 0 blueglow w2
            .setsource 1 greenglow w3
            .endsources
            .beginconnects 1
            .setconnect 0 setemissive getval4f
            .endconnects
            .setnormalize false

            # blue glowing emission
            new nipol blueglow
                sel blueglow
                .addkey4f 0 1 0 0 0
                .addkey4f 1 0 0 0 0
                .addkey4f 2 1 0 0 0
            sel ..

            # a green glowing emission
            new nipol greenglow
                sel greenglow
                .addkey4f 0 0 0 0 0
                .addkey4f 1 0 1 0 0
                .addkey4f 2 0 0 0 0
            sel ..
        sel ..
    sel ..
    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/marble.bmp none
        .settexture 1 bmp/autobahn.bmp none
    sel ..
}

#--------------------------------------------------------------------
#   main
#   Execution starts here.
#--------------------------------------------------------------------
sel    /usr/scene
source licht.tcl

new n3dnode pos
    sel pos
    .tx -1
    .tz -3

    new nipol rotx
        sel rotx
        .connect rx
        .addkey1f 0 0
        .addkey1f 5 360
    sel ..
    
    new nipol rotz
        sel rotz
        .connect rz
        .addkey1f 0 0
        .addkey1f 7 360
    sel ..

    new nmeshmixer mesh
        sel mesh

        new nmeshnode mesh1
        mesh1.setfilename "meshes/torus1.n3d"
        mesh1.setreadonly true

        new nmeshnode mesh2
        mesh2.setfilename "meshes/torus4.n3d"
        mesh2.setreadonly true

        new nmeshnode mesh3
        mesh3.setfilename "meshes/torus5.n3d"
        mesh3.setreadonly true

		.beginsources 3
		.setsource 0 mesh1 w1
		.setsource 1 mesh2 w2
		.setsource 2 mesh3 w3
		.endsources

		.setnormalize  true
    
        setupdatecoord true
        setupdatenorm  true
        setupdatecolor false
        setupdateuv0   false
        setupdateuv1   false
        setupdateuv2   false
        setupdateuv3   false
 
    sel ..

    environ_shader
sel ..

#--------------------------------------------------------------------
#   Create a small tk window with scales for density and fog color.
#--------------------------------------------------------------------
toplevel     .r
wm title     .r     "Weight Controls"
wm geometry  .r     571x61+1+420
wm resizable .r     0 0

frame .r.d
pack  .r.d -side top -expand 1
scale .r.d.w4 -length 550 -orient horizontal -from 0.0 -to 1.0 -showvalue 0 \
        -label "" -variable w2 -resolution 0.0001 \
   	    -command { /sys/servers/channel.setchannel1f w2 }
label .r.d.t1   -text "Weight 2" -width 10
pack  .r.d.t1 .r.d.w4 -side left -expand 1

frame .r.e
pack  .r.e -side top -expand 1
scale .r.e.w5 -length 550 -orient horizontal -from 0.0 -to 1.0 -showvalue 0 \
        -label "" -variable w3 -resolution 0.0001 \
   	    -command { /sys/servers/channel.setchannel1f w3 }
label .r.e.t1   -text "Weight 3" -width 10
pack  .r.e.t1 .r.e.w5 -side left -expand 1

set w1 1.0
/sys/servers/channel.setchannel1f w1 1.0

#--------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------
        



        
