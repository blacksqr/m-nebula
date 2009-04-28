#
#   Nebula TEKDEMO
#
#   machine.tcl
#   A complex object with shaders, effects, some hierarchical animation
#
#   created:    Jan-2004    Eric Werner
#   
#   (C) COPYRIGHT 2004 RADONLABS GMBH
# 
# -----------------------------------

source routines.tcl

# ---
# good old lighthouse shader from Nomads
# --

proc shader_lighthouse {} {
  new nshadernode shader
    sel shader
    setnumstages 2

    setcolorop 0 "mul tex prim"
    setcolorop 1 "ipol tex prev prev.a"

    begintunit 0
      setaddress wrap wrap
      setminmagfilter linear linear
      settexcoordsrc uv0
      setenabletransform true
      setalphaenable true
    endtunit

    begintunit 1
      setaddress wrap wrap
      setminmagfilter linear linear
      settexcoordsrc spheremap
      setenabletransform true
    endtunit 

    setcullmode ccw
    setlightenable true
    setdiffuse 1 1 1 1
    setemissive 0 0 0 0
    setambient 1 1 1 1
    setalphaenable false
  sel ..
}

proc shader_env {} {
  new nshadernode shader
      sel shader
      .setnumstages 1
      .setcolorop 0 "mul tex prev"

      .begintunit 0
        .setaddress         wrap   wrap
        .setminmagfilter    linear linear
        .settexcoordsrc     spheremap
        .setenabletransform true
      .endtunit

      .setlightenable true
      .setdiffuse  1 1 1 1
      .setemissive 0 0 0 0
      .setambient  1 1 1 1
      .setalphaenable false
  sel ..
}

proc shader_belt {} {
  new nshadernode shader
    sel shader
    .setnumstages 1
    .setcolorop 0 "mul tex prev"

    .begintunit 0
      .setaddress         wrap   wrap
      .setminmagfilter    linear linear
      .settexcoordsrc     uv0
      .setenabletransform true
    .endtunit

    .setlightenable true
    .setdiffuse  1 1 1 1
    .setemissive 0 0 0 0
    .setambient  1 1 1 1
    .setalphaenable true
    new nipol texipol
      sel texipol
      .connect txyz0
      .addkey3f 0 0 0 0
      .addkey3f 0.15 -1 0 0
    sel ..
  sel ..
}

proc machinepart {name mesh shader tex00 tex01 tex10 tex11} {
  new n3dnode $name
    sel $name
    new nmeshnode mesh
      sel mesh
      .setfilename meshes/machine/machine_$mesh\.n3d
    sel ..
    shader_$shader
    new ntexarraynode tex
      sel tex
      .settexture 0 $tex00 $tex01
      .settexture 1 $tex10 $tex11
    sel ..
  sel ..
}

#--------------------------------------------------------------------
#   main()
#--------------------------------------------------------------------

sel /usr/scene
source licht.tcl

new n3dnode machine
  sel machine

  new nipol spin
    sel spin
    .connect ry
    .addkey1f 0 0
    .addkey1f 25 360
  sel ..

  new n3dnode pos
    sel pos
    .txyz -5 0 0

    # ------------
    # AXIS
    # ------------

    new n3dnode axis
      sel axis

      machinepart axis axis env bmp/machine/machine_env.bmp none none none

      new nipol spin
        sel spin
        .connect rx
        .addkey1f 0 0
        .addkey1f 3 360
      sel ..

      new n3dnode wheel1
        sel wheel1
        tx 1.75
        machinepart mantel tire lighthouse bmp/machine/machine_wheel.bmp bmp/machine/machine_wheel_alpha.bmp bmp/machine/machine_glow.bmp none
        machinepart felge rim lighthouse bmp/machine/machine_wheel.bmp bmp/machine/machine_wheel_alpha.bmp bmp/machine/machine_env.bmp none
      sel ..

      new n3dnode wheel2
        sel wheel2
        tx -1.75
        sxyz 1 1.05 1
        machinepart mantel tire lighthouse bmp/machine/machine_wheel.bmp bmp/machine/machine_wheel_alpha.bmp bmp/machine/machine_glow.bmp none
        machinepart felge rim lighthouse bmp/machine/machine_wheel.bmp bmp/machine/machine_wheel_alpha.bmp bmp/machine/machine_env.bmp none
      sel ..
    sel ..


    # ------------
    # BODY
    # ------------
    new n3dnode body
      sel body
      rx -1
      new nipol wiggle
        sel wiggle
        .connect ty
        .addkey1f 0     0.025
        .addkey1f 0.09  0.0
        .addkey1f 0.18   0.025
      sel ..
      new nipol woggle
        sel woggle
        .connect rz
        .addkey1f 0     1.0
        .addkey1f 0.45   -1.0
        .addkey1f 1.04  1.0
      sel ..


      machinepart body body lighthouse bmp/machine/machine_green.bmp bmp/machine/machine_alpha.bmp bmp/glow.bmp none
      machinepart smokestack smokestack env bmp/machine/machine_glow.bmp none none none
      new n3dnode smoke
        sel smoke
        txyz 0.246 3.575 -3.033
        source psys/machine_smoke.psys
      sel ..
      machinepart alu alu env bmp/machine/machine_alu.bmp none none none
      new n3dnode esparkle
        sel esparkle
        txyz 0.299 2.71 -0.591
        source psys/machine_electric.psys
      sel ..
      machinepart rivets rivets env bmp/machine/machine_glow.bmp none none none
      machinepart technix technix env bmp/machine/machine_glow.bmp none none none
      machinepart chrome chrome env bmp/machine/machine_env.bmp none none none
      machinepart glass glass env bmp/machine/machine_env.bmp none none none
      machinepart radonlogo radonlogo env bmp/machine/machine_env.bmp none none none
        sel glass
          sel shader
          .setdiffuse 0.8 0.8 0.8 0.4
          .setalphaenable true
        sel ..
      sel ..
      machinepart bigwheel bigwheel lighthouse bmp/machine/machine_green.bmp bmp/machine/machine_alpha.bmp bmp/glow.bmp none
        sel bigwheel
        txyz 1.463 1.405 -2.076
        new nipol spin
          sel spin
          .connect rx
          .addkey1f 0 0
          .addkey1f 1 360
        sel ..
      sel ..
      machinepart smallwheel smallwheel env bmp/machine/machine_green.bmp none none none
        sel smallwheel
        txyz 1.46 0.272 -0.461
        new nipol spin
          sel spin
          .connect rx
          .addkey1f 0 0
          .addkey1f 0.5 360
        sel ..
      sel ..
      machinepart belt belt belt bmp/machine/machine_leather.bmp none none none
      machinepart airscrew airscrew env bmp/machine/machine_alu.bmp none none none
        sel airscrew
        txyz 0.925 2.903 -2.295
        new nipol spin
          sel spin
          .connect ry
          .addkey1f 0 0
          .addkey1f 0.4 -360
        sel ..
      sel ..

      # Gearbox  -------------------------------------------

      machinepart wicket wicket lighthouse bmp/machine/machine_green.bmp bmp/machine/machine_alpha.bmp bmp/machine/machine_glow.bmp none
        sel wicket
        txyz 0.305 0.879 -2.975
        new nipol spin
          sel spin
          .connect ry
          .addkey1f  0.0 0
          .addkey1f  2.0 -100
          .addkey1f  3.0 -70
          .addkey1f  4.5 -120
          .addkey1f  5.8  -5
          .addkey1f  6.3 -30
          .addkey1f  8.5 -50
          .addkey1f 10.0 -110
          .addkey1f 10.5 -120
          .addkey1f 11.0 -50
          .addkey1f 11.5 -40
          .addkey1f 12.0 -70
          .addkey1f 13.3 -10
        sel ..
      sel ..
      machinepart cogwheel1 plane env bmp/machine/machine_cog.bmp bmp/machine/machine_cog_alpha.bmp none none
        sel cogwheel1
          sel shader
          setalphatestenable true
          setalphaenable true
          setalpharef 0.6
        sel ..
        txyz -0.087 1.189 -2.684
        rx -90
        sxyz 0.569 0.569 0.569
        new nipol spin
          sel spin
          .connect rz
          .addkey1f 0 0
          .addkey1f 4 360
        sel ..
      sel ..
      machinepart cogwheel2 plane env bmp/machine/machine_cog.bmp bmp/machine/machine_cog_alpha.bmp none none
        sel cogwheel2
          sel shader
          setalphatestenable true
          setalphaenable true
          setalpharef 0.6
        sel ..
        txyz 0.137 0.939 -2.741
        rx -90
        sxyz 0.324 0.324 0.324
        new nipol spin
          sel spin
          .connect rz
          .addkey1f 0 0
          .addkey1f 4 360
        sel ..
      sel ..
      machinepart cogwheel3 plane env bmp/machine/machine_cog.bmp bmp/machine/machine_cog_alpha.bmp none none
        sel cogwheel3
          sel shader
          setalphatestenable true
          setalphaenable true
          setalpharef 0.6
        sel ..
        txyz -0.157 0.82 -2.741
        rx -90
        sxyz 0.324 0.324 0.324
        new nipol spin
          sel spin
          .connect rz
          .addkey1f 0 0
          .addkey1f 4 -360
        sel ..
      sel ..
      machinepart cogwheel4 plane env bmp/machine/machine_cog.bmp bmp/machine/machine_cog_alpha.bmp none none
        sel cogwheel4
          sel shader
          setalphatestenable true
          setalphaenable true
          setalpharef 0.6
        sel ..
        txyz -0.398 1.013 -2.741
        rx -90
        sxyz 0.324 0.324 0.324
        new nipol spin
          sel spin
          .connect rz
          .addkey1f 0 0
          .addkey1f 4 360
        sel ..
      sel ..
      machinepart barrel anim8_barrel env bmp/machine/machine_alu.bmp none none none
        sel barrel
        txyz -0.127 0.49 -2.722
        new nipol spin
          sel spin
          .connect rx
          .addkey1f 0 0
          .addkey1f 2.5 360
        sel ..
      sel ..
      machinepart cock1 anim8_cock env bmp/machine/machine_env.bmp none none none
        sel cock1
        txyz 0 0.763 -2.801
        new nipol pumpipol
          sel pumpipol
          .connect tx
          .addkey1f 0 0.2
          .addkey1f 1.4 -0.1
          .addkey1f 4 0.2
        sel ..
      sel ..
      machinepart cock2 anim8_cock env bmp/machine/machine_env.bmp none none none
        sel cock2
        rz 180
        txyz 0 1.081 -2.802
        new nipol pumpipol
          sel pumpipol
          .connect tx
          .addkey1f 0 -0.308
          .addkey1f 0.5 -0.14
          .addkey1f 2.4 -0.308
        sel ..
      sel ..



      # Pump -------------------------------------------

      machinepart compensator compensator env bmp/machine/machine_alu.bmp none none none
        sel compensator
        txyz -0.605 3.265 -1.43
        new nipol spin
          sel spin
          .connect rx
          .addkey1f 0.0 -18
          .addkey1f 0.6 18
          .addkey1f 1.2 -18
        sel ..
        machinepart thing1 thing1 env bmp/machine/machine_env.bmp none none none
          sel thing1
          tz 0.546
          new nipol spin
            sel spin
            .connect rx
            .addkey1f 0.0  14
            .addkey1f 0.3  0
            .addkey1f 0.6 -22
            .addkey1f 0.9  0
            .addkey1f 1.2  14
          sel ..
        sel ..
        machinepart thing2 thing2 env bmp/machine/machine_env.bmp none none none
          sel thing2
          tz -0.548
          new nipol spin
            sel spin
            .connect rx
            .addkey1f 0.0  23
            .addkey1f 0.3  0
            .addkey1f 0.6 -16
            .addkey1f 0.9  0
            .addkey1f 1.2  23
          sel ..
        sel ..
      sel ..
      machinepart cylinder cylinder env bmp/machine/machine_env.bmp none none none
        sel cylinder
        txyz -0.605 0 -0.882
        new nipol pump
          sel pump
          .connect ty
          .addkey1f 0.0 2.847
          .addkey1f 0.6 2.513
          .addkey1f 1.2 2.847
        sel ..
      sel ..
    sel ..

    # ------------
    # WHEELY
    # ------------

    new n3dnode wheely
      sel wheely
      txyz 0 0.1 -3.773

      machinepart wheelyfork wheelyfork env bmp/machine/machine_alu.bmp none none none

      new nipol spin
        sel spin
        .connect ry
        .addkey1f 0.0 -17
        .addkey1f 0.15 2
        .addkey1f 0.4 -24
        .addkey1f 0.7 5
        .addkey1f 1.1 -13
        .addkey1f 1.4 3
        .addkey1f 1.9 -14
        .addkey1f 2.2 -25
        .addkey1f 2.8 -2
        .addkey1f 3.1 -17
      sel ..

      new n3dnode wheelytire
        sel wheelytire
        txyz 0 -0.673 -0.287

        new nipol spin
          sel spin
          .connect rx
          .addkey1f 0 0
          .addkey1f 1.9 360
        sel ..

        machinepart wheelytire wheelytire lighthouse bmp/machine/machine_wheel.bmp bmp/machine/machine_wheel_alpha.bmp bmp/machine/machine_glow.bmp none
        machinepart wheelyrim wheelyrim lighthouse bmp/machine/machine_green.bmp bmp/machine/machine_alpha.bmp bmp/machine/machine_glow.bmp none
      sel ..
    sel ..

    machinepart shadow plane env bmp/machine/machine_alpha.bmp bmp/machine/shadow_alpha.bmp none none
      sel shadow
      txyz -0.02 -0.99 -1.614
      sxyz 4.522 3.853 6.428
        sel shader
        setalphaenable true
        setdiffuse 1 1 1 0.8
      sel ..
        sel tex
        .sethighquality 0 true
      sel ..
    sel ..

  sel ..
sel ..

# ---
# EOF
