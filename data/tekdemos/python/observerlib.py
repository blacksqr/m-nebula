#--------------------------------------------------------------------
#   observerlib.py
#   ===============
#   Support-Funktionen fuer Gfx-Server-Ersatz.
#
#   tod3d6
#   tod3d7hal
#   tod3d7emu
#   togl
#   orig
#   map_input
#   init_observer
#
#   26-Nov-99   floh    Input-Mappings mit .beginmap/.endmap
#   15-Feb-00   floh    + win2k aware (needs get_platform from nsh.tcl)
#
#   (C) 1999 A.Weissflog
#--------------------------------------------------------------------
#--------------------------------------------------------------------
#   init_gfxserver
#   Stellt die gewuenschten Gfx-Server-Parameter ein. Wird z.B.
#   beim Wechsel des Gfxserv per tod3d oder togl aufgerufen.
#   26-Jul-99   floh    created
#   28-Jul-99   bernd
#--------------------------------------------------------------------

import npython

def init_gfxserver():
	npython.set ('/sys/servers/gfx.setviewvolume',-0.1,+0.1,-0.075,+0.075,+0.1,+2500)
	npython.set ('/sys/servers/gfx.setclearcolor',0.5,0.5,0.5,0.5)
	npython.set ('/sys/servers/gfx.opendisplay')
#--------------------------------------------------------------------
#   tod3d7hal
#   Schaltet nach Direct3D V7 HAL Device
#   13-Jan-00   floh    created
#--------------------------------------------------------------------
def tod3d7hal():
	if npython.exists('/sys/servers/gfx'):
		npython.delete('/sys/servers/gfx')
	gfx=npython.new ('nd3d7server','/sys/servers/gfx')
	#gfx.setdisplaymode='dev(0)-w(640)-h(480)'
#	#gfx.setdisplaymode='dev(0)-w(580)-h(420)'
	gfx.setdisplaymode('dev(20)-w(640)-h(480)')
	init_gfxserver ()
#--------------------------------------------------------------------
#   tod3d7tnlhal
#   Switch to d3d7 hardware tl rasterizer (only if supported)
#   13-Jan-00   floh    created
#--------------------------------------------------------------------
def tod3d7tnlhal():
	if npython.exists('/sys/servers/gfx'):
		npython.delete('/sys/servers/gfx')
	gfx=npython.new ('nd3d7server','/sys/servers/gfx')
	if npython.exists('/sys/share/display/10]'):
		npython.set ('/sys/servers/gfx.setdisplaymode','dev(10)-w(580)-h(420)')
	else:
		#puts ('"d3d','hardware','tl','not','supported!"')
		npython.set ('/sys/servers/gfx.setdisplaymode','w(580)-h(420)')
	init_gfxserver ()
#--------------------------------------------------------------------
#   tod3d7emu
#   Schaltet nach Direct3D V7 RGB Emulation
#   13-Jan-00   floh    created
#--------------------------------------------------------------------
def tod3d7emu():
	if npython.exists('/sys/servers/gfx'):
		npython.delete('/sys/servers/gfx')
	gfx=npython.new ('nd3d7server','/sys/servers/gfx')
	npython.set ('/sys/servers/gfx.setdisplaymode','dev(20)-w(580)-h(420)')
	init_gfxserver ()
#--------------------------------------------------------------------
#   tod3d7ref
#   Switch to d3d7 reference rasterizer (only if supported)
#   13-Jan-00   floh    created
#--------------------------------------------------------------------
def tod3d7ref():
	if npython.exists('/sys/share/display/21]'):
		if npython.exists('/sys/servers/gfx'):
			npython.delete('/sys/servers/gfx')
		gfx=npython.new ('nd3d7server','/sys/servers/gfx')
		npython.set ('/sys/servers/gfx.setdisplaymode','dev(21)-w(580)-h(420)')
		init_gfxserver ()
	else:
		pass
		#puts ('"d3d','reference','rasterizer','not','supported!"')
#--------------------------------------------------------------------
#   tod3d8
#   Schaltet nach Direct3D V8
#   13-Jan-00   floh    created
#--------------------------------------------------------------------
def tod3d8():
	if npython.exists('/sys/servers/gfx'):
		npython.delete('/sys/servers/gfx')
	gfx=npython.new ('nd3d8server','/sys/servers/gfx')
	npython.set ('/sys/servers/gfx.setdisplaymode','dev(0)-w(580)-h(420)')
	init_gfxserver ()
#--------------------------------------------------------------------
#   togl
#   Schaltet nach OpenGL
#   26-Jul-99   floh    created
#--------------------------------------------------------------------
def togl():
	if npython.exists('/sys/servers/gfx'):
		npython.delete('/sys/servers/gfx')
	gfx=npython.new ('nglserver','/sys/servers/gfx/')
	npython.set ('/sys/servers/gfx.setdisplaymode','w(580)-h(420)')
	init_gfxserver ()
#------------------------------------------------------------------
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
	npython.set ('/usr/lookat.txyz',0,0,0)
	npython.set ('/usr/lookat.rxyz',-25,45,0)
	npython.set ('/usr/camera.tz',5)
def janaview():
#	puts ('"janaview"')
	npython.set ('/usr/lookat.txyz',0,30,0)
	npython.set ('/usr/lookat.rxyz',-15,0,0)
	npython.set ('/usr/camera.tz',35)
def lookn():
	npython.set ('/usr/lookat.txyz',0,0,10)
	npython.set ('/usr/lookat.rxyz',0,0,0)
	npython.set ('/usr/camera.txyz',0,100,20)
def lookw():
	npython.set ('/usr/lookat.txyz',0,0,10)
	npython.set ('/usr/lookat.rxyz',0,180,0)
	npython.set ('/usr/camera.tz',5)
def looke():
	npython.set ('/usr/lookat.txyz',0,0,10)
	npython.set ('/usr/lookat.rxyz',0,270,0)
	npython.set ('/usr/camera.tz',5)
def looks():
	npython.set ('/usr/lookat.txyz',0,0,10)
	npython.set ('/usr/lookat.rxyz',0,90,0)
	npython.set ('/usr/camera.tz',5)
#--------------------------------------------------------------------
#   map_input
#   Input-Mapping fuer nobserver Klasse.
#   26-Jul-99   floh    created
#   26-Nov-99   floh    .beginmap/.endmap
#   20-Dec-99   floh    + Konsole an/aus
#   22-Dec-99   floh    + an neues Input-Mapping angepasst
#--------------------------------------------------------------------
def map_input():
  	cwd=npython.psel()
	npython.sel('/sys/servers/input')
	npython.set('beginmap')
	npython.set('map','keyb0:shift.pressed','dolly')
	npython.set('map','keyb0:ctrl.pressed','orbit')
	npython.set('map','mouse0:btn0.pressed','pan')
	npython.set('map','mouse0:btn1.pressed','orbit')
	npython.set('map','mouse0:btn2.pressed','dolly')
	npython.set('map','keyb0:space.down',"script:orig()")
	npython.set('map','keyb0:j.down',"script:janaview()")
	npython.set('map','keyb0:esc.down',"script:npython.set('/sys/servers/console.toggle')")
	npython.set('endmap')
	npython.sel (cwd)

#--------------------------------------------------------------------
#   private_init_observer
#   23-May-00   floh    created
#--------------------------------------------------------------------
def private_init_observer():
	#tod3d7hal()
	#tod3d8()
	togl()
	input=npython.new ('ninputserver','/sys/servers/input')
	sgraph2=npython.new ('nscenegraph2','/sys/servers/sgraph2')
	shadow=npython.new ('nsbufshadowserver','/sys/servers/shadow')
	channel=npython.new ('nchannelserver','/sys/servers/channel')
	console=npython.new ('nconserver','/sys/servers/console')
	math=npython.new ('nmathserver','/sys/servers/math')
	particle=npython.new ('nparticleserver','/sys/servers/particle')
	specialfx=npython.new ('nspecialfxserver','/sys/servers/specialfx')
	file2=npython.new('nfileserver2','/sys/servers/file2')
	map_input ()
	scene=npython.new ('n3dnode','/usr/scene')
	camera=npython.new ('n3dnode','/usr/camera')
	lookat=npython.new ('n3dnode','/usr/lookat')
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
def init_observer():
	private_init_observer ()
	observer=npython.new ('nobserver','/observer')
	observer.setsleep(0)
	