-- start.lua
-- startup script for Nebulized ODE buggy test.

dofile( mangle( 'home:bin/nebthunker.lua' ) )

function SetAssign( name, directory )
  local path = mangle( 'home:data/odetests/'..directory..'//' )
  local fs = lookup( '/sys/servers/file2' )
  puts( path )
  fs:setassign( name, path )
end

-- gfxClass = 'nd3d8server'
gfxClass = 'nglserver'

inputClass = 'ninputserver'
osName = os.getenv( 'OS' ) -- for some reason os isn't a global name... hmm.
if ( osName ~= nil ) then 
  osName = string.upper( osName )
  if ( string.find( osName, 'WIN', 1, true ) ) then 
    inputClass = 'ndi8server'
  end
end
puts( 'using input server: '..inputClass..'\n' )


-- create servers

new( gfxClass,              '/sys/servers/gfx' )
new( inputClass,            '/sys/servers/input' )
new( 'nconserver',          '/sys/servers/console' )
new( 'nscenegraph2',        '/sys/servers/sgraph2' )
new( 'nsbufshadowserver',   '/sys/servers/shadow' )
new( 'nchannelserver',      '/sys/servers/channel' )
new( 'nspecialfxserver',    '/sys/servers/specialfx' )
new( 'nprimitiveserver',    '/sys/servers/primitive' )

-- setup OdePhysics
new( 'nodeserver',          '/sys/servers/ode' )
new( 'nodephysicscontext',  '/sys/servers/ode/contexts/physics' )
new( 'nodecollidecontext',  '/sys/servers/ode/contexts/collide' )


-- specify assigns

SetAssign( 'meshes', 'meshes' )

-- open display

gfx = lookup( '/sys/servers/gfx' )
gfx:setdisplaymode( 'dev(0)-type(win)-w(640)-h(480)-bpp(32)' )
gfx:setviewvolume( -0.1, 0.1, -0.075, 0.075, 0.1, 5000.0 )
gfx:setclearcolor( 0.5, 0.5, 0.5, 1.0 )

-- init shadow server

nebula.sys.servers.shadow:setcastshadows( true )

-- map input

dofile( mangle( 'home:data/odetests/input.lua' ) )

-- setup lights

new( 'nroot', '/lights' )
dofile( mangle( 'home:data/odetests/light.lua' ) )

-- setup collision

ode = lookup( '/sys/servers/ode' )

ode:begincollclasses()
  ode:addcollclass( 'ground' )
  ode:addcollclass( 'car' )
  ode:addcollclass( 'blob' )
ode:endcollclasses()

ode:begincolltypes()
  ode:addcolltype( 'car', 'car', 'ignore' )
  ode:addcolltype( 'ground', 'ground', 'ignore' )
  ode:addcolltype( 'ground', 'car', 'exact' )
  ode:addcolltype( 'blob', 'blob', 'exact' )
  ode:addcolltype( 'blob', 'ground', 'exact' )
ode:endcolltypes()

ode:setcollidecontext( '/sys/servers/ode/contexts/collide' )

-- use a simple space
new( 'nodesimplespace',     '/sys/servers/ode/spaces/root' )
-- or a hash space
--new( 'nodehashspace',       '/sys/servers/ode/spaces/root' )
-- or a quad tree space
--new( 'nodequadtreespace',   '/sys/servers/ode/spaces/root' )
--sel( '/sys/servers/ode/spaces/root' )
--call( 'setparams', 0, 0, 0, 10, 0, 10, 7 )

-- associate a collide context with the space
sel( '/sys/servers/ode/spaces/root' )
call( 'setcontext', '/sys/servers/ode/contexts/collide' )

-- setup physics

ode:setphysicscontext( '/sys/servers/ode/contexts/physics' )

ode:beginsurfaces( 2 )
  ode:addsurface( 0, 'buggy' )
  ode:setsurfparam( 0, 'mu', -1.0 )
  --ode:setsurfparam( 0, 'mu', 1.0 )
  --ode:setsurfparam( 0, 'slip1', 0.1 )
  --ode:setsurfparam( 0, 'slip2', 0.1 )
  ode:setsurfparam( 0, 'slip1', 1.5 )
  ode:setsurfparam( 0, 'slip2', 1.5 )
  ode:setsurfparam( 0, 'erp', 0.5 )
  ode:setsurfparam( 0, 'cfm', 0.3 )
  ode:enablesurfparam( 0, 'approxfboth' )
  
  ode:addsurface( 1, 'blob' )
  ode:setsurfparam( 1, 'mu', -1.0 )
  ode:setsurfparam( 1, 'mu2', 0.0 )
  ode:setsurfparam( 1, 'erp', 0.9 )
  ode:setsurfparam( 1, 'cfm', 0.0005 ) -- prevents sphere/mesh jitter
  ode:enablesurfparam( 1, 'approxfboth' )
ode:endsurfaces()

phys = lookup( '/sys/servers/ode/contexts/physics' )
phys:setgravity( 0.0, 0.0, -0.5 )
--phys:setcfm( 0.001 )

-- work

nebula.sys.servers.gfx:opendisplay()
new( 'nodetest', '/main' )
nebula.main:setgrid( true )
nebula.main:start()
delete( '/main' )

-- cleanup

delete( '/sys/servers/ode/spaces' )
delete( '/sys/servers/ode/contexts' )
delete( '/sys/servers/ode' )
delete( '/sys/servers/specialfx' )
delete( '/sys/servers/channel' )
delete( '/sys/servers/shadow' )
delete( '/sys/servers/sgraph2' )
delete( '/sys/servers/primitive' )
delete( '/sys/servers/console' )
delete( '/sys/servers/input' )
delete( '/sys/servers/gfx' )
