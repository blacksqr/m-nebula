# tcl - python converstion

#   $# copy
#	$\wsource	import
#	$\wproc %name% { %args.. } {	func %name% ( %args..) :
#	new %name% %path%			name('%name','%path')   ##unless path=$path then no quoting and remove $
#	sel %path%					
#	.variable arg arg ..
#	sel ..
#	}

#	nonkeyword args..		funccall ( args,)
import string
import glob,os.path


selpathlist=[]
usedfunctions=[]
selobjectlist=[]	
indent =0
activeobject=""
currentpath=""


def iskeyword( word ):
	return keywordlist.has_key(word)
#	for key in keywordlist:
#		if key[0] == word:
#			return 1
#	return 0
	
def fixvariables(vars):
	global alert;
	"remove leading $ and quote others, Also need to handle double quotes and true/false, and remove [..]"
	r=[]
	s=[]
	quoteopen=0
	tkvar=0
	h=""
	for v in vars:
		if v[0]=='[':
			v=v[1:]
		if v[-1]==']':
			v=v[:-1]
		if len(v) > 3:
			if v[0]=='{':
				v=v[1:]
			if v[-1]=='}':
				v=v[:-1]
		if v[0] == '-':	# it's a '-name value' for a tkinter call
			if quoteopen or tkvar:
				print "Error - quote still open or tkvar set and <-name> found", quoteopen,tkvar
				print v
			else:
				tkvar=1	
				h = v[1:] + "="
		elif quoteopen:			# we're looking for a closing brace..
			h = h + v + " "	# keep appending 
			if v[-1] == '"':	# until the quote is closed
				quoteopen = 0
				tkvar=0	
				r.append(h)
				h=""
		elif v[0]=='$':			# it's a variable
			if tkvar:
				h = h + v[1:]
				r.append(h)
				h=""
				tkvar=0
			else :
				r.append(v[1:])
		elif v=='false':		# false
			r.append("0")
		elif v == 'true':		# true
			r.append("1")
		elif v[0]=='"':		# starting quote
			if v[-1] =='"':	# quote is closed
				if tkvar:
					h = h + v
					r.append(h)
					tkvar=0
					h=""
				else:
					r.append(v)	# just append it using the double quotes
			else:					# need to append until variable with closeing brace
				quoteopen=1
				h = h+ v + " "
		elif tkvar:
			h = h + v
			r.append(h)
			tkvar=0
			h=""
		else:
			try:
				f = float(v)
				r.append(v)	# if float conversion works then just use the string
			except:
				t = "\'"+ string.replace (v,'.tcl','.py')	+ "\'"
				# t = "\'"+v+"\'"
				if '$' in v:	#embeded TCL variable
					alert=1	#flag to deal with manually
				r.append(t)
	return r
		
	
# now for a set of functions to handle the keywords
def key_new( line ):
	"new name path => variable=new(name,path)"
	# first lets make a new variable name...
	offset = len ( line[2])
	if line[2][-1] == '/':	# means the path has a trailing slash which we don't like
		offset=-1				# Note however that the trailing / is left in the output ??
	p = string.rfind(line[2],'/',0,offset)	# path variable
	if p >=0 :
		p=p + 1
		varname = line[2][p:]
	else:
		varname = line[2]
	# special case where tcl uses $ to prepend a variable
	if varname[0] == '$':
		varname=varname[1:]+"1"	# real hack to give new object a different name !!
	
	# now lets process the variables to either quote or remove leading $
	fixed=fixvariables(line[1:3])	
	cmd=varname+"=new ("+fixed[0]+","+fixed[1]+")"
	return pad()+cmd


## ahndle cases where there isn't a space between variables etc and braces
## ie.  '{name'   instead of '{ name'
def fixuplist ( list ):	# split special characters from list members and insert as seperate items
	"fixup lists"
	sp="{}()"
	pos =0 
	for l in list:
		if len(l) > 1:
			for c in sp:
				if l[0]==c:	# special character to deal with at beginning
					list.insert(pos,c)	# insert the special as an item
					list[pos+1]=l[1:]	# replace old value with new
					break
				if l[-1]==c:	# is there a special at the end
					list[pos] = list[pos][:-1]
					list.insert(pos+1,c)
					break
		pos = pos + 1
	return list			
				
def key_proc ( line) :		#proc donut {name tx ty tz time} {
	"proc to def.."
	global indent
	line = fixuplist (line)
	cmd = "def "+line[1]+"("
	x = line.index('{')
	y = line[x+1:]
	x = y.index('}')
	y = y[:x] 
	cmd =pad()+cmd + string.join(y,',') +'):'
	indent = indent + 1
	return cmd 
	
def key_source( line ):
	"convert source file to import * from file"
	#cmd = "import * from "+line[1]
	cmd = "execfile ("+string.join(fixvariables(line[1:2]))+")"
	return pad()+cmd
	
def key_puts ( line ):
	"change puts to print"
	cmd="print "+string.join(fixvariables(line[1:]))
	return pad()+cmd
	
def key_for ( line ):
	global infor,indent
	infor=1
	cmd= pad()+"# "+ string.join(fixvariables(line[0:]),',') 
	indent +=1
	return cmd

def key_if ( line ):
	global indent,alert
	print "IF: ",line
	alert=1
	cmd= pad() + "if "+ string.join(fixvariables(line[1:-1]),' ') + ":"
	indent +=1
	return cmd
	
def TKconvert ( cmd ):
	'uppercase certain words'
	wordlist = ('top','bottom','side','left','right','bevel','raise')
	for word in wordlist:
		cmd=string.replace (cmd, word, string.upper(word))
	return cmd
	
def key_tk( line ):
	"tkinter command"
	func = string.upper(line[0][0]) + line[0][1:]	#uppercase the command
	t =  string.join(fixvariables(line[1:2]))
	t = string.split(t,".")	#remove $ and split on dots
	cmd = t[-1]+ "="+func+"(" + string.join(t[-2:-1],".")+","+ string.join(fixvariables(line[2:]),',') + ")"
	cmd=TKconvert(cmd)
	return pad() + cmd




def key_global ( line ):
	"manage the global command"
	cmd= "global " + line[1]
	return pad()+cmd
	
def key_set ( line ) : 	# needed for some absolute "setting" where tcl format is '/path/path variable'
	"SET with /path [vars]"
	global alert
	if len(line) >1:	# must include variables
		if ( line[0][0] == '/' ):	# it's an implied set
			if '$' in line[0]:
				alert=1
			cmd = "set ('" +line[0]+"',"+string.join(fixvariables(line[1:]),',')+")"
		elif ( line[1]=="cwd" and line[2] =='[psel]' ):
			cmd="cmd=psel()"
		elif ( '/' in line[1] ):	# 'nebula' set
			if '$' in line[1]:
				alert =1
			cmd = "set ('" +line[1]+"',"+string.join(fixvariables(line[2:]),',')+")"
		elif ( '/' in line[2] ):	# nebula set again
			cmd = line[1]+"=set ("+string.join(fixvariables(line[2:]),',')+")"	# should only be a single var
		elif ( line[2] == '[lindex'):
			#alert=1
			cmd=line[1]+"="+string.join(fixvariables(line[3:4]))+"["+string.join(fixvariables(line[4:]),",")+"]"
		elif ( '[' in line[2] ):
			alert=1
			cmd=line[1] + " = " + string.join(fixvariables(line[2:]),',')
			#cmd="set (" +	string.join(fixvariables(line[1:]),',') +")"
		else:
			cmd = line[1]+"="+string.join(fixvariables(line[2:]),',')	# should only be a single var
	elif ( line[0][0] == "/"):	#assume an implied set\
		alert=1
		cmd="set ("+string.join(fixvariables(line[0:]))+")"
	else:
		print "PROBLEM with",line
		cmd=string.join(fixvariables(line[0:]),',') +"#### PROBLEM WITH LINE"
		#cmd = "set ('" + line[0] + "')"
	return pad()+cmd

def key_sel ( line ):
	"record selected object"
	global activeobject, selobjectlist,selpathlist
	if ( line[1] == '..' ):
		selobjectlist.pop()
		selpathlist.pop()
		if len(selobjectlist)>0:
			activeobject = selobjectlist[-1]
		else:
			activeobject=""
	elif line[1][0]=='/':	# an absolute path which resets everything
		selpathlist=[line[1]]
	else: 
		activeobject = line[1]
		if activeobject[0]=='$':
			activeobject=activeobject[1:]+"1"
		selobjectlist.append( activeobject )
		selpathlist.append( line[1] )
	# lets return a select object anyway
	return pad() + "sel ("+ string.join(fixvariables(line[1:2])) + ")"
			
def key_none( line):
	return 1

def key_closebrace(line):
	global indent,infor
	indent = indent -1
	if indent <0:
		indent=0
	if infor:
		infor=0
		return "#"+string.join(fixvariables(line[0:]),",")+"\t# End of loop"
	return  "#" + string.join(fixvariables(line[0:]),",")

def key_default(line):
	global activeobject,currentpath,selpathlist
	if line[0][0] == '.':	# means we need to prepend active object
		if len(line)>1:
			cmd = activeobject+"."+line[0][1:]+"("+ string.join(fixvariables(line[1:]),',')+")"
		else:
			cmd = activeobject+"."+line[0][1:]+"()"	# force a zero length tuple - forces the function call
	elif string.find (line[0],'.') >=0 :	#fully qualified object 
		if len(line)>1:
			cmd = line[0]+"("+ string.join(fixvariables(line[1:]),',')+")"
		else:
			cmd = line[0]+"()"	# force a zero length tuple - forces the function call
	
	else:  # here is where the REAL hacking starts.. we need to know about function calls etc..
		if line[0]=='plainobject':
			cmd = line[1]+"="+line[0]+" ("+ string.join(fixvariables(line[1:]),',')+")"
		else:
			cmd = line[0]+" ("+ string.join(fixvariables(line[1:]),',')+")"
	return pad()+cmd

def pad():
	global indent
	tabs="\t\t\t\t\t\t\t\t\t\t\t\t\t"
	return tabs[:indent]
	
keywordlist={'new': key_new, 'sel':key_sel, 'source':key_source, 'proc':key_proc, '}':key_closebrace ,'set':key_set,\
			'global':key_global, 'for':key_for, 'if':key_if, 'puts':key_puts,\
			'frame':key_tk,'label':key_tk,'entry':key_tk,'checkbutton':key_tk,'button':key_tk,\
			'pack':key_tk }
import sys


#main

if len (sys.argv) < 2 :
	print "Please select a script to convert"
	sys.exit()
alert=0
infor=0
files = glob.glob(sys.argv[1])
for fin in files:
	selpathlist=[]
	usedfunctions=[]
	selobjectlist=[]	
	indent =0
	activeobject=""
	currentpath=""
	fraw = os.path.normpath(fin)
	f = open (fraw)
	lines = f.readlines()
	f.close()
	
	# note that the output file is always put in the current directory
	(p,filename)=os.path.split(fraw)
	outfile = filename[:(string.find(filename,'.'))]+".py"
	
	fout = open ( outfile,'w+' )
	
	for line in lines:
	
		l = string.strip(line)
		words = string.split(l)
		if len(words) == 0:	# blank lines
			newline = l
		elif words[0][0] == "#":
			# must be a comment line
			newline = l
		elif iskeyword(words[0]):
			# keywork we know about
			#p = keywordlist.find(words[0])
			func = keywordlist[words[0]]	# function
			newline=func(words)
		elif words[0][0] == "/":	#presume an implied set
			newline=key_set(words)
		else :
			# handle rest of lines
			newline = key_default(words)
			#newline = "TO DO "+l
		#print "OLD:",l
		if len(newline) > 0 :
			#fout.write("#"+l+"\n")
			fout.write(newline)
			if alert:
				fout.write("\t# CHECK -")
				fout.write(l)
				alert=0
			fout.write("\n")
			#print newline
		
	fout.close();
	