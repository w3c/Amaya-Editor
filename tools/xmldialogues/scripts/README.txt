Readme for the main program Am_dial_managment.pl

WARNING:
You need to do a "cvs update" before using this program, and of course a "cvs
commit" after
This script works with Perl-5.00503-10 or greater with XML::Parser
and Unicode::String

Following modules are also needed (see Amaya CVS base):
- Configfile
- Initialisation
- Import_am_msg
- Export_am_msg
- Dial_tool_box
- Forcer
- Edition
- Read_label
- Read_text

If you don't have perl, you can consult the www.perl.com for more
informations 

You can now run the program with 
- 'perl Am_dial_managment.pl'
- or  make it executable ('chmod u+x Am_dial_managment.pl')
	and type './Am_dial_managment.pl'

The first times you start this script, it will ask for:
1) the directory which includes the Amaya hierarchy
2) the name of the object subdirectory (used to build)
3) the directory which includes config files (xx-amayadialogue,
   xx-amayamsg, and xx-libdialogue)
