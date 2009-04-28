#-------------------------------------------------------------------------------
#   lod.tcl
#   =======
#   Flip geometry based on lod value (not really a real world example).
#
#   (C) 2001 RadonLabs GmbH
#-------------------------------------------------------------------------------
sel ('/usr/scene')
execfile ('licht.py')
# a top 3d node
pos=new ('n3dnode','pos')
sel ('pos')
# a flipflop which selects meshes based on lod
lodsel=new ('nflipflop','lodsel')
sel ('lodsel')
mesh1=new ('nmeshnode','mesh1')
mesh1.setfilename("../meshes/torus1.n3d")
mesh2=new ('nmeshnode','mesh2')
mesh2.setfilename("../meshes/torus4.n3d")
mesh3=new ('nmeshnode','mesh3')
mesh3.setfilename("../meshes/torus5.n3d")
lodsel.setchannel('lod')
lodsel.setreptype('oneshot')
lodsel.addkey(0,'mesh3')
lodsel.addkey(0.998,'mesh2')
lodsel.addkey(0.999,'mesh1')
lodsel.addkey(1.000,'mesh1')
sel ('..')
sn=new ('nshadernode','sn')
sel ('sn')
sn.setnumstages(1)
sn.setcolorop(0,"mul tex prev" )
sn.begintunit(0)
sn.setaddress('wrap','wrap')
sn.setminmagfilter('linear','linear')
sn.settexcoordsrc('uv0')
sn.setenabletransform(0)
sn.endtunit()
sn.setlightenable(1)
sn.setdiffuse(1,1,1,1)
sn.setemissive(0,0,0,0)
sn.setambient(1,1,1,1)
sn.setalphaenable(0)
sel ('..')
tex=new ('ntexarraynode','tex')
sel ('tex')
tex.settexture(0,'../bmp/marble.bmp','none')
sel ('..')
sel ('..')
