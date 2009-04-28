#
#   Nebula TEKDEMO
#
#   spherical.tcl
#
#   Demonstrate Nebula spherical terrain renderer.
#   Nebula's spherical terrain renderer is based on Thatcher Ulrich's
#   adaptive lod terrain renderer presented on Gamasutra.
#
#   upd.:   18-May-2000 Bernd
#           08-Jan-2001 floh    added changes for detailmapping
#
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
sel ('/usr/scene')
# create vanilla lighting
execfile ('licht.py')
# display number of triangles
set ('/sys/servers/console.watch','gfx_triangles')
# the root 3d node
pos=new ('n3dnode','pos')
sel ('pos')
pos.txyz(10,-20,-40)
# some interpolators to move terrain around
ry=new ('nipol','ry')
sel ('ry')
ry.connect('ry')
ry.addkey1f(0,0)
ry.addkey1f(10,360)
sel ('..')
tz=new ('nipol','tz')
sel ('tz')
tz.connect('tz')
tz.addkey1f(0,50)
tz.addkey1f(15,-1000)
tz.addkey1f(30,50)
sel ('..')
# the terrain node...
terrain=new ('nterrainnode','terrain')
sel ('terrain')
# NOTE ! ADJUST SETDETAIL FOR
# HIGHER RESOLUTION + LESSER POLY-PLOPPING
# vs.
# LOWER RESOLUTION  + OBIOUS PLOPPING
# predefined value 170 = a low res-result
terrain.setdetail(170.000000)
terrain.setstaticerror(7.000000)
terrain.setradius(50.000000)
terrain.setuvscale(0.100000)
terrain.settreefile("../bmp/map.nqs")
sel ('..')
sn=new ('nshadernode','sn')
sel ('sn')
sn.setnumstages(2)
sn.setcolorop(0,"mul tex prev" )
sn.setcolorop(1,"mul tex prev" )
sn.begintunit(0)
sn.setaddress("wrap","wrap")
sn.setminmagfilter("linear_mipmap_nearest","linear")
sn.settexcoordsrc("uv0")
sn.setenabletransform(0)
sn.endtunit()
sn.begintunit(1)
sn.setaddress("wrap","wrap")
sn.setminmagfilter("linear_mipmap_linear","linear")
sn.settexcoordsrc("uv1")
sn.setenabletransform(0)
sn.endtunit()
sn.setlightenable(1)
sn.setdiffuse(1,1,1,1)
sn.setemissive(0,0,0,0)
sn.setambient(1,1,1,1)
sel ('..')
tex=new ('ntexarraynode','tex')
sel ('tex')
tex.settexture(0,'../bmp/marble.bmp','none')
tex.settexture(1,'../bmp/detailmap.bmp','none')
sel ('..')
sel ('..')
#-----
# EOF
#-----
