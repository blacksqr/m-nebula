nomads/code/templates/readme.txt
=================================

This directory contains template files for a vanilla nRoot derived class.
These can be used as a starting point for your own classes.

* Source files - put in code/src/classname/
nclassname_cmds.cc		- command and persistency functions
nclassname_main.cc		- the main body of the class

* Header file - put in code/inc/classname/
nclassname.h			- class header

* Package file - put in code/src/
npackagename.pak

The Tcl script classbuilder.tcl is a convenient way of creating your own classes
without having to go through the templates inserting your own class names.

--
EOF