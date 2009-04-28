#--------------------------------------------------------------------
#   projectedsmoke.tcl
#   ==================
#   An image projected on a wall ok smoke.
#
#--------------------------------------------------------------------
sel ('usr/scene')
pos=new ('n3dnode','pos')
sel ('pos')
pos.tz(-30)
pos.ty(-30)
pos.tx(-30)
#    new nipol ry
#        sel ry
#        .connect ry
#        .addkey1f 0 0
#        .addkey1f 20 360
#    sel ..
sr=new ('nspriterender','sr')
sel ('sr')
# the particle emitter (emits particles from vertices of a vbuffer)
pemit=new ('nmeshemitter','pemit')
sel ('pemit')
# this mesh is just a line, so that the 'smoke' forms
# a projection walll
mesh=new ('nmeshnode','mesh')
sel ('mesh')
mesh.setactive(0)
mesh.setreadonly(1)
mesh.setfilename("../meshes/line.n3d")
sel ('..')
# particle emitter attributes...
pemit.settimes(0,0,0)
pemit.setlifetime(5)
pemit.setfreq(100)
pemit.setspeed(0)
pemit.setaccel(0,3,0)
pemit.setmeshnode('mesh')
sel ('..')
# particle renderer attributes...
sr.setemitter('pemit')
sr.setspin(0)
sr.setspinaccel(0)
sr.beginkeys(3)
sr.setkey(0,2.0,0.0,0.0,0.0,0.0,1.0)
sr.setkey(1,3.0,0.2,0.8,0.8,0.8,1.0)
sr.setkey(2,2.0,0.1,0.0,0.0,0.0,1.0)
sr.endkeys()
sel ('..')
# the shadernode is responsible to project the image
# on the smoke, we use 2 layers, the base layer defines
# the "particle", the second is the projected image
sn=new ('nshadernode','sn')
sel ('sn')
sn.setnumstages(2)
sn.setcolorop(0,"mul tex prev" )
sn.setcolorop(1,"mul tex prev" )
sn.sxyz1(0.05,0.025,0.05)
sn.begintunit(0)
sn.setaddress('wrap','wrap')
sn.setminmagfilter('linear_mipmap_nearest','linear')
sn.settexcoordsrc('uv0')
sn.setenabletransform(0)
sn.endtunit()
sn.begintunit(1)
sn.setaddress('wrap','wrap')
sn.setminmagfilter('linear_mipmap_nearest','linear')
sn.settexcoordsrc('eyespace')
sn.setenabletransform(1)
sn.endtunit()
sn.setlightenable(0)
sn.setalphaenable(1)
sn.setalphablend('one','one')
sn.setzwriteenable(0)
sel ('..')
tex=new ('ntexarraynode','tex')
sel ('tex')
tex.settexture(0,'../bmp/trees.bmp','none')
tex.settexture(1,'../bmp/particle.bmp','none')
sel ('..')
sel ('..')
