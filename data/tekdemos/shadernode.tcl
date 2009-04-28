#--------------------------------------------------------------------
#   shadernode.tcl
#   ==============
#   Demonstrate the new shadernodes which replace the obsolete
#   nmatnode objects.
#
#   Please note that some shaders may not work on all host systems.
#   Please note also that some shaders cannot be dynamically lighted
#   without requiring an additional "post-diffuse-lighting" operation.
#
#   23-Oct-00   floh    created
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   Lets define a few cool shaders...
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   modulate_shader
#   Most simple shader, simply defines one texture layer which is
#   modulated with the polygons base color.
#--------------------------------------------------------------------
proc modulate_shader {} {
    new nshadernode sn
        sel sn

        .setnumstages 1
        .setcolorop 0 "mul tex prev"
        
        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform false
        .endtunit

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false

    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/marble.bmp none
    sel ..
}


proc const_shader {} {
    new nshadernode sn
        sel sn

        .setnumstages 1
		.setconst 0 1 0 0 1
        .setcolorop 0 "replace prev"
		.setalphaop 0 "replace prev"
		.setcullmode none
        
        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform false
        .endtunit

        .setlightenable false
        .setdiffuse  1 0 0 1
        .setemissive 0 0 0 1
        .setambient  1 0 0 1		
        .setalphaenable false
		.setcolormaterial diffuse		

    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/marble.bmp none
    sel ..
}


#--------------------------------------------------------------------
#   gloss_shader
#   2 texture layers, the first is modulated on the base color,
#   the second texture layer uses environment mapping to give a
#   shiny appearance.
#--------------------------------------------------------------------
proc gloss_shader {} {

    new nshadernode sn
        sel sn

        .setnumstages 2

        .setcolorop 0 "mul tex prev"    
        .setcolorop 1 "add tex prev"

        # the first texture unit defines the base map
        .begintunit 0
           .setaddress wrap wrap
           .setminmagfilter linear linear
           .settexcoordsrc uv0
           .setenabletransform false
        .endtunit

        # the second texture unit defines the environment mapping effect
        .begintunit 1
            .setaddress clamp clamp
            .setminmagfilter linear linear
            .settexcoordsrc spheremap
            .setenabletransform true
        .endtunit

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false

    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/marble.bmp none
        .settexture 1 bmp/colorgloss.bmp none
    sel ..
}

#--------------------------------------------------------------------
#   shinethrough_shader
#   The base texture defines an (uv animated) lava effect, which
#   shines through the second static texture layer. The shinethrough
#   spots are defined by the second layers alpha map.
#--------------------------------------------------------------------
proc shinethrough_shader {} {
    new nshadernode sn
        sel sn

        .setnumstages 3

        .setcolorop 0 "replace tex"
        .setcolorop 1 "ipol tex prev tex.a"
        .setcolorop 2 "mul primary prev"

        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz0 3 3 3

        .begintunit 1
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz1 4 4 4

        # this animates the lava's uv coordinates
        new nipol ip
            sel ip
            .connect txyz0
            .addkey3f  0 0 0 0
            .addkey3f 10 5 0 0
        sel ..


        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false
    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/lava.bmp none
        .settexture 1 bmp/pla1.bmp bmp/mask.bmp
    sel ..
}

#--------------------------------------------------------------------
#   blend_shader
#   Blend between two textures, post-multiply the polygons base color
#   to get the lighting correct.
#--------------------------------------------------------------------
proc blend_shader {} {

    new nshadernode sn
        sel sn

        .setnumstages 3
        .setcolorop 0 "replace tex"
        .setcolorop 1 "ipol tex prev const"
        .setcolorop 2 "mul primary prev"

        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz0 5 5 5

        .begintunit 1
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz1 5 5 5

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false

        # interpolate the color constant's alpha for the blend
        # between the two textures
        new nipol ip
            sel ip
            .connect setconst0
            .addkey4f  0 0 0 0 0
            .addkey4f  5 1 1 1 1
            .addkey4f 10 0 0 0 0
        sel ..


    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/brick.bmp none
        .settexture 1 bmp/marble.bmp none
    sel ..
}

#--------------------------------------------------------------------
#   funkyplasma_shader()
#   Three shifting base texture layers to generate a plasma effect,
#   plus an environment mapped gloss layer to make it look cool.
#--------------------------------------------------------------------
proc funkyplasma_shader {} {
    new nshadernode sn
        sel sn

        .setnumstages 3
        .setcolorop 0 "replace tex"
        .setcolorop 1 "adds -tex prev"
        .setcolorop 2 "add tex prev"

        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz0 2 2 2
        
        .begintunit 1
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz1 2 2 2 

        .begintunit 2
            .setaddress clamp clamp
            .setminmagfilter linear linear
            .settexcoordsrc spheremap
            .setenabletransform true
        .endtunit

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false

        new nipol ip
            sel ip
            .connect txyz0
            .addkey3f  0 0 0 0
            .addkey3f  15 1 2 4
        sel ..

        new nipol ip1
            sel ip1
            .connect txyz1
            .addkey3f    0 0 0 0
            .addkey3f  20 -1 -2 -4
        sel ..

    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/funky.bmp none
        .settexture 1 bmp/funky.bmp none
        .settexture 2 bmp/colorgloss.bmp none
    sel ..
}

#--------------------------------------------------------------------
#   environ_shader()
#   Marmor base texture with some environment mirrored in...
#--------------------------------------------------------------------
proc environ_shader {} {
    new nshadernode sn
        sel sn

        .setnumstages 2
        .setcolorop 0 "mul  tex prev"
        .setcolorop 1 "ipol tex prev const.a"

        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform true
        .endtunit
        .sxyz0 2 2 2
        
        .begintunit 1
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc spheremap
            .setenabletransform true
        .endtunit

        .setconst0 0.2 0.2 0.2 0.2

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 0 0 0 0
        .setambient  1 1 1 1
        .setalphaenable false
    sel ..
    new ntexarraynode tex
        sel tex
        .settexture 0 bmp/marble.bmp none
        .settexture 1 bmp/autobahn.bmp none
    sel ..
}

#--------------------------------------------------------------------
#   gen_visual()
#   A simple visual hierarchy using the new nmeshnode object
#   and nshadernode objects. The shader object is always
#   generated under the name of 'pos/sn' so that it can be
#   replaced later.
#--------------------------------------------------------------------
proc gen_visual {} {
    new n3dnode pos
        sel pos

        # the donut mesh
        new nmeshnode mesh
            sel mesh
            .setfilename meshes/torus1.n3d
        sel ..

        # generate the initial shadernode
        modulate_shader

        # let the donut rotate
        new nipol ip
            sel ip
            #.setactive true
            .connect rxyz
            .addkey3f  0   0    0   0  
            .addkey3f 20 360 1080 720
        sel ..
    sel ..
}

#--------------------------------------------------------------------
#   toggle_anim
#   Toggle animating the donut.
#--------------------------------------------------------------------
proc toggle_anim {} {
    if {[/usr/scene/pos/ip.getactive] == "true"} {
        /usr/scene/pos/ip.setchannel "bubu"
    } else {
        /usr/scene/pos/ip.setchannel "time"
    }
}

#--------------------------------------------------------------------
#   replace_shader()
#   Delete the current shader and replace it with a new one.
#--------------------------------------------------------------------
proc replace_shader {new_shader} {
    sel /usr/scene/pos
    delete sn
    delete tex
    $new_shader
}

#--------------------------------------------------------------------
#   Execution starts here!
#--------------------------------------------------------------------
source licht.tcl

sel /usr/scene

# generate a standard visual hierarchy
gen_visual

# create a small Tk window with a shader selector
toplevel     .r
wm title     .r     "Shaders"
wm geometry  .r     214x100+580+177
wm resizable .r     0 0

frame .r.f1
set mb1_label "Modulate"
menubutton .r.f1.mb1 -width 12 \
                     -indicatoron 1 \
                     -relief raised \
                     -text "Modulate" \
                     -menu .r.f1.mb1.m

menu .r.f1.mb1.m -tearoff 0
.r.f1.mb1.m add command -label "Modulate"     -command { replace_shader modulate_shader;     .r.f1.mb1 configure -text "Modulate" }
.r.f1.mb1.m add command -label "Const Alpha"     -command { replace_shader const_shader;     	 .r.f1.mb1 configure -text "Const Alpha" }
.r.f1.mb1.m add command -label "Gloss"        -command { replace_shader gloss_shader;        .r.f1.mb1 configure -text "Gloss" }
.r.f1.mb1.m add command -label "Shinethrough" -command { replace_shader shinethrough_shader; .r.f1.mb1 configure -text "Shinethrough" }
.r.f1.mb1.m add command -label "Blend"        -command { replace_shader blend_shader;        .r.f1.mb1 configure -text "Blend" }
.r.f1.mb1.m add command -label "Funky Plasma" -command { replace_shader funkyplasma_shader;  .r.f1.mb1 configure -text "Funky Plasma" }
.r.f1.mb1.m add command -label "Environment"  -command { replace_shader environ_shader;      .r.f1.mb1 configure -text "Environment" }

checkbutton .r.f1.cb1 -text "Rotation Off" -command { toggle_anim }

pack .r.f1 .r.f1.mb1 .r.f1.cb1 -side top -expand 1
