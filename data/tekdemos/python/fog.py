#
#   fog.tcl
#
#   NEBULA tek demos showing various fogging effects.
#
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
#--------------------------------------------------------------------
#   gen_lib
#   Generate a lib of shadernodes,
#--------------------------------------------------------------------
def gen_lib():
	cwd=psel()
#	set ('cwd','[psel]')
	if (exists('/usr/lib')):
		sel ('/usr')
		delete ('lib')
	sel ('/usr')
	lib=new ('nroot','lib')
	sel ('lib')
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
	shader.setambient(1,1,1,1)
	shader.setalphaenable(0)
	shader.setfogenable(1)
	sel ('..')
	sel(cwd)

#--------------------------------------------------------------------
#   Generate named a textured rotating cube at position (x,y,z).
#--------------------------------------------------------------------
def gen_cube(name,x,y,z):
	name1=new ('n3dnode',name)
	sel (name)
	name1.txyz(x,y,z)
	# generate vbuffer, texture and material
	mesh=new ('nmeshnode','mesh')
	mesh.setfilename('../meshes/torus1.n3d')
	# linknode to actual shadernode, shared
	# shadernodes will speed the whole thing up
	shader=new ('nlinknode','shader')
	shader.settarget('/usr/lib/shader')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,'../bmp/marble.bmp','none')
	sel ('..')
	# generate a rotation interpolator
	ip=new ('nipol','ip')
	sel ('ip')
	ip.connect('rxyz')
	ip.addkey3f(0,0,0,0)
	ip.addkey3f(2,360,360,360)
	sel ('..')
	sel ('..')
#--------------------------------------------------------------------
#   Generate a grid of rotating cubes...
#--------------------------------------------------------------------
def gen_grid(name,y):
	global gen_cube
	name1=new ('n3dnode',name)
	sel (name)
	name1.ty(y)
	name1.ry(-30)
	for z in range(-150,10,10):
		gen_cube ('cube_'+str(z),-10,0,z)
	sel ('..')
#--------------------------------------------------------------------
#   Define one ambient and 1 directional light.
#--------------------------------------------------------------------
def gen_lights():
	sel ('/usr/scene')
	dlight=new ('n3dnode','dlight')
	sel ('dlight')
	light1=new ('n3dnode','light1')
	sel ('light1')
	light=new ('nlightnode','light')
	sel ('light')
	light.setattenuation(1,0,0)
	light.settype('directional')
	light.setcolor(0.7,0.2,0.7,1)
	sel ('..')
	sel ('..')
	amb=new ('nlightnode','amb')
	sel ('amb')
	amb.settype('ambient')
	amb.setcolor(0.3,0.3,0.3,1)
	sel ('..')
	sel ('..')
#--------------------------------------------------------------------
#   A few small helper functions to set fog and bg colors from
#   Tk scalers.
#--------------------------------------------------------------------
def set_red(c):
	color=set('/usr/scene/fog.getcolor')
	red=float(c)
	green=color[1]
	blue=color[2]
	alpha=color[3]
	set ('/usr/scene/fog.setcolor',red, green, blue, alpha)
	set ('/sys/servers/gfx.setclearcolor',red,green,blue,alpha)

def set_green(c):
	color=set('/usr/scene/fog.getcolor')
	red=color[0]
	green=float(c)
	blue=color[2]
	alpha=color[3]
	set ('/usr/scene/fog.setcolor',red, green, blue, alpha)
	set ('/sys/servers/gfx.setclearcolor',red,green,blue,alpha)
	
def set_blue(c):
	color=set('/usr/scene/fog.getcolor')
	red=color[0]
	green=color[1]
	blue=float(c)
	alpha=color[3]
	set ('/usr/scene/fog.setcolor',red, green, blue, alpha)
	set ('/sys/servers/gfx.setclearcolor',red,green,blue,alpha)
	
def set_fogdensity(c):
	set( '/usr/scene/fog.setdensity',float(c))
	
#--------------------------------------------------------------------
#   main
#   Execution starts here.
#--------------------------------------------------------------------
sel ('/usr/scene')
# setup lighting
gen_lights ()
# generate 2 grids of rotating cubes
gen_lib ()
gen_grid ('grid1',-10)
gen_grid ('grid2',0)
gen_grid ('grid3',+10)
# generate a fog node
fog=new ('nfognode','fog')
sel ('fog')
fog.setmode('exp')
fog.setcolor(0.5,0.5,0.5,1.0)
fog.setdensity(0.01)
sel ('..')
#--------------------------------------------------------------------
#   Create a small tk window with scales for density and fog color.
#--------------------------------------------------------------------
global tl
	
tl=Toplevel ()
tl.title("Exponential Fog Controls" )
tl.geometry('571x121+1+420')
tl.resizable(0,0)

f_density=IntVar()
f_red=IntVar()
f_green=IntVar()
f_blue=IntVar()

f_density.set(set ('/usr/scene/fog.getdensity'))
f_color=set ('/sys/servers/gfx.getclearcolor')
f_red.set(f_color[0])
f_green.set(f_color[1])
f_blue.set(f_color[2])

f1=Frame (tl)
f1.pack (side=TOP,expand=1)
w2=IntVar()
w3=IntVar()
s1=Scale (f1,length=550,orient=HORIZONTAL,from_=0.00001,to=0.10,showvalue=0,label="",variable=f_density,resolution=0.0001,command=set_fogdensity)
l1=Label (f1,text="Density" ,width=10)
#t1.pack (s1,side=LEFT,expand=1)
l1.pack (side=LEFT,expand=0)
s1.pack (side=RIGHT,expand=1)

f2=Frame (tl)
f2.pack (side=TOP,expand=1)
s2=Scale (f2,length=550,orient=HORIZONTAL,from_=0.0,to=1.0,showvalue=0,label ="",variable=f_red,resolution=0.01,command=set_red)
l2=Label (f2,text="Red" ,width=10)
l2.pack (side=LEFT,expand=0)
s2.pack (side=RIGHT,expand=1)

f3=Frame (tl)
f3.pack (side=TOP,expand=1)
s3=Scale (f3,length=550,orient=HORIZONTAL,from_=0.0,to=1.0,showvalue=0,label ="",variable=f_green,resolution=0.01,command=set_green)
l3=Label (f3,text="Green" ,width=10)
l3.pack (side=LEFT,expand=0)
s3.pack (side=RIGHT,expand=1)

f4=Frame (tl)
f4.pack (side=TOP,expand=1)
s4=Scale (f4,length=550,orient=HORIZONTAL,from_=0.0,to=1.0,showvalue=0,label ="",variable=f_blue,resolution=0.01,command=set_blue)
l4=Label (f4,text="Blue" ,width=10)
l4.pack (side=LEFT,expand=0)
s4.pack (side=RIGHT,expand=1)

#--------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------
