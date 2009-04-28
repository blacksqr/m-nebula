#
#   Nebula TEKDEMO
#
#   MipMapping
#
#   created:    15-May-99   Bernd Beyreuther
#   upd.:       03-Nov-2000 Bernd Beyreuther
#   
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
# 

sel /usr/scene
new nlightnode amb
    sel amb
    .settype ambient
    .setcolor 1 1 1 1
sel ..

# the torus with mipmapping activated ---------------

new n3dnode obj
    sel obj
    ty  -3
    tx  -35

    # Some animation -------------------------------
    new nipol tz
        sel tz
        .connect tz
        .addkey1f  0  -8
        .addkey1f  5 -100
        .addkey1f 10  -8
    sel ..
    # Rotation -------------------------------------
    new nipol roty
        sel roty
        .connect ry
        .addkey1f 0 0
        .addkey1f 2 360
    sel ..
    new nipol rotx
        sel rotx
        .connect rx
        .addkey1f 0 0
        .addkey1f 3 360
    sel ..

    new nmeshnode mesh
        sel mesh
        .setfilename "meshes/torus5.n3d"
    sel ..
    new ntexarraynode tex
        sel tex
        settexture 0 "bmp/256.bmp" none
    sel ..
    new nshadernode shader
        sel shader
        .setnumstages 1
        .setcolorop 0 "replace tex prev"
        .begintunit 0
            .setminmagfilter "linear_mipmap_linear" "linear"
            .setaddress "wrap" "wrap"
            .settexcoordsrc "uv0"
        .endtunit
        .setdiffuse 1.000000 1.000000 1.000000 0.000000
        .setemissive 0.000000 0.000000 0.000000 0.000000
        .setambient 0.000000 0.000000 0.000000 0.000000
        .setlightenable false
    sel ..
sel ..

# Make a copy of the first torus
# Modify its position and
# turn off the mipmapping

obj.clone obj1
    sel obj1
    tx -70

        sel shader
        .begintunit 0
            .setminmagfilter linear linear
        .endtunit
    sel ..
sel ..

