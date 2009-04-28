#--------------------------------------------------------------------
#   meshemitter.tcl
#   ===============
#   Demonstrates nmeshemitter class, which emits particles from
#   a mesh (a static mesh in this case for simplicity).
#
#   01-Nov-00   floh    created
#--------------------------------------------------------------------

proc posipol {} {
    new nipol posz
        sel posz
        .connect tz
        .addkey1f 0 0
        .addkey1f 1 -20
        .addkey1f 2 -20
        .addkey1f 3 0
    sel ..

    new nipol posy
        sel posy
        .connect ty
        .addkey1f 0   0
        .addkey1f 0.2 1
        .addkey1f 0.4 -1
        .addkey1f 0.6 1
        .addkey1f 0.8 0
    sel ..

    new nipol posx
        sel posx
        .connect tx
        .addkey1f 0 0
        .addkey1f 1 -20
        .addkey1f 2 20
        .addkey1f 3 0
    sel ..

    new nipol ip
        sel ip
        #.setactive true
        .connect rxyz
        .addkey3f 0   0    0   0  
        .addkey3f 10 360 1080 720
    sel ..
}

sel /usr/scene
source licht.tcl


# this is the actual donut
new n3dnode donut_pos
    sel donut_pos

    .sxyz 0.5 0.5 0.5

    posipol


    new nmeshnode donut
        sel donut
        .setfilename "meshes/torus1.n3d"
    sel ..

    new nshadernode shader
        sel shader

        .setnumstages 1
        .setcolorop 0 "mul tex prev"    

        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform false
        .endtunit

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false
    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/lava.bmp none
    sel ..
sel ..

new n3dnode psystem_pos
    sel psystem_pos

    .sxyz 0.5 0.5 0.5

    posipol

    # define the shader for the particle system, the particle
    # renderer generates vertex colors, thus we turn off lighting,
    new nshadernode sn
        sel sn

        .setnumstages 1
        .setcolorop 0 "mul tex prev"

        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform false
        .endtunit

        .setlightenable false
        .setalphaenable true
        .setzwriteenable false
        .setalphablend one one

    sel ..

    new ntexarraynode tex0
        sel tex0
        .settexture 0 bmp/glow.bmp none
    sel ..

    # define the particle system hierarchy, consisting of a
    # nspriterender object at the top, and a nmeshemitter and
    # nmeshnode below it
    # the particle renderer (renders particles as viewer aligned sprites...)
    new nspriterender sr
        sel sr

        # the particle emitter (emits particles from vertices of a vbuffer)
        new nmeshemitter pemit
            sel pemit

            # here's the mesh which contains the vertices
            # that should be used as initial particle positions
            # note that it is set to 'readonly' and 'setactive false'
            # because it may not render itself (it only serves as
            # the source for the particle emitter
            new nmeshnode mesh
                sel mesh
                .setactive false
                .setreadonly true
                .setfilename "meshes/torus1.n3d"
            sel ..

            # particle emitter attributes...
            .setreptype oneshot
            .settimes 0 0 0
            .setlifetime 1.2
            .setfreq 800
            .setspeed 0
            .setaccel 0 0 0
            .setmeshnode mesh

        sel ..

        # particle renderer attributes...
        .setemitter pemit
        .setspin 0
        .setspinaccel 0
        .beginkeys 6
        .setkey 0 1.2 0.1 1.0 0.0 0.2 1.0
        .setkey 1 1.0 0.2 1.0 0.2 0.2 1.0
        .setkey 2 0.4 0.3 1.0 0.6 0.2 1.0
        .setkey 3 0.3 0.4 0.5 0.0 0.7 1.0
        .setkey 4 0.2 0.5 0.2 0.5 1.0 1.0
        .setkey 5 0.01 0.6 0.0 0.0 0.0 1.0
        .endkeys
    sel ..
sel ..

