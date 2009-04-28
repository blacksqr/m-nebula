sel /usr/scene
.rz -90

new n3dnode heinz
  sel heinz
  new n3dnode peter
    sel peter
    .txyz 0 30 30
    
    new n3dnode hans
      sel hans
      new nlightnode light
      light.settype point
      light.setcolor 1 1 1 1
      light.setattenuation 1 0 0

      new nmeshnode mesh
      mesh.setfilename meshes/bumpcube.n3d
      mesh.setreadonly false
      sel ..
  sel ..

  new nipol ipol
    sel ipol
    .connect rxyz
    .beginkeys 5 3
    .addkey3f 0 0 0 0
    .addkey3f 3 -90 -90 0
    .addkey3f 6 -180 -180 0
    .addkey3f 9 -270 -90 0
    .addkey3f 12 -360 0 0
    .endkeys
    sel ..
sel ..

new n3dnode gerd
  sel gerd
  .rz 180
  .ry 90
  .tz -10
  .ty -10

  new n3dnode karl
    sel karl
    new nmeshnode mesh
    mesh.setfilename meshes/bumpsphere.n3d
    mesh.setreadonly true
    sel ..

  new nbumpmeshnode klaus
    sel klaus
    .setreadonly false
    .setmesh /usr/scene/gerd/karl/mesh
    .setlight /usr/scene/heinz/peter/hans
    sel ..

  new nshadernode shader
    sel shader
    .setnumstages 2

    .setcolorop 0 "dot primary tex"
    .setcolorop 1 "mul prev tex"
    .setalphaop 1 "adds const.a -prev.a"

    .begintunit 0
      .setaddress wrap wrap
      .setminmagfilter linear linear
      .settexcoordsrc uv0
      .setenabletransform false
    .endtunit
    .begintunit 1
      .setaddress wrap wrap
      .setminmagfilter linear linear
      .settexcoordsrc uv0
      .setenabletransform false
    .endtunit

    .setconst0 0.2 0.2 0.2 1.0

    .setlightenable true
    .setdiffuse  1 1 1 1
    .setemissive 0 0 0 0
    .setambient  1 1 1 1
    .setalphaenable true

    sel ..

    new ntexarraynode tex
      sel tex
      .settexture 0 bmp/bump_normals.tga none
      .settexture 1 bmp/marble.bmp none
      sel ..

    sel ..



  sel ..
