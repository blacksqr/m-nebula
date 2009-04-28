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
sel ('/usr/scene')
execfile ('licht.py')
animmesh=new ('n3dnode','animmesh')
sel ('animmesh')
animmesh.sxyz(0.3,0.3,0.3)
rotz=new ('nipol','rotz')
sel ('rotz')
rotz.connect('rz')
rotz.addkey1f(0,0)
rotz.addkey1f(5,360)
sel ('..')
rotx=new ('nipol','rotx')
sel ('rotx')
rotx.connect('rx')
rotx.addkey1f(0,0)
rotx.addkey1f(4,360)
sel ('..')
mesh=new ('nmeshipol','mesh')
sel ('mesh')
mesh1=new ('nmeshnode','mesh1')
mesh1.setfilename("../meshes/torus1.n3d")
mesh1.setreadonly(1)
mesh2=new ('nmeshnode','mesh2')
mesh2.setfilename("../meshes/torus2.n3d")
mesh2.setreadonly(1)
mesh3=new ('nmeshnode','mesh3')
mesh3.setfilename("../meshes/torus3.n3d")
mesh3.setreadonly(1)
mesh4=new ('nmeshnode','mesh4')
mesh4.setfilename("../meshes/torus4.n3d")
mesh4.setreadonly(1)
mesh5=new ('nmeshnode','mesh5')
mesh5.setfilename("../meshes/torus5.n3d")
mesh5.setreadonly(1)
mesh.beginkeys(7)
mesh.setkey(0,0,'mesh1')
mesh.setkey(1,1,'mesh2')
mesh.setkey(2,2,'mesh3')
mesh.setkey(3,3,'mesh5')
mesh.setkey(4,4,'mesh4')
mesh.setkey(5,5,'mesh5')
mesh.setkey(6,6,'mesh1')
mesh.endkeys()
mesh.setupdatecoord(1)
mesh.setupdatenorm(1)
mesh.setupdatecolor(0)
mesh.setupdateuv0(1)
mesh.setupdateuv1(0)
mesh.setupdateuv2(0)
mesh.setupdateuv3(0)
sel ('..')
shader=new ('nshadernode','shader')
sel ('shader')
shader.setnumstages(1)
shader.setcolorop(0,"mul tex prev" )
shader.begintunit(0)
shader.setaddress('wrap','wrap')
shader.setminmagfilter('linear_mipmap_linear','linear')
shader.settexcoordsrc('uv0')
shader.setenabletransform(0)
shader.endtunit()
shader.setlightenable(1)
shader.setdiffuse(1,1,1,1)
shader.setemissive(0,0,0,0)
shader.setambient(1,1,1,1)
shader.setalphaenable(0)
sel ('..')
tex=new ('ntexarraynode','tex')
sel ('tex')
tex.settexture(0,'../bmp/lava.bmp','none')
sel ('..')
sel ('..')
