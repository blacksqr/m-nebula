#set cwd [psel]
set ('cwd','psel')	# CHECK -set cwd [psel]
#set times [psys/sr/pe.gettimes]
times=set ('psys/sr/pe.gettimes')
#set pe2_start_time  [lindex $times 0]
set ('pe2_start_time','lindex',times,0)	# CHECK -set pe2_start_time  [lindex $times 0]
#set curkey [psys/sr.getkey $i]
curkey=set ('psys/sr.getkey',i)
#set pe2_keys($i,size)   [lindex $curkey 0]
set ('pe2_keys($i,size)','lindex',curkey,0)	# CHECK -set pe2_keys($i,size)   [lindex $curkey 0]
#set psysname_with_ext [file tail $file]
set ('psysname_with_ext','file','tail',file)	# CHECK -set psysname_with_ext [file tail $file]
#/usr/scene/$psysname.setname psys
set ('/usr/scene/$psysname.setname','psys')	# CHECK -/usr/scene/$psysname.setname psys
#set file [tk_getSaveFile -filetypes $types -initialfile unnamed -defaultextension .n -title "Save particle system"]
set ('file','tk_getSaveFile','-filetypes',types,'-initialfile','unnamed','-defaultextension','.n','-title',"Save particle system" )	# CHECK -set file [tk_getSaveFile -filetypes $types -initialfile unnamed -defaultextension .n -title "Save particle system"]
#set curtexalpha [/usr/scene/psys/tex.gettexture $stage]
curtexalpha=set ('/usr/scene/psys/tex.gettexture',stage)
#set curalpha [lindex $curtexalpha 1]
set ('curalpha','lindex',curtexalpha,1)	# CHECK -set curalpha [lindex $curtexalpha 1]
#set ke_chn_array(2,color)       "blue"
ke_chn_array(2,color)="blue"
#set ke_chn_array(2,minval)      0.0
ke_chn_array(2,minval)=0.0
