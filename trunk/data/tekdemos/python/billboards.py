#
#   Nebula TEKDEMO
#
#   Leuchter: Billboars und Sprites
#   Please note: You may use planes (as demonstrated here) or complex
#   3D-Object as billboards or sprites.
#
#   PLEASE NOTE: THIS TEKDEMO USES THE OLD NOT T&L ACCELERATED
#   NEBULA SYSTEM BASED ON THE VBUF-TEXTURE-MATERIAL-NODES !!!
#
#   created:    02-Nov-99
#   upd.:       10-Nov-99
#               06-Feb-2000 floh    removed some fluff
#               10-Nov-2000 bernd   Ganz neuer Leuchter
#
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
#--------------------------------------------------------------------
#   proc genlib
#   Since we have so many identical objects we create a small
#   lib of each unique object, and use nlinknode objects in the
#   hierarchie to reference the original objects. This will make
#   the whole thing generally more efficient.
#--------------------------------------------------------------------
def gen_lib():
	lib=new ('nroot','/usr/lib')
	sel ('/usr/lib')
# this is the complete candle billboard
	billboard=new ('n3dnode','billboard')
	sel ('billboard')
	billboard.setbillboard(1)
	candle_mesh=new ('nmeshnode','candle_mesh')
	candle_mesh.setfilename("../meshes/plane3.n3d")
	candle_shader=new ('nshadernode','candle_shader')
	sel ('candle_shader')
	candle_shader.setnumstages(1)
	candle_shader.setcolorop(0,"mul tex prev" )
	candle_shader.begintunit(0)
	candle_shader.setaddress('wrap','wrap')
	candle_shader.setminmagfilter('linear_mipmap_nearest','linear')
	candle_shader.settexcoordsrc('uv0')
	candle_shader.setenabletransform(0)
	candle_shader.endtunit()
	candle_shader.setemissive(0,0,0,0)
	candle_shader.setdiffuse(1,1,1,1)
	candle_shader.setambient(1,1,1,1)
	candle_shader.setlightenable(1)
	candle_shader.setalphaenable(1)
	candle_shader.setalphablend('srcalpha','invsrcalpha')
	candle_shader.setzwriteenable(0)
	candle_shader.setfogenable(1)
	candle_shader.setzfunc('lessequal')
	candle_shader.setcullmode('ccw')
	candle_shader.setcolormaterial('material')
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,"../bmp/candle.bmp","../bmp/alpha.bmp")
	sel ('..')
	packer=new ('n3dnode','packer')
	sel ('packer')
	packer.setsprite(1)
	glow=new ('n3dnode','glow')
	sel ('glow')
	glow.txyz(0.000000,0.000000,-0.500000)
	glow_mesh=new ('nmeshnode','glow_mesh')
	glow_mesh.setfilename("../meshes/plane2.n3d")
	glow_shader=new ('nshadernode','glow_shader')
	sel ('glow_shader')
	glow_shader.setnumstages(1)
	glow_shader.setcolorop(0,"mul tex prev" )
	glow_shader.begintunit(0)
	glow_shader.setaddress('wrap','wrap')
	glow_shader.setminmagfilter('linear_mipmap_nearest','linear')
	glow_shader.settexcoordsrc('uv0')
	glow_shader.setenabletransform(0)
	glow_shader.endtunit()
	glow_shader.setambient(0,0,0,0)
	glow_shader.setdiffuse(0,0,0,0)
	glow_shader.setemissive(1.000000,0.459146,0.000000,0.000000)
	glow_shader.setlightenable(1)
	glow_shader.setalphaenable(1)
	glow_shader.setalphablend('one','one')
	glow_shader.setzwriteenable(0)
	glow_shader.setfogenable(1)
	glow_shader.setzfunc('lessequal')
	glow_shader.setcullmode('ccw')
	glow_shader.setcolormaterial('material')
	emflicker=new ('nipol','emflicker')
	sel ('emflicker')
	emflicker.connect("setemissive")
	emflicker.beginkeys(8,4)
	emflicker.setkey4f(0,0.000000,1.000000,0.100000,0.000000,0.000000)
	emflicker.setkey4f(1,0.500000,0.800000,0.800000,0.000000,0.000000)
	emflicker.setkey4f(2,1.000000,1.000000,0.800000,0.000000,0.000000)
	emflicker.setkey4f(3,1.500000,1.000000,0.400000,0.000000,0.000000)
	emflicker.setkey4f(4,2.000000,1.000000,0.800000,0.000000,0.000000)
	emflicker.setkey4f(5,2.500000,1.000000,0.200000,0.000000,0.000000)
	emflicker.setkey4f(6,3.000000,1.000000,0.800000,0.000000,0.000000)
	emflicker.setkey4f(7,3.500000,1.000000,0.100000,0.000000,0.000000)
	emflicker.endkeys()
	sel ('..')
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,"../bmp/glow.bmp","none")
	sel ('..')
	sel ('..')
	sel ('..')
	sel ('..')
#--------------------------------------------------------------------
#   proc candle
#
#   Generate a complete candle.
#--------------------------------------------------------------------
def candle(name,x,y,z):
	name1=new ('n3dnode',name)
	sel (name)
	name1.txyz(x,y,z)
	bboard=new ('nlinknode','bboard')
	bboard.settarget('/usr/lib/billboard')
	sel ('..')
#--------------------------------------------------------------------
gen_lib ()
sel ('/usr/scene')
#txyz (9,1,18)
execfile ('licht.py')
leuchter=new ('n3dnode','leuchter')
sel ('leuchter')
roty=new ('nipol','roty')
sel ('roty')
roty.connect('ry')
roty.addkey1f(0,0)
roty.addkey1f(10,360)
sel ('..')
kerzen=new ('n3dnode','kerzen')
sel ('kerzen')
kerzen.txyz(0.000000,0.240000,0.000000)
candle ('kerze1',0.128751,0.044909,0.979375)
candle ('kerze2',0.492323,0.044909,0.850907)
candle ('kerze3',0.797434,0.044909,0.602000)
candle ('kerze4',0.958019,0.044909,0.248713)
candle ('kerze5',0.982107,0.044909,-0.128661)
candle ('kerze6',0.861668,0.044909,-0.498006)
candle ('kerze7',0.612761,0.044909,-0.795088)
candle ('kerze8',0.243416,0.044909,-0.963702)
candle ('kerze9',-0.117900,0.044909,-0.979761)
candle ('kerze10',-0.487245,0.044909,-0.875381)
if 1:
	candle ('kerze12',-0.800385,0.044909,-0.602386)
	candle ('kerze14',-0.969000,0.044909,-0.249100)
	candle ('kerze15',-0.985058,0.044909,0.120246)
	candle ('kerze16',-0.880678,0.044909,0.505649)
	candle ('kerze17',-0.615713,0.044909,0.810760)
	candle ('kerze18',-0.270455,0.044909,0.955287)
	candle ('kerze19',-0.639801,0.829441,0.149805)
	candle ('kerze20',-0.342719,0.829441,0.567326)
	
	candle ('kerze21',0.155095,0.829441,0.639589)
	candle ('kerze22',0.564586,0.829441,0.350536)
	candle ('kerze23',0.652908,0.829441,-0.163336)
	candle ('kerze24',0.331738,0.829441,-0.572827)
	candle ('kerze25',-0.141988,0.829441,-0.653119)
	candle ('kerze26',-0.567537,0.829441,-0.348008)
	candle ('kerze27',-0.439070,1.438668,-0.131218)
	candle ('kerze28',-0.350748,1.438668,0.270244)
	candle ('kerze29',0.147065,1.438668,0.438858)
	candle ('kerze30',0.460206,1.438668,0.109659)
	

if 1:
	candle ('kerze31',0.355826,1.438668,-0.283774)
	candle ('kerze32',-0.101841,1.438668,-0.444359)
sel ('..')
leuchta=new ('n3dnode','leuchta')
sel ('leuchta')
#leuchta.setminlod(0.000000)
leuchta.txyz(0.000000,0.000000,0.000000)
leuchta.rxyz(0.000000,0.000000,0.000000)
leuchta.sxyz(1.000000,1.000000,1.000000)
mesh=new ('nmeshnode','mesh')
mesh.setfilename("../meshes/leuchta.n3d")
sn=new ('nshadernode','sn')
sel ('sn')
sn.setnumstages(1)
sn.setcolorop(0,"mul tex prev" )
sn.begintunit(0)
sn.setaddress('wrap','wrap')
sn.setminmagfilter('linear_mipmap_nearest','linear')
sn.settexcoordsrc('spheremap')
sn.setenabletransform(1)
sn.endtunit()
sn.setambient(1.000000,1.000000,1.000000,1.000000)
sn.setdiffuse(1.000000,1.000000,1.000000,1.000000)
sn.setemissive(1.000000,2.000000,0.000000,0.000000)
sn.setlightenable(1)
sn.setalphaenable(0)
sn.setzwriteenable(1)
sn.setfogenable(1)
sn.setzfunc('lessequal')
sn.setcullmode('ccw')
sn.setcolormaterial('material')
sel ('..')
tex=new ('ntexarraynode','tex')
sel ('tex')
tex.settexture(0,"../bmp/candles.bmp",'none')
sel ('..')
sel ('..')
sel ('..')
# ---
# EOF
       
