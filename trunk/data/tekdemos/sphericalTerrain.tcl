#
#   Nebula TEKDEMO
#
#   spherical.tcl
#   
#   Demonstrate Nebula spherical terrain renderer.
#   Nebula's spherical terrain renderer is based on Thatcher Ulrich's
#   adaptive lod terrain renderer presented on Gamasutra.
#
#   upd.:   18-May-2000 Bernd
#           08-Jan-2001 floh    added changes for detailmapping
#
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

sel /usr/scene

# create vanilla lighting
source licht.tcl

# display number of triangles
/sys/servers/console.watch gfx_triangles

# the root 3d node
new n3dnode pos
    sel pos

    .txyz 10 -20 -40

    # some interpolators to move terrain around
    new nipol ry
        sel ry
        .connect ry
        .addkey1f 0 0
        .addkey1f 10 360
    sel ..
    new nipol tz
        sel tz
        .connect tz
        .addkey1f 0 50
        .addkey1f 15 -1000
        .addkey1f 30 50
    sel ..

    # the terrain node...
    new nterrainnode terrain
        sel terrain

        # NOTE ! ADJUST SETDETAIL FOR 
        # HIGHER RESOLUTION + LESSER POLY-PLOPPING 
        # vs. 
        # LOWER RESOLUTION  + OBIOUS PLOPPING
        # predefined value 170 = a low res-result

        .setdetail 170.000000
        .setstaticerror 7.000000
        .setradius 50.000000
        .setuvscale 0.100000
        .settreefile "bmp/map.nqs"
    sel ..

    new nshadernode sn
        sel sn

        .setnumstages 2
        .setcolorop 0 "mul tex prev"
        .setcolorop 1 "mul tex prev"

        .begintunit 0
        .setaddress "wrap" "wrap"
        .setminmagfilter "linear_mipmap_nearest" "linear"
        .settexcoordsrc "uv0"
        .setenabletransform false
        .endtunit

        .begintunit 1
        .setaddress "wrap" "wrap"
        .setminmagfilter "linear_mipmap_linear" "linear"
        .settexcoordsrc "uv1"
        .setenabletransform false
        .endtunit

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1

    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/marble.bmp none
        .settexture 1 bmp/detailmap.bmp none
    sel ..
sel ..

#-----
# EOF
#-----
