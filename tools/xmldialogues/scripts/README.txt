This is small informations that you need for your first use of "Amaya dialogues
managment"
 
It can not function whithout: 
- obviously the module essential Perl-5.00503-10 or greater
- XML::Parser
- Unicode::String
and the specialy created modules already present on Amaya cvs base:
- Initialisation
- Import_am_msg
- Export_am_msg
- Dial_tool_box
- Forcer
- Edition

If you don't have perl on you machine, ask for to your computer director or
download it from a ftp site. you can consult the www.perl.com for more
informations. 

If its your first used, 
- open a terminal
- connect you as root
- type 'perl -MCPAN -e shell' that will install CPAN if it's missing
- type 'install Bundle::CPAN'
now you are loading the module CPAN that functions as RPM but specific to Perl
let the default mode installation and don't answer yes to parametrability.
Any parametres will be asked to indicate by exemple the ftp site (if you don't
have any idee answer ftp://ftp.lib6.fr/pub/perl/CPAN/)

- and 'install XML::Parser'
- and 'install Unicode::String'
With this module it can be a problem if its the version 2.06.
You can also debug it with a small insertion into the file bug (it' s
$install_dir/.cpan/built/t/op.t ) like that command "diff" shows (old
then new line):
# diff /tmp/op.t op.t 
46c46
< print "not " if $x->as_bool;
---
> #print "not " if $x->as_bool;
it will be OK

You can now run the program with 
- 'perl Am_dial_managment.pl'
- or  make it executable and type './Am_dial_managment.pl'

Warning:
You need to do a "cvs update" before using this program, and of course a "cvs
commit" after
