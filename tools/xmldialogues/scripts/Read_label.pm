#!/usr/bin/perl -w
package Read_label;

use strict;

BEGIN {
	use vars qw( @ISA @EXPORT );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&import_a_language
						)			
}
# global variables
	my @list_of_label = (); 
	my $in_labelfile = "";
	
################################################################################
##							sub exported
################################################################################

sub initlabel {# fill the %labels from $in_labelfile that is reading
	 $in_labelfile  = shift;
	

	
	my $line = "";
	my $line_count = 0; # used to indicate the line of an error
	
# open $in_labelfile only if it exists and is readable
	unless (-r $in_labelfile) { 
		print "fichier $in_labelfile introuvable\n";
		}
   else {
		open (LABEL, "<$in_labelfile") || die "erreur de lecture de $in_labelfile: $!\n";
	}

#	drop the comments at the beginning	
#	comments have to be either empty lines either don't begin with "#define" 
	do{
		$line = <LABEL>;
		$line_count++;
	} 
	while ($line eq "" || !($line =~ /^\/\*that is the real begin of labels used\*\//i) );		
#	the first line in witch we are interested can be read	now

	
#	reads and adds all the labels
#	warning, the rest of the file must be well-formed without errors 	
	 @list_of_label = ();	# to avoid pbs
	 while ($line = <LABEL>) {
		$line_count++;	
		if ($line ne "") {
			addlabel ($line, $line_count);
		}
  	}
	close (LABEL) || die "problem during LABEL'file $in_labelfile is closed: $!\n";
	return 
} #end initlabel
################################################################################
## 								End  sub main
################################################################################

#--------------------------------------------------------------------

sub addlabel {
# extract From a line (!="") given in parameter the label and adds them in @list_of_label 
	my $line = shift;
	my $line_num = shift; 
	
	my $label;
	my $label_ref;
	my @else; 

	if ( $line ne "" && $line !~ /^\/\*/ && $line =~ /^#define/i ) {
		chomp ($line);
		($_,$label,$label_ref,@else) = split (/\s+/, $line]);
		$labels{$label} = $label_ref;
	}
	elsif ( $line eq "" || $line =~ /^\/\*/) {} # it's normal
	else {
	   print "label file $in_labelfile not well-formed at line $line_num\n";
   }
} #end addlabel

#--------------------------------------------------------------------



#--------------------------------------------------------------------
#--------------------------------------------------------------------
#--------------------------------------------------------------------




1;
__END__
#------------------end of file Read_label.pm-------------------------------



