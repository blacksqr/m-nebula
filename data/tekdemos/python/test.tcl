set cwd [psel]
set times [psys/sr/pe.gettimes]
set pe2_start_time  [lindex $times 0]
set curkey [psys/sr.getkey $i]
set pe2_keys($i,size)   [lindex $curkey 0]
set psysname_with_ext [file tail $file]
/usr/scene/$psysname.setname psys
set file [tk_getSaveFile -filetypes $types -initialfile unnamed -defaultextension .n -title "Save particle system"]
set curtexalpha [/usr/scene/psys/tex.gettexture $stage]
set curalpha [lindex $curtexalpha 1]
set ke_chn_array(2,color)       "blue"
set ke_chn_array(2,minval)      0.0
