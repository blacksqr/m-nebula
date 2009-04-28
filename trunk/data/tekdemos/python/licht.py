#
#   Std-Lights, used by most of the demos
#
#   upd.:   30-Jul-99
#
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
sel ('/usr/scene')
dlight=new ('n3dnode','dlight')
sel ('dlight')
light1=new ('n3dnode','light1')
sel ('light1')
light=new ('nlightnode','light')
sel ('light')
light.setattenuation(1,0,0)
light.settype('point')
light.setcolor(0.1,0.5,1,1)
sel ('..')
light1.txyz(-50,5,50)
sel ('..')
light2=new ('n3dnode','light2')
sel ('light2')
light=new ('nlightnode','light')
sel ('light')
light.setattenuation(1,0,0)
light.settype('point')
light.setcolor(1,0.5,0.1,1)
sel ('..')
light2.txyz(50,5,50)
sel ('..')
amb=new ('nlightnode','amb')
sel ('amb')
amb.settype('ambient')
amb.setcolor(0.2,0.2,0.2,0)
sel ('..')
sel ('..')
