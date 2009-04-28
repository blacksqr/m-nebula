connect("localhost:gfxserv")
sn=new ('nshadernode','name')
sel("name")
for x  in xrange (1000000):
	#sn=new ('nshadernode','name'+str(x))
	sn.setalphaenable(0)
	#set("setalphaenable",0)
	if 0:
		sn.setrenderpri(0)
		sn.setnumstages(2)
		sn.setdiffuse(1.000000,1.000000,1.000000,1.000000)
		sn.setemissive(10.100000,10.100000,10.100000,10.00000)
		sn.setambient(10.000000,10.000000,10.000000,10.000000)
		sn.setlightenable(1)
		sn.setalphaenable(0)
		sn.setzwriteenable(1)
	if ( not (x % 1000)):
		print x
