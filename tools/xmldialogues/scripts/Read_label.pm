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
##							sub exported (see how to use it at the end)
################################################################################

sub init_label {
# fill the %labels from $in_labelfile that is reading
#return (in one list):
#1)the number of labels recognizerd
#2)the list of the labels @list_of_label
#3)the list of the hash variable %label_refs

	$in_labelfile  = shift; #name of the label file
	my $comment_for_begining_of_h_file = shift;


	my $line = "";
	my $line_count = 0; # used to indicate the line of an error
	my $continue = 0; # to indicate that one label at least is recognise 

#to avoid problem	
	$num_of_label = 0 ; 
	@list_of_label = (); 
	%label_refs = (); 
	
	
	unless (defined ( $comment_for_begining_of_h_file) ) {
						
		print "There must be a specific commentary at the begining like:\n"
				. "\t/* Beginning of labels */\n"
				. "please fill the comment_for_begining_of_h_file variable into "									
				. "Am_dial_managment and verify that this parameter is given to "
				. "Read_label::init_label\n"
				. "Now press <ctrl>-c and restart the proram after modification\n";
				<STDIN>;
	}
					
					
					
# open $in_labelfile only if it exists and is readable
   if (-r $in_labelfile) { # it's ok
		if (! (open (LABEL, "<$in_labelfile")) ) {
		 	warn "Error no reading possible in $in_labelfile: $!\n";	
		}
		else {
#	drop the comments at the beginning	
#	comments have to be either empty lines either don't begin with "#define" 
			
			
			do {
				$line = <LABEL> ;
				if (defined ($line) ) {
					chomp ($line);
					$line_count++;
					#print $line . "\n";
					if ( $line eq $comment_for_begining_of_h_file) {
						$continue = 1;
					}
				}
			} while ( defined ($line) && $line ne $comment_for_begining_of_h_file );
			
			

#	the first line in which we are interested can be (but not necessary)read	now
			if ( $continue == 0) {
				close (LABEL) || warn "problem during LABEL'file $in_labelfile is closed: $!\n";
				
				do {
					print "\n\tPlease write this line at the begining of the good labels:\n",
							"$comment_for_begining_of_h_file\n",
							"\tInto $in_labelfile \n",
							"\tDone? (Yes/No):\t";
					$_ = <STDIN>;
					chomp;				
				}
				while (!defined ($_) || !/^y/i );
				init_label ($in_labelfile, $comment_for_begining_of_h_file) ; 
				##warning : recursivity, can do some errors
			}
			else { #continue == 1
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
			if ($num_of_label == @list_of_label) {	 
				return ( $num_of_label,
						@list_of_label,
						%label_refs
						);
			}
			else {
				print "\tProblem during the reading of $in_labelfile\n";
			}
		}#end else open...	
	}	 
	else  {   #(!(-r $in_labelfile))
		print "\tFile $in_labelfile not found\n";
	}
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
	if ( $line eq "" || $line =~ /^\/\*/ || $line =~ /^#endif/) 
		{} # it's normal
	elsif ( $line ne "" && $line !~ /^\/\*/ && $line =~ /^#define/i ) {
		($_,$label,$label_ref,@else) = split (/\s+/, $line);
		if (	defined $label_ref 
				&&  !(defined ( $label_refs{$label} )) 
			############ used to treat the specific file EDITOR.h manualy
			#	&& $label ne "TEXT_UNIT"
			#	&&	$label ne "GRAPHICS_UNIT"
			#	&&	$label ne "SYMBOL_UNIT"
			#	&&	$label ne "PICTURE_UNIT"
			#	&&	$label ne "REFERENCE_UNIT"
			#	&&	$label ne "PAGE_BREAK" 
			) {
			$num_of_label += 1;
			push (@list_of_label, $label );
			$label_refs{$label} = $label_ref;
		}
		else {
			if (defined ( $label_refs{$label}) ) {		
				print "The label $label at line $line_num allredy exists (before)\n";
			}
			else { 	
				print "label file $in_labelfile not well-formed at line $line_num\n";
			}
		}
	}
	else {
	   print "label file $in_labelfile not well-formed at line $line_num\n";
   }
} #end addlabel

#--------------------------------------------------------------------


1;
__END__
that is the good way to use it :
use Read_label qw (&init_label);

{
my @a = (); # sorted list
my %b = (); # table keys/values
my @list = Read_label::init_label ("/home/ehuck/opera/Amaya/amaya/amayamsg.h", $comment);
my $total = $list[0];

my $i = 1;
	
	do {
		push (@a, $list[$i]  );
		$i++;
	}while ( $i <= $total );
	#or : WARNING it's not the same 
	
	$i = $total + 1;
	do {
		$b{$list[$i]} = $list[$i+1];
		$i += 2;
	}while ( $i <= ($total * 3) );

}
#------------------end of file Read_label.pm-------------------------------



