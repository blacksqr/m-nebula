----------------------------------------------------------------------
-- observerlib.lua
--
-- This is a conversion of the Tcl version written by Andre
----------------------------------------------------------------------

dofile(mangle('home:bin/console.lua'))

----------------------------------------------------------------------
--  init_gfxserver
----------------------------------------------------------------------
function init_gfxserver()
    a = nebula.sys.servers
    a.gfx:setviewvolume(-0.1, 0.1, -0.075, 0.075, 0.1, 2500)
    a.gfx:setclearcolor(0.5, 0.5, 0.5, 0.5)
    a.gfx:opendisplay()
end

----------------------------------------------------------------------
--  tod3d8
----------------------------------------------------------------------
function tod3d8()
    if exists('/sys/servers/gfx') then
        delete('/sys/servers/gfx')
    end
    gfx = newthunk('nd3d8server', '/sys/servers/gfx')
    gfx:setdisplaymode('dev(0)-w(580)-h(420)')
    init_gfxserver()
end

----------------------------------------------------------------------
--  togl
----------------------------------------------------------------------
function togl()           
    if exists('/sys/servers/gfx') then
        delete('/sys/servers/gfx')
    end
    gfx = newthunk('nglserver', '/sys/servers/gfx')
    gfx:setdisplaymode('dev(0)-w(580)-h(420)')
    init_gfxserver()
end

----------------------------------------------------------------------
--  orig
----------------------------------------------------------------------
function orig()
    a = nebula.usr
    a.lookat:txyz(0, 0, 0)
    a.lookat:rxyz(-25, 45, 0)
    a.camera:txyz(0, 0, 5)
end

function janaview()
    puts'janaview\n'
    a = nebula.usr
    a.lookat:txyz(0, 30, 0)
    a.lookat:rxyz(-15, 0, 0)
    a.camera:txyz(0, 0, 35)
end

function berndview()
    puts'berndview\n'
    a = nebula.usr
    a.lookat:txyz(0, 30, 0)
    a.lookat:rxyz(-15, 0, 0)
    a.camera:txyz(0, 0, 500)
end

function eriview()
    puts'tHA ALL N3W : eriview\n'
    a = nebula.usr
    a.lookat:txyz(0, 0, 0)
    a.lookat:rxyz(-25, 45, 0)
    a.camera:txyz(0, 0, 100)
end

function katiview()
    puts'fuer kati!!\n'
    a = nebula.usr
    a.lookat:txyz(0, 0, 0)
    a.lookat:rxyz(-15, 32, 0)
    a.camera:txyz(0, 0, 250)
end

function lookn()
    a = nebula.usr
    a.lookat:txyz(0, 0, 10)
    a.lookat:rxyz(0, 0, 0)
    a.camera:txyz(0, 100, 20)
end

function lookw()
    a = nebula.usr
    a.lookat:txyz(0, 0, 10)
    a.lookat:rxyz(0, 180, 0)
    a.camera:txyz(0, 0, 5)
end

function looke()
    a = nebula.usr
    a.lookat:txyz(0, 0, 10)
    a.lookat:rxyz(0, 270, 0)
    a.camera:txyz(0, 0, 5)
end

function looks()
    a = nebula.usr
    a.lookat:txyz(0, 0, 10)
    a.lookat:rxyz(0, 90, 0)
    a.camera:txyz(0, 0, 5)
end

----------------------------------------------------------------------
--  map_input
----------------------------------------------------------------------
function map_input()
    a = nebula.sys.servers.input
    a:beginmap()
    a:map('keyb0:shift.pressed', 'pan')
    a:map('keyb0:ctrl.pressed',  'orbit')
    a:map('mouse0:btn0.pressed', 'pan')
    a:map('mouse0:btn1.pressed', 'orbit')
    a:map('mouse0:btn2.pressed', 'dolly')
    a:map('keyb0:space.down',    'script:orig()')
    a:map('keyb0:j.down',        'script:janaview()')
    a:map('keyb0:b.down',        'script:berndview()')
    a:map('keyb0:e.down',        'script:eriview()')
    a:map('keyb0:k.down',        'script:katiview()')        
    a:map('keyb0:esc.down',      'script:nebula.sys.servers.console:toggle()')
    a:map('keyb0:f2.down',       'script:nebula.sys.servers.console:watch("gfx*")')
    a:map('keyb0:f2.up',         'script:nebula.sys.servers.console:unwatch()')
    a:endmap()
end

----------------------------------------------------------------------
--  private_init_observer
----------------------------------------------------------------------
function private_init_observer()
    -- get platform has been removed as there
    -- isn't any facility in Lua to the OS
    -- environment.  Due to this Gl and a 
    -- generic input server are all that will
    -- be created by default
    
    togl()

    pushcwd('/sys/servers')
    new('ninputserver',         'input')
    new('nscenegraph2',         'sgraph2')
    new('nsbufshadowserver',    'shadow')
    new('nchannelserver',       'channel')
    new('nconserver',           'console')
    new('nmathserver',          'math')
    new('nparticleserver',      'particle')
    new('nspecialfxserver',     'specialfx')
    popcwd()
    
    map_input()

    new('nroot', '/usr')
    new('n3dnode', '/usr/scene')
    new('n3dnode', '/usr/camera')
    new('n3dnode', '/usr/lookat')

    orig()
end

----------------------------------------------------------------------
--  init_observer
----------------------------------------------------------------------
function init_observer()
    private_init_observer()
    observer = newthunk('nobserver', '/observer')
    observer:setsleep(0)
end

puts('To start type: init_observer(); nebula.observer:start()')

----------------------------------------------------------------------
--  EOF
----------------------------------------------------------------------
