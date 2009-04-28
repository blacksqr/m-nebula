#
#   NEBULA WAVEFRONT OBJ VIEWER
#
#   viewer.tcl
#
#
#   (C) COPYRIGHT 2001 RADONLABS GMBH
#
global ('nomads_home')
global ('curdir')
#--------------------------------------------------------------------
#   loadobj
#   Load an *.obj file (smoothing remains unchanged).
#--------------------------------------------------------------------
def loadobj(objfile):
	catch ('{','exec','polypipe_nonsmooth.bat',objfile,'temp.n3d','}','err')
	set ('/usr/scene/obj/mesh.setfilename','temp.n3d')
#--------------------------------------------------------------------
#   loadobjsmoothed
#   Load an *.obj file (all edges will be smoothed).
#--------------------------------------------------------------------
def loadobjsmoothed(objfile):
	catch ('{','exec','polypipe.bat',objfile,'temp.n3d','}','err')
	set ('/usr/scene/obj/mesh.setfilename','temp.n3d')
#--------------------------------------------------------------------
#   loadtexture
#   Load a *.bmp file to be mapped onto the obj.
#--------------------------------------------------------------------
def loadtexture(texfile):
	set ('/usr/scene/obj/shader/tex.settexture',0 texfile 'none')
#--------------------------------------------------------------------
#   proc load_psys_dialog
#   Create file open dialog and load selected particle system.
#   28-Feb-2001   jernd    created
#--------------------------------------------------------------------
def load_dialog(loadproc):
# create a file load dialog
	set ('types','{')
	{ ("nebula mesh" ,'.msh','}')
	{ ("obj",'.obj','}')
	{ ("texture",'.bmp','}')
set ('file','[tk_getOpenFile','-filetypes',types,'-title')
if ('{$file','!=')
$loadproc (file)
												puts ("Cancelled")
#--------------------------------------------------------------------
#   createemptyobject
#   Create an empty 3d node structure.
#--------------------------------------------------------------------
										def createemptyobject():
											sel ('/usr/scene')
											execfile ('licht.py')
											set ('/usr/scene/dlight/amb.setcolor',0 0 0 0)
											obj=new ('n3dnode','obj')
											sel ('obj')
											mesh=new ('nmeshnode','mesh')
											shader=new ('nshadernode','shader')
											sel ('shader')
											shader.setnumstages(1)
											shader.setcolorop(0,"mul tex prev" )
											shader.begintunit(0)
											shader.setaddress('wrap','wrap')
											shader.setminmagfilter('linear','linear')
											shader.settexcoordsrc('uv0')
											shader.setenabletransform(0)
											shader.endtunit()
											shader.setlightenable(1)
											shader.setdiffuse(1,1,1,1)
											shader.setemissive(0,0,0,0)
											shader.setambient(0,0,0,0)
											shader.setalphaenable(0)
											tex=new ('ntexarraynode','tex')
											sel ('tex')
											tex.setgenmipmaps(0,1)
											sel ('..')
											sel ('..')
											sel ('..')
#--------------------------------------------------------------------
#   create_window
#   Erzeugt das Tk-Frontend.
#--------------------------------------------------------------------
										def create_window():
											global ('3ddevice')
											global ('nomads_home')
											global ('bg')
											set ('platform','[get_platform]')
											wm ('title','.',"Nebula OBJ Viewer" )
											wm ('geometry','.','214x200+580+1')
											wm ('resizable','.',0,0)
											frame ('.f0','-relief','sunken','-borderwidth',1)
											pack ('.f0','-side','top','-expand',1)
# D3D/OpenGL-Switcher
											menubutton ('.f0.mb','-width',213,'-text',3ddevice,'-textvariable','3ddevice','-indicatoron',1,'-menu','.f0.mb.m1','-relief','raised','-direction','below')
											menu ('.f0.mb.m1','-tearoff',0)
											if ('{($platform=="win9x")','||','($platform=="win2k")}','{')
											.f0.mb.m1('add','radiobutton','-label',",'Direct3D','V7','HAL',",'-variable','3ddevice','-command','{','tod3d7hal')
											set ('/sys/servers/gfx.setclearcolor',bg bg bg bg '}')
											.f0.mb.m1('add','radiobutton','-label',",'Direct3D','V7','TnL','HAL',",'-variable','3ddevice','-command','{','tod3d7tnlhal')
											set ('/sys/servers/gfx.setclearcolor',bg bg bg bg '}')
											.f0.mb.m1('add','radiobutton','-label',",'Direct3D','V7','EMU',",'-variable','3ddevice','-command','{','tod3d7emu')
											set ('/sys/servers/gfx.setclearcolor',bg bg bg bg '}')
										.f0.mb.m1('add','radiobutton','-label',",'OpenGL',",'-variable','3ddevice','-command','{','togl')
										set ('/sys/servers/gfx.setclearcolor',bg bg bg bg)
									pack ('.f0.mb','-side','top','-fill','x')
									button ('.f0.ba','-height',1,'-text',"load obj" ,'-command','{','load_dialog','loadobj','}')
									pack ('.f0.ba','-side','top','-fill','x','-expand',1)
									button ('.f0.bb','-text',"load obj smoothed" ,'-command','{','load_dialog','loadobjsmoothed','}')
									pack ('.f0.bb','-side','top','-fill','x','-expand',1)
									button ('.f0.bc','-text',"load texture" ,'-command','{','load_dialog','loadtexture','}')
									pack ('.f0.bc','-side','top','-fill','x','-expand',1)
									button ('.f0.bd','-text',"unload texture" ,'-command','{','/usr/scene/obj/shader/tex.settexture',0,'none','none','}')
									pack ('.f0.bd','-side','top','-fill','x','-expand',1)
									button ('.f0.be','-text',"light switch" ,'-command','{','licht2','}')
									pack ('.f0.be','-side','top','-fill','x','-expand',1)
									scale ('.f0.roty','-length',160,'-orient','horizontal','\')
									-from (0,'-to',359,'-showvalue',0,'-variable','bbb','\')
									-command ('{','/usr/scene/obj.ry','}')
									pack ('.f0.roty','-side','bottom','-expand',1)
								global ('lichtswitch')
								set ('lichtswitch',0)
# -------------------------------------------------------------------
								def licht2():
									global ('home')
									global ('lichtswitch')
									if ('{',lichtswitch==1,'}','{')
									puts ("Colorful")
									set ('/usr/scene/dlight/light1/light.setcolor',0.1 0.5 1 1)
									set ('/usr/scene/dlight/light2/light.setcolor',1 0.5 0.1 1)
									set ('lichtswitch',0)
								puts ("White")
								set ('/usr/scene/dlight/light1/light.setcolor',1 1 1 1)
								set ('/usr/scene/dlight/light2/light.setcolor',1 1 1 1)
								set ('lichtswitch',1)
							return ()
#--------------------------------------------------------------------
#   main
#   HIER GEHTS LOS
#--------------------------------------------------------------------
						if ('[','catch','{','set','test',3ddevice,'}','err',']','{')
						set ('3ddevice',",'OpenGL',")
# NomadsHome setzen
					set ('nomads_home','[/sys/share/assigns/home.gets]')
					set ('home',nomads_home)
# Libs laden
					execfile (nomads_home/bin/nsh.tcl)
					execfile (nomads_home/bin/observerlib.tcl)
					execfile (nomads_home/bin/loadtk.tcl)
					cd (nomads_home/data/tekdemos)
# observer-Objekt initialisieren
					init_observer ()
# Window erzeugen
					create_window ()
# Create the empty object
					createemptyobject ()
# und los....
					set ('/observer.setsleep',0.02)
					set ('/observer.setgrid',1)
					set ('/observer.start')
#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
