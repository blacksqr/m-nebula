#-------------------------------------------------------------------------------
#   flipflop.tcl
#   ============
#   Flip through textures using a flipflop object.
#-------------------------------------------------------------------------------

sel    /usr/scene
source licht.tcl

new n3dnode animmesh
    sel animmesh
    .sxyz 0.3 0.3 0.3

    new nipol rotz
        sel rotz
        .connect rz
        .addkey1f 0 0
        .addkey1f 10 360
    sel ..

    new nipol rotx
        sel rotx
        .connect rx
        .addkey1f 0 0
        .addkey1f 20 360
    sel ..

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

        beginkeys 7
            setkey 0 0  mesh1
            setkey 1 3  mesh2
            setkey 2 6  mesh3
            setkey 3 9  mesh5
            setkey 4 12 mesh4
            setkey 5 15 mesh5
            setkey 6 18 mesh1
        endkeys         
    
        setupdatecoord true
        setupdatenorm  true
        setupdatecolor false
        setupdateuv0   true
        setupdateuv1   false
        setupdateuv2   false
        setupdateuv3   false
 
    sel ..

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
    sel ..

    new nflipflop flip
        sel flip

        new ntexarraynode tex0
            sel tex0
            .settexture 0 bmp/marble.bmp none
            .settexture 1 bmp/autobahn.bmp none
        sel ..

        new ntexarraynode tex1
            sel tex1
            .settexture 0 bmp/brick.bmp none
            .settexture 1 bmp/autobahn.bmp none
        sel ..

        new ntexarraynode tex2
            sel tex2
            .settexture 0 bmp/funky.bmp none
            .settexture 1 bmp/autobahn.bmp none
        sel ..

        .addkey  0.0 tex0
        .addkey  5.0 tex1
        .addkey 10.0 tex2
        .addkey 15.0 tex0
    sel ..
sel .. 
