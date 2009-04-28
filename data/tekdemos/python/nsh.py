#!/bin/sh
#
#	/nomads/bin/nsh.tcl
#
#	Stellt verschiedene Standard-Tools und Spracherweiterungen
#	für die NebulaSHell und für die Standard-Nebula-Scripts
#	zur Verfügung.
#	Mit nsh_help bekommt eine Übersicht über die implementierten Befehle.
#
#   *** ACHTUNG !!! Alle DEMO-spezifischen Befehle habe ich
#   nach /nomads/scripts/commands.tcl
#   verlegt. (Flieger, Samml, etc.)
#
#
#	created:	Okt-1998
#	upd.:		25-May-99
#               21-Jun-99   floh    "<error>" durch "null" ersetzt
#               22-Jun-99   bernd   Help gekillt
#               15-Feb-00   floh    + new proc get_platform()
#
#	(c) 1998-1999 Bernd Beyreuther
#
# set         home                $env(NOMADS_HOME)
# source      $home/bin/assigns.tcl
#--------------------------------------------------------------------

import npython

#   get_platform()
#   Detect platform and return one of the following strings:
#   win9x   -> Windows 95 and 98
#   winnt   -> Windows NT 4
#   win2k   -> Windows 2000
#   unix    -> Unix variant
#   unknown -> unknown
#--------------------------------------------------------------------
def get_platform():
	return "win2k"
#	global ('tcl_platform')
#	set ('p','"unknown"')
#	if ('{$tcl_platform(platform)','==','"windows"}','{')
#	if ('{$tcl_platform(os)','==','"Windows','NT"}','{')
#	if ('{$tcl_platform(osVersion)','>=','5.0}','{')
#	set ('p','"win2k"')
#	puts ('"Windows',2000,'detected"')
#	set ('p','"winnt"')
#	puts ('"Windows','NT','detected"')
#	set ('p','"win9x"')
#	puts ('"Windows9x','detected"')
#	set ('p','"unix"')
#	puts ('"Unix','detected"')
#	set ('p','"unknown"')
##
#	return (p)
# -------------------------------------------------------------------
def st():
	npython.sel ('/usr/scene/std')
	return ()
# -------------------------------------------------------------------
def sc():
	npython.sel ('/usr/scene')
	return ()
# -------------------------------------------------------------------
def roty():
#	catch ('{','delete','roty','}','err')
#	catch ('{','delete','roty2','}','err')
	roty=npython.new ('nipol','roty')
	npython.sel ('roty')
	roty.connect('ry')
	roty.addkey1f(0,0)
	roty.addkey1f(3,360)
	npython.sel ('..')
	return ()
# -------------------------------------------------------------------
def roty2():
	catch ('{','delete','roty','}','err')
	catch ('{','delete','roty2','}','err')
	roty2=npython.new ('nipol','roty2')
	npython.sel ('roty2')
	roty2.connect('ry')
	roty2.addkey1f(0,360)
	roty2.addkey1f(3,0)
	npython.sel ('..')
	return ()
# -------------------------------------------------------------------
def rotx():
	rotx=npython.new ('nipol','rotx')
	npython.sel ('rotx')
	rotx.connect('rx')
	rotx.addkey1f(0,0)
	rotx.addkey1f(5,360)
	npython.sel ('..')
	return ()
# -------------------------------------------------------------------
def rotz():
	rotz=npython.new ('nipol','rotz')
	npython.sel ('rotz')
	rotz.connect('rz')
	rotz.addkey1f(0,0)
	rotz.addkey1f(5,360)
	npython.sel ('..')
	return ()
# -------------------------------------------------------------------
def stop():
	catch ('{','delete','rotx','}','err')
	catch ('{','delete','roty','}','err')
	catch ('{','delete','roty2','}','err')
	catch ('{','delete','rotz','}','err')
	set("rxyz",0,0,0)
	return ()
# -------------------------------------------------------------------
def gfx():
	#connect ('localhost:gfxserv')
	npython.sel ('/')
	return ()
# -------------------------------------------------------------------
def l():
	npython.sel ('/')
	npython.sel ('usr')
	npython.sel ('scene')
	npython.sel ('dlight')
	return ()
# -------------------------------------------------------------------
#def ndir():
#	set ('cur_dir','[psel]')
#	set ('result','""')
#	set ('x','[.gethead]')
#	while ('{$x','!=','"null"}','{')
#	sel (x)
#	lappend ('result','[.getname]')
#	set ('x','[.getsucc]')
#	sel (cur_dir)
#	return (result)
# -------------------------------------------------------------------
def new_objects(class1,name,number):
	pass
#	for ('{set','i','0}','{$i','<',number},'{incr','i','1}','{')
#	name$i1=new (class,name$i)
#	return ()
# -------------------------------------------------------------------
def ndir2():
	pass
#	#set ('dirinhalt','[ndir]')
#	foreach ('eintrag',dirinhalt,'{')
#	puts ('stdout',eintrag)
#	return ()
# -------------------------------------------------------------------
def tree():
#	set ('cur_dir','[psel]')
#	set ('result','""')
#	set ('x','[.gethead]')
#	while ('{$x','!=','"null"}','{')
#	sel (x)
#	puts ('stdout','[.getfullname]')
#	tree ()
#	set ('x','[.getsucc]')
#	sel (cur_dir)
	return ()
# -------------------------------------------------------------------
def licht():
#	global ('home')
#	set ('current','[getfullname]')
#	sel ('/usr/scene/dlight/light1')
#	if ('{[','.getactive',']','==','"true"}','{')
#	puts ('stdout','"licht','aus"')
#	cur_dir1.setactive='false'
#	sel ('/usr/scene/dlight/light2')
#	cur_dir1.setactive='false'
#	puts ('stdout','"-','LICHT','AN','-"')
#	cur_dir1.setactive='true'
#	sel ('/usr/scene/dlight/light2')
#	cur_dir1.setactive='true'
#	sel (current)
	return ()
# -------------------------------------------------------------------
def zeit():
#	set ('current','[getfullname]')
#	sel ('/sys/servers/time')
	# ask two times with a break (!), because otherwise gettime will
	# return same value on fast machines
#	set ('time1','[.gettime]')
#	after (10)
#	set ('time2','[.gettime]')
#	if ('{$time1','==',time2},'{')
#	current1.starttime=()
#	current1.stoptime=()
#	sel (current)
	return ()
# -------------------------------------------------------------------
def viewlod():
#	set ('/sys/servers/console.watch','sg*')
	return ()
# -------------------------------------------------------------------
def init_loddisp():
#	sg_lod=new ('nenv','/sys/var/sg_lod')
#	refresh_loddisp ()
	return ()
# -------------------------------------------------------------------
#   13-Nov-00   floh    standard-vbuf Name jetzt 'mesh'
#
# -------------------------------------------------------------------
def refresh_loddisp():
	#	set ('obj','""')
	#	if ('{[exists','/usr/scene/std]}','{')
	#        /sys/var/sg_lod.setf [ /usr/scene/std.getcurrentlod ]
	#        after 100 refresh_loddisp
	pass
