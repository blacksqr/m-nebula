#
#   Basic Routines used by all TEKDEMOS
#
#   created:    08-Nov-2000
#   upd.:       08-Nov-2000
#   
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

proc plainobject { name mesh txt } {

    new n3dnode $name
        sel $name

        new nmeshnode mesh
        mesh.setfilename  $mesh

        new nshadernode shader
            sel shader
            .setnumstages 1
            .setcolorop 0 "mul tex prev"
        
            .begintunit 0
                .setaddress         wrap   wrap
                .setminmagfilter    linear linear
                .settexcoordsrc     uv0
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
            .settexture 0 $txt none
        sel ..
    sel ..

}
