import site,sys,time
sys.path.append('c:\\development\\python\\dist\\src\\lib\\lib-tk')
import traceback
from Tkinter import *
sys.argv=[""]	# needed to allow Tkinter to load in a module..

execfile ("nsh.py")
execfile ("observerlib.py")
tl=0

def load_script(script,brightness):
	global nomads_home,demorunning,tl	#,bg
	sel ('/')
	delete ('/usr/scene')
	scene=new ('n3dnode','/usr/scene')
	set ('/sys/servers/console.unwatch')
	set ('/sys/servers/time.starttime')
	set ('/sys/servers/shadow.setcastshadows',0)
	#try:
	if tl:	# known global variable for sub windows
		Toplevel.destroy(tl)
	#except:
	#	pass
	tl=0
	set ('/sys/servers/time.lockdeltat',0.0)
	set ('/sys/servers/gfx.setclearcolor',brightness,brightness,brightness,brightness)
	bg = brightness
	set ('/sys/servers/gfx.setclearcolor', bg, bg, bg, bg)
	execfile ( script )

	#set ('demorunning',nomads_home/data/tekdemos/$script)
	# Standard-Kameraposition setzen
	orig ()
	

def menucall():
	global demo,scripts,currentdemo,win
	index=demo.get()
	currentdemo.set(scripts[index][0])
	try:
		s = scripts[index]
		load_script ( s[1], s[2] )
		if len(s) ==6 :
			if len(s[3])==3 :
				set('/usr/lookat.txyz',s[3][0],s[3][1],s[3][2])
			if len(s[4])==3:
				set('/usr/lookat.rxyz',s[4][0],s[4][1],s[4][2])
			if len(s[5])==1:
				set('/usr/camera.tz',s[5][0])
	except:
		traceback.print_exception (sys.exc_type,sys.exc_value,sys.exc_traceback)


def GFXSetGL():
	global GFXDevice
	GFXDevice.set("GL")
	togl()
def GFXSetDX():
	global GFXDevice
	GFXDevice.set("DX8")
	tod3d8()

def viewsource():
	pass
	
def toggleconsole():
	set('/sys/servers/console.toggle')
	
scripts = [\
	["Mesh Interplation",'meshanim.py',0.5],\
	["Mesh Mixing",'meshmix.py',0.5],\
	["Skin Deformation",'boneanim.py',0.5],\
	["Hierarchical Animation",'newplanets.py',0],\
	["Animated Light",'lightanim.py',0.2],\
	["Flip Flop (Texture)",'flipflop.py',0.5],\
	["Shaders",'shadernode.py',0],\
	["Flat Terrain",'flatterrain.py',0,[15,10,0],[0,0,0],[30]],\
	["Bigger Flat Terrain",'flatterrain1.py',0,[20,10,0],[-20,0,0],[40]],\
	["Billboards + Sprites",'billboards.py',0,[.7,0,0],[-5,25,0],[22]],\
	
	["Spherical Terrain",'sphericalterrain.py',0,[0,5,0],[0,0,0],[20]],\
	#["Sky Dome",'sky1.py',0],\
	["Static Mesh Emitter",'staticmeshemitter.py',0],\
	["Mesh Particle Emitter",'meshemitter.py',0,[0,0,0],[0,0,0],[5]],\
	["Shadows",'shadow.py',0,[],[],[40]],\
	["Alpha Test",'alphatest.py',0],\
	["Exponential Fog",'fog.py',0,[0,5,0],[0,0,0],[0]],\
	#['particle.py',0,[0,0,0],[-45,0,0],[10]],\
	]

connect("localhost:gfxserv")

	
win = Tk()
win.title("Nebula Python Demos")
win.geometry("270x100+580+1")	
win.resizable(0,0)

# need some Tkinter variables for "menu" callbacks
demo=IntVar()
currentdemo=StringVar()
GFXDevice=StringVar()
GFXDevice.set("GL")

currentdemo.set(scripts[0][0])
currentdemoindex=0

# main frame
f0 = Frame(win,relief=SUNKEN,borderwidth=1)
f0.pack(side=TOP, expand=0)

# Button to select the GFX driver
RB = Menubutton(f0, width=213,text=GFXDevice, underline=0,indicatoron=1,relief=RAISED,textvariable=GFXDevice)
RB.pack(side=TOP, fill=X)
RB.menu=Menu(RB, tearoff=0)
RB.menu.add_radiobutton   (label=" Direct3D V8 ",command=GFXSetDX ) 
RB.menu.add_radiobutton  (label=" OpenGL ",command=GFXSetGL )  
RB['menu'] = RB.menu

# Buttons to match tcl version :)
b= Button(f0,height=1,text="Current Source",command=viewsource)
b.pack(side=RIGHT,fill=X,expand=1)
b1= Button(f0,height=1,text="Toggle Console",command=toggleconsole)
b1.pack(side=RIGHT,fill=X,expand=1)

# now for the second frame
f1=Frame(win,bd=2,relief=SUNKEN)
f1.pack(expand=YES,fill=BOTH )
f1.label=Label(f1,text="Select Demo:")
f1.label.pack(side=LEFT,expand=NO,fill=X )

# build the menu dynamically from previously specified array
RB1 = Menubutton(f1, width=213, underline=0,indicatoron=1,relief=RAISED,textvariable=currentdemo)
RB1.pack(side=TOP, fill=X)
RB1.menu=Menu(RB1, tearoff=0)
x=0
for l in scripts:
	RB1.menu.add_radiobutton   (label=l[0],command=menucall,variable=demo,value=x ) 
	x+=1
RB1['menu'] = RB1.menu

# setup for the first one
init_observer ()
set ('/observer.setsleep',0.02)
set ('/observer.setgrid',0)
set ('/observer.startsinglestep')
demo.set(0)
menucall()
# have to use a loop as Python doesn't support an "interperter single step call back"
# See the Trigger function in npythonrun.cc
#win.mainloop()		# don't use this as it'll never call back...
try:
	while 1:
		set ('/observer.rendersinglestep')
		win.update()
		#time.sleep(0.02)
except:
	pass
disconnect()
sys.exit()

