#!/usr/bin/perl -w
package Edition;
use strict;

BEGIN {
	use vars qw( @ISA @EXPORT );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&product_translate
						);
}


use XML::Parser;
use Unicode::String qw(utf8 latin1);
	#to indicate that all string will be in utf8 (as they are read) by default
#	Unicode::String->stringify_as( utf8 );

 
#use IO::File;
use Iso qw( &return_code_in_ISO_639
				&belongs_to_list
			 );







	my $base = ""; #complete name of the base
						# ex : "/home/ehuck/opera/Amaya/tools/xmldialogues/base/base_am_msg.xml";
	my $where = "";# directory where the result files are putting (idem under but directory "/docs")
	my $sufix = "";# "-amayamsg"; # sufix of the dialogues files = result
	#$specific_sufix no more used
	#my $specific_sufix = "";# ".amaya.trans"; #used to indicate those specific files
	my $head_name = "";# "amayamsg.h";#name of the label file created
	#my $head_dir = "";
	#my $head_file = $head_dir.$head_name;
	
	my $begin_label = "/* Beginning of labels */";
	my $begin_message = "";
	

	my $lang = ""; # prefix for a language
	my $codage_current = ""; #used while language wanted not found
	my $encoding = ""; #to store the encoding
	
	
	my @list_of_files_created = ();
	
#	global variable used during parsing
	my $lang_already_exist = 0;
	my $current_tag = "";
	my $current_label = "";
	my $current_language = "";
	my $reference_value = 0;
	
	my $text = "";
	my $engish_text = "";
	
################################################################################
#						sub  edition (exported)
# 	this function creates the header file that define the Labels and a
#	ready-to-translate file 	
#	warning: need 8 parameters :see under
################################################################################

sub product_translate {

	($base , $head_name ,$where ,$sufix ,$begin_label ,$lang ) = @_;
	
	#$head_file = $head_dir.$head_name;
	print "\tBegin of producte ready-to-translate file from $base\n";
	
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

# to avoid problems
@list_of_files_created = ();
$reference_value = 0;
$encoding = "";

# what language
	if (! defined ($lang) || belongs_to_list($lang) == 0 ) {
		print "jygfg \n\n";
		do {
			$lang = Iso::return_code_in_ISO_639 ();
			print "\tAre you OK for $lang ? (Yes or No) [y]\n";
			chomp ($_ = <STDIN> );
			if ($_ eq "") {
				$_ = "y";
			}
		} while ($_ !~ /^y/i );
	}
		
# the files generated	
	push (@list_of_files_created, $where . $lang . $sufix );
	push (@list_of_files_created, $where . $head_name );

# create the two new files
	foreach (@list_of_files_created) {
		open ( BIDON, ">$_" ) || die "Can't create $_ because: $! \n";
		close ( BIDON )  || die "Can't create and close $_ because: $! \n";
	}
	open (OUTMESSAGE, ">>". $list_of_files_created[0]) 
		|| die ("Can't create :",$list_of_files_created[0]," because: $! \n");
	open (OUTLABEL, ">>" . $list_of_files_created[1] ) 
		|| die ("Can't create :", $list_of_files_created[1]." because: $! \n");



#	do the parse
	open ( IN,"<$base" ) || die ("Can't open $base because $!\n");
	$parser->parse (*IN); 						
	close ( IN ) || print ("Can't close $base because $!\n");

#	close the two files generated	
	close ( OUTMESSAGE ) 
		|| die ("Can't close",$list_of_files_created[0]," because: $! \n");
	close (OUTLABEL) 
		|| die ("Can't close", $list_of_files_created[1]." because: $! \n");

# 	indicate that all is OK	
	print "\tThis are the files created:\n";
	foreach (@list_of_files_created) {
		print "\t$_\n";
	}
}
################################################################################
##             end sub "main" 
################################################################################
#------------------------------------------------------------------------

#------------------------------------------------------------------------


#------------------THE HANDLERS--------------------------------------

sub start_hl {
	my ($expat, $element,%attributes ) = @_; 	# element is the name of the tag
	
	$current_tag = $element ;
	
	if ( $element eq "message" ) {
		if ( defined ($attributes {"xml:lang"}) ) {			 
			$current_language = $attributes{'xml:lang'} ;
			if ($current_language eq $lang ) {
				$text = "";# to have a new text
			}
			elsif ($current_language eq "en") {
				$engish_text = "";
			}
		}
		else {
			print "the message at line ". $expat->current_line ."don't have a lang attribute\n"
		}
	}
	elsif ( $element eq "label") {
		$current_label = $attributes {"define"};#	to remember the last label if there's a text between begin and end tag
		$reference_value++  ;
		print OUTLABEL "#define $current_label" . "\t\t" . $reference_value ."\n" ; 
	}
	elsif ( $element eq "base") {
		#nothing	
	}
	elsif ( $element eq "language" ) {
		#store the codage specify for the language
		if ( $attributes {"encoding"} ) {			 
			$codage_current = $attributes{'encoding'} ;
		}
		else {
			print "The language at line ". $expat->current_line ."don't have a codage attribute\n"
		}
	}	
	elsif ($element eq "control") {
		if ($lang_already_exist == 0 || ($encoding ne "utf8" && $encoding ne "latin1")) {
			do {
     			print "\tIn what type of encoding are messages for language $lang?\n",
   	  				"\tutf8 (default)=>1 or latin1=>2  [1] : ";
     			$_ = <STDIN> ;
				chomp ($_);
				if ($_ eq "") {
					$_ = 1;
				}
  			} while ($_ eq "" || $_ =~ /^\D/ || $_ <= 0 || $_ >= 3 );
	  		if ($_ == 2) {
	  	  		$encoding = "latin1";
  			}
  			else {
				$encoding = "utf8";
  			}
	
		}
		print OUTMESSAGE "# encoding= $encoding\n";
	}
	elsif ($element eq "messages") { #warning: messageS != message
		print OUTLABEL "$begin_label/\n";
	}
	elsif ( $element eq "") {
		print "Empty element at line " .  $expat->current_line . "\n";
	} 
	else {  #treat all the tag mismatched
		print "Tag  $element unknown at line " . $expat->current_line . "\n";
	}	

}  # End start_hndl

#--------------------------------------------------------------------

#--------------------------------------------------------------------
sub printer {
	my $string = Unicode::String->new (shift);
	
	if (defined ( $encoding ) ) {
		if ( $encoding eq "latin1" ) {
			print OUTMESSAGE $string->latin1 ;
		}
		else {
			print OUTMESSAGE $string->utf8 ;
		}	
	}
	else {
		print OUTMESSAGE $string->utf8;
	}
}
#--------------------------------------------------------------------

sub end_hl { #	do the modification if necessary
	my ($p, $end_tag) = @_;
	
	if (defined ($current_tag)) {
		if ($end_tag eq "label") {
			if (defined ($text)) {
				printer ("$reference_value $text\n");
			}
			else {
				printer ("$reference_value **$engish_text\n");
			}
		}
		elsif ($end_tag eq "message") {
			$current_language = "";
		}
		elsif ($end_tag eq "language") {
			$codage_current = "";
		}
		else {
			$current_tag = "";
		}
	}
}  # End end_hndl

#--------------------------------------------------------------------

sub char_hl {
#	use Unicode::String qw(utf8 latin1);
	my $p = shift ; 
	my $data = Unicode::String->new( shift ) ;
	
	if  ($data ne "" && $data =~ /^[\n\t]/ ) {
			$text =~ s/&amp;/&/g;
			$text	=~ s/&lt;/</g;
			
		if ( $current_tag eq "language") {
			if ($data eq $lang) {
				$encoding = $codage_current;
			}			
		}
		elsif ( $current_tag eq "message" && $current_language eq "en") {
			$engish_text .= $data;	
		}
		elsif ( $current_tag eq "message" && $current_language eq $lang) {
			$text .= $data;
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
