#!/usr/bin/perl -w
package Edition;

use strict;
use XML::Parser;
use Unicode::String qw(utf8 latin1);
	#to indicate that all string will be in utf8 (as they are read) by default
#	Unicode::String->stringify_as( utf8 );
 
use IO::File;


BEGIN {
	use vars qw( @ISA @EXPORT );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&editer
						);
}



# 	this function creates the header file that define the Labels and a
#	ready-to-translate file 	



################
## sub  main
################
	my $base = "/home/ehuck/opera/Amaya/tools/xmldialogues/base_am_msg.xml"; #complete name of the base
	my $where = "/home/ehuck/xmldoc/"; # directory where the result files are putting
	my $sufix = "-amayamsg"; # sufix of the dialogues files = result
	my $specific_sufix = ".amaya.trans"; #used to indicate those specific files
	my $head_name = "amayamsg.h";#name of the label file created

	my $prefix = ""; # the prefix for a lamguage
	my @list_of_files_created = ();
#	global variable used during parsing
	my $lang_already_exist = 0;
	my $current_tag = "";
	my $current_language = "";


sub edition {
	$base = shift ;
	$where = shift ;
	$sufix = shift ;
	$head_name = shift;

	do {
		print "What the prefix of the new language?\n";
		chomp ($prefix = <STDIN>);
		print "Are you OK for $prefix ? (Yes or No)\n";
		chomp ($_ = <STDIN> );
	}
	while ($_ !~ /^o/i );
	push (@list_of_files_created, $where . $prefix . $sufix . $specific_sufix);
	push (@list_of_files_created, $where . $head_name . $specific_sufix );
# create the two new files
	foreach (@list_of_files_created) {
		open ( BIDON, ">$_" ) || die "Can't create $_ because: $! \n";
		close (BIDON) || die "Can't create and close $_ because: $! \n";
	}
	open (OUTMESSAGE, ">>". $list_of_files_created[0]) 
		|| die ("Can't create",$list_of_files_created[0]," because: $! \n");
	open (OUTLABEL, ">>" . $list_of_files_created[1] ) 
		|| die ("Can't create", $list_of_files_created[1]." because: $! \n");

# declaration of the parser
	my $parser = new XML::Parser (
				ErrorContext  => 2 	#number of lines shown 
											#after a mistake   
             );
#	declaration of the subs used when events are noted	
	$parser->setHandlers(
				Start => \&start_hl,
			   End   => \&end_hl,
			   Char => \&char_hl,# used because NoExpand isn't 'pris en compte`
			   Comment => \&comment_hl,
			   Default => \&default_hl
				);	
#	do the parse
	open (IN,"<$base" ) || die ("Can't open $base because $!\n");
	$parser->parse (*IN); 						
	close (IN) || print ("Can't close $base because $!\n");
#	close the two files generated	
	close (OUTMESSAGE) 
		|| die ("Can't close",$list_of_files_created[0]," because: $! \n");
	close (OUTLABEL) 
		|| die ("Can't close", $list_of_files_created[1]." because: $! \n");


# 	indicate that all is OK	
	print "This are the files created:\n"
	foreach (@list_of_files_created) {
		print $_, "\n";
	}
}
################
## end sub main (edition)
################
#------------------------------------------------------------------------

#------------------------------------------------------------------------


#------------------THE HANDLERS--------------------------------------

sub start_hl {
	my ($expat, $element,%attributes ) = @_; 	# element is the name of the tag

	if ($element eq "") {
		
	}
	elsif ($element eq "") {
		
	}
	elsif ($element eq "") {
		
	}
	elsif ($element eq "") {
		
	elsif ($element eq "") {
		
	}
	else {
	}
}  # End start_hndl

#--------------------------------------------------------------------

#--------------------------------------------------------------------

	if (defined ( $language_out_codages{$current_language} ) ) {
		if ( $language_out_codages{$current_language} eq "latin1" ) {
			print $fhs $string->latin1 ;
		}
		elsif ( $language_out_codages{$current_language} eq "utf8" ) {
			print $fhs $string->utf8 ;
		}
		else {
			print $fhs $string->utf8 ;
		}	
	}
	else {
		print $fhs $string->utf8;
	}

#--------------------------------------------------------------------

sub end_hl { #	do the modification if necessary
	my ($p, $end_tag) = @_;

}  # End end_hndl

#--------------------------------------------------------------------

sub char_hndl {
#	use Unicode::String qw(utf8 latin1);
	my $p = shift ; 
	my $data = Unicode::String->new( shift ) ;
	my $fh;
	
	if  ($data ne "" && $data =~ /[^\n\t]/ ) {
		if ( $current_element eq "language") {
		   $data =~ s/\s+// ; 
			if ( length ($data) > 1) {
			   push (@list_of_lang_occur , $data ) ;
				$language_out_codages { $data } = $codage ;
			}
		}
		elsif ( $current_element eq "message" && length ($data) >= 1) {
			$data =~ s/&amp;/&/g;
			$data	=~ s/&lt;/</g;
			push (@text_patches, $data);				
		}
	}

}  #End char_hndl

#--------------------------------------------------------------------

sub comment_hl { # it's just the comment that is automaticaly copied

}	#End comment_hndl

#--------------------------------------------------------------------

sub default_hl {	#for all the cases of an invalid xml document
	my ( $p, $data ) = @_;
   my $line = $p->current_line;

	if ( $data =~ /^<\?xml/ ) { 	# it' the head line
	}
	elsif ( $data ne "\n" ) {
		print "voici une irregularite" . $data . "\n";
	}
} #End default_hndl
#------------------end of file export_am_msg.pl-------------------------------
1;
__END__
