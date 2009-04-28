#
#   Nebula TEKDEMO
#
#   MeshAnim
#   Does Interpolation between several 3D-Meshes
#
#   created:    15-May-99   Bernd Beyreuther
#   upd.:       07-Nov-2000 Bernd Beyreuther
#   
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
# 

sel    /usr/scene
source licht.tcl

new n3dnode animmesh
    sel animmesh

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
            setkey 0 0 mesh1
            setkey 1 1 mesh2
            setkey 2 2 mesh3
            setkey 3 3 mesh5
            setkey 4 4 mesh4
            setkey 5 5 mesh5
            setkey 6 6 mesh1
        endkeys         
    
        setupdatecoord true
        setupdatenorm  true
        setupdatecolor false
        setupdateuv0   true
        setupdateuv1   false
        setupdateuv2   false
        setupdateuv3   false
 
    sel ..

    new nshadernode shader
        sel shader

        .setnumstages 1
        .setcolorop 0 "mul tex prev"    

        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear_mipmap_linear linear
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
