$!
$! Quickie VMS .com file to build under VMS.
$!
$! Written by John Carr (jrcarr@iup.bitnet)
$! modified by Adam Bryant (adb@bu.edu)
$! 
$ cc main,parse,io/def= -
(SYS_CONFIG="""sys$library:cext.cnf""",CONFIG_FILE="""cext.cnf""")
$!
$! Options decrease .exe file size by 2/3
$!
$ link/exe=cextract main,parse,io, sys$input/opt
sys$share:vaxcrtl.exe/share
$!
