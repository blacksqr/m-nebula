#
#   Direct 3D Startup Code for the NEBULA TEKDEMOS
#   Workaround since NLAUNCH accepts no additional arguments.
#   nebula home has to be the current working dir
#
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#
#	20-Aug-01	leaf	switched to d3d8, for shadow demo
#   20-Sep-01   leaf    switched back to d3d7, works on more cards

set     nebula_home    [/sys/share/assigns/home.gets]
set     home           $nebula_home
set     3ddevice      " Direct3D V8 "
source  $home/data/tekdemos/tekdemos.tcl
