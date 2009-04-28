#-------------------------------------------------------------------------------
#   lod.tcl
#   =======
#   Flip geometry based on lod value (not really a real world example).
#
#   (C) 2001 RadonLabs GmbH
#-------------------------------------------------------------------------------

sel /usr/scene
source licht.tcl

# a top 3d node
new n3dnode pos
    sel pos

    # a flipflop which selects meshes based on lod
    new nflipflop lodsel
        sel lodsel
        
        new nmeshnode mesh1
        mesh1.setfilename "meshes/torus1.n3d"

        new nmeshnode mesh2
        mesh2.setfilename "meshes/torus4.n3d"

        new nmeshnode mesh3
        mesh3.setfilename "meshes/torus5.n3d"

        .setchannel lod
        .setreptype oneshot
        .addkey 0      mesh3
        .addkey 0.998  mesh2
        .addkey 0.999  mesh1
        .addkey 1.000  mesh1
    sel ..

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

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false
    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/marble.bmp none
    sel ..
sel ..   
  
    
