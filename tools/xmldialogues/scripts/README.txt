This is small informations that you need for your first use of "Gestionnaire des
dialogues d'Amaya"
 
It can not function whithout: 
- obviously the module essential Perl-5.00503-10 or greater
- XML::Parser
- Unicode::String
and the specialy created modules:
- Initialisation
- Import_am_msg
- Export_am_msg
- Dial_tool_box
- Forcer
- Edition

If its your first used, 
- open a terminal
- connect you as root
- type 'perl -CPAN - e shell' that will install CPAN if it lakes
- and 'install XML::Parser'
- and 'install Unicode::String'

# diff /tmp/op.t op.t 
46c46
< print "not " if $x->as_bool;
---
> #print "not " if $x->as_bool;

it will be OK
