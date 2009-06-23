#!/usr/bin/perl -w
package Export_am_msg;

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
						&export
						);
}



# 	this function creates the header file that define the Labels and all the files of
#	texts for all translates from the base in XML  

# si besoin
#charset=utf8 version (avec le fichier.h)





	my $base = ""; #complete name of the base
	my $where = ""; # directory where the result files are putting
	my $sufix = ""; # sufix of the dialogues files = result
	my $head_name = "";#complete name of the ".h" file
	
	my @list_of_lang_occur = ();
	my $current_label = "";
	my $current_language = "";
	my $current_element; #to know in wuitch tag we are to treat texts
	my $reference_value = 0;

	my @list_of_dialogues_files = ();#to list the exacts names of the created files
	
	my @list_handles = (); #to record the names of the handles
	my %handle_names_ref = ();	#to have the handle ref of each languages when output	
									
	my %record_verification = (); 	#To remember what kind of languages are allready
										#read for the same label, it can be some lake
	my $english_text_reference = ""; #somes languages don't have text for a label
	
	my @text_patches = (); #used because with the html, the text is cut in several patches
	
	my %language_out_codages = (); 	#To indicate in which cadage the output file are
										#some needs encoding utf-8 to iso-latin1
	my $codage ; 	#because the codage is an attribute of <language> and the
						#coresspondig language is known after as a char
	
	#for small particularyty
	my $label_ending = "";
	my $comment_for_begining_of_h_file  = "";
	#for compilation
	my $begin_compilation = "";
	my $end_compilation = "";
	
	
	#for the specific file EDITOR.h
	my $recopy = 1;
	
################################################################################
## 									sub  main
################################################################################
sub export {
	$base = shift ; #complete name of the base
	$where = shift ; # where out put files ares 
	$sufix = shift ; # sufix of messages files
	$head_name = shift; # location/name of the ".h"
	$label_ending = shift; #name of the last label
	$comment_for_begining_of_h_file = shift;
	$begin_compilation = shift;
	$end_compilation = shift;
		
# to avoid problem when many calls
	$reference_value = 0;
	@list_handles = (); 
	%handle_names_ref = ();	
	@list_of_dialogues_files = ();
	@list_of_lang_occur = ();

#to treat EDITOR.h
	if ($head_name =~ /EDITOR.h/) {$recopy = 0};
	
# declaration of the parser
	my $parser = new XML::Parser (
				ErrorContext  => 0 ,	#number of lines shown 
											#after a mistake  
				NoExpand	=> 1  #like his name (i.e. don't 
									#translate '&lt'; into '<') 
             );
#	declaration of the subs used when events are noted	
	$parser->setHandlers(
				Start => \&start_hndl,
			   End   => \&end_hndl,
			   Char => \&char_hndl,# used because NoExpand isn't used
			   Comment => \&comment_hndl,
			   Default => \&default_hndl
				);
	
	
	print "\tBegin EXPORT of $base\n";						
# pb pour le cas ou les fichiers existent deja			
	open ( IN, "<$base") || die "can't read $base because: $! \n";
	#push (@list_of_dialogues_files, "$base") ;
	
	$parser->parse (*IN); 
	
	close ( IN ) || die "Can't close $head_name because: $! \n";
	
	my $number = @list_of_lang_occur;
	print "\tThis is the $number languages occured : @list_of_lang_occur \n"
			,"\tAnd the names of the generated files\n";
	foreach  (@list_of_dialogues_files) {
		print $_ , "\n";
	}
	
	print "\tEnd EXPORT\n";
}
################
## end sub main (export)
################
#------------------------------------------------------------------------

sub init_record_verification { #to reinititialise to 0  
	my $lang;
	
	foreach $lang (@list_of_lang_occur) {
		$record_verification { $lang } = 0 ;
	} 
} #	End init_record_verification


#------------------------------------------------------------------------


#------------------THE HANDLERS--------------------------------------

sub start_hndl {
	my $expat = shift; 
	$current_element = my $element = shift; 	# element is the name of the tag
	my %attributes ;
	my $string ;
	
	my $attribute;
	 
# 	store the attributes and their value	
 	$attribute = shift;
	while (defined $attribute ) {		
		$attributes { $attribute } = shift ;
		$attribute = shift;
	}	
#	print $numberparam . "\n";
#	foreach ( keys (%attributes)) {
#	 print "$_ give ". $attributes {$_} . "\n";
#	}


#	use the result 				
	if ( $element eq "message" ) {
		if ( $attributes {"xml:lang"} ) {			 
			$current_language = $attributes{'xml:lang'} ;
		}
		else {
			print "the message at line ". $expat->current_line ."don't have a lang attribute\n"
		}
		@text_patches = (); # to have a new text
	}
	elsif ( $element eq "label") {
		$current_label = $attributes { "define"};#	to remember the last label if there's a text between begin and end tag
		$string = "#define $current_label" . "\t\t" . $reference_value ."\n" ;
		if ($recopy) { 
			print HEADFILE $string ;
		} 
	}
	elsif ( $element eq "base") {
		#nothing	
	}
	elsif ( $element eq "language" ) {
		#store the codage specify for the language
		if ( $attributes {"encoding"} ) {			 
			$codage = $attributes{'encoding'} ;
		}
		else {
			print "the language at line ". $expat->current_line ."don't have a codage attribute\n"
		}
	}	
	elsif ($element eq "control") {
		#nothing
	}
	elsif ($element eq "messages") { #warning: messageS != message
		if ($recopy) { 
			open ( HEADFILE, ">$head_name") || die "can't product $head_name because: $! \n";
			push (@list_of_dialogues_files ,"$head_name");
			if (defined ($begin_compilation)) {
				print  HEADFILE "$begin_compilation\n";
			}
			print  HEADFILE "$comment_for_begining_of_h_file\n";
		}
	}
	elsif ( $element eq "") {
		print "empty element at line " .  $expat->current_line . "\n";
	} 
	else {  #treat all the tag mismatched
		print "tag  $element unknown at line " . $expat->current_line . "\n";
	}	
}  # End start_hndl

#--------------------------------------------------------------------
sub open_file {
  my ($filename) = shift;
  my $fh;
  if (-r $filename) {
  	rename ( $filename, $filename . ".old" )  || 	
		 	warn "can't rename the old $filename to $filename.old before Export because of: $!\n";
  }
  
  $fh = IO::File->new( ">$filename" ) || die "pb with creating $filename: &!";
  
  return $fh;
}
#--------------------------------------------------------------------
sub print_in_a_file { 	#as is name, and take the good codage if it's specified
	my $fhs = shift;
	my $string = Unicode::String->new( shift ) ;
 			
			
		#	if ($fh->open("< file")) {
       #        print <$fh>;
       #        $fh->close;
       #    }

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
}
#--------------------------------------------------------------------

sub end_hndl { #	do the modification if necessary
	my ($p, $end_tag) = @_;
	my $file_name;
	my $prefix;
	my $fh;
	my $handle_count = 0;
	my $string ;
	
	if ( $end_tag eq "message" ) {
#			print @text_patches ;
			$string = join ( '',@text_patches );
			if ( $current_language eq "en") { # to reparate/fill the lakes
				$english_text_reference= "$reference_value " .  "**" . $string ;
			}				
			$string = "$reference_value " . $string ;
			$fh =  $list_handles[$handle_names_ref{$current_language}];
			print_in_a_file ( $fh,"$string\n");
			$record_verification {$current_language } = 1 ;
			
	}	
	elsif ( $end_tag eq "control" ) { 	#one time only
# must create as many files as  present languages (and erase the olds) 
## JK: list_of lang_occur is not initalized for some obscure reason. I force 
## it here
    @list_of_lang_occur = split(' ', "en fr de it tr es pt fi ru hu ka cn tw sk ja nl uk");
		foreach $prefix (@list_of_lang_occur) {
			$file_name = $where . $prefix . $sufix ;
			push (@list_of_dialogues_files ,$file_name);

			$handle_names_ref {$prefix} = $handle_count++ ;
			push ( @list_handles , open_file ($file_name) );
			# to indicate in the head line the encoding
			$current_language = $prefix ; #to avoid small problem 
			print_in_a_file (  $list_handles [ $handle_names_ref{$prefix}], 
									"# encoding= " . $language_out_codages { $prefix } .	"\n");
				
		}
		init_record_verification ();		
	}
		
	elsif ( $end_tag eq "label") {	
		#if it lake translates
		foreach $prefix ( keys (%record_verification) ) {
#			print $prefix . "\n";
			if ($record_verification {$prefix } == 0) {
				$fh = $handle_names_ref { $prefix};
		      $fh = $list_handles [$fh];
				if ( $current_label ne $label_ending ) {		
					print_in_a_file ( $fh ,"$english_text_reference\n" );
				}
			}
		}
		$reference_value++ ; # to increment the reference number
		init_record_verification ();
	}
	elsif ($end_tag eq "messages") {
		if ($recopy) { 
			if (defined ($end_compilation)) {
				print HEADFILE $end_compilation ;
			}
			close ( HEADFILE ) || die "can't close $head_name because: $! \n";
 		}
		foreach $prefix (@list_of_lang_occur) {
			# must close as many files as  present languages
		 	close ( $list_handles [$handle_names_ref {$prefix} ] ) 
			|| warn "===>can't close $prefix$sufix because : $!\n";
		}
	}	
	else { ; #nothing
	}
}  # End endhndl

#--------------------------------------------------------------------

sub char_hndl {
#	use Unicode::String qw(utf8 latin1);
	my $p = shift ; 
	my $data = Unicode::String->new( shift ) ;
	my $fh;
	
	if  ($data ne "") {
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
			$data	=~ s/\t/ /g;
			$data	=~ s/\n/ /g;
			push (@text_patches, $data);				
		}
	}

}  #End char_hndl

#--------------------------------------------------------------------

sub comment_hndl { # it's just the comment that isn't copied
	my ($p, $data) = @_;
	my $line = $p->current_line;
	
	#print  "Comment line $line:\t$data\n";
}	#End comment_hndl

#--------------------------------------------------------------------

sub default_hndl {	#for all the cases of an invalid xml document
	my ( $p, $data ) = @_;
   my $line = $p->current_line;

	if ( $data =~ /^<\?xml/ ) { 	# it' the head
		#print "Head-line $line:\t$data\n";
	
	}
	elsif ( $data ne "\n" ) {
	print "voici une irregularite" . $data . "at line $line\n";
#	  print " Y'en a marre! :line $line \=> $data\n";
	}
} #End default_hndl
#------------------end of file export_am_msg.pl-------------------------------
1;
__END__
