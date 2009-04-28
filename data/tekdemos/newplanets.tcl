#
#   Nebula TEKDEMO
#
#   NewPlanets
#
#
#   created:    Andre Weissflog
#   upd:        10-Nov-2000
#   
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

source routines.tcl


#--------------------------------------------------------------------
#   make_rotator
#--------------------------------------------------------------------
proc rot { time channel } {
    new nipol $channel
        sel $channel
        .connect  $channel
        .addkey1f 0     0
        .addkey1f $time 360
    sel ..
}


#--------------------------------------------------------------------
#   main()
#--------------------------------------------------------------------
sel /usr/scene
source licht.tcl

plainobject sun meshes/smooth.n3d bmp/lava.bmp
    sel sun
    rot 20 ry

    plainobject planet1 meshes/smooth.n3d bmp/pla2.bmp
        sel planet1
        .tx    2
        .sxyz 0.4 0.4 0.4
        rot 10 ry
        rot  5 rz
    sel ..

    new n3dnode extrarot1
        sel extrarot1

        rx -5

        rot 25 ry
        plainobject planet2 meshes/smooth.n3d bmp/pla3.bmp
            sel planet2
            .tx    3
            .tz    2
            .sxyz 0.3 0.3 0.3
            rot  6 ry
            rot 10 rx

            plainobject moon1 meshes/smooth.n3d bmp/pla1.bmp
                sel moon1
                .sxyz 0.3 0.3 0.3
                .tx   1.5
            sel ..

            new n3dnode extrarot3
                sel extrarot3
                rot 4 ry
                plainobject moon2 meshes/smooth.n3d bmp/pla1.bmp
                    sel moon2
                    .sxyz 0.2 0.2 0.2
                    .tx   3
                sel ..
            sel ..


        sel ..

    sel ..

    new n3dnode extrarot2
        sel extrarot2

        rx  25    
        rot 15 ry

        plainobject planet3 meshes/smooth.n3d bmp/pla1.bmp
            sel planet3
            .tx    -1.3
            .tz    -1.3
            .sxyz 0.1 0.1 0.1
            rot 1 rz
        sel ..
    sel ..

sel ..
    
    
    
