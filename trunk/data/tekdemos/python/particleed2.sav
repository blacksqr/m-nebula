#--------------------------------------------------------------------
#   particleed2.tcl
#   ===============
#   New and improved!
#
#   particleed2 works on a Nebula hierarchie like this:
#
#   new n3dnode psys
#       new nspriterender sr
#           new npointemitter pe
#       new ntexarraynode tex
#       new nshadernode shd
#
#   11-Jan-01   floh    created
#
#   (C) 2001 RadonLabs GmbH -- A.Weissflog
#--------------------------------------------------------------------
global nomads_home
global home
execfile (home/bin/keyframeed.tcl)
#--------------------------------------------------------------------
#   Global Variables:
#
#   pe2_showgrid        - turn on/off grid
#   pe2_showtriangles   - turn on/off triangle count
#   pe2_freezetime      - stop time
#
#   pe2_start_time      - start emitting on local timeline
#   pe2_stop_time       - stop emitting on local timeline
#   pe2_repeat_time     - repeat after on local timeline
#   pe2_lifetime        - lifetime of particel
#   pe2_freq            - emitter frequency
#   pe2_speed           - emission speed
#   pe2_accel_x         - global acceleration vector
#   pe2_accel_y
#   pe2_accel_z
#   pe2_innercone       - inner emitter cone limit
#   pe2_outercone       - outer emitter cone limit
#   pe2_spin            - particle spin
#   pe2_spinaccel       - particle spin acceleration
#   pe2_numkeys         - number of keys in particle keyframe array
#   pe2_keys            - particle keyframe array
#
#       pe2_keys(x,size)
#       pe2_keys(x,bounce)
#       pe2_keys(x,red)
#       pe2_keys(x,green)
#       pe2_keys(x,blue)
#       pe2_keys(x,alpha)
#--------------------------------------------------------------------
pe2_showgrid=0
pe2_showtriangles=0
pe2_freezetime=0
pe2_lockfeedback=0
#--------------------------------------------------------------------
#   proc update_variables_from_psys
#
#   Update global variables from default particle system
#   hierarchie as described by proc new_psys
#
#   11-Jan-01   floh    created
#--------------------------------------------------------------------
def update_variables_from_psys():
	global pe2_start_time
	global pe2_stop_time
	global pe2_repeat_time
	global pe2_lifetime
	global pe2_freq
	global pe2_speed
	global pe2_accel_x
	global pe2_accel_y
	global pe2_accel_z
	global pe2_innercone
	global pe2_outercone
	global pe2_spin
	global pe2_spinaccel
	global pe2_numkeys
	global pe2_keys
	global pe2_lockfeedback
# prevent gui from updating object
	pe2_lockfeedback=1
	cmd=psel()
	sel ('/usr/scene')
	times=set ('psys/sr/pe.gettimes')
	pe2_start_time=times[0]
	pe2_stop_time=times[1]
	pe2_repeat_time=times[2]
	pe2_lifetime=set ('psys/sr/pe.getlifetime')
	pe2_freq=set ('psys/sr/pe.getfreq')
	pe2_speed=set ('psys/sr/pe.getspeed')
	accel=set ('psys/sr/pe.getaccel')
	pe2_accel_x=accel[0]
	pe2_accel_y=accel[1]
	pe2_accel_z=accel[2]
# get sprite render object attributes
	pe2_innercone=set ('psys/sr/pe.getinnercone')
	pe2_outercone=set ('psys/sr/pe.getoutercone')
	pe2_spin=set ('psys/sr.getspin')
	pe2_spinaccel=set ('psys/sr.getspinaccel')
	pe2_numkeys=set ('psys/sr.getnumkeys')
	# 'for','set','i','0}','{$i','<',pe2_numkeys,'incr','i}','{'	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
	for i in range(0,pe2_numkeys):	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
		curkey=set ('psys/sr.getkey',i)
		pe2_keys[i]["size"]=curkey[0]
		pe2_keys[i]["bounce"]=curkey[1]
		pe2_keys[i]["red"]=curkey[2]
		pe2_keys[i]["green"]=curkey[3]
		pe2_keys[i]["blue"]=curkey[4]
		pe2_keys[i]["alpha"]=curkey[5]
#'}'	# End of loop
# update the keyframe editor
	update_keyeditor_keys ()
# allow gui to update object
	pe2_lockfeedback=0
#'}'
#--------------------------------------------------------------------
#   proc new_psys
#
#   Create a new default particle system (complete with 3dnode, shader
#   and texture) under /usr/scene/psys, and update global
#   variables.
#
#   11-Jan-01   floh    created
#--------------------------------------------------------------------
def new_psys():
	cmd=psel()
	sel ('/usr/scene')
# if exists, delete previous particle system
	if exists('psys'):	# CHECK -if {[exists psys]} {
		delete ('psys')
#'}'
# create new default particle system
	psys=new ('n3dnode','psys')
	sel ('psys')
# the particle system object itself
	sr=new ('nspriterender','sr')
	sel ('sr')
	pe=new ('npointemitter','pe')
	sel ('pe')
	pe.settimes(0,0,0)
	pe.setlifetime(1)
	pe.setfreq(100)
	pe.setspeed(5)
	pe.setaccel(0,0,0)
	pe.setinnercone(0)
	pe.setoutercone(0)
	sel ('..')
	sr.setemitter('pe')
	sr.setspin(0)
	sr.setspinaccel(0)
	sr.beginkeys(6)
	sr.setkey(0,0.5,0.0,1.0,1.0,1.0,1.0)
	sr.setkey(1,0.5,0.0,1.0,1.0,1.0,1.0)
	sr.setkey(2,0.5,0.0,1.0,1.0,1.0,1.0)
	sr.setkey(3,0.5,0.0,1.0,1.0,1.0,1.0)
	sr.setkey(4,0.5,0.0,1.0,1.0,1.0,1.0)
	sr.setkey(5,0.5,0.0,1.0,1.0,1.0,1.0)
	sr.endkeys()
	sel ('..')
# the default shader (additive transparent)
	shd=new ('nshadernode','shd')
	sel ('shd')
	shd.setminlod(0.000000)
	shd.setrenderpri(0)
	shd.setnumstages(1)
	shd.setcolorop(0,"mul tex prev" )
	shd.setalphaop(0,"nop")
	shd.setconst(0,0.000000,0.000000,0.000000,0.000000)
	shd.begintunit(0)
	shd.setaddress("wrap","wrap")
	shd.setminmagfilter("linear_mipmap_nearest","linear")
	shd.settexcoordsrc("uv0")
	shd.setenabletransform(0)
	shd.txyz(0.000000,0.000000,0.000000)
	shd.rxyz(0.000000,0.000000,0.000000)
	shd.sxyz(1.000000,1.000000,1.000000)
	shd.endtunit()
	shd.setdiffuse(0.000000,0.000000,0.000000,0.000000)
	shd.setemissive(0.000000,0.000000,0.000000,0.000000)
	shd.setambient(0.000000,0.000000,0.000000,0.000000)
	shd.setlightenable(0)
	shd.setalphaenable(1)
	shd.setzwriteenable(0)
	shd.setfogenable(0)
	shd.setalphablend("one","one")
	shd.setzfunc("lessequal")
	shd.setcullmode("ccw")
	shd.setcolormaterial("material")
	sel ('..')
# the default texture
	tex=new ('ntexarraynode','tex')
	sel ('tex')
	sel ('..')
	sel ('..')
# update the global variables
	update_variables_from_psys ()
# update the keyframe editor
	ke_refresh ()
	sel (cwd)
#'}'
#--------------------------------------------------------------------
#   proc load_psys
#   Load particle system defined by filename.
#   19-Jan-01   floh    created
#--------------------------------------------------------------------
def load_psys(file):

	cmd=psel()
# delete current particle system if exists
	sel ('/usr/scene')
	if exists('psys'):	# CHECK -if {[exists psys]} {
		delete ('psys')
#'}'
	get (file)
# rename particle system object to psys
	psysname_with_ext = 'file','tail',file	# CHECK -set psysname_with_ext [file tail $file]
	psysname = 'file','rootname',psysname_with_ext	# CHECK -set psysname [file rootname $psysname_with_ext]
	set ('/usr/scene/$psysname.setname','psys')	# CHECK -/usr/scene/$psysname.setname psys
# update the global variables
	update_variables_from_psys ()
	sel (cwd)

#'}'
#--------------------------------------------------------------------
#   proc save_psys_dialog
#   Create file save dialog and save current particle system.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
def save_psys_dialog():
	if 0:
		# create a file save dialog
		types='{'
		{"Nebula ('particle','system"','.psys')
		#'}'
		file = 'tk_getSaveFile',filetypes=types,initialfile=unnamed,defaultextension=.n,title="Save particle system" 	# CHECK -set file [tk_getSaveFile -filetypes $types -initialfile unnamed -defaultextension .n -title "Save particle system"]
		if file '!=':	# CHECK -if {$file != ""} {
			set ('/usr/scene/psys.saveas',file)
		# Nebula appends a .n, thus we rename the file to *.psys
			file ('rename',force=file.n,file)
		#'}','else','{'
		print "Cancelled"
		#'}'
		#'}'
#--------------------------------------------------------------------
#   proc load_psys_dialog
#   Create file open dialog and load selected particle system.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
def load_psys_dialog():
	if 0:
		# create a file load dialog
		types='{'
		{"Nebula ('particle','system"','.psys')
		#'}'
		file = 'tk_getOpenFile',filetypes=types,title="Load particle system" 	# CHECK -set file [tk_getOpenFile -filetypes $types -title "Load particle system"]
		if file '!=':	# CHECK -if {$file != ""} {
		# load particle system
			load_psys (file)
		# update the keyframe editor
			ke_refresh ()
		#'}','else','{'
		print "Cancelled"
		#'}'
		#'}'
#--------------------------------------------------------------------
#   proc load_shader
#   Load a shader serialized object file.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
def load_shader():
# create a file load dialog
	if 0:
		types='{'
		{"Nebula ('shader"','.shd')
		#'}'
		file = 'tk_getOpenFile',filetypes=types,title="Load shader" 	# CHECK -set file [tk_getOpenFile -filetypes $types -title "Load shader"]
		if file '!=':	# CHECK -if {$file != ""} {
			cmd=psel()
			sel ('/usr/scene/psys')
			delete ('shd')
			get (file)
		# rename shader to "shd"
			# 'for','set','o','/usr/scene/psys.gethead]','{$o','!=',"null",'set','o',o.getsucc],'{'	# CHECK -for {set o [/usr/scene/psys.gethead]} {$o != "null"} {set o [$o.getsucc]} {
			if '[$o.isa' 'nshadernode]':	# CHECK -if {[$o.isa nshadernode]} {
				$o.setname('shd')
				break ()
		#'}'	# End of loop
		#'}'
			sel (cwd)
		#'}','else','{'
		print "Cancelled"
		#'}'
		#'}'
#--------------------------------------------------------------------
#   proc load_tex
#   Define a texture image for the ntexarraynode object.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
def load_tex(stage):
	if 0:
		# create a file load dialog
		types='{'
		{"Bmp ('file"','.bmp')
		#'}'
		file = 'tk_getOpenFile',filetypes=types,title="Load texture map for stage $stage" 	# CHECK -set file [tk_getOpenFile -filetypes $types -title "Load texture map for stage $stage"]
		if file:	# CHECK -if {$file != ""} {
			curtexalpha=set ('/usr/scene/psys/tex.gettexture',stage)
			curalpha=curtexalpha[1]
			set ('/usr/scene/psys/tex.settexture',stage,file,curalpha)
		else:
			print "Cancelled"
		#'}'
		#'}'
#--------------------------------------------------------------------
#   proc load_alpha
#   Define a texture image for the ntexarraynode object.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
def load_alpha(stage):
	if 0:
		# create a file load dialog
		types='{'
		{"Bmp ('file"','.bmp')
		#'}'
		file = 'tk_getOpenFile',filetypes=types,title="Load alpha map for stage $stage" 	# CHECK -set file [tk_getOpenFile -filetypes $types -title "Load alpha map for stage $stage"]
		if file:	# CHECK -if {$file != ""} {
			curtexalpha=set ('/usr/scene/psys/tex.gettexture',stage)
			curtex=curtexalpha[0]
			set ('/usr/scene/psys/tex.settexture',stage,curtex,file)
		else:
			print "Cancelled"
		#'}'
		#'}'
#--------------------------------------------------------------------
#   proc pe2_updkeys_callback
#   Called by keyframe editor whenever a key value changes.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_updkeys_callback(channel,key,value):
	global pe2_keys
	if channel == 0:	# CHECK -if {$channel == 0} {
# red channel
		pe2_keys($key,red)=value
	elif channel == 1:
# green channel
		pe2_keys($key,green)=value
	elif channel == 2:
# blue channel
		pe2_keys($key,blue)=value
	elif channel == 3:
# alpha channel
		pe2_keys($key,alpha)=value
	elif channel == 4:
# bounce channel
		pe2_keys($key,bounce)=value
	elif channel == 5:
# size channel
		pe2_keys($key,size)=value
#'}'
# set the new key in the sprite renderer
	set ('/usr/scene/psys/sr.setkey',key,\
		pe2_keys[key]["size"],\
		pe2_keys[key]["bounce"],\
		pe2_keys[key]["red"],\
		pe2_keys[key]["green"],\
		pe2_keys[key]["blue"],\
		pe2_keys[key]["alpha"])
		#'}'
#--------------------------------------------------------------------
#   proc pe2_update_times
#   Validate time variables and update emitter object.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_update_times():
	global pe2_start_time
	global pe2_stop_time
	global pe2_repeat_time
	if pe2_stop_time < pe2_start_time:	# CHECK -if {$pe2_stop_time < $pe2_start_time} {
		pe2_stop_time=pe2_start_time
	if pe2_repeat_time < pe2_stop_time:	# CHECK -if {$pe2_repeat_time < $pe2_stop_time} {
		pe2_repeat_time=pe2_stop_time
	set ('/usr/scene/psys/sr/pe.settimes',pe2_start_time,pe2_stop_time,pe2_repeat_time)
#--------------------------------------------------------------------
#   proc pe2_reset_times
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_reset_times():
	global pe2_start_time
	global pe2_stop_time
	global pe2_repeat_time
	pe2_start_time=0.0
	pe2_stop_time=0.0
	pe2_repeat_time=0.0
	pe2_update_times ()
#--------------------------------------------------------------------
#   proc pe2_update_lifetime_freq
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_update_lifetime_freq():
	global pe2_lifetime
	global pe2_freq
	set ('/usr/scene/psys/sr/pe.setlifetime',pe2_lifetime)
	set ('/usr/scene/psys/sr/pe.setfreq',pe2_freq)
#--------------------------------------------------------------------
#   proc pe2_reset_lifetime_freq
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_reset_lifetime_freq():
	global pe2_lifetime
	global pe2_freq
	pe2_lifetime=1.0
	pe2_freq=100.0
	pe2_update_lifetime_freq ()
#--------------------------------------------------------------------
#   proc pe2_update_accel
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_update_accel():
	global pe2_accel_x
	global pe2_accel_y
	global pe2_accel_z
	set ('/usr/scene/psys/sr/pe.setaccel',pe2_accel_x,pe2_accel_y,pe2_accel_z)
#--------------------------------------------------------------------
#   proc pe2_reset_accel
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_reset_accel():
	global pe2_accel_x
	global pe2_accel_y
	global pe2_accel_z
	pe2_accel_x=0.0
	pe2_accel_y=0.0
	pe2_accel_z=0.0
	pe2_update_accel ()
#--------------------------------------------------------------------
#   proc pe2_update_cones
#   Update inner and outer cone of object.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_update_cones():
	global pe2_innercone
	global pe2_outercone
	if pe2_innercone < 0.0:	# CHECK -if {$pe2_innercone < 0.0} {
		pe2_innercone=0.0
	elif pe2_innercone > 180.0:
		pe2_innercone=180.0
	if pe2_outercone < 0.0:	# CHECK -if {$pe2_outercone < 0.0} {
		pe2_outercone=0.0
	elif pe2_outercone > 180.0:
		pe2_outercone=180.0
	set ('/usr/scene/psys/sr/pe.setinnercone',pe2_innercone)
	set ('/usr/scene/psys/sr/pe.setoutercone',pe2_outercone)
#--------------------------------------------------------------------
#   proc pe2_reset_cones
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_reset_cones():
	global pe2_innercone
	global pe2_outercone
	pe2_innercone=0.0
	pe2_outercone=0.0
	pe2_update_cones ()
#--------------------------------------------------------------------
#   proc pe2_update_spin
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_update_spin():
	global pe2_spin
	global pe2_spinaccel
	set ('/usr/scene/psys/sr.setspin',pe2_spin)
	set ('/usr/scene/psys/sr.setspinaccel',pe2_spinaccel)
#--------------------------------------------------------------------
#   proc pe2_reset_spin
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_reset_spin():
	global pe2_spin
	global pe2_spinaccel
	pe2_spin=0.0
	pe2_spinaccel=0.0
	pe2_update_spin ()
#--------------------------------------------------------------------
#   proc pe2_update_speed
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_update_speed():
	global pe2_speed
	set ('/usr/scene/psys/sr/pe.setspeed',pe2_speed)
#--------------------------------------------------------------------
#   proc pe2_toggletime
#   Freeze/unfreeze time based on pe2_freezetime state.
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_toggletime():
	global pe2_freezetime
	if pe2_freezetime:	# CHECK -if {$pe2_freezetime} {
		set ('/sys/servers/time.stoptime')	# CHECK -/sys/servers/time.stoptime
	else:
		set ('/sys/servers/time.starttime')	# CHECK -/sys/servers/time.starttime
#--------------------------------------------------------------------
#   proc pe2_togglegrid
#   Toggle grid based on pe2_showgrid state.
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_togglegrid():
	global pe2_showgrid
	if pe2_showgrid:	# CHECK -if {$pe2_showgrid} {
		set ('/observer.setgrid',1)
	else:
		set ('/observer.setgrid',0)
#--------------------------------------------------------------------
#   proc pe2_toggletriangles
#   Toggle triangle counter based on pe2_showtriangles
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_toggletriangles():
	global pe2_showtriangles
	if pe2_showtriangles:	# CHECK -if {$pe2_showtriangles} {
		set ('/sys/servers/console.watch','gfx_triangles')
	else:
		set ('/sys/servers/console.unwatch')	# CHECK -/sys/servers/console.unwatch
#====================================================================
#   GUI STUFF
#====================================================================
#--------------------------------------------------------------------
#   proc validate_cmd [newcontent] [updcommand]
#
#   Called by the entry widget for input validation and to
#   call back update routines which update the Nebula objects.
#
#   Strategy:
#   - reject new content if it contains illegal characters
#   - call provided update routine
#
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def validate_cmd(newcontent,updcallback):
	global pe2_lockfeedback
# do nothing if gui may not update object
	if pe2_lockfeedback:	# CHECK -if {$pe2_lockfeedback} {
		return (1)
# see if it is a valid floating point number
	try:
		f=float(newcontent)
		updcallback ()	#AJM should this be called with newcontent ?
		return (1)
	except:
		print "INVALID INPUT" 
		return (1)
#--------------------------------------------------------------------
#   proc pe2_focus_next
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_focus_next(cur):
	focus ('tk_focusNext',cur)	#AJM ?
#--------------------------------------------------------------------
#   proc pe2_setup_focus
#   Setup focus handling for entry widgets when pressing return.
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_setup_focus(entry_list):
	for cur in entrylist:
	# 'for','set','i','0}','{$i','<','llength',entry_list],'incr','i}','{'	# CHECK -for {set i 0} {$i < [llength $entry_list]} {incr i} {
		#cur=entry_list[i]
		bind (cur,'<Return>',"pe2_focus_next cur" )	#AJM ?
#'}'	# End of loop
#'}'
#--------------------------------------------------------------------
#   proc pe2_enable_texture_buttons
#   Enable/disable texture loader buttons based on
#   how many stages the current shader has.
#   18-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_enable_texture_buttons(w):
	return
	numstages=set ('/usr/scene/psys/shd.getnumstages')
	$w.ls.r2.tex0('configure',state=normal)
	$w.ls.r2.tex1('configure',state=normal)
	$w.ls.r2.tex2('configure',state=normal)
	$w.ls.r2.tex3('configure',state=normal)
	if numstages < 4:	# CHECK -if {$numstages < 4} {
		$w.ls.r2.tex3('configure',state=disabled)
#'}'
	if numstages < 3:	# CHECK -if {$numstages < 3} {
		$w.ls.r2.tex2('configure',state=disabled)
#'}'
	if numstages < 2:	# CHECK -if {$numstages < 2} {
		$w.ls.r2.tex1('configure',state=disabled)
#'}'
	if numstages < 1:	# CHECK -if {$numstages < 1} {
		$w.ls.r2.tex0('configure',state=disabled)
#'}'
	$w.ls.r3.alpha0('configure',state=normal)
	$w.ls.r3.alpha1('configure',state=normal)
	$w.ls.r3.alpha2('configure',state=normal)
	$w.ls.r3.alpha3('configure',state=normal)
	if numstages < 4:	# CHECK -if {$numstages < 4} {
		$w.ls.r3.alpha3('configure',state=disabled)
#'}'
	if numstages < 3:	# CHECK -if {$numstages < 3} {
		$w.ls.r3.alpha2('configure',state=disabled)
#'}'
	if numstages < 2:	# CHECK -if {$numstages < 2} {
		$w.ls.r3.alpha1('configure',state=disabled)
#'}'
	if numstages < 1:	# CHECK -if {$numstages < 1} {
		$w.ls.r3.alpha0('configure',state=disabled)
#'}'
#'}'
#--------------------------------------------------------------------
#   proc gen_loadsave_frame
#
#   Generate frame object with buttons for
#
#   New
#   Save
#   Load
#   Load Shader
#   Load Texture 1..4
#   Load Alpha   1..4
#
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
def gen_loadsave_frame(w):
	ls=Frame(w,relief=groove,borderwidth=2)
	ls=Pack(w,SIDE=BOTTOM,fill=x)
# row1: New Save Load Shader
	r1=Frame(ls,)
	r1=Pack(ls,SIDE=TOP,fill=x,pady=2)
	new=Button(r1,text="New",command=new_psys)
	save=Button(r1,text="Save...") #,command=save_psys_dialog)
	load=Button(r1,text="Load...") #,command=load_psys_dialog)
	shader=Button(r1,text="Shader...") #,command=load_shader)
	new.Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	save.Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	load.Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	shader.Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
# row2: Tex0 Tex1 Tex2 Tex3
	r2=Frame(ls,)
	r2=Pack(ls,SIDE=TOP,fill=x,pady=2)
	tex0=Button(r2,text="Tex0...",command="load_tex 0" )
	tex1=Button(r2,text="Tex1...",command="load_tex 1" )
	tex2=Button(r2,text="Tex2...",command="load_tex 2" )
	tex3=Button(r2,text="Tex3...",command="load_tex 3" )
	
	tex0=Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	tex1=Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	tex2=Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	tex3=Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
# row3: Alpha0 Alpha1 Alpha2 Alpha3
	r3=Frame(ls,)
	r3=Pack(ls,SIDE=TOP,fill=x,pady=2)
	alpha0=Button(r3,text="Alpha0...",command="load_alpha 0" )
	alpha1=Button(r3,text="Alpha1...",command="load_alpha 1" )
	alpha2=Button(r3,text="Alpha2...",command="load_alpha 2" )
	alpha3=Button(r3,text="Alpha3...",command="load_alpha 3" )
	alpha0=Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	alpha1=Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	alpha2=Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	alpha3=Pack(SIDE=LEFT,expand=1,padx=2,fill=x)
	pe2_enable_texture_buttons (w)
#'}'
#--------------------------------------------------------------------
#   proc gen_control_frame
#
#   Create a frame with buttons and sliders for emitter
#   and some renderer attributes.
#
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
def gen_control_frame(w):
	global pe2_start_time
	global pe2_stop_time
	global pe2_repeat_time
	global pe2_lifetime
	global pe2_freq
	global pe2_speed
	global pe2_accel_x
	global pe2_accel_y
	global pe2_accel_z
	global pe2_innercone
	global pe2_outercone
	global pe2_spin
	global pe2_spinaccel
	global pe2_freezetime
	global pe2_showgrid
	global pe2_showtriangles
	c=Frame(w,relief=groove,borderwidth=2)
	c=Pack(w,SIDE=TOP,fill=x)
# freezetime, showgrid, showtriangles
	f=Frame(c)
	f=Pack(SIDE=TOP,fill=x,pady=1)
	cb1=Checkbutton(f,text="freeze",indicatoron=1,variable=pe2_freezetime,command=pe2_toggletime)
	cb2=Checkbutton(f,text="grid",indicatoron=1,variable=pe2_showgrid,command=pe2_togglegrid)
	cb3=Checkbutton(f,text="tricount",indicatoron=1,variable=pe2_showtriangles,command=pe2_toggletriangles)
	cb1=Pack(SIDE=LEFT,padx=2)
	cb2=Pack(SIDE=LEFT,padx=2)
	cb3=Pack(SIDE=LEFT,padx=2)
# start/stop/repeat times
	f=w.c.f1
	f=Frame(,)
	f=Pack(,SIDE=TOP,fill=x,pady=1)
	l=Label(f,text="start/sTOP/repeat")
	l=Pack(f,SIDE=LEFT,padx=2)
	e1=Entry(f,width=8,textvariable=pe2_start_time,validate=focusout,vcmd="validate_cmd %P pe2_update_times" )
	e2=Entry(f,width=8,textvariable=pe2_sTOP_time,validate=focusout,vcmd="validate_cmd %P pe2_update_times" )
	e3=Entry(f,width=8,textvariable=pe2_repeat_time,validate=focusout,vcmd="validate_cmd %P pe2_update_times" )
	b1=Button(f,text="Reset",command=pe2_reset_times)
	b1=Pack(SIDE=RIGHT,padx=2)
	e1=Pack(SIDE=RIGHT,padx=2)
	e2=Pack(SIDE=RIGHT,padx=2)
	e3=Pack(SIDE=RIGHT,padx=2)
#	pe2_setup_focus ("$f.e1 $f.e2 $f.e3" )
# lifetime/freq/speed
	f=w.c.f2
	f=Frame(,)
	f=Pack(,SIDE=TOP,fill=x,pady=1)
	l=Label(f,text="lifetime/freq/speed")
	l=Pack(f,SIDE=LEFT,padx=2)
	e1=Entry(f,width=8,textvariable=pe2_lifetime,validate=focusout,vcmd="validate_cmd %P pe2_update_lifetime_freq" )
	e2=Entry(f,width=8,textvariable=pe2_freq,validate=focusout,vcmd="validate_cmd %P pe2_update_lifetime_freq" )
	e3=Entry(f,width=8,textvariable=pe2_speed,validate=focusout,vcmd="validate_cmd %P pe2_update_speed" )
	b1=Button(f,text="Reset",command="pe2_reset_lifetime_freq")
	b1=Pack(SIDE=RIGHT,padx=2)
	e1=Pack(SIDE=RIGHT,padx=2)
	e2=Pack(SIDE=RIGHT,padx=2)
	e3=Pack(SIDE=RIGHT,padx=2)
#	pe2_setup_focus ("$f.e1 $f.e2 $f.e3" )
# accel
	f=w.c.f3
	f=Frame(,)
	f=Pack(,SIDE=TOP,fill=x,pady=1)
	l=Label(f,text="global accel" )
	l=Pack(f,SIDE=LEFT,padx=2)
	e1=Entry(f,width=8,textvariable=pe2_accel_x,validate=focusout,vcmd="validate_cmd %P pe2_update_accel" )
	e2=Entry(f,width=8,textvariable=pe2_accel_y,validate=focusout,vcmd="validate_cmd %P pe2_update_accel" )
	e3=Entry(f,width=8,textvariable=pe2_accel_z,validate=focusout,vcmd="validate_cmd %P pe2_update_accel" )
	b1=Button(f,text="Reset",command=pe2_reset_accel)
	b1=Pack(SIDE=RIGHT,padx=2)
	e1=Pack(SIDE=RIGHT,padx=2)
	e2=Pack(SIDE=RIGHT,padx=2)
	e3=Pack(SIDE=RIGHT,padx=2)
#	pe2_setup_focus ("$f.e1 $f.e2 $f.e3" )
# inner/outer cone
	f=w.c.f4
	f=Frame(,)
	f=Pack(,SIDE=TOP,fill=x,pady=1)
	l=Label(f,text="inner/outer cone" )
	l=Pack(f,SIDE=LEFT,padx=2)
	e1=Entry(f,width=8,textvariable=pe2_innercone,validate=focusout,vcmd="validate_cmd %P pe2_update_cones" )
	e2=Entry(f,width=8,textvariable=pe2_outercone,validate=focusout,vcmd="validate_cmd %P pe2_update_cones" )
	b1=Button(f,text="Reset",command=pe2_reset_cones)
	b1=Pack(SIDE=RIGHT,padx=2)
	e1=Pack(SIDE=RIGHT,padx=2)
	e2=Pack(SIDE=RIGHT,padx=2)
	e3=Pack(SIDE=RIGHT,padx=2)
#	pe2_setup_focus ("$f.e1 $f.e2" )
# spin/spin accel
	f=w.c.f5
	f=Frame(,)
	f=Pack(,SIDE=TOP,fill=x,pady=1)
	l=Label(f,text="spin/spin accel" )
	l=Pack(f,SIDE=LEFT,padx=2)
	e1=Entry(f,width=8,textvariable=pe2_spin,validate=focusout,vcmd="validate_cmd %P pe2_update_spin" )
	e2=Entry(f,width=8,textvariable=pe2_spinaccel,validate=focusout,vcmd="validate_cmd %P pe2_update_spin" )
	b1=Button(f,text="Reset",command=pe2_reset_spin)
	b1=Pack(SIDE=RIGHT,padx=2)
	e1=Pack(SIDE=RIGHT,padx=2)
	e2=Pack(SIDE=RIGHT,padx=2)
	e3=Pack(SIDE=RIGHT,padx=2)
#	pe2_setup_focus ("$f.e1 $f.e2" )
#'}'
#--------------------------------------------------------------------
#   proc update_keyeditor_keys
#   17-Jan-01   floh    created
#--------------------------------------------------------------------
def update_keyeditor_keys():
	global ke_num_channels
	global ke_chn_array
	global pe2_numkeys
	global pe2_keys
# configure the keyframe editor
	ke_num_channels=6
# red
	ke_chn_array[0]["name"]="red"
	ke_chn_array[0]["color"]="red"
	ke_chn_array[0]["minval"]=0.0
	ke_chn_array[0]["maxval"]=1.0
	ke_chn_array[0]["callback"]=pe2_updkeys_callback
	ke_chn_array[0]["numkeys"]=pe2_numkeys
	for i in range(0,pe2_numkeys):	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
		ke_chn_array[0][i]=pe2_keys[i]["red)
#'}'	# End of loop
# green
	ke_chn_array[1]["name"]="green"
	ke_chn_array[1]["color"]="green"
	ke_chn_array[1]["minval"]=0.0
	ke_chn_array[1]["maxval"]=1.0
	ke_chn_array[1]["callback"]=pe2_updkeys_callback
	ke_chn_array[1]["numkeys"]=pe2_numkeys
	# 'for','set','i','0}','{$i','<',pe2_numkeys,'incr','i}','{'	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
	for i in range(0,pe2_numkeys):	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
		ke_chn_array[1][i]=pe2_keys[i]["green)
#'}'	# End of loop
# blue
	ke_chn_array[2]["name"]="blue"
	ke_chn_array[2]["color"]="blue"
	ke_chn_array[2]["minval"]=0.0
	ke_chn_array[2]["maxval"]=1.0
	ke_chn_array[2]["callback"]=pe2_updkeys_callback
	ke_chn_array[2]["numkeys"]=pe2_numkeys
	# 'for','set','i','0}','{$i','<',pe2_numkeys,'incr','i}','{'	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
	for i in range(0,pe2_numkeys):	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
		ke_chn_array[2][i]=pe2_keys[i]["green)
#'}'	# End of loop
# alpha
	ke_chn_array[3]["name"]="alpha"
	ke_chn_array[3]["color"]="DarkGrey"
	ke_chn_array[3]["minval"]=0.0
	ke_chn_array[3]["maxval"]=1.0
	ke_chn_array[3]["callback"]=pe2_updkeys_callback
	ke_chn_array[3]["numkeys"]=pe2_numkeys
	# 'for','set','i','0}','{$i','<',pe2_numkeys,'incr','i}','{'	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
	for i in range(0,pe2_numkeys):	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
		ke_chn_array[3][i]=pe2_keys[i]["alpha)
#'}'	# End of loop
# bounce
	ke_chn_array[4]["name"]="bounce"
	ke_chn_array[4]["color"]="salmon"
	ke_chn_array[4]["minval"]=0.0
	ke_chn_array[4]["maxval"]=2.0
	ke_chn_array[4]["callback"]=pe2_updkeys_callback
	ke_chn_array[4]["numkeys"]=pe2_numkeys
	# 'for','set','i','0}','{$i','<',pe2_numkeys,'incr','i}','{'	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
	for i in range(0,pe2_numkeys):	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
		ke_chn_array[4][i]=pe2_keys[i]["bounce)
#'}'	# End of loop
# size
	ke_chn_array[5]["name"]="size"
	ke_chn_array[5]["color"]="turquoise2"
	ke_chn_array[5]["minval"]=0.0
	ke_chn_array[5]["maxval"]=5.0
	ke_chn_array[5]["callback"]=pe2_updkeys_callback
	ke_chn_array[5]["numkeys"]=pe2_numkeys
	# 'for','set','i','0}','{$i','<',pe2_numkeys,'incr','i}','{'	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
	for i in range(0,pe2_numkeys):	# CHECK -for {set i 0} {$i < $pe2_numkeys} {incr i} {
		ke_chn_array[5][i]=pe2_keys[i]["size"]
#'}'	# End of loop
#'}'
#--------------------------------------------------------------------
#   proc gen_key_editor
#   Generate the keyframe editor.
#   16-Jan-01   floh    created
#--------------------------------------------------------------------
def gen_key_editor(w):
	global ke_num_channels
	global ke_chn_array
	global pe2_numkeys
	global pe2_keys
	kf=Frame(w,relief=groove,borderwidth=2)
	kf=Pack(w,SIDE=TOP,fill=x)
	update_keyeditor_keys ()
	ke_create (w.kf)
#'}'
#--------------------------------------------------------------------
#   proc gen_window
#   Generate the entire window.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
def gen_window(w,x,y):
	toplevel (w)
	wm ('title',w,"Particle Ed2" )
	wm ('resizable',w,0,0)
	wm ('geometry',w,'+$x+$y')	# CHECK -wm geometry $w +$x+$y
	all=Frame(w,)
	all=Pack(w,SIDE=TOP)
	gen_control_frame (w.all)
	gen_key_editor (w.all)
	gen_loadsave_frame (w.all)
	#return w
#'}'
#--------------------------------------------------------------------
#   proc pe2_open_new
#   Open the particle ed, expects a name for the toplevel
#   window, and a x/y position on screen. Creates a new
#   default particle system.
#
#   19-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_open_new(win,x,y):
# create a default particle system
	new_psys ()
# create the particle ed window
	gen_window (win,x,y)
#'}'
#--------------------------------------------------------------------
#   proc pe2_open_load
#   Open the particle ed, expects a name for the toplevel
#   window, and a x/y position on screen. Load an existing particle
#   system.
#   19-Jan-01   floh    created
#--------------------------------------------------------------------
def pe2_open_load(win,x,y,file):
# create a default particle system
	load_psys (file)
# create the particle ed window
	gen_window (win,x,y)
#'}'
#--------------------------------------------------------------------
#   main
#   The main window.
#   15-Jan-01   floh    created
#--------------------------------------------------------------------
# comment out the following return to get standalone editor
return ()
# load tclnebula extension
# load tclnebula
load ('tk84')
nomads_home=set ('/sys/share/assigns/home.gets')
home=nomads_home
# Libs laden
execfile ("nsh.py")
execfile ("observerlib.py")
# initialize Nebula nobserver object
init_observer ()
# create a default particle system
new_psys ()
# create the particle ed window
gen_window ('.w',580,0)
# go!
set ('/sys/servers/gfx.setclearcolor',0,0,0,1)
set ('/observer.setsleep',0.02)
set ('/observer.setgrid',0)
set ('/observer.start')	# CHECK -/observer.start
#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
