$!
$! Quickie VMS .com file to build under VMS.
$!
$! Written by John Carr (jrcarr@iup.bitnet)
$! modified by Adam Bryant (adb@bu.edu)
$! 
$ cc main,parse,io
$!
$! Options decrease .exe file size by 2/3
$!
$ link/exe=cextract main,parse,io, sys$input/opt
sys$share:vaxcrtl.exe/share
