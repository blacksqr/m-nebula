#
# This script sets up some randomness in the ODE node creation
#
#
proc create_window { } {

# frame .r is deleted by the tekdeoms frame when the
# demo is switched.  This makes it easy to integrate 
# into the Nebula Tekdemos
	toplevel .r
    wm title     .r     "ODE Physics Demo" 
    wm geometry  .r     214x150+580+175
    wm resizable .r     0 0

    frame .r.ff1 -relief sunken -borderwidth 1
    pack  .r.ff1 -side top -expand 1 -fill both

    button   .r.ff1.fbb1   -text "Sphere"   -command { make_sphere }
    pack     .r.ff1.fbb1  -side right -fill x -expand 1
    button   .r.ff1.fbb3   -text "Box"   -command { make_box }
    pack     .r.ff1.fbb3  -side right -fill x -expand 1
    button   .r.ff1.fbb6   -text "Cylinder"   -command { make_cylinder }
    pack     .r.ff1.fbb6  -side right -fill x -expand 1

    frame .r.ff2 -relief sunken -borderwidth 1
    pack  .r.ff2 -side top -expand 1 -fill both
    button   .r.ff2.fbb1   -text "10 Spheres"   -command { make_some sphere }
    pack     .r.ff2.fbb1  -side right -fill x -expand 1
    button   .r.ff2.fbb3   -text "10 Boxes"   -command { make_some box }
    pack     .r.ff2.fbb3  -side right -fill x -expand 1
    button   .r.ff2.fbb6   -text "10 Cylinders"   -command { make_some cylinder }
    pack     .r.ff2.fbb6  -side right -fill x -expand 1
	
    frame .r.ff3 -relief sunken -borderwidth 1
    pack  .r.ff3 -side top -expand 1 -fill both
    button   .r.ff3.fbb1   -text "Sphere Chain"   -command { chain sphere }
    pack     .r.ff3.fbb1  -side right -fill x -expand 1
    button   .r.ff3.fbb3   -text "Box Chain"   -command { chain box }
    pack     .r.ff3.fbb3  -side right -fill x -expand 1
    button   .r.ff3.fbb6   -text "Cylinder Chain"   -command { chain cylinder }
    pack     .r.ff3.fbb6  -side right -fill x -expand 1

    frame .r.ff4 -relief sunken -borderwidth 1
    pack  .r.ff4 -side top -expand 1 -fill both
    button   .r.ff4.fbb1   -text "Toggle Debug Lines"   -command { /observer.togglevis }
    pack     .r.ff4.fbb1  -side right -fill x -expand 1
}

proc globit { } {
  global global_incr
  set global_incr 0
}
 
proc make_node_mesh { type x z } {
   global global_incr
   incr global_incr 1

   new n3dnode /usr/scene/mesh$global_incr
   sel /usr/scene/mesh$global_incr

   .txyz $x 75.000000 $z
   .rxyz 0.000000 0.000000 0.000000
   .sxyz 1.000000 1.000000 1.000000

   new nmeshnode mesh
   sel mesh
      .setfilename "../../bin/win32/gg_sk1.n3d"
      .setreadonly false
   sel ..


   new ntexarraynode tex
   tex.settexture 0 "../../bin/win32/g_texture.bmp" "none"

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

   sel ..
   new nodenode /usr/scene/mesh$global_incr/no
   /usr/scene/mesh$global_incr/no.setcollisionlink /usr/scene/mesh$global_incr/mesh $type
}

proc make_plane { } {
   global global_incr
   incr global_incr 1
  	
   new n3dnode /usr/scene/plane$global_incr

   new nmeshnode /usr/scene/plane$global_incr/mesh
      /usr/scene/plane$global_incr/mesh.setfilename "../../bin/win32/xzplane.n3d"
      /usr/scene/plane$global_incr/mesh.setreadonly false

   new ntexarraynode /usr/scene/plane$global_incr/tex
      /usr/scene/plane$global_incr/tex.settexture 0 "../../bin/win32/marb2.bmp" "none"

   new nshadernode /usr/scene/plane$global_incr/sn
   /usr/scene/plane$global_incr/sn.setnumstages 1
      /usr/scene/plane$global_incr/sn.setcolorop 0 "mul tex prev"
      /usr/scene/plane$global_incr/sn.begintunit 0
      /usr/scene/plane$global_incr/sn.setaddress wrap wrap
      /usr/scene/plane$global_incr/sn.setminmagfilter linear linear
      /usr/scene/plane$global_incr/sn.settexcoordsrc uv0
      /usr/scene/plane$global_incr/sn.setenabletransform false
      /usr/scene/plane$global_incr/sn.endtunit
      /usr/scene/plane$global_incr/sn.setlightenable true
      /usr/scene/plane$global_incr/sn.setdiffuse  1 1 1 1
      /usr/scene/plane$global_incr/sn.setemissive 0 0 0 0
      /usr/scene/plane$global_incr/sn.setambient  1 1 1 1
      /usr/scene/plane$global_incr/sn.setalphaenable false

   new nodenode /usr/scene/plane$global_incr/n2
   /usr/scene/plane$global_incr/n2.makeplane 0 1 0 0
}

proc make_some_mesh { type } {
	for {set x 1 } {$x < 11 } { incr x 1 } {
	   set xx [expr rand() * 100 - 50]
	   set zz [expr rand() * 100 - 50]
       make_node_mesh $type $xx $zz
	}
}

proc make_some { type } {
	for {set x 1 } {$x < 11 } { incr x 1 } {
       make_$type
	}
}

proc chain { type } {
    global global_incr

    set xx [expr rand() * 100 - 50]
    set zz 0
	make_node_mesh $type $xx $zz

#   This means -- connect the newest mesh to the world 
#   at its current location - becomes the chain's pivot
#   /usr/scene/mesh$global_incr/no.connectfixed null 

    new nipol /usr/scene/mesh$global_incr/no/ni
	/usr/scene/mesh$global_incr/no/ni.connect addforce
	/usr/scene/mesh$global_incr/no/ni.addkey3f 0 0 0 0
	/usr/scene/mesh$global_incr/no/ni.addkey3f 10 -100 50000 100
	/usr/scene/mesh$global_incr/no/ni.addkey3f 10 100 50000 -100
	/usr/scene/mesh$global_incr/no/ni.addkey3f 20 -500 0 500
	/usr/scene/mesh$global_incr/no/ni.addkey3f 30 500 0 -500
	/usr/scene/mesh$global_incr/no/ni.addkey3f 40 0 0 0
	/usr/scene/mesh$global_incr/no/ni.setscale [expr rand() * .5]

	for {set x 1 } {$x < 9 } { incr x 1 } {
        set zz [expr $x * 19]
		set other $global_incr
		make_node_mesh $type $xx $zz
#	    /usr/scene/mesh$global_incr/no.connectwithslider /usr/scene/mesh$other/no 0 1 0
	    /usr/scene/mesh$global_incr/no.connectwithball /usr/scene/mesh$other/no $xx 75 [expr $zz - 5]
#	    /usr/scene/mesh$global_incr/no.connectwithhinge /usr/scene/mesh$other/no $xx 75 [expr $zz + 10] 0 1 0
	}
}

proc make_sphere { } {
   global global_incr
   incr global_incr 1
   
   set x [expr rand() * 100 - 50]
   set z [expr rand() * 100 - 50]

   new n3dnode /usr/scene/sphere$global_incr
   /usr/scene/sphere$global_incr.txyz $x 75.000000 $z
   /usr/scene/sphere$global_incr.rxyz 0.000000 0.000000 0.000000
   /usr/scene/sphere$global_incr.sxyz 1.000000 1.000000 1.000000

   new nodenode /usr/scene/sphere$global_incr/node$global_incr
   /usr/scene/sphere$global_incr/node$global_incr.makesphere [expr rand() * 10 + 10]
}

proc make_box { } {
   global global_incr
   incr global_incr 1
   
   set x [expr rand() * 100 - 50]
   set z [expr rand() * 100 - 50]

   new n3dnode /usr/scene/box$global_incr
   /usr/scene/box$global_incr.txyz $x 75.000000 $z
   /usr/scene/box$global_incr.rxyz 0.000000 0.000000 0.000000
   /usr/scene/box$global_incr.sxyz 1.000000 1.000000 1.000000

   new nodenode /usr/scene/box$global_incr/node$global_incr

   set bminx [expr rand() * -10 ]
   set bminy [expr rand() * -10 ]
   set bminz [expr rand() * -10 ]
   set bmaxx [expr rand() * 10]
   set bmaxy [expr rand() * 10 + 5]
   set bmaxz [expr rand() * 10]

   /usr/scene/box$global_incr/node$global_incr.makebox $bminx $bminy $bminz $bmaxx $bmaxy $bmaxz

}

proc make_cylinder { } {
   global global_incr
   incr global_incr 1
   
   set x [expr rand() * 100 - 50]
   set z [expr rand() * 100 - 50]

   new n3dnode /usr/scene/cyl$global_incr
   /usr/scene/cyl$global_incr.txyz $x 75.000000 $z
   /usr/scene/cyl$global_incr.rxyz 0.000000 0.000000 0.000000
   /usr/scene/cyl$global_incr.sxyz 1.000000 1.000000 1.000000

   new nodenode /usr/scene/cyl$global_incr/node$global_incr

   set radius [expr rand() * 5 + 2 ]
   set length [expr rand() * 10 + 5 ]

   /usr/scene/cyl$global_incr/node$global_incr.makecylinder $radius $length

}

globit

make_plane
create_window

/observer.togglevis
