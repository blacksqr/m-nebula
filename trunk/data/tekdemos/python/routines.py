#
#   Basic Routines used by all TEKDEMOS
#
#   created:    08-Nov-2000
#   upd.:       08-Nov-2000
#
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
def plainobject(name,mesh,txt):
	name1=new ('n3dnode',name)
	sel (name)
	mesh1=new ('nmeshnode','mesh')
	mesh1.setfilename(mesh)
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
	sel ('..')
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	tex.settexture(0,txt,'none')
	sel ('..')
	sel ('..')
	return name1