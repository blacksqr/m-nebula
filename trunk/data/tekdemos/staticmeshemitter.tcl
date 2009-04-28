#
#   Demonstrates static mesh emitter, simply render a "static" particle
#   on mesh vertex positions.
#
#   (C) 2001 RadonLabs GmbH
#
sel    /usr/scene
source licht.tcl

new n3dnode pos
    sel pos

    .sxyz 0.3 0.3 0.3

    new nipol rotz
        sel rotz
        .connect rz
        .addkey1f 0 0
        .addkey1f 5 360
    sel ..

    new nipol rotx
        sel rotx
        .connect rx
        .addkey1f 0 0
        .addkey1f 4 360
    sel ..

    new nspriterender sr
        sel sr

        new nstaticmeshemitter me
            sel me

            new nmeshipol mesh
                sel mesh

                new nmeshnode mesh1
                mesh1.setfilename "meshes/torus1.n3d"
                mesh1.setreadonly true

                new nmeshnode mesh2
                mesh2.setfilename "meshes/torus2.n3d"
                mesh2.setreadonly true

                new nmeshnode mesh3
                mesh3.setfilename "meshes/torus3.n3d"
                mesh3.setreadonly true

                new nmeshnode mesh4
                mesh4.setfilename "meshes/torus4.n3d"
                mesh4.setreadonly true

                new nmeshnode mesh5
                mesh5.setfilename "meshes/torus5.n3d"
                mesh5.setreadonly true

                .setreadonly true

                .beginkeys 7
                    .setkey 0 0 mesh1
                    .setkey 1 1 mesh2
                    .setkey 2 2 mesh3
                    .setkey 3 3 mesh5
                    .setkey 4 4 mesh4
                    .setkey 5 5 mesh5
                    .setkey 6 6 mesh1
                .endkeys         
    
                .setupdatecoord true
                .setupdatenorm  true
                .setupdatecolor false
                .setupdateuv0   true
                .setupdateuv1   false
                .setupdateuv2   false
                .setupdateuv3   false 
            sel ..

            .setlifetime 10
            .setmeshnode mesh
            
        sel ..

#        #.setminlod 0.000000
        .setreptype "loop"
        .setchannel "time"
        .setscale 1.000000
        .setstretch false
        .setspin 0.000000
        .setspinaccel 0.000000
        .setemitter "me"
        .beginkeys 7
        .setkey 0 0.100000 0.000000 0.300000 0.000000 0.000000 1.000000
        .setkey 1 0.200000 0.000000 0.300000 0.300000 0.000000 1.000000
        .setkey 2 0.300000 0.000000 0.000000 0.300000 0.000000 1.000000
        .setkey 3 0.400000 0.000000 0.000000 0.300000 0.300000 1.000000
        .setkey 4 0.500000 0.000000 0.000000 0.000000 0.300000 1.000000
        .setkey 5 0.400000 0.000000 0.000000 0.000000 0.000000 1.000000
        .setkey 6 0.100000 0.000000 0.300000 0.000000 0.000000 1.000000
        .endkeys
    sel ..

    new nshadernode shd
        sel shd
#        #.setminlod 0.000000
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
#        #.setminlod 0.000000
        .settexture 0 "bmp/glow.bmp" "none"
        .setgenmipmaps 0 true
        .setgenmipmaps 1 true
        .setgenmipmaps 2 true
        .setgenmipmaps 3 true
    sel ..

sel ..
