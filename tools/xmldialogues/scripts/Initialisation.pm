#!/usr/bin/perl -w
package Initialisation;

use strict;
use Read_label qw ( &init_label );

BEGIN {
	use vars qw( @ISA @EXPORT );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&create_base
						);
}	

# pour les statistiques
my $num_of_label = 0;
# and for a small control of any duplicates
my %label = ();
my @list_of_label = ();	
###########################################################################
###                       subs exported
########################################################################
sub create_base {	
#function that read amayamsg.h file and write it on the base_am_msg.xml
# after recognizing the different elements
#	WARNING : need those 4 parameters
	my $head_directory = shift;
	my $head_file = shift;
   my $in_headfile = $head_directory . $head_file ;
	
	my $base_directory = shift;
	my $base_name = shift;
	my $out_basename = $base_directory . $base_name ;

# to avoid pb with %label if 2 call to this function 	
	%label = ();
  
	my $date = `date`;# to execute the command shell
	chomp ( $date );
	
	my $num_line = 0;	
	$num_of_label =0;

#	initialization of the base
	open (OUT, ">$out_basename") || die "can't create $out_basename: $!";
	print OUT "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\n";
	print OUT "<base version=\"0\" last_update=\"$date\">\n";

# 	indicates the languages that occur into the field <control>, 
#	Engish is mandatory, beause it's the refference 
	print OUT "<control>\n</control>\n";
	
#to indicate the begin of the labels
	print OUT "<messages>\n";
	
	
#	read the file source only if it exists and is readable
	my @list = ();
	unless (-r $in_headfile ) {
		print "fichier $in_headfile introuvable";
		}
   else {
	 	@list = Read_label::init_label ($in_headfile);
		my $i;
		for ($i = 1,$i <= $list[0],$i++ ) {
			push (@list_of_label, $list[$i]  );
		}
		for ($i,$i <= ($list[0] * 2 ),$i++ ) {
			$label { $list[$i] }= $list[$i];
		}		
  	}
   	
#	ending 
	print OUT "</messages>\n";
	print OUT "<!--end of base -->\n</base>";
   
	close (OUT) || warn "problem during the OUT closing: $!";
	
	print "\tBASE Created,\n\tIts name is $out_basename\n",
	"\tThere was ",$list[0] ," labels recognized\n";

}#########################################################
### 											end sub exported
########################################################


sub add { #	write the new label given in first parameter in 'OUT'  
 	my $label = shift;
	my $num_line = shift;
	if (defined ($label { $label}) ) {
		print "The label $label allredy exists\n";
	}
	else {
		$label { $label} = $num_line;
		print OUT "<label define=\"";
		print OUT $label;
		if ( $label =~ /MSG_MAX/ || $label =~ /MAX_EDITOR_LABEL/) {
			print OUT "\">\n";
			print OUT "<!--this label must still empty end the latest because it";
			print OUT "shows the end of labels that are used for Amaya --></label>\n"
		}
		else {
			print OUT "\"></label>\n";
		}
		$num_of_label++;
	}
}
1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Initialisation - Perl extension for blah blah blah

=head1 SYNOPSIS

  use Initialisation;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for Initialisation was created by Emmanuel Huck for the W3C. 
it create the base xml for what kind of dialogue you need

Blah blah blah.

=head1 AUTHOR

E. HUCK, huck.manu@caramail.com

=head1 SEE ALSO

perl(1).

=cut
