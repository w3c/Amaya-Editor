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
#	WARNING : need those 5 parameters
	my $head_directory = shift;
	my $head_file = shift;
   my $in_headfile = $head_directory . $head_file ;
	
	my $base_directory = shift;
	my $base_name = shift;
	my $out_basename = $base_directory . $base_name ;
	
	my $comment_at_the_begining = shift; #for the function  ::init_label

# to avoid pb with %label if 2 call to this function 	
	%label = ();
	@list_of_label = ();
	$num_of_label = 0;
	
# to avoid to erase the old base
	if (-r $out_basename) {
		rename ( $out_basename, $out_basename . ".old" )  || 	
		 	die "can't rename the old $out_basename to $out_basename.old because of: $! \nthe old base still exist \n";
		print "\tOld base ($out_basename) renames as $out_basename.old\n";
	}  
	

#	initialization of the base
	open (OUT, ">$out_basename") || die "can't create $out_basename: $!";
	print OUT "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\n";
	{my $date = `date`;# to execute the command shell
	chomp ( $date );	
	print OUT "<base version=\"0\">\n";}

# 	indicates the languages that occur into the field <control>, 
#	Engish is mandatory, beause it's the refference 
	print OUT "<control><language encoding=\"latin1\">en</language></control>\n";
	
#to indicate the begin of the labels
	print OUT "<messages>\n";
	
	
#	read the file source only if it exists and is readable
	@list_of_label = ();
	my @list = ();
	do {
	 	$_ = @list = Read_label::init_label ($in_headfile, $comment_at_the_begining);
	}while ($_ == 0);
	my $total = $list[0];
	my $i = 1;	
	do {
		push (@list_of_label, $list[$i]  );
		$i++;
	}while ( $i <= $total );
	
	
# write the labels
	foreach (@list_of_label){
		print OUT "<label define=\"$_\"></label>\n";
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
