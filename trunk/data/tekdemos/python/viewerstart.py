#
#   Open GL Startup Code f�r die TEKDEMOS
#   Workaround since NLAUNCH accepts no additional arguments.
#   nebula home has to be the current working dir
#
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
set ('nomads_home','[/sys/share/assigns/home.gets]')
set ('home',nomads_home)
set ('3ddevice',",'OpenGL',")
execfile (home/data/tekdemos/viewer.tcl)
