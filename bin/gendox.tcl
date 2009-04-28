#--------------------------------------------------------------------
#   gendocs.tcl
#   ==========
#   Generate doxygen documentation.
#
#   13-Jan-02   leaf    created
#   03-Aug-02   ling    handles non nebula .cfg files
#   03-Aug-02   leaf    generates navigation bar
#--------------------------------------------------------------------

if [info exists env(NOMADS_HOME)] {
	set home $env(NOMADS_HOME)
} else {
	set home [file join [file dir [info script]] .. ]
}

cd $home/code/doxycfg

set nav ""

if [expr $argc] {
    set cfg [lindex $argv 0]
    puts "Generating docs for $cfg"
    exec 2>>doxygen.warnings doxygen $cfg
} else {
    foreach cfg [glob -nocomplain *.cfg] {
        if { $cfg != "commondoc.cfg" } {
            if [string match nebula* $cfg] {
                set nebula nebula
                set cfgname [string range $cfg 6 end-4]
            } else {
                set nebula ""
                set cfgname [string range $cfg 0 end-4]
            }
            puts "Generating docs for $cfg"
            exec 2>>doxygen.warnings doxygen $cfg
            set nav "$nav |\n\t<a href=\"$nebula$cfgname/html/index.html\" target=\"mainFrame\">$cfgname</a>"
        }
    }
}

# open file and write out html.
set navfn [file join .. .. doc doxydoc navigation.html]
if [file exists $navfn] {
    file delete $navfn
}
set nbfile [open $navfn w]
puts $nbfile {<html>
<head>
<title>Nebula Device Doxygen Documentation</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<style type="text/css">
<!--
a:link {  color: #66CCFF; text-decoration: none}
a:visited {  color: #CCCCCC; text-decoration: none}
a:hover {  color: #000000; background-color: #FFCC66}
body {  font-family: Arial, Helvetica, sans-serif; }
td {  font-size: 10px}
-->
</style>
</head>
<body bgcolor="#666699" topmargin="0" leftmargin="2" text="#FFFFFF">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr> 
    <td><img src="../website/img/smalln.gif" width="20" height="17" border="0" align="middle"/></td>
    <td width="8%"><a href="http://nebuladevice.sourceforge.net/cgi-bin/twiki/view/Nebula/WebHome" target="_top"><font color="#FFFFFF">NEBULA</font></a></td>
    <td>Modules:}
puts $nbfile $nav
puts $nbfile {    </td>
  </tr>
</table>
</body>}
