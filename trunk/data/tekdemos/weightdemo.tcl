#
#   Nebula TEKDEMO
#
#   Animations Blending
#
#   Zeigt wie verschiedene Animation ineinandergemixt werden können
#   ACHTUNG ! Besonderheit ! Dieses Script läuft nur korrekt,
#   wenn es mit nomads/data/tekdemos als aktuelles Arbeitsdirectory gestartet
#   wird. 
#
#   upd.:   18-May-2000
#   
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

source licht.tcl

#------------------------------------------------------------------------------
# Szene zusammenbasteln
#------------------------------------------------------------------------------

sc 
get     nosepaul.n
    sel  nosepaul
    txyz    -5 -5 -5

#------------------------------------------------------------------------------
# TK Fenster dazu
#------------------------------------------------------------------------------

toplevel     .r
wm title     .r     "ANIM BLENDING" 
wm geometry  .r     571x121+1+420
wm resizable .r     0 0

global w0
global w1
global w2
global w3
global w4
global w5

set   w0 0.0
set   w1 0.0
set   w2 0.0
set   w3 0.5
set   w4 0.0
set   w5 0.04

frame  .r.a
pack   .r.a -side left -expand 1
frame  .r.b
pack   .r.b -side left -expand 1
frame  .r.c
pack   .r.c -side left -expand 1

scale .r.a.regler0 -length 200 -orient horizontal	\
        -from 0.0 -to 1.0 -showvalue 0	        \
        -label Jump                            \
        -variable w0   -resolution 0.005        \
   	    -command { /sys/servers/channel.setchannel1f height }
pack   .r.a.regler0   -side top -expand 1

scale .r.a.regler1 -length 200 -orient horizontal	\
        -from 0.0 -to 1.0 -showvalue 0	        \
        -label Run \
        -variable w1   -resolution 0.005        \
   	    -command { /sys/servers/channel.setchannel1f walk }
pack   .r.a.regler1   -side top -expand 1

scale .r.b.regler3 -length 200 -orient horizontal	\
        -from 0.0 -to 1.0 -showvalue 0	        \
        -label Direction \
        -variable w3   -resolution 0.005        \
   	    -command { /sys/servers/channel.setchannel1f direction }
pack   .r.b.regler3   -side top -expand 1

scale .r.b.regler5 -length 200 -orient horizontal	\
        -from 0.008 -to 0.08 -showvalue 0	        \
        -label Speed \
        -variable w5   -resolution 0.0002        \
   	    -command { /sys/servers/time.lockdeltat  }
pack   .r.b.regler5   -side top -expand 1

scale .r.c.regler6 -length 200 -orient horizontal	\
        -from 0.0 -to 1.0 -showvalue 0	        \
        -label Yes \
        -variable w6   -resolution 0.0002        \
   	    -command { /sys/servers/channel.setchannel1f nick }
pack   .r.c.regler6   -side top -expand 1

