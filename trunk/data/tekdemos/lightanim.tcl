#
#   Nebula TEKDEMO
#
#   Light Anim
#   Two colord point lights, changing position + color over time
#
#   created:    15-May-99   Bernd Beyreuther
#   upd.:       08-Nov-2000 
#   
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
# 

# 
#   This proc generates a lenseflare object
#
proc genLenseFlare {} {
	new n3dnode flare
		sel flare

		new nlenseflare f
			sel f
			.beginflares 6
			.setblindcolor 0.000000 0.000000 0.000000 0.000000

			.setflaresizeat 0 0.200000
			.setflaresizeat 1 0.30000
			.setflaresizeat 2 0.400000
			.setflaresizeat 3 0.20000
			.setflaresizeat 4 0.40000
			.setflaresizeat 5 0.20000

			.setflareposat 0 0.500000
			.setflareposat 1 1.200000
			.setflareposat 2 1.600000
			.setflareposat 3 2.000000
			.setflareposat 4 2.500000
			.setflareposat 5 3.00000

			.setflarecolorat 0 0.3 0.0 0.7 1
			.setflarecolorat 1 0.0 0.3 0.4 1
			.setflarecolorat 2 0.1 0.3 0.0 1
			.setflarecolorat 3 0.4 0.0 0.0 1
			.setflarecolorat 4 0.3 0.1 0.3 1
			.setflarecolorat 5 0.5 0.5 0.5 1
			.endflares
		sel ..

		new nshadernode shader
			sel shader
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

		new ntexarraynode tex
			sel tex
			.settexture 0 "bmp/flare.bmp" "none"
			.setgenmipmaps 0 true
			.setgenmipmaps 1 true
			.setgenmipmaps 2 true
			.setgenmipmaps 3 true
		sel ..
	sel ..
}

sel     /usr/scene
source routines.tcl

# Switching off the ambient light
new nlightnode amb
amb.settype    ambient
amb.setcolor   0 0 0 0

# The two lightsources rotating
new n3dnode dlight    
    sel dlight
    new nipol roty 
        sel roty
        .connect ry
        .addkey1f 0 0
        .addkey1f 6 360
    sel ..
    new nipol rotx
        sel rotx
        .connect rx
        .addkey1f  0   0
        .addkey1f  7  60
        .addkey1f 13   0
    sel ..


    plainobject light1 meshes/lampe.n3d bmp/marble.bmp
        sel light1
        txyz 2    0    2
        sxyz 0.25 0.25 0.25

            sel shader
            setambient 0 0 0 0
            setdiffuse 0 0 0 0
            new nipol em
                sel em
                .connect setemissive 
                .addkey4f 0   0 1 1 0
                .addkey4f 1.5 1 0 0 1
                .addkey4f 3   0 0 1 1
                .addkey4f 4   0 1 1 0
            sel ..
        sel ..


        new nlightnode light1
            sel light1
		    .setattenuation 1   0   0 
			.settype        point
			.setcolor       0.1 0.5 1 1

            new nipol col
                sel col
                .connect setcolor
                .addkey4f 0   0 1 1 0
                .addkey4f 1.5 1 0 0 1
                .addkey4f 3   0 0 1 1
                .addkey4f 4   0 1 1 0
            sel ..
        sel ..

        genLenseFlare

    sel ..

    plainobject light2 meshes/lampe.n3d bmp/marble.bmp
        sel light2
        txyz -2   0   -2
        sxyz  0.25 0.25  0.25

            sel shader
            setdiffuse 0 0 0 0
            setambient 0 0 0 0
            new nipol em
                sel em
                .connect setemissive
                .addkey4f 0   1 1 0 0
                .addkey4f 1.5 1 1 1 1
                .addkey4f 2   0 1 0 1
                .addkey4f 6   1 1 0 0
            sel ..
        sel ..

        new nlightnode li2
            sel li2
		    .setattenuation 1 0 0 
			.settype  point
			.setcolor 0.1 0.5 1 1

            new nipol col
                sel col
                .connect setcolor
                .addkey4f 0   1 1 0 0
                .addkey4f 1.5 1 1 1 1
                .addkey4f 2   0 1 0 1
                .addkey4f 6   1 1 0 0
            sel ..
        sel ..
    sel ..
sel ..

plainobject ding meshes/nebula.n3d bmp/marble.bmp
    sel ding

    new nipol rotz
        sel rotz
        .connect rz
        .addkey1f  0 0
        .addkey1f 10 360
    sel ..
    new nipol rotx
        sel rotx
        .connect rx
        .addkey1f  0  360
        .addkey1f 30    0
    sel ..
sel ..
