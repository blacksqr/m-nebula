#--------------------------------------------------------------------
#   shadernode.tcl
#   ==============
#   Demonstrate the new shadernodes which replace the obsolete
#   nmatnode objects.
#
#   Please note that some shaders may not work on all host systems.
#   Please note also that some shaders cannot be dynamically lighted
#   without requiring an additional "post-diffuse-lighting" operation.
#
#   23-Oct-00   floh    created
#--------------------------------------------------------------------
#--------------------------------------------------------------------
#   Lets define a few cool shaders...
#--------------------------------------------------------------------
#--------------------------------------------------------------------
#   modulate_shader
#   Most simple shader, simply defines one texture layer which is
#   modulated with the polygons base color.
#--------------------------------------------------------------------

from Tkinter import *
global tl
shaderindex=IntVar()
currentshader=StringVar()


def modulate_shader():
	sn=new ('nshadernode','sn')
#	sel ('sn')
	sn.setnumstages(1)
	sn.setcolorop(0,"mul tex prev" )
	sn.begintunit(0)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(0)
	sn.endtunit()
	sn.setlightenable(1)
	sn.setdiffuse(1,1,1,1)
	sn.setemissive(0,0,0,0)
	sn.setambient(1,1,1,1)
	sn.setalphaenable(0)
#	sel ('..')
	tex=new ('ntexarraynode','tex')
#	sel ('tex')
	tex.settexture(0,'../bmp/marble.bmp','none')
#	sel ('..')
#--------------------------------------------------------------------
#   gloss_shader
#   2 texture layers, the first is modulated on the base color,
#   the second texture layer uses environment mapping to give a
#   shiny appearance.
#--------------------------------------------------------------------
def gloss_shader():
	sn=new ('nshadernode','sn')
	sel ('sn')
	sn.setnumstages(2)
	sn.setcolorop(0,"mul tex prev" )
	sn.setcolorop(1,"add tex prev" )
# the first texture unit defines the base map
	sn.begintunit(0)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(0)
	sn.endtunit()
# the second texture unit defines the environment mapping effect
	sn.begintunit(1)
	sn.setaddress('clamp','clamp')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('spheremap')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.setlightenable(1)
	sn.setdiffuse(1,1,1,1)
	sn.setemissive(0,0,0,0)
	sn.setambient(1,1,1,1)
	sn.setalphaenable(0)
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,'../bmp/marble.bmp','none')
	tex.settexture(1,'../bmp/colorgloss.bmp','none')
	sel ('..')
#--------------------------------------------------------------------
#   shinethrough_shader
#   The base texture defines an (uv animated) lava effect, which
#   shines through the second static texture layer. The shinethrough
#   spots are defined by the second layers alpha map.
#--------------------------------------------------------------------
def shinethrough_shader():
	sn=new ('nshadernode','sn')
	sel ('sn')
	sn.setnumstages(3)
	sn.setcolorop(0,"replace tex" )
	sn.setcolorop(1,"ipol tex prev tex.a" )
	sn.setcolorop(2,"mul primary prev" )
	sn.begintunit(0)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.sxyz0(3,3,3)
	sn.begintunit(1)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.sxyz1(4,4,4)
# this animates the lava's uv coordinates
	ip=new ('nipol','ip')
	sel ('ip')
	ip.connect('txyz0')
	ip.addkey3f(0,0,0,0)
	ip.addkey3f(10,5,0,0)
	sel ('..')
	sn.setlightenable(1)
	sn.setdiffuse(1,1,1,1)
	sn.setemissive(0,0,0,0)
	sn.setambient(1,1,1,1)
	sn.setalphaenable(0)
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,'../bmp/lava.bmp','none')
	tex.settexture(1,'../bmp/pla1.bmp','../bmp/mask.bmp')
	sel ('..')
#--------------------------------------------------------------------
#   blend_shader
#   Blend between two textures, post-multiply the polygons base color
#   to get the lighting correct.
#--------------------------------------------------------------------
def blend_shader():
	sn=new ('nshadernode','sn')
	sel ('sn')
	sn.setnumstages(3)
	sn.setcolorop(0,"replace tex" )
	sn.setcolorop(1,"ipol tex prev const" )
	sn.setcolorop(2,"mul primary prev" )
	sn.begintunit(0)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.sxyz0(5,5,5)
	sn.begintunit(1)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.sxyz1(5,5,5)
	sn.setlightenable(1)
	sn.setdiffuse(1,1,1,1)
	sn.setemissive(0,0,0,0)
	sn.setambient(1,1,1,1)
	sn.setalphaenable(0)
# interpolate the color constant's alpha for the blend
# between the two textures
	ip=new ('nipol','ip')
	sel ('ip')
	ip.connect('setconst0')
	ip.addkey4f(0,0,0,0,0)
	ip.addkey4f(5,1,1,1,1)
	ip.addkey4f(10,0,0,0,0)
	sel ('..')
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,'../bmp/brick.bmp','none')
	tex.settexture(1,'../bmp/marble.bmp','none')
	sel ('..')
#--------------------------------------------------------------------
#   funkyplasma_shader()
#   Three shifting base texture layers to generate a plasma effect,
#   plus an environment mapped gloss layer to make it look cool.
#--------------------------------------------------------------------
def funkyplasma_shader():
	sn=new ('nshadernode','sn')
	sel ('sn')
	sn.setnumstages(3)
	sn.setcolorop(0,"replace tex" )
	sn.setcolorop(1,"adds -tex prev" )
	sn.setcolorop(2,"add tex prev" )
	sn.begintunit(0)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.sxyz0(2,2,2)
	sn.begintunit(1)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.sxyz1(2,2,2)
	sn.begintunit(2)
	sn.setaddress('clamp','clamp')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('spheremap')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.setlightenable(1)
	sn.setdiffuse(1,1,1,1)
	sn.setemissive(0,0,0,0)
	sn.setambient(1,1,1,1)
	sn.setalphaenable(0)
	ip=new ('nipol','ip')
	sel ('ip')
	ip.connect('txyz0')
	ip.addkey3f(0,0,0,0)
	ip.addkey3f(15,1,2,4)
	sel ('..')
	ip1=new ('nipol','ip1')
	sel ('ip1')
	ip1.connect('txyz1')
	ip1.addkey3f(0,0,0,0)
	ip1.addkey3f(20,-1,-2,-4)
	sel ('..')
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,'../bmp/funky.bmp','none')
	tex.settexture(1,'../bmp/funky.bmp','none')
	tex.settexture(2,'../bmp/colorgloss.bmp','none')
	sel ('..')
#--------------------------------------------------------------------
#   environ_shader()
#   Marmor base texture with some environment mirrored in...
#--------------------------------------------------------------------
def environ_shader():
	sn=new ('nshadernode','sn')
	sel ('sn')
	sn.setnumstages(2)
	sn.setcolorop(0,"mul tex prev" )
	sn.setcolorop(1,"ipol tex prev const.a" )
	sn.begintunit(0)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('uv0')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.sxyz0(2,2,2)
	sn.begintunit(1)
	sn.setaddress('wrap','wrap')
	sn.setminmagfilter('linear','linear')
	sn.settexcoordsrc('spheremap')
	sn.setenabletransform(1)
	sn.endtunit()
	sn.setconst0(0.2,0.2,0.2,0.2)
	sn.setlightenable(1)
	sn.setdiffuse(1,1,1,1)
	sn.setemissive(0,0,0,0)
	sn.setambient(1,1,1,1)
	sn.setalphaenable(0)
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,'../bmp/marble.bmp','none')
	tex.settexture(1,'../bmp/autobahn.bmp','none')
	sel ('..')
#--------------------------------------------------------------------
#   gen_visual()
#   A simple visual hierarchie using the new nmeshnode object
#   and nshadernode objects. The shader object is always
#   generated under the name of 'pos/sn' so that it can be
#   replaced later.
#--------------------------------------------------------------------
def gen_visual():
	global modulate_shader
	pos=new ('n3dnode','pos')
	sel ('pos')
# the donut mesh
	mesh=new ('nmeshnode','mesh')
	sel ('mesh')
	mesh.setfilename('../meshes/torus1.n3d')
	sel ('..')
# generate the initial shadernode
	modulate_shader()
# let the donut rotate
	ip=new ('nipol','ip')
	sel ('ip')
	ip.connect('rxyz')
	ip.addkey3f(0,0,0,0)
	ip.addkey3f(20,360,1080,720)
	sel ('..')
	sel ('..')
#--------------------------------------------------------------------
#   toggle_anim
#   Toggle animating the donut.
#--------------------------------------------------------------------
def toggle_anim():
	if (set('/usr/scene/pos/ip.getconnect') =='rxyz'):
		set ('/usr/scene/pos/ip.disconnect')
	else:
		set ('/usr/scene/pos/ip.connect','rxyz')
	
#--------------------------------------------------------------------
#   replace_shader()
#   Delete the current shader and replace it with a new one.
#--------------------------------------------------------------------
def replace_shader():
	global shaderindex,shaders,currentshader
	index=int(shaderindex.get())
	currentshader.set(shaders[index][0])
	sel ('/usr/scene/pos')
	delete ('sn')
	delete ('tex')
	shaders[index][1]()
	
#--------------------------------------------------------------------
#   Execution starts here!
#--------------------------------------------------------------------

shaders=[\
	["Modulate",modulate_shader],\
	["Gloss",gloss_shader],\
	["Shinethrough",shinethrough_shader],\
	["Blend",blend_shader],\
	["Funky Plasma",funkyplasma_shader],\
	["Environment",environ_shader] \
	]
execfile ('licht.py')
sel ('/usr/scene')
# generate a standard visual hierarchie
gen_visual ()

# create a small Tk window with a shader selector
tl=Toplevel()
tl.title("Shaders" )
tl.geometry('214x100+580+177')
tl.resizable(0,0)
f1=Frame (tl)
f1.pack (side=TOP,expand=1)

# build the menu dynamically from previously specified array
currentshader.set(shaders[0][0])
RBb = Menubutton(f1, width=213, underline=0,indicatoron=1,relief=RAISED,textvariable=currentshader)
RBb.pack(side=TOP, expand=1)
RBb.menu=Menu(RBb, tearoff=0)
x=0
for l in shaders:
	RBb.menu.add_radiobutton (label=l[0],command=replace_shader,variable=shaderindex,value=x ) 
	x+=1
RBb['menu'] = RBb.menu
cb=Checkbutton (f1,text="Rotation Off" ,command=toggle_anim)
cb.pack(side=TOP,expand=1)
