#
#   Nebula TEKDEMO
#
#   NewPlanets
#
#   created by:                 Andy Miller
#   based on the version by:    Andre Weissflog

#
#   Std-Lights, used by most of the demos
#
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

def default_lights():
	sel('/usr/scene')
	
	new('n3dnode','dlight')
	sel ('dlight')
	
	new ('n3dnode','light1')
	sel ('light1')
	new ('nlightnode','light')
	sel ('light')
	set ('setattenuation', 1, 0, 0) 
	set ('settype','point')
	set ('setcolor',0.1, 0.5, 1, 1)
	sel ('..')
	set ('txyz', -50, 5, 50)
	sel ('..')
	new ('n3dnode','light2')
	sel ('light2')
	new ('nlightnode','light')
	sel ('light')
	set ('setattenuation', 1, 0, 0) 
	set ('settype','point')
	set ('setcolor',1, 0.5, 0.1, 1)
	sel ('..')
	set ('txyz', -50, 50, 50)
	sel ('..')
	new ('nlightnode','amb')
	sel ('amb')
	set ('settype','ambient')
	set ('setcolor', 0.2, 0.2, 0.2, 0)
	sel ('..')
	sel ('..')


def plainobject ( name, meshfile, texture ):
	new ('n3dnode', name)
	sel (name)
	
	new ('nmeshnode','mesh')
	set ('mesh.setfilename',meshfile)
	
	new ('nshadernode','shader')
	sel ('shader')
	set('setnumstages', 1)
	set('setcolorop', 0 ,"mul tex prev")
	
	set('begintunit', 0)
	set('setaddress','wrap','wrap')
	set('setminmagfilter','linear','linear')
	set('settexcoordsrc','uv0')
	set('setenabletransform',0)
	set('endtunit')
	
	set('setlightenable',1)
	set('setdiffuse',1,1,1,1)
	set('setemissive',0,0,0,0)
	set('setambient',1,1,1,1)
	set('setalphaenable',0)
	sel ('..')
	new ('ntexarraynode','tex')
	sel('tex')
	set('settexture',0,texture,'none')
	sel ('..')
	sel ('..')



#--------------------------------------------------------------------
#   make_rotator
#--------------------------------------------------------------------
def rot ( timein, channel ):
	new ('nipol', channel)
	sel (channel)
	set('connect',channel)
	set('addkey1f',0,0)
	set('addkey1f',timein, 360.0)
	sel('..')



#--------------------------------------------------------------------
#   main()
#--------------------------------------------------------------------
try:
	sel ('/usr/scene')
	default_lights()
	
	plainobject ('sun','meshes/smooth.n3d','bmp/lava.bmp')
	sel ('sun')
	rot (20,'ry')
		
	plainobject ('planet1','meshes/smooth.n3d','bmp/pla2.bmp')
	sel ('planet1')
	set('tx',2)
	set('sxyz',0.4,0.4,0.4)
	rot (10,'ry')
	rot (5,'rz')
	sel ('..')
	
	new ('n3dnode','extrarot1')
	sel ('extrarot1')
	set ('rx',-5)
	rot (25 ,'ry')
	
	plainobject ('planet2','meshes/smooth.n3d','bmp/pla3.bmp')
	sel ('planet2')
	set('tx',3)
	set('tz',2)
	set('sxyz',0.3,0.3,0.3)
	rot(6,'ry')
	rot(10,'rx')
	
	plainobject ('moon1','meshes/smooth.n3d','bmp/pla1.bmp')
	sel ('moon1')
	set('sxyz',0.3,0.3,0.3)
	set('tx',1.5)
	sel ('..')
	
	new ('n3dnode','extrarot3')
	sel ('extrarot3')
	rot (4,'ry')
	plainobject ('moon2','meshes/smooth.n3d','bmp/pla1.bmp')
	sel ('moon2')
	set('sxyz', 0.2, 0.2, 0.2)
	set('tx',3)
	sel ('..')
	sel ('..')
	
	
	sel ('..')
	sel ('..')
	
	new ('n3dnode','extrarot2')
	sel ('extrarot2')
	
	set('rx',25)
	rot (15 ,'ry')
	
	plainobject ('planet3','meshes/smooth.n3d',' bmp/pla1.bmp')
	sel ('planet3')
	set('tx', -1.3)
	set('tz', -1.3)
	set('sxyz', 0.1, 0.1, 0.1)
	rot (1,'rz')
	sel ('..')
	sel ('..')
	
	sel ('..')
except:
    print "Unexpected error:", sys.exc_info()[0]
    raise

    
