#
#   startup.tcl
#
#   Simple startup script for a basic gfxserv initialisation used in the NEBULA TUTORIAL.
#
#   created:    16-Aug-99   Bernd Beyreuhter
#   upd.:       16-Aug-99
#               11-May-00   floh    added specialfx server
#               24-Nov-00   bernd   new shader/mesh system 
#				20-Aug-01	leaf	new scenegraph2 etc
#
#   (C) 1999 Radon Labs
#   Beyreuther, Weissflog, Flemming GbR
#
#   (C) 2000 RADON LABS GMBH, BERLIN
#

# -------------------------------------------------------------------
# ndir implements a simple ls command for the nebula console
# -------------------------------------------------------------------
proc ndir {} {
    set cur_dir [psel]
    set result ""
    set x [.gethead]
    while {$x != "null"} {
        sel $x
        lappend result [.getname]
        set x [.getsucc]
    }
    sel $cur_dir
return $result
}

# -------------------------------------------------------------------
# orig resets the camera view
# -------------------------------------------------------------------
proc orig { } {
    /usr/lookat.txyz   0    0    0
    /usr/lookat.rxyz  -25   45   0
    /usr/camera.tz     5 
}

# -------------------------------------------------------------------
#   MAIN
# -------------------------------------------------------------------


new     nglserver       /sys/servers/gfx
# To use Direct 3D Acceleration, replace previous line with
#new   nd3d8server      /sys/servers/gfx

new     ninputserver    /sys/servers/input
# To use Direct Input (Joystick Support etc.), replace previous line with
# new     ndxinputserver    /sys/servers/input

# basic servers 
new nscenegraph2        /sys/servers/sgraph2
new nsbufshadowserver   /sys/servers/shadow
new nchannelserver      /sys/servers/channel
new nconserver          /sys/servers/console
new nmathserver         /sys/servers/math
new nparticleserver     /sys/servers/particle
new nspecialfxserver    /sys/servers/specialfx
new nfileserver2        /sys/servers/file2
new nprimitiveserver    /sys/servers/primitive

/sys/servers/gfx.setdisplaymode  w(640)-h(480)
/sys/servers/gfx.setviewvolume  -0.1 +0.1 -0.075 +0.075 +0.1 +2500

# Adjust Background Color here ( RGBA 0 - 1 )
/sys/servers/gfx.setclearcolor   0.0 0.2 0.3 0

/sys/servers/gfx.opendisplay

/sys/servers/input.beginmap
/sys/servers/input.map keyb0:shift.pressed    pan
/sys/servers/input.map keyb0:ctrl.pressed     orbit
/sys/servers/input.map mouse0:btn0.pressed    pan
/sys/servers/input.map mouse0:btn1.pressed    orbit
/sys/servers/input.map mouse0:btn2.pressed    dolly
/sys/servers/input.map keyb0:space.down       "script:orig"
/sys/servers/input.map keyb0:esc.down         "script:/sys/servers/console.toggle"
/sys/servers/input.endmap

new     n3dnode     /usr/scene
new     n3dnode     /usr/camera
new     n3dnode     /usr/lookat

orig
new nobserver /observer

# this restricts framerate, to be OS friendly
/observer.setsleep  0.02

# Turn on/off grid here
/observer.setgrid true

/observer.start
