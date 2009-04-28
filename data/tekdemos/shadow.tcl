#--------------------------------------------------------------------
#   shadow test
#   (C) 2001 RadonLabs GmbH
#--------------------------------------------------------------------
/sys/servers/shadow.setcastshadows true

sel /usr/scene

# an ambient light node
new nlightnode amb
    sel amb
    .setcolor 0 0 0 0
sel ..

# a rotating directional lightnode
new n3dnode light
    sel light
    .rx -45
    
    new nlightnode l
        sel l
        .settype directional
        .setcolor 1 1 1 1
	.setcastshadows true
    sel ..

    new nipol ip
        sel ip
        .connect ry
        .addkey1f 0 0
        .addkey1f 20 360
    sel ..
sel ..

# some ground geometry
new n3dnode land
    sel land

    .sxyz 2 1 2
    
    new nmeshnode mesh
        sel mesh
        .setfilename meshes/testplane.n3d
    sel ..
    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/marble.bmp none
        .setgenmipmaps 0 true
    sel ..
    new nshadernode shd
        sel shd
        .setnumstages 1
        .setcolorop 0 "mul tex prev"
        .begintunit 0
        .setminmagfilter "linear_mipmap_nearest" "linear"
        .settexcoordsrc "uv0"
        .endtunit
        .setdiffuse 1.000000 1.000000 1.000000 1.000000
        .setemissive 0.000000 0.000000 0.000000 0.000000
        .setambient 1.000000 1.000000 1.000000 1.000000
        .setlightenable true
        .setalphaenable false
        .setzwriteenable true
    sel ..        
sel ..

# a donut routine
proc donut {name tx ty tz time} {

    new n3dnode $name
        sel $name
        .txyz $tx $ty $tz

        new nipol rot
            sel rot
            .connect rxyz
            .addkey3f 0 0 0 0
            .addkey3f $time 360 720 1440
        sel ..    
    
        new nmeshnode mesh
            sel mesh
            .setfilename "meshes/sdonut.n3d"
            .setcastshadow true
        sel ..
        new ntexarraynode tex
            sel tex
            .settexture 0 bmp/marble.bmp none
            .setgenmipmaps 0 true
        sel ..
        new nshadernode shd
            sel shd
            .setnumstages 1
            .setcolorop 0 "mul tex prev"
            .begintunit 0
            .setminmagfilter "linear_mipmap_nearest" "linear"
            .settexcoordsrc "uv0"
            .endtunit
            .setdiffuse 1.000000 1.000000 1.000000 1.000000
            .setemissive 0.000000 0.000000 0.000000 0.000000
            .setambient 1.000000 1.000000 1.000000 1.000000
            .setlightenable true
            .setalphaenable false
            .setzwriteenable true
        sel ..        
    sel ..
}

# generate a bunch of shadow casting donuts
for {set y 0} {$y < 3} {incr y} {
    for {set x 0} {$x < 3} {incr x} {
        for {set z 0} {$z < 3} {incr z} {
            set posx [expr $x * 10]
            set posy [expr $y * 10 + 5]
            set posz [expr $z * 10]
            donut d_($x)_($y)_($z) $posx $posy $posz [expr ($x + $y + $z + 1) * 5]
        }
    }
}

