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
						&init_label
						)			
}
# global variables
	my $num_of_label = 0 ; #how many are recognized
	my @list_of_label = (); # to record the sequence
	my %label_refs = (); #to record the references
	
	my $in_labelfile = "";
	
################################################################################
##							sub exported
################################################################################

sub init_label {# fill the %labels from $in_labelfile that is reading
	 $in_labelfile  = shift;
	

	
	my $line = "";
	my $line_count = 0; # used to indicate the line of an error
	my $continue = 1; # to indicate that one label at least is rcognise 
	
# open $in_labelfile only if it exists and is readable
	unless (-r $in_labelfile) { 
		print "fichier $in_labelfile introuvable\n";
		}
   else {
		open (LABEL, "<$in_labelfile") || die "erreur de lecture de $in_labelfile: $!\n";
	}

#	drop the comments at the beginning	
#	comments have to be either empty lines either don't begin with "#define" 
	
	while ( $continue && defined ($line = <LABEL>)  ) {
		$line_count++;
		if ($line && $line =~ /^\/\*that is the real begin of labels used\*\//i ) {
			$continue = 0 ;
		}		
	} 
#	the first line in witch we are interested can be (but not necessary)read	now
	if ( $continue) {
		do {
			print "\n\tPlease write this line at the begining of the good labels:\n",
					"/*that is the real begin of labels used*/\n",
					"\tInto $in_labelfile \n",
					"\tAre you ok? (Yes/No):\t";
			$_ = <STDIN>;
			chomp;				
		}
		while (!defined ($_) || $_ !~ /^y/i );
		close (LABEL) || die "problem during LABEL'file $in_labelfile is closed: $!\n";
		init_label ($in_labelfile) ; ##warning, can do some error
	}
	else {
#	reads and adds all the labels
#	warning, the rest of the file must be well-formed without errors 	
	 	@list_of_label = ();	# to avoid pbs
	 	while ($line = <LABEL>) {
			$line_count++;
			chomp ($line);	
			if ($line ne "") {
				addlabel ($line, $line_count);
			}
  		}	
	
	close (LABEL) || die "problem during LABEL'file $in_labelfile is closed: $!\n";
	}
	
	
	return ( $num_of_label,
				@list_of_label,
				%label_refs
			);
	
	 
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

	chomp ($line);
	if ( $line ne "" && $line !~ /^\/\*/ && $line =~ /^#define/i ) {
		($_,$label,$label_ref,@else) = split (/\s+/, $line);
		$num_of_label++;
		push (@list_of_label,$label );
		$label_refs{$label} = $label_ref;		
	}
	elsif ( $line eq "" || $line =~ /^\/\*/ || $line =~ /^#endif/) {} # it's normal
	else {
	   print "label file $in_labelfile not well-formed at line $line_num\n";
   }
} #end addlabel

#--------------------------------------------------------------------




1;
__END__
#------------------end of file Read_label.pm-------------------------------



