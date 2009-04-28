#--------------------------------------------------------------------
#   observerlib.tcl
#   ===============
#   Support-Funktionen fuer Gfx-Server-Ersatz.
#
#   tod3d6
#   tod3d7hal
#   tod3d7emu
#   togl
#   orig
#   map_input
#   init_observer
#
#   26-Nov-99   floh    Input-Mappings mit .beginmap/.endmap
#   15-Feb-00   floh    + win2k aware (needs get_platform from nsh.tcl)
#   17-Feb-02   floh    + nfileserver2 added
#
#   (C) 1999 A.Weissflog
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   init_gfxserver
#   Stellt die gewuenschten Gfx-Server-Parameter ein. Wird z.B.
#   beim Wechsel des Gfxserv per tod3d oder togl aufgerufen.
#   26-Jul-99   floh    created
#   28-Jul-99   bernd   
#--------------------------------------------------------------------
proc init_gfxserver { } {
    /sys/servers/gfx.setviewvolume -0.1 +0.1 -0.075 +0.075 +0.1 +2500
    /sys/servers/gfx.setclearcolor 0.5 0.5 0.5 0.5
    /sys/servers/gfx.opendisplay
}

#--------------------------------------------------------------------
#   tod3d7hal
#   Schaltet nach Direct3D V7 HAL Device
#   13-Jan-00   floh    created   
#--------------------------------------------------------------------
proc tod3d7hal { } {
    if {[exists /sys/servers/gfx]} { delete /sys/servers/gfx }
    new nd3d7server /sys/servers/gfx
    /sys/servers/gfx.setdisplaymode dev(0)-w(580)-h(420)
    init_gfxserver
}

#--------------------------------------------------------------------
#   tod3d7tnlhal
#   Switch to d3d7 hardware tl rasterizer (only if supported)
#   13-Jan-00   floh    created
#--------------------------------------------------------------------
proc tod3d7tnlhal { } {
    if {[exists /sys/servers/gfx]} { delete /sys/servers/gfx }
    new nd3d7server /sys/servers/gfx
    if {[exists /sys/share/display/10]} {
        /sys/servers/gfx.setdisplaymode dev(10)-w(580)-h(420)
    } else {
        puts "d3d hardware tl not supported!"
        /sys/servers/gfx.setdisplaymode w(580)-h(420)
    }
    init_gfxserver
}

#--------------------------------------------------------------------
#   tod3d7emu
#   Schaltet nach Direct3D V7 RGB Emulation
#   13-Jan-00   floh    created
#--------------------------------------------------------------------
proc tod3d7emu { } {
    if {[exists /sys/servers/gfx]} { delete /sys/servers/gfx }
    new nd3d7server /sys/servers/gfx
    /sys/servers/gfx.setdisplaymode dev(20)-w(580)-h(420)
    init_gfxserver
}

#--------------------------------------------------------------------
#   tod3d7ref
#   Switch to d3d7 reference rasterizer (only if supported)
#   13-Jan-00   floh    created
#--------------------------------------------------------------------
proc tod3d7ref { } {
    if {[exists /sys/share/display/21]} {
        if {[exists /sys/servers/gfx]} { delete /sys/servers/gfx }
        new nd3d7server /sys/servers/gfx
        /sys/servers/gfx.setdisplaymode dev(21)-w(580)-h(420)
        init_gfxserver
    } else {
        puts "d3d reference rasterizer not supported!"
    }
}

#--------------------------------------------------------------------
#   tod3d8
#   Schaltet nach Direct3D V8
#   13-Jan-00   floh    created   
#--------------------------------------------------------------------
proc tod3d8 { } {
    if {[exists /sys/servers/gfx]} { delete /sys/servers/gfx }
    new nd3d8server /sys/servers/gfx
    /sys/servers/gfx.setdisplaymode dev(0)-w(580)-h(420)
    init_gfxserver
}

#--------------------------------------------------------------------
#   togl
#   Schaltet nach OpenGL
#   26-Jul-99   floh    created
#--------------------------------------------------------------------
proc togl { } {
    if {[exists /sys/servers/gfx]} { delete /sys/servers/gfx }
    new nglserver /sys/servers/gfx/
    /sys/servers/gfx.setdisplaymode w(580)-h(420)
    init_gfxserver
}

#--------------------------------------------------------------------
#   orig
#   Stellt Kamera und Lookat-Node auf Ausgangs-Position ein
#   26-Jul-99   floh    created
#   28-Jul-99   bernd   Standard-Settings von Rechts Oben
#   10-Nov-99   bernd   Camera Positioning currently is somehow strange /
#                       may not work as expected.
#                       Camera Animation will not work.
#                       Will be fixed later.
#--------------------------------------------------------------------
proc orig { } {
    /usr/lookat.txyz   0    0    0
    /usr/lookat.rxyz  -25   45   0
    /usr/camera.tz     5 
}

proc janaview { } {
    puts "janaview"
    /usr/lookat.txyz   0    30   0
    /usr/lookat.rxyz   -15  0    0
    /usr/camera.tz     35
}


proc berndview { } {
    puts "berndview"
    /usr/lookat.txyz   0    30   0
    /usr/lookat.rxyz   -15  0    0
    /usr/camera.tz     500
}

proc eriview { } {
    puts "tHA ALL N3W : eriview"
    /usr/lookat.txyz   0    0    0
    /usr/lookat.rxyz  -25   45   0
    /usr/camera.tz     100
}

proc katiview { } {
    puts "fuer kati!!"
    /usr/lookat.txyz   0    0   0
    /usr/lookat.rxyz   -15  32  0
    /usr/camera.tz     250
}

proc lookn { } {
    /usr/lookat.txyz   0    0   10
    /usr/lookat.rxyz   0    0   0
    /usr/camera.txyz   0    100 20  
}

proc lookw { } {
    /usr/lookat.txyz   0     0   10
    /usr/lookat.rxyz   0   180   0
    /usr/camera.tz     5 
}

proc looke { } {
    /usr/lookat.txyz   0     0   10
    /usr/lookat.rxyz   0   270   0
    /usr/camera.tz     5 
}

proc looks { } {
    /usr/lookat.txyz   0     0   10
    /usr/lookat.rxyz   0   90   0
    /usr/camera.tz     5 
}

#--------------------------------------------------------------------
#   map_input
#   Input-Mapping fuer nobserver Klasse.
#   26-Jul-99   floh    created
#   26-Nov-99   floh    .beginmap/.endmap
#   20-Dec-99   floh    + Konsole an/aus
#   22-Dec-99   floh    + an neues Input-Mapping angepasst   
#--------------------------------------------------------------------
proc map_input { } {
    set cwd [psel]
        sel /sys/servers/input
        .beginmap
        .map keyb0:shift.pressed    pan
        .map keyb0:ctrl.pressed     orbit
        .map mouse0:btn0.pressed    pan
        .map mouse0:btn1.pressed    orbit
        .map mouse0:btn2.pressed    dolly
        .map keyb0:space.down       "script:orig"
        .map keyb0:j.down           "script:janaview"
        .map keyb0:b.down           "script:berndview"
        .map keyb0:e.down           "script:eriview"
        .map keyb0:k.down           "script:katiview"        
        .map keyb0:esc.down         "script:/sys/servers/console.toggle"
        .map keyb0:f2.down          "script:/sys/servers/console.watch gfx*"
        .map keyb0:f2.up            "script:/sys/servers/console.unwatch"
        .endmap
    sel $cwd
}

#--------------------------------------------------------------------
#   private_init_observer
#   23-May-00   floh    created
#--------------------------------------------------------------------
proc private_init_observer { } {
global 3ddevice

    set platform [get_platform]
    switch [get_platform] {
        win9x -
        win2k {
            puts "Using Win9x/Win2k settings."
            if  [ catch { set test $3ddevice } err ] {
                set 3ddevice " OpenGL "
            }    
            if { $3ddevice==" OpenGL "   }        { togl }
            if { $3ddevice==" Direct3D V6 " }     { tod3d6 }
            if { $3ddevice==" Direct3D V7 HAL " } { tod3d7hal }
            if { $3ddevice==" Direct3D V7 EMU " } { tod3d7emu }
            if { $3ddevice==" Direct3D V8 " }     { tod3d8 }

            new ninputserver /sys/servers/input
        }

        winnt {
            puts "Using WinNT settings."
            togl
            new ninputserver /sys/servers/input
        }

        unix {
            puts "Using Unix settings."
            togl
            new ninputserver /sys/servers/input
        }

        default {
            puts "Unknown platform."
            exit
        }
    }

    new nfileserver2        /sys/servers/file2
    new nscenegraph2        /sys/servers/sgraph2
    new nsbufshadowserver   /sys/servers/shadow
    new nchannelserver      /sys/servers/channel
    new nconserver          /sys/servers/console
    new nmathserver         /sys/servers/math
    new nparticleserver     /sys/servers/particle
    new nspecialfxserver    /sys/servers/specialfx
    new nprimitiveserver    /sys/servers/primitive

    map_input

    new n3dnode /usr/scene
    new n3dnode /usr/camera
    new n3dnode /usr/lookat

    orig
}

#--------------------------------------------------------------------
#   init_observer
#   26-Jul-99   floh    created
#   27-Jul-99   bernd   Plattformabhängige Server
#   12-Aug-99   bernd   Zwar wird immer noch die Unterscheidung
#                       zwischen den Plattformen vorgenommen, aber
#                       eigentlich verwenden alle die selben Server.
#   18-Aug-99   bernd   Wenn 3ddevice da ist, dann so initialisieren
#                       wie es dort steht.
#   15-Feb-00   floh    + Win2k aware (needs nsh.tcl however)
#   01-Apr-00   floh    + added particle server
#   11-May-00   floh    + added specialfx server
#   23-May-00   floh    + ruft private_init_observer auf
#--------------------------------------------------------------------
proc init_observer { } {
    private_init_observer
    new nobserver /observer
    /observer.setsleep 0
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
