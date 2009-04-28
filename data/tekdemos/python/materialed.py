#
#   Nebula TEKDEMO
#
#   Das Material, die Textur und das Mesh kann modifiziert werden.
#   Ein zusammengestutzter MATERIAL-ED.
#
#   created: 	17-Dec-98   floh
#	upd.:	    03-Aug-99   Bernd
#               18-Jan-2000 floh    + Env-Mapping gefixt,
#                                   + Textur-Button raus
#
#   THE MATERIAL EDITOR USES THE OLD NEBULA SYSTEM: VBUF, TEXTURE, MAT
#
#
#   (C) 1998,1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
#--------------------------------------------------------------------
#   mat_getstate()
#   Fragt das Objekt nach einem Renderstate und returniert
#   dessen Wert
#--------------------------------------------------------------------
def mat_getstate(rstate):
	global ('mat_name')
	return ('[',mat_name.getstate,rstate])
#--------------------------------------------------------------------
#   mat_setstate()
#   Setzt einen Renderstate.
#--------------------------------------------------------------------
def mat_setstate(rstate,rparam):
	global ('mat_name')
	$mat_name.setstate(rstate,rparam)
#--------------------------------------------------------------------
#   mat_getambient()
#   mat_getdiffuse()
#   mat_getspecular()
#   mat_getemission()
#   Fragt Objekt nach einer Lichteigenschaft
#--------------------------------------------------------------------
def mat_getambient():
	global ('mat_name')
	return ('[',mat_name.getambient])
def mat_getdiffuse():
	global ('mat_name')
	return ('[',mat_name.getdiffuse])
def mat_getspecular():
	global ('mat_name')
	return ('[',mat_name.getspecular])
def mat_getemission():
	global ('mat_name')
	return ('[',mat_name.getemission])
#--------------------------------------------------------------------
#   mat_setcolor
#--------------------------------------------------------------------
def mat_setcolor(cmd,var,comp,val):
#
# cmd -> setambient/setdiffuse/setspecular/setemission
# var -> mat_ambient/mat_diffuse...
# comp -> 0=r, 1=g, 2=b, 3=a
# val -> neuer Wert fuer r,g,b oder a
#
	set (0,0)
	set (1,1)
	set (2,2)
	set (3,3)
	global (var$0)
	global (var$1)
	global (var$2)
	global (var$3)
	global ('mat_name')
	set (var$comp,val)
	set ('r','[expr','[subst',$var$0]/255.0])
	set ('g','[expr','[subst',$var$1]/255.0])
	set ('b','[expr','[subst',$var$2]/255.0])
	set ('a','[expr','[subst',$var$3]/255.0])
	$mat_name.$cmd(r,g,b,a)
	return ()
#--------------------------------------------------------------------
#   mat_colorscale
#--------------------------------------------------------------------
def mat_colorscale(var,name,cmd,comp,fr):
#
# var  -> mat_amb, mat_diff,...
# name -> lesbarer Name fuer Slider
# cmd  -> setambient, setdiffuse,...
# comp -> 0=r, 1=g, 2=b, 3=a
# fr   -> Names des Vater-Frames
#
	global (var$comp)
	frame ('.r.$fr.f_$var$comp')
	pack ('.r.$fr.f_$var$comp','-side','top','-expand',1)
	label ('.r.$fr.f_$var$comp.label','-width',8,'-text',name)
	scale ('.r.$fr.f_$var$comp.scale','-length',90,'-orient','horizontal\')
	-from (0,'-to',255,'-showvalue',0,'-variable',var$comp\)
	-command ("mat_setcolor $cmd $var $comp" )
	label ('.r.$fr.f_$var$comp.num','-width',4,'-textvariable',var$comp)
	pack ('.r.$fr.f_$var$comp.label','.r.$fr.f_$var$comp.scale','.r.$fr.f_$var$comp.num','-side','left','-expand',1)
	return ()
#--------------------------------------------------------------------
#   mat_optionmenu
#
#   Baut ein Options-Menu, welches einen Renderstate mit
#   mehreren moeglichen Optionen verwaltet
#--------------------------------------------------------------------
def mat_optionmenu(var,name,rstate,args):
	global (var)
	frame ('.r.f_$rstate','-relief','sunken','-borderwidth',1)
	pack ('.r.f_$rstate','-side','left','-expand',1)
	label ('.r.f_$rstate.label','-width',20,'-text',name)
	menubutton ('.r.f_$rstate.mb','-width',20,'-textvariable',var\)
	-indicatoron ('1\')
	-menu ('.r.f_$rstate.mb.rnu\')
	-relief ('raised','-bd','2\')
	-highlightthickness ('2\')
	-direction ('flush')
	menu ('.r.f_$rstate.mb.rnu','-tearoff',0)
	foreach ('i',args,'{')
	.r.f_$rstate.mb.rnu('add','radiobutton','-label',i,'-variable',var\)
	-command ("mat_setstate $rstate $i" )
pack ('.r.f_$rstate.label','.r.f_$rstate.mb','-side','left','-expand',1)
return ()
#--------------------------------------------------------------------
#   mat_edit()
#
#   Main-Routine, bekommt Name eines nMaterial-Objekts, baut
#   das Editor-Fenster auf und wartet dann auf dessen
#   Zerstoerung.
#
#   created:    17-Dec-98   floh
#   upd.:       02-Aug-98   Bernd
#
#--------------------------------------------------------------------
												def mat_edit(name):
# globale Variablen, die die Material-Beschreibung spiegeln
global ('mat_name')
global ('mat_texture')
global ('mat_textureblend')
global ('mat_alphablendenable')
global ('mat_amb')
global ('mat_amb0')
global ('mat_amb1')
global ('mat_amb2')
global ('mat_amb3')
global ('mat_diff')
global ('mat_diff0')
global ('mat_diff1')
global ('mat_diff2')
global ('mat_diff3')
global ('mat_emm')
global ('mat_emm0')
global ('mat_emm1')
global ('mat_emm2')
global ('mat_emm3')
# erstmal sehen, ob Objekt existiert und den richtigen Typ hat...
set ('old_cwd','[psel]')
if ('{[sel',name],'==',name},'{')
if ('{[','isa','nmatnode]}','{')
puts ("Object accepted." )
												puts ("Not a nmaterial object!" )
												return (0)
										puts ("Object does not exist!" )
										return (0)
									sel (old_cwd)
									set ('mat_name','[',name.getfullname])
# globale Variablen auf Stand des Objekts bringen
									set ('mat_texture','[mat_getstate','texture_enable]')
									set ('mat_textureblend','[mat_getstate','texture_blend]')
									set ('mat_alphablendenable','[mat_getstate','alpha_enable]')
									set ('mat_amb','[',mat_name.getambient,']')
									set ('mat_diff','[',mat_name.getdiffuse,']')
									set ('mat_emm','[',mat_name.getemission,']')
									set ('mat_amb0','[expr','[lindex',mat_amb,'0]*255.0]')
									set ('mat_amb1','[expr','[lindex',mat_amb,'1]*255.0]')
									set ('mat_amb2','[expr','[lindex',mat_amb,'2]*255.0]')
									set ('mat_amb3','[expr','[lindex',mat_amb,'2]*255.0]')
									set ('mat_diff0','[expr','[lindex',mat_diff,'0]*255.0]')
									set ('mat_diff1','[expr','[lindex',mat_diff,'1]*255.0]')
									set ('mat_diff2','[expr','[lindex',mat_diff,'2]*255.0]')
									set ('mat_diff3','[expr','[lindex',mat_diff,'3]*255.0]')
									set ('mat_emm0','[expr','[lindex',mat_emm,'0]*255.0]')
									set ('mat_emm1','[expr','[lindex',mat_emm,'1]*255.0]')
									set ('mat_emm2','[expr','[lindex',mat_emm,'2]*255.0]')
									set ('mat_emm3','[expr','[lindex',mat_emm,'2]*255.0]')
# Transparenz - Art (mat_tp) und Filtering (mat_filter) ergeben sich aus einer bestimmten
# Kombination mehrerer Einzelwerte, die ich zusammengefaﬂt habe, um  den Materialeditor
# als Demo verst‰ndlicher zu machen. Hm.
# Das war mir jetzte auf die schnelle zuviel
# das korrekt abzufragen. Deswegen setze ich die n‰chsten beiden Werte
# einfach so wie ich das vorgesehen habe. Ein korrektes Auslesen findet also hier
# nicht statt.
# Bei der Textur ud dem 3D-Objekt bin ich eben mal auch noch zu faul, daﬂ sauber abzufragen -
# sollte aber bei Gelegenheit mal gemacht werden. ( Is ja simpel. )
									global ('mat_tp')
									global ('mat_filter')
									global ('bild')
									global ('mesh')
									set ('mat_tp',"Standard")
									set ('mat_filter',"linear")
									set ('bild',"marble.bmp")
									set ('mesh',"nebula.n3d")
# ------------------------
# Window aufbauen
									toplevel ('.r')
									wm ('title','.r',"Material Editor" )
									wm ('geometry','.r','793x126+1+420')
									wm ('resizable','.r',0,0)
# -------------------
# CheckButtons
# -------------------
									frame ('.r.boolbuttons','-relief','sunken','-borderwidth',1)
									place ('.r.boolbuttons','-x',0,'-y',0,'-width',200,'-height',40)
									checkbutton ('.r.boolbuttons.envmap','\')
									-text ("Environment Mapping" ,'\')
									-onvalue ("sphere_map",'\')
									-offvalue ("off",'\')
									-variable ('mat_envmapenable','\')
									-command ('{',mat_name.setstate,'texgen_mode',mat_envmapenable,'}')
									pack ('.r.boolbuttons.envmap','-side','left','-expand',1)
#--------------------
# Filtering
#--------------------
									frame ('.r.filter','-relief','sunken','-borderwidth',1)
									place ('.r.filter','-x',200,'-y',0,'-width',185,'-height',40)
									label ('.r.filter.label','-width',6,'-text',"Filtering")
									menubutton ('.r.filter.mb1','-width',9,'-textvariable','mat_filter','\')
									-indicatoron (1,'\')
									-menu ('.r.filter.mb1.rnu','\')
									-relief ('raised','-bd',2,'\')
									-highlightthickness (2,'\')
									-direction ('flush')
									menu ('.r.filter.mb1.rnu','-tearoff',0)
									old_cwd1.r.filter.mb1.rnu('add','radiobutton','\')
									-label ('linear','\')
									-variable ('mat_filter','\')
									-command ('{',mat_name.setstate,'texture_min_filter',mat_filter)
									$mat_name.setstate('texture_mag_filter',mat_filter)
								old_cwd1.r.filter.mb1.rnu('add','radiobutton','\')
								-label ('nearest','\')
								-variable ('mat_filter','\')
								-command ('{',mat_name.setstate,'texture_min_filter',mat_filter)
								$mat_name.setstate('texture_mag_filter',mat_filter)
							pack ('.r.filter.label','.r.filter.mb1','-side','left','-expand',1)
							mat_optionmenu ('mat_textureblend',"texture blend" ,'\')
							texture_blend ('decal','modulate','replace','blend')
# -------------------------------------------------------------------------------------
# Textur-Image ausw‰hlen
# -------------------------------------------------------------------------------------
							frame ('.r.image','-relief','sunken','-borderwidth',1)
							place ('.r.image','-x',385,'-y',0,'-width',185,'-height',40)
							menubutton ('.r.image.mb1','-width',20,'-textvariable','bild','\')
							-indicatoron (1,'\')
							-menu ('.r.image.mb1.rnu','\')
							-relief ('raised','-bd',2,'\')
							-highlightthickness (2,'\')
							-direction ('flush')
							menu ('.r.image.mb1.rnu','-tearoff',0)
							cd ('bmp')
							foreach ('bmp','[lsort','-ascii','[glob','-nocomplain','--','*.{bmp}]]','{')
							old_cwd1.r.image.mb1.rnu('add','radiobutton','\')
							-label (bmp,'\')
							-variable ('bild','\')
							-command ("$mat_name/../tex.settexture ../bmp/$bmp none" )
						cd ('..')
						pack ('.r.image.mb1','-side','top','-expand',1)
# -------------------------------------------------------------------------------------
# 3D-Mesh ausw‰hlen
# -------------------------------------------------------------------------------------
						frame ('.r.mesh','-relief','sunken','-borderwidth',1)
						place ('.r.mesh','-x',570,'-y',0,'-width',222,'-height',40)
						menubutton ('.r.mesh.mb1','-width',20,'-textvariable','mesh','\')
						-indicatoron (1,'\')
						-menu ('.r.mesh.mb1.rnu','\')
						-relief ('raised','-bd',2,'\')
						-highlightthickness (2,'\')
						-direction ('flush')
						menu ('.r.mesh.mb1.rnu','-tearoff',0)
						cd ('meshes')
						foreach ('msh','[lsort','-ascii','[glob','-nocomplain','--','*.{n3d}]]','{')
						old_cwd1.r.mesh.mb1.rnu('add','radiobutton','\')
						-label (msh,'\')
						-variable ('mesh','\')
						-command ("$mat_name/../vbuf.setvbuffer meshes/$msh" )
					cd ('..')
					pack ('.r.mesh.mb1','-side','top','-expand',1)
# -------------------------------------------------------------------------------------
# Alphablending
# -------------------------------------------------------------------------------------
					frame ('.r.alpha','-relief','sunken','-borderwidth',1)
					place ('.r.alpha','-x',570,'-y',40,'-width',222,'-height',85)
					checkbutton ('.r.alpha.alpha','\')
					-onvalue ("true",'\')
					-text ("Transparency",'\')
					-offvalue ("false",'\')
					-variable ('mat_alphablendenable','\')
					-command ('{',mat_name.setstate,'alpha_enable',mat_alphablendenable})
					pack ('.r.alpha.alpha','-side','top','-expand',1)
					menubutton ('.r.alpha.mb1','-width',8,'-textvariable','mat_tp','\')
					-indicatoron (1,'\')
					-menu ('.r.alpha.mb1.rnu','\')
					-relief ('raised','-bd',2,'\')
					-highlightthickness (2,'\')
					-direction ('flush')
					menu ('.r.alpha.mb1.rnu','-tearoff',0)
					old_cwd1.r.alpha.mb1.rnu('add','radiobutton','\')
					-label ('Standard','\')
					-variable ('mat_tp','\')
					-command ('{',mat_name.setstate,'alpha_src_blend','srcalpha')
					$mat_name.setstate('alpha_dest_blend','invsrcalpha')
				old_cwd1.r.alpha.mb1.rnu('add','radiobutton','\')
				-label ('Additive','\')
				-variable ('mat_tp','\')
				-command ('{',mat_name.setstate,'alpha_src_blend','one')
				$mat_name.setstate('alpha_dest_blend','one')
			pack ('.r.alpha.mb1','-side','top','-expand',1)
			mat_colorscale ('mat_diff',"Transp.",'setdiffuse',3,'alpha')
# -------------------------------------------------------------------------------------
# Licht
# -------------------------------------------------------------------------------------
			frame ('.r.c1','-relief','sunken','-borderwidth',1)
			place ('.r.c1','-x',0,'-y',40,'-width',200,'-height',85)
			mat_colorscale ('mat_amb',"ambient r" ,'setambient',0,'c1')
			mat_colorscale ('mat_amb',"ambient g" ,'setambient',1,'c1')
			mat_colorscale ('mat_amb',"ambient b" ,'setambient',2,'c1')
			frame ('.r.c2','-relief','sunken','-borderwidth',1)
			place ('.r.c2','-x',200,'-y',40,'-width',185,'-height',85)
			mat_colorscale ('mat_diff',"diffuse r" ,'setdiffuse',0,'c2')
			mat_colorscale ('mat_diff',"diffuse g" ,'setdiffuse',1,'c2')
			mat_colorscale ('mat_diff',"diffuse b" ,'setdiffuse',2,'c2')
			frame ('.r.c3','-relief','sunken','-borderwidth',1)
			place ('.r.c3','-x',385,'-y',40,'-width',185,'-height',85)
			mat_colorscale ('mat_emm',"emission r" ,'setemission',0,'c3')
			mat_colorscale ('mat_emm',"emission g" ,'setemission',1,'c3')
			mat_colorscale ('mat_emm',"emission b" ,'setemission',2,'c3')
			return (1)
# ---------------------------------- main ---------------------------
		sel ('/usr/scene')
		execfile ('licht.py')
		std=new ('n3dnode','std')
		sel ('std')
		ty (-0.5)
		rx (90)
		mat=new ('nmatnode','mat')
		sel ('mat')
		mat.setstate('texture_enable',1)
		mat.setstate('texture_blend','modulate')
		mat.setstate('texture_min_filter','linear')
		mat.setstate('texture_mag_filter','linear')
		mat.setstate('texture_address_u','wrap')
		mat.setstate('texture_address_v','wrap')
		mat.setstate('alpha_enable',0)
		mat.setstate('alpha_src_blend','srcalpha')
		mat.setstate('alpha_dest_blend','invsrcalpha')
		mat.setstate('light_enable',1)
		mat.setstate('texgen_mode_u','off')
		mat.setstate('texgen_mode_v','off')
		mat.setambient(1,1,1,1)
		mat.setdiffuse(1,1,1,1)
		mat.setemission(0,0,0,0)
		mat.setshininess(0)
		sel ('..')
		tex=new ('ntexnode','tex')
		tex.settexture('../bmp/marble.bmp','none')
		vbuf=new ('nvbufnode','vbuf')
		vbuf.setvbuffer('../meshes/nebula.n3d')
		sel ('..')
		mat_edit ('/usr/scene/std/mat')
# EOF
