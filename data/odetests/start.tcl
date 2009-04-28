#-- startup script for Nebulized ODE buggy test.
set     nomads_home    [/sys/share/assigns/home.gets]
set     home           $nomads_home

#-- create servers
new nscenegraph2        /sys/servers/sgraph2
new nglserver           /sys/servers/gfx
new ninputserver        /sys/servers/input
new nsbufshadowserver   /sys/servers/shadow
new nchannelserver      /sys/servers/channel
new nconserver          /sys/servers/console
new nmathserver         /sys/servers/math
new nparticleserver     /sys/servers/particle
new nspecialfxserver    /sys/servers/specialfx
new nprimitiveserver    /sys/servers/primitive


# setup OdePhysics
new nodeserver          /sys/servers/ode
new nodephysicscontext  /sys/servers/ode/contexts/physics
new nodecollidecontext  /sys/servers/ode/contexts/collide


#-- set file server assigns
#proc setassign { assign dir } {
#  set path [/sys/servers/file2.manglepath home:data/odetests/$dir//]
#  /sys/servers/file2.setassign $assign $path
#}

proc setassign { assign dir } {
  /sys/servers/file2.setassign $assign [/sys/servers/file2.manglepath home:data/odetests/$dir//]
}

setassign test1 "hello/bye/there"

#-- open display

sel /sys/servers
gfx.setdisplaymode dev(0)-type(win)-w(640)-h(480)-bpp(32)
gfx.setviewvolume -0.1 0.1 -0.075 0.075 0.1 5000.0 
gfx.setclearcolor 0.5 0.5 0.5 1.0 

#-- init shadow server

/sys/servers/shadow.setcastshadows true

# #-- map input

source ${nomads_home}data/odetests/input.tcl

# #-- setup lights

new nroot lights
source ${nomads_home}data/odetests/light.tcl

#-- setup assings

setassign "meshes" "meshes"

#-- setup collision

sel /sys/servers/ode

.begincollclasses
  .addcollclass ground
  .addcollclass car
  .addcollclass blob
.endcollclasses

.begincolltypes
  .addcolltype car car ignore
  .addcolltype ground ground ignore
  .addcolltype ground car exact
  .addcolltype blob blob exact
  .addcolltype blob ground exact
.endcolltypes

.setcollidecontext /sys/servers/ode/contexts/collide

# use a simple space
#new nodesimplespace /sys/servers/ode/spaces/root
# or a hash space
new nodehashspace  /sys/servers/ode/spaces/root
# or a quad tree space
#new nodequadtreespace   /sys/servers/ode/spaces/root
#sel /sys/servers/ode/spaces/root
# .setparams' 0 0 0  10 0 10  7

# associate a collide context with the space
/sys/servers/ode/spaces/root.setcontext /sys/servers/ode/contexts/collide

#-- setup physics

.setphysicscontext /sys/servers/ode/contexts/physics

.beginsurfaces 2 
  .addsurface 0 buggy
  .setsurfparam    0 mu -1.0 
#  .setsurfparam    0 slip1 0.1
#  .setsurfparam    0 slip2 0.1 
  .setsurfparam    0 slip1 1.5
  .setsurfparam    0 slip2 1.5
  .setsurfparam    0 erp 0.5 
  .setsurfparam    0 cfm 0.3 
  .enablesurfparam 0 approxfboth
  
  .addsurface 1 blob
  .setsurfparam    1 mu -1.0
  .setsurfparam    1 mu2 0.0
  .setsurfparam    1 erp 0.9
# cfm prevents sphere/mesh jitter
  .setsurfparam    1 cfm 0.0005
  .enablesurfparam 1 approxfboth
.endsurfaces

sel /sys/servers/ode/contexts/physics
  .setgravity 0.0 0.5 0.0 
sel /
#-- work

/sys/servers/gfx.opendisplay
new nodetest /main
/main.setgrid true
#/main.setsleep 0.02
/main.start

delete /main

delete /sys/servers/ode/spaces
delete /sys/servers/ode/contexts
delete /sys/servers/ode
delete /sys/servers/specialfx
delete /sys/servers/channel
delete /sys/servers/shadow
delete /sys/servers/sgraph2
delete /sys/servers/primitive
delete /sys/servers/console
delete /sys/servers/input
delete /sys/servers/gfx