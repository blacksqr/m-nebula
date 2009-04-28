# ---
# $parser:ntclserver$ $class:n3dnode$ $ver:Mon Aug 21 21:51:53 2000$
# ---
# Created Feb 2002: Andy Miller
SIZE=25

def addterrain ( name, heightmap ):
	ter=new ('nflatterrainnode',name)
	ter.setdetail(20)  #100
	ter.setstaticerror(7)
	ter.setradius(25)
	ter.setuvscale(0.040000)
	ter.setheightmap(heightmap)
	return ter

def addtexture ( name, texture ) :
	tex=new ('ntexarraynode',name )
	x=0
	for textfile in texture:
		tex.settexture(x,textfile,'none')
		x=+1
	return tex
	
def addshader (name ) : 
	stages=2		
	sn=new ('nshadernode',name)
	sn.setrenderpri(0)
	sn.setnumstages(stages)
	
	for x in range(stages):
		sn.setcolorop(x,"mul tex prev")
		sn.setalphaop(x,'nop')
		sn.setconst(x,0.000000,0.000000,0.000000,0.000000)
		sn.begintunit(x)
		sn.setaddress('clamp','clamp')
		sn.setminmagfilter('linear_mipmap_nearest','linear')
		sn.settexcoordsrc('uv'+str(x))
		sn.setenabletransform(0)
		sn.txyz(0.000000,0.000000,0.000000)
		sn.rxyz(0.000000,0.000000,0.000000)
		sn.sxyz(1.000000,1.000000,1.000000)
		sn.endtunit()
	sn.setdiffuse(1.000000,1.000000,1.000000,1.000000)
	sn.setemissive(10.100000,10.100000,10.100000,10.00000)
	sn.setambient(10.000000,10.000000,10.000000,10.000000)
	sn.setlightenable(1)
	sn.setalphaenable(0)
	sn.setzwriteenable(1)
	sn.setfogenable(1)
	sn.setalphablend('srcalpha','invsrcalpha')
	sn.setzfunc('lessequal')
	sn.setcullmode('cw')
	sn.setcolormaterial('material')
	return sn



#main
sel ('/usr/scene')
# create vanilla lighting
execfile ('licht.py')
# display number of triangles
set ('/sys/servers/console.watch','gfx_triangles')
#set ('/sys/servers/console.watch','gfx_framerate')
#set ('/sys/servers/console.watch','*')
#
scenebase = '/usr/scene/'
ZTILES=6
XTILES=6
weld = 0
glist=[]
for x in range( XTILES ):
	for z in range ( ZTILES ):
		gridname=scenebase+'grid_'+str(x)+'_'+str(z)
		grid  = new ('n3dnode',gridname)
#		grid.setminlod(0)
		grid.txyz(x*SIZE,0,-z*SIZE)		# position each tile
		grid.rxyz(0,0,0)
		grid.sxyz(1,1,1)
		glist.append(grid) # save a list so we can do welds...
		# use absolute path names so we don't need all those 'sel' statements :)
		ter = addterrain ( gridname+'/ter','../bmp/flatTerrainHeightMap.bmp')
		tex = addtexture ( gridname+'/tex',['../bmp/detailmap.bmp','../bmp/flatTerrainTexture.bmp'] )
		sn = addshader( gridname+'/sn')
		
if weld:
	for x in glist:
		pass
		#grid = glist[0,0]
		#grid.weld=glist[0,1].getfullname,glist[1,0].getfullname
		# ter1.weld='none','/usr/scene/grid2/ter'

# EOF
