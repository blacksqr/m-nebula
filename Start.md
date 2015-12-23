#how to start development with m-nebula
# How to start with m-nebula #

## Environment requirements ##
  * nebula 1
  * TCL 8.5
  * Python 2.5(optional)
  * Windows (XP, Vista ...)
  * Visual Studio 6, 2003, 2005, 2008 (also Express edition)
## Environment setup ##
  * install tcl/tk 8.5 from http://www.activestate.com/activetcl/ (or build it yourself) to some comfortable path (like c:\tcl8.5 )
  * install python 2.5 from python.org to some comfortable path (like c:\pythont2.5 )
  * In Visual Studio Tools menu choose Options and select <VC++ Directories> from <Projects and Solutions> in the opened dialog.
  * In the <Show directories for:> choose "Executable files" and add <tcl install path>\bin there.
  * In the <Show directories for:> choose "Include files" and add <tcl install path>\include and  <python install path>\include there.
  * In the <Show directories for:> choose "Library files" and add <tcl install path>\lib and  <python install path>\libs there.
## Visual Studio solution generation ##
  * enter to folder of nebula
  * enter to code/src under it
  * run clean.bat
  * run updsrc.tcl
  * if you received empty tk window - the solution nebula.sln generated and could be found in code/vstudio|vstudio2005 folder.