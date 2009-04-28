#
#   Demonstrate alpha test
# 

sel    /usr/scene
source licht.tcl

new n3dnode animmesh
    sel animmesh

    new nipol rx
        sel rx
        .connect rx
        .addkey1f 0 0
        .addkey1f 50 360
    sel ..
    new nipol rz
        sel rz
        .connect rz
        .addkey1f 0 0
        .addkey1f 60 360
    sel ..

    new nmeshnode mesh
        sel mesh
        .setfilename "meshes/torus1.n3d" 
    sel ..

    new nshadernode sn
        sel sn

        .setnumstages 3
        .setcolorop 0 "replace tex"
        .setcolorop 1 "adds -tex prev"
        .setcolorop 2 "add tex prev"
        .setalphaop 2 "mul tex prev"

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
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz1 2 2 2 

        .begintunit 2
            .setaddress clamp clamp
            .setminmagfilter linear linear
            .settexcoordsrc spheremap
            .setenabletransform true
        .endtunit

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false
        .setalphatestenable true
        .setalpharef 0.7
        .setalphafunc less
        .setcullmode none

        new nipol ip
            sel ip
            .connect txyz0
            .addkey3f  0 0 0 0
            .addkey3f  15 1 2 4
        sel ..

        new nipol ip1
            sel ip1
            .connect txyz1
            .addkey3f   0 0 0 0
            .addkey3f  20 -2 -4 -8
        sel ..

    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/funky.bmp bmp/mask2.bmp
        .settexture 1 bmp/funky.bmp bmp/mask2.bmp
        .settexture 2 bmp/colorgloss.bmp none
    sel ..
sel ..
