# ---
# $parser:ntclserver$ $class:n3dnode$ $ver:Sun Dec 17 21:42:55 2000$
# ---
sel ('/usr/scene')
sky=new ('n3dnode','sky')
sel ('sky')
#sky.setminlod(0.000000)
sky.txyz(0.000000,-100.000000,0.000000)
sky.rxyz(0.000000,0.000000,0.000000)
sky.sxyz(1.000000,1.000000,1.000000)
sky.setlockviewer(1)
dome=new ('nskydome','dome')
sel ('dome')
# This nipol will control the dome's day/night factor
# set by the Sun angle.  Angle of 0 (or 360) is directly overhead.
# angle 70 = twilight, 100 = dark, 250 = dawn, 290 = day
#
# I imagine most will programatically connect this, but it demos
# like this nicely
nip=new ('nipol','nip')
nip.connect('setsunangle')
nip.addkey1f(0,0)
nip.addkey1f(10,360)
nip.setscale(10)
sel ('..')
# The dome's "Cloud Factor" determines the speed of the cloud cover
#
# 1000 = default, 100 = REAL FAST, 10 = nauseating
dome.setcloudfactor(1000)
shader=new ('nshadernode','shader')
sel ('shader')
tex=new ('ntexarraynode','tex')
sel ('tex')
tex.settexture(0,"../bmp/skystars.bmp","none")
tex.settexture(1,"../bmp/cloudlay1.tga","none")
sel ('..')
shader.setnumstages(2)
shader.setcolorop(0,"mul prev tex" )
shader.setcolorop(1,"ipol tex prev tex.a" )
#    .setcolorop 2 "mul primary prev"
shader.setconst(0,0.000000,0.000000,1.000000,1.000000)
shader.setconst(1,1.000000,0.000000,0.000000,1.000000)
shader.begintunit(0)
shader.setaddress("wrap","wrap")
shader.setminmagfilter('nearest_mipmap_nearest','nearest_mipmap_nearest')
shader.settexcoordsrc("uv0")
shader.setenabletransform(0)
shader.txyz(0.000000,0.000000,0.000000)
shader.rxyz(0.000000,0.000000,0.000000)
shader.sxyz(1.000000,1.000000,1.000000)
shader.endtunit()
shader.begintunit(1)
shader.setaddress("wrap","wrap")
shader.setminmagfilter('nearest_mipmap_nearest','nearest_mipmap_nearest')
shader.settexcoordsrc("uv1")
shader.setenabletransform(0)
shader.txyz(0.000000,0.000000,0.000000)
shader.rxyz(0.000000,0.000000,0.000000)
shader.sxyz(1.000000,1.000000,1.000000)
shader.endtunit()
shader.setdiffuse(1.000000,1.000000,1.000000,1.000000)
shader.setemissive(1.000000,1.000000,1.000000,1.000000)
shader.setambient(1.000000,1.000000,1.000000,1.0000000)
#   A render pri of -10 should cause this to draw BEHIND everything else
#   thus scale shouldn't become a factor.
shader.setrenderpri(-10)
shader.setlightenable(1)
shader.setalphaenable(1)
shader.setzwriteenable(0)
#    .setfogenable true
shader.setalphablend('srcalpha','destalpha')
#    .setzfunc "always"
shader.setcullmode("none")
shader.setcolormaterial("material")
sel ('..')
sel ('/usr/scene')
# ---
# EOF
