#!/usr/bin/perl -w
package Initialisation;

use strict;

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
my %label;	
###########################################################################
###                       subs exported
########################################################################
sub create_base {	
#function that read amayamsg.h file and write it on the base_am_msg.xml
# after recognizing the different elements
#	WARNING : need those 4 parameters
	my $head_directory = shift;
	my $head_file = shift;
   my $in_filename = $head_directory . $head_file ;
	
	my $base_directory = shift;
	my $base_name = shift;
	my $out_basename = $base_directory . "/" . $base_name ;

# to avoid pb with %label if 2 call to this function 	
	%label = ();

   my $line;
   my $define;
   my $ref_name;
   my $value;
   my @rest;
	my $date;
	my $num_line = 0;
	
	$num_of_label =0;
	$date = `date` ; # to execute the command shell
	chomp ( $date );
#	initialization of the base
	open (OUT, ">$out_basename") || die "can't create $out_basename: $!";
	print OUT "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\n";
	print OUT "<base_message version=\"0\" last_update=\"$date\">\n";

# 	indicates the languages that occur into the field <control>, 
#	Engish is mandatory, beause it's the refference 
	print OUT "<control>\n</control>\n";
	
#to indicate the begin of the labels
	print OUT "<messages>\n";
	
	
#	read the file source only if it exists and is readable
	unless (-r $in_filename ) {
		print "fichier $in_filename introuvable";
		}
   else {
		open (IN, "<$in_filename") || die "erreur de lecture de $in_filename: $!";
		}
# jump after comments to the first line in witch we're interested 
# treat the particularity of EDITOR.h that begin a few "define"-line unused
	
	do {
		$line = <IN>;
		$num_line++;
		$line =~ s/\s+/ /g;
		if ( defined ($line) ) {
			($define,$ref_name,$value) = split (/\s+/, $line);				
		}
		$value = 123 unless ( defined ($value)) ; # for loop when necessary		
	}
	while ( $value eq "" || $value ne '0' );
	add ($ref_name, $num_line);		
	
	
#	reads and adds all the labels
#	warning, the file must be well-formed without errors 	
	while ($line = <IN>) {
		$num_line++;
		chomp ($line);
		$line =~ s/\s+/ /g;
		if ( $line ne "") {
			($define,$ref_name,$value) = split (/ /, $line);
			if ( defined ($define)  
					&& defined ($value)
					&& $define eq "#define"					
					&& $value =~ /^\d/ ) {
				# to avoid that $ref_name still had spaces
				$ref_name =~ s/\s+//g;						 
				add ($ref_name, $num_line);
			}
		}
  	}
   	
#	ending 
	print OUT "</messages>\n";
	print OUT "<!--end of base -->\n</base_message>";
   
	close (IN) || die "problem during the IN closed: $!";
	close (OUT) || warn "problem during the OUT closing: $!";
	
	print "\tBASE Created,\n\tIts name is $out_basename\n",
	"\tThere was $num_of_label labels recognized\n";

}#########################################################
### 											end sub exported
########################################################


sub add { #	write the new label given in first parameter in 'OUT'  
 	my $label = shift;
	my $num_line = shift;
	if (defined ($label { $label}) ) {
		print "The label $label at line $num_line line allredy exists at line"
		  		. $label{ $label} ."\n";
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
