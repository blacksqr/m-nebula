#
#   meshmix.tcl
#
#   Demonstrate weighted mesh mixing.
#
#   (C) COPYRIGHT 2001 RADONLABS GMBH
#
import sys,site,os,os.path
from Tkinter import *
global tl	#needed to allow this 'window' to be destroyed
sys.argv=[""]

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
	sn.setdiffuse(0.5,0.5,0.5,1.0)
	sn.setemissive(1,0,0,0)
	sn.setambient(0,0,0,0)
	sn.setalphaenable(0)
# mix red/green emission based on weights
	mix=new ('nmixer','mix')
	sel ('mix')
	mix.beginsources(2)
	mix.setsource(0,'blueglow','w2')
	mix.setsource(1,'greenglow','w3')
	mix.endsources()
	mix.beginconnects(1)
	mix.setconnect(0,'setemissive','getval4f')
	mix.endconnects()
	mix.setnormalize(0)
# blue glowing emission
	blueglow=new ('nipol','blueglow')
	sel ('blueglow')
	blueglow.addkey4f(0,1,0,0,0)
	blueglow.addkey4f(1,0,0,0,0)
	blueglow.addkey4f(2,1,0,0,0)
	sel ('..')
# a green glowing emission
	greenglow=new ('nipol','greenglow')
	sel ('greenglow')
	greenglow.addkey4f(0,0,0,0,0)
	greenglow.addkey4f(1,0,1,0,0)
	greenglow.addkey4f(2,0,0,0,0)
	sel ('..')
	sel ('..')
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,'../bmp/marble.bmp','none')
	tex.settexture(1,'../bmp/autobahn.bmp','none')
	sel ('..')
#--------------------------------------------------------------------
#   main
#   Execution starts here.
#--------------------------------------------------------------------
sel ('/usr/scene')
execfile ('licht.py')
pos=new ('n3dnode','pos')
sel ('pos')
pos.tx(-1)
pos.tz(-3)
rotx=new ('nipol','rotx')
sel ('rotx')
rotx.connect('rx')
rotx.addkey1f(0,0)
rotx.addkey1f(5,360)
sel ('..')
rotz=new ('nipol','rotz')
sel ('rotz')
rotz.connect('rz')
rotz.addkey1f(0,0)
rotz.addkey1f(7,360)
sel ('..')
mesh=new ('nmeshmixer','mesh')
sel ('mesh')
mesh1=new ('nmeshnode','mesh1')
mesh1.setfilename("../meshes/torus1.n3d")
mesh1.setreadonly(1)
mesh2=new ('nmeshnode','mesh2')
mesh2.setfilename("../meshes/torus4.n3d")
mesh2.setreadonly(1)
mesh3=new ('nmeshnode','mesh3')
mesh3.setfilename("../meshes/torus5.n3d")
mesh3.setreadonly(1)
mesh.beginsources(3)
mesh.setsource(0,'mesh1','w1')
mesh.setsource(1,'mesh2','w2')
mesh.setsource(2,'mesh3','w3')
mesh.endsources()
mesh.setnormalize(1)
mesh.setupdatecoord (1)
mesh.setupdatenorm (1)
mesh.setupdatecolor (0)
mesh.setupdateuv0 (0)
mesh.setupdateuv1 (0)
mesh.setupdateuv2 (0)
mesh.setupdateuv3 (0)
sel ('..')
environ_shader ()
sel ('..')
#--------------------------------------------------------------------
#   Create a small tk window with scales for density and fog color.
#--------------------------------------------------------------------
def cmd1(w2):
	set('/sys/servers/channel.setchannel1f','w2',float(w2))

def cmd2(w3):
	set('/sys/servers/channel.setchannel1f','w3',float(w3))
	
tl=Toplevel ()
tl.title("Weight Controls" )
tl.geometry('571x61+1+500')
tl.resizable(0,0)
f1=Frame (tl)
f1.pack (side=TOP,expand=1)
w2=IntVar()
w3=IntVar()
s1=Scale (f1,length=550,orient=HORIZONTAL,from_=0.0,to=1.0,showvalue=0,label="",variable=w2,resolution=0.0001,command=cmd1)
l1=Label (f1,text="Weight 2" ,width=10)
#t1.pack (s1,side=LEFT,expand=1)
l1.pack (side=LEFT,expand=0)
s1.pack (side=RIGHT,expand=1)

f2=Frame (tl)
f2.pack (side=TOP,expand=1)
s2=Scale (f2,length=550,orient=HORIZONTAL,from_=0.0,to=1.0,showvalue=0,label ="",variable=w3,resolution=0.0001,command=cmd2)
l2=Label (f2,text="Weight 3" ,width=10)
l2.pack (side=LEFT,expand=0)
s2.pack (side=RIGHT,expand=1)
#t2.pack (w5,side=LEFT,expand=1)
#set ('w1',1.0)
set ('/sys/servers/channel.setchannel1f','w1', 1.0)
#--------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------
