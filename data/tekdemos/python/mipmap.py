#
#   Nebula TEKDEMO
#
#   MipMapping
#
#   created:    15-May-99   Bernd Beyreuther
#   upd.:       03-Nov-2000 Bernd Beyreuther
#
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
sel ('/usr/scene')
amb=new ('nlightnode','amb')
sel ('amb')
amb.settype('ambient')
amb.setcolor(1,1,1,1)
sel ('..')
# the cube with mipmapping activated ---------------
obj=new ('n3dnode','obj')
sel ('obj')
ty (-3)
tx (-35)
# Some animation -------------------------------
tz=new ('nipol','tz')
sel ('tz')
tz.connect('tz')
tz.addkey1f(0,-8)
tz.addkey1f(5,-100)
tz.addkey1f(10,-8)
sel ('..')
# Rotation -------------------------------------
roty=new ('nipol','roty')
sel ('roty')
roty.connect('ry')
roty.addkey1f(0,0)
roty.addkey1f(2,360)
sel ('..')
rotx=new ('nipol','rotx')
sel ('rotx')
rotx.connect('rx')
rotx.addkey1f(0,0)
rotx.addkey1f(3,360)
sel ('..')
vbuf=new ('nvbufnode','vbuf')
sel ('vbuf')
vbuf.setvbuffer('../meshes/cube1.n3d')
sel ('..')
tex=new ('ntexnode','tex')
sel ('tex')
settexture ('../bmp/256.bmp','none')
sel ('..')
mat=new ('nmatnode','mat')
sel ('mat')
mat.setstate('texture_enable',1)
mat.setstate('texture_min_filter','linear_mipmap_linear')
mat.setstate('texture_address_u','wrap')
mat.setstate('texture_address_v','wrap')
mat.setambient(1,1,1,1)
sel ('..')
sel ('..')
# Make a copy of the first cube
# Modify its position and
# switch of Mipmapping
obj.clone('obj1')
sel ('obj1')
tx (-70)
sel ('mat')
mat.setstate('texture_min_filter','linear')
sel ('..')
sel ('..')
