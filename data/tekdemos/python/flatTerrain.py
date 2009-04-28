# ---
# $parser:ntclserver$ $class:n3dnode$ $ver:Mon Aug 21 21:51:53 2000$
# ---
sel ('/usr/scene')
# create vanilla lighting
execfile ('licht.py')
# display number of triangles
set ('/sys/servers/console.watch','gfx_triangles')
grid1=new ('n3dnode','grid1')
sel ('grid1')
grid1.txyz(-12.000000,0.000000,-20.000000)
grid1.rxyz(15.00000,0.0000,0.000000)
grid1.sxyz(1.000000,1.000000,1.000000)
ter=new ('nflatterrainnode','ter')
sel ('ter')
ter.setdetail(100.000000)
ter.setstaticerror(7.000000)
ter.setradius(25.000000)
ter.setuvscale(0.040000)
ter.setheightmap("../bmp/flatTerrainHeightMap.bmp")
sel ('..')
tex=new ('ntexarraynode','tex')
sel ('tex')
tex.settexture(0,"../bmp/detailmap.bmp","none")
tex.settexture(1,"../bmp/flatTerrainTexture.bmp","none")
sel ('..')
sn=new ('nshadernode','sn')
sel ('sn')
sn.setrenderpri(0)
sn.setnumstages(2)
sn.setcolorop(0,"mul tex prev" )
sn.setalphaop(0,"nop")
sn.setcolorop(1,"mul tex prev" )
sn.setalphaop(1,"nop")
sn.setconst(0,0.000000,0.000000,0.000000,0.000000)
sn.setconst(1,0.000000,0.000000,0.000000,0.000000)
sn.begintunit(0)
sn.setaddress("clamp","clamp")
sn.setminmagfilter("linear_mipmap_nearest","linear")
sn.settexcoordsrc("uv0")
sn.setenabletransform(0)
sn.txyz(0.000000,0.000000,0.000000)
sn.rxyz(0.000000,0.000000,0.000000)
sn.sxyz(1.000000,1.000000,1.000000)
sn.endtunit()
sn.begintunit(1)
sn.setaddress("clamp","clamp")
sn.setminmagfilter("linear_mipmap_linear","linear")
sn.settexcoordsrc("uv1")
sn.setenabletransform(0)
sn.txyz(0.000000,0.000000,0.000000)
sn.rxyz(0.000000,0.000000,0.000000)
sn.sxyz(1.000000,1.000000,1.000000)
sn.endtunit()
sn.setdiffuse(1.000000,1.000000,1.000000,1.000000)
sn.setemissive(0.000000,0.000000,0.000000,0.000000)
sn.setambient(1.000000,1.000000,1.000000,1.000000)
sn.setlightenable(1)
sn.setalphaenable(0)
sn.setzwriteenable(1)
sn.setfogenable(1)
sn.setalphablend("srcalpha","invsrcalpha")
sn.setzfunc("lessequal")
sn.setcullmode("cw")
sn.setcolormaterial("material")
sel ('..')
sel ('..')
# ---
# EOF
