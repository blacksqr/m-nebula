##
## Utility functions for nebula
## Converted from the TCl versions...
##
## Created by Andy Miller, Dec 2001
## 

from npython import *
def init_gfxserver():
	sel('/sys/servers/gfx')
	set('setviewvolume', -0.1, 0.1, -0.075, 0.075, 0.1, 2500.0)
	set('setclearcolor', 0.5, 0.5, 0.5, 0.5)
	set('opendisplay')

#--------------------------------------------------------------------
#   tod3d7hal
#   Schaltet nach Direct3D V7 HAL Device
#   13-Jan-00   floh    created   
#--------------------------------------------------------------------
def tod3d7hal():
	if (npython.exists ('/sys/servers/gfx')):
		delete ('/sys/servers/gfx')
	new ('nd3d7server','/sys/servers/gfx')
	set('/sys/servers/gfx.setdisplaymode','dev(0)-w(580)-h(420)')
	init_gfxserver()

#--------------------------------------------------------------------
#   orig
#   Stellt Kamera und Lookat-Node auf Ausgangs-Position ein
#   26-Jul-99   floh    created
#   28-Jul-99   bernd   Standard-Settings von Rechts Oben
#   10-Nov-99   bernd   Camera Positioning currently is somehow strange /
#                       may not work as expected.
#                       Camera Animation will not work.
#                       Will be fixed later.
#--------------------------------------------------------------------
def orig():
	set('/usr/lookat.txyz',0,0,0)
	set('/usr/lookat.rxyz',-25,45,0)
	set('/usr/camera.tz',5)


def map_input():
  	cwd=psel()
	sel('/sys/servers/input')
	set('beginmap')
	set('map','keyb0:shift.pressed','pan')
	set('map','keyb0:ctrl.pressed','orbit')
	set('map','mouse0:btn0.pressed','pan')
	set('map','mouse0:btn1.pressed','orbit')
	set('map','mouse0:btn2.pressed','dolly')
	set('map','keyb0:space.down',"script:orig")
	set('map','keyb0:j.down',"script:janaview")
	set('map','keyb0:esc.down',"script:/sys/servers/console.toggle")
	set('endmap')
	sel (cwd)



def private_init_observer():
	tod3d7hal()
	new ('ninputserver','/sys/servers/input')
	new ('nscenegraph2','/sys/servers/sgraph2')
	new ('nsbufshadowserver','/sys/servers/shadow')
	new ('nchannelserver','/sys/servers/channel')
	new ('nconserver','/sys/servers/console')
	new ('nmathserver','/sys/servers/math')
	new ('nparticleserver','/sys/servers/particle')
	new ('nspecialfxserver','/sys/servers/specialfx')
	
	map_input ()

	new ('n3dnode','/usr/scene')
	new ('n3dnode','/usr/camera')
	new ('n3dnode','/usr/lookat')
	orig ()

#--------------------------------------------------------------------
#   init_observer
#   26-Jul-99   floh    created
#   27-Jul-99   bernd   Plattformabhängige Server
#   12-Aug-99   bernd   Zwar wird immer noch die Unterscheidung
#                       zwischen den Plattformen vorgenommen, aber
#                       eigentlich verwenden alle die selben Server.
#   18-Aug-99   bernd   Wenn 3ddevice da ist, dann so initialisieren
#                       wie es dort steht.
#   15-Feb-00   floh    + Win2k aware (needs nsh.tcl however)
#   01-Apr-00   floh    + added particle server
#   11-May-00   floh    + added specialfx server
#   23-May-00   floh    + ruft private_init_observer auf
#--------------------------------------------------------------------
def init_observer ():
	private_init_observer()
	new ('nobserver','/observer')
	set('/observer.setsleep',0)

def load_script ( script, brightness ):
#	global nomads_home
#	global demorunning
#	global bg
	
	#foreach job [after info] { after cancel $job }
	sel('/')
	delete ('/usr/scene')
	new ('n3dnode','/usr/scene')
	sel('/sys/servers/console')
	set('unwatch')
	sel('/sys/servers/time')
	set('starttime')
	sel('/sys/servers/shadow')
	set('setcastshadows',0)
	
	# Eventuelle TK-Subfenster killen
	#destroy .r
	#destroy .ped2
	#update
	
	# Eventuellen TIMELOCK korrigieren
	sel('/sys/servers/time')
	set('lockdeltat',0.0)
	sel('/sys/servers/gfx')
	set('setclearcolor',brightness,brightness,brightness,brightness)
	#set ('bg',brightness)
	
	execfile(script)
	
	#source $nomads_home/data/tekdemos/$script
	#set    demorunning $nomads_home/data/tekdemos/$script
	
	# Standard-Kameraposition setzen
	orig()


init_observer()
load_script('newplanets.py', 0.5)
set('/observer.setsleep', 0.02)
set('/observer.setgrid',0)
set('/observer.start')
#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
