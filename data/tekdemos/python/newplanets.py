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
execfile ('routines.py')
#--------------------------------------------------------------------
#   make_rotator
#--------------------------------------------------------------------
def rot(time,channel):
	channel1=new ('nipol',channel)
	sel (channel)
	channel1.connect(channel)
	channel1.addkey1f(0,0)
	channel1.addkey1f(time,360)
	sel ('..')
#--------------------------------------------------------------------
#   main()
#--------------------------------------------------------------------
sel ('/usr/scene')
execfile ('licht.py')
sun=plainobject ('sun','../meshes/smooth.n3d','../bmp/lava.bmp')
sel ('sun')
rot (20,'ry')
planet1=plainobject ('planet1','../meshes/smooth.n3d','../bmp/pla2.bmp')
sel ('planet1')
planet1.tx(2)
planet1.sxyz(0.4,0.4,0.4)
rot (10,'ry')
rot (5,'rz')
sel ('..')
extrarot1=new ('n3dnode','extrarot1')
sel ('extrarot1')
extrarot1.rx (-5)
rot (25,'ry')
planet2=plainobject ('planet2','../meshes/smooth.n3d','../bmp/pla3.bmp')
sel ('planet2')
planet2.tx(3)
planet2.tz(2)
planet2.sxyz(0.3,0.3,0.3)
rot (6,'ry')
rot (10,'rx')
moon1=plainobject ('moon1','../meshes/smooth.n3d','../bmp/pla1.bmp')
sel ('moon1')
moon1.sxyz(0.3,0.3,0.3)
moon1.tx(1.5)
sel ('..')
extrarot3=new ('n3dnode','extrarot3')
sel ('extrarot3')
rot (4,'ry')
moon2=plainobject ('moon2','../meshes/smooth.n3d','../bmp/pla1.bmp')
sel ('moon2')
moon2.sxyz(0.2,0.2,0.2)
moon2.tx(3)
sel ('..')
sel ('..')
sel ('..')
sel ('..')
extrarot2=new ('n3dnode','extrarot2')
sel ('extrarot2')
extrarot2.rx (25)
rot (15,'ry')
planet3=plainobject ('planet3','../meshes/smooth.n3d','../bmp/pla1.bmp')
sel ('planet3')
planet3.tx(-1.3)
planet3.tz(-1.3)
planet3.sxyz(0.1,0.1,0.1)
rot (1,'rz')
sel ('..')
sel ('..')
sel ('..')
