# ---
# $parser:ntclserver$ $class:n3dnode$ $ver:Mon Aug 21 21:51:53 2000$
# ---

sel /usr/scene

# create vanilla lighting
source licht.tcl

# display number of triangles
/sys/servers/console.watch gfx_triangles

new n3dnode grid1
sel grid1

    .txyz -12.000000 0.000000 -20.000000
    .rxyz 15.00000 0.0000 0.000000
    .sxyz 1.000000 1.000000 1.000000

    new nflatterrainnode ter
        sel ter
        .setdetail 100.000000
        .setstaticerror 7.000000
        .setradius 25.000000
        .setuvscale 0.040000
        .setheightmap "bmp/flatTerrainHeightMap.bmp"
    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 "bmp/detailmap.bmp" "none"
        .settexture 1 "bmp/flatTerrainTexture.bmp" "none"
    sel ..

    new nshadernode sn
        sel sn
        .setrenderpri 0
        .setnumstages 2
        .setcolorop 0 "mul tex prev"
        .setalphaop 0 "nop"
        .setcolorop 1 "mul tex prev"
        .setalphaop 1 "nop"
        .setconst 0 0.000000 0.000000 0.000000 0.000000
        .setconst 1 0.000000 0.000000 0.000000 0.000000
        .begintunit 0
        .setaddress "clamp" "clamp"
        .setminmagfilter "linear_mipmap_nearest" "linear"
        .settexcoordsrc "uv0"
        .setenabletransform false
        .txyz 0.000000 0.000000 0.000000
        .rxyz 0.000000 0.000000 0.000000
        .sxyz 1.000000 1.000000 1.000000
        .endtunit
        .begintunit 1
        .setaddress "clamp" "clamp"
        .setminmagfilter "linear_mipmap_linear" "linear"
        .settexcoordsrc "uv1"
        .setenabletransform false
        .txyz 0.000000 0.000000 0.000000
        .rxyz 0.000000 0.000000 0.000000
        .sxyz 1.000000 1.000000 1.000000
        .endtunit
        .setdiffuse 1.000000 1.000000 1.000000 1.000000
        .setemissive 0.000000 0.000000 0.000000 0.000000
        .setambient 1.000000 1.000000 1.000000 1.000000
        .setlightenable true
        .setalphaenable false
        .setzwriteenable true
        .setfogenable true
        .setalphablend "srcalpha" "invsrcalpha"
        .setzfunc "lessequal"
        .setcullmode "cw"
        .setcolormaterial "material"
    sel ..
sel ..

# ---
# EOF
