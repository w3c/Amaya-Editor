#!/usr/bin/perl -w

# 	this function creates the header file that define the Labels and all the files of
#	texts for all translates from the base in XML  

# si besoin
#charset=utf8 version (avec le fichier.h)




use strict;
use XML::Parser;
use Unicode::String qw(utf8 latin1);
use IO::File;

################
## main main
################
	my $base = "/home/ehuck/xmldoc/base_am_msg.xml";
	my $where = "/home/ehuck/xmldoc"; # directory where the result files are putting
	my $sufix = "-amayamsg"; # sufix of the dialogues files = result
	
	my @list_of_lang_occur = ();
	my $curent_label;
#	my $last_element_occur;
	my $curent_language ;
	my $curent_element; #to know in wuitch tag we are to treat texts
	my $reference_value = 0;

	my @list_of_dialogues_files = ();#to list the exacts names of the created files
	
	my @list_handles = (); #to record the names of the handles
	my %handle_names_ref ;	#to have the handle ref of each languages when output	
									#
	my %record_verification ; 	#To remember what kind of languages are already
										#read for the same label, it can be some lake
	my $english_text_reference; #somes languages don't have text for a label
	
	my @text_patches = (); #used because with the html, the text is cut in several patches
	
	
{


# declaration of the parser
	my $parser = new XML::Parser (
				ErrorContext  => 0 	#number of lines shown 
											#after a mistake  
				#, NoExpand	=> 1  #like his name (i.e. don't
									#translate '&lt'; into '<') 
             );
#	declaration of the subs used when events are noted	
	$parser->setHandlers(
				Start => \&start_hndl,
			   End   => \&end_hndl,
			   Char => \&char_hndl,# used because NoExpand isn't 'pris en compte
			   Comment => \&comment_hndl,
			   Default => \&default_hndl
				);
				
				
	# pb pour le cas ou les fichiers existent deja			
	open ( IN, "<$base") || die "can't read $where/amayamsg.h because: $! \n";
	push (@list_of_dialogues_files, "$where/amayamsg.h") ;
	
	$parser->parse (*IN); 
	
	close ( IN ) || die "can't close $where/amayamsg.h because: $! \n";
	print "the new file names are:\n";
	
	my $number = @list_of_lang_occur;
	print "voici les $number langues @list_of_lang_occur \n";
	
	print "\tEnd EXPORT\n";
}################
## end main
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
	$curent_element = my $element = shift; 	# element is the name of the tag
	my %attributes ;
	my $numberparam = 0; #double of parameters, because they're going by pair
	my $string ;
	
	my $attribute;
	 
# 	store the attributes and their value	
 	$attribute = shift;
	while (defined $attribute ) {		
		$attributes { $attribute } = shift ;
		$attribute = shift;
		$numberparam += 2 ;
	}	
#	print $numberparam . "\n";
#	foreach ( keys (%attributes)) {
#	 print "$_ give ". $attributes {$_} . "\n";
#	}


#	use the result 
				
	if ( $element eq "message" ) {
		if ( $attributes {"xml::lang"} ) {			 
			$curent_language = $attributes{'xml::lang'} ;
		}
		else {
			print "the message at line ". $expat->current_line ."don't have a lang attribute\n"
		}
		@text_patches = (); # to have a new text
	}
	elsif ( $element eq "label") {
		$curent_label = $attributes { "define"};#	to remember the last label if there's a text between begin and end tag
		$string = "#define $curent_label" . "\t\t" . $reference_value++ ."\n" ;
		print HEADFILE $string ; 
	}
	elsif ( $element eq "base_message") {
		#nothing	
	}
	elsif ( $element eq "language" ) {
		#nothing
	}	
	elsif ($element eq "control") {
		#nothing
	}
	elsif ($element eq "messages") { #warning: messageS != message
		open ( HEADFILE, ">$where/amayamsg.h") || die "can't create $where/amayamsg.h because: $! \n";
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
  
  $fh = IO::File->new( ">$filename" ) || die "pb with creating $filename: &!";
  return $fh;
}
#--------------------------------------------------------------------
sub print_in_a_file {
	my  $fhs = shift;
	
	print $fhs shift;
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
			if ( $curent_language eq "en") { # to reparate/fill the lakes
				$english_text_reference= "$reference_value " .  "**" . $string ;
			}				
			$string = "$reference_value " . $string ;
			$fh =  $list_handles [ $handle_names_ref{$curent_language} ];
	    	print_in_a_file ( $fh,"$string\n");
			$record_verification {$curent_language } = 1 ;
			
	}	
	elsif ( $end_tag eq "control" ) { 	#one time only
# must create as many files as  present languages (and erase the olds) 
		foreach $prefix (@list_of_lang_occur) {
			$file_name = $where . '/' . $prefix . $sufix ;
			push (@list_of_dialogues_files ,$file_name);

			$handle_names_ref {$prefix} = $handle_count++ ;
			push ( @list_handles , open_file ($file_name) );	
		}
		init_record_verification ();		
	}
		
	elsif ( $end_tag eq "label") {	#if it lake translates
		foreach $prefix ( keys (%record_verification) ) {
#			print $prefix . "\n";
			if ($record_verification {$prefix } == 0) {
				$fh = $handle_names_ref { $prefix};
#				print $fh . "\n";			
		      $fh = $list_handles [$fh];
#				print $fh . "\n";




				print_in_a_file ( $fh ,"$english_text_reference\n" );




			}
		}
		init_record_verification ();
	}
	elsif ($end_tag eq "messages") {
		close ( HEADFILE ) || die "can't close $where/amayamsg.h because: $! \n";
 
		foreach $prefix (@list_of_lang_occur) {
			# must close as many files as  present languages
		 	close ( $list_handles [$handle_names_ref {$prefix} ] ) 
			|| die "===>can't close $prefix$sufix\n";
		}
	}	
	else { ; #nothing
	}
}  # End endhndl

#--------------------------------------------------------------------

sub char_hndl {
#	use Unicode::String qw(utf8 latin1);
	my ($p, $data) = @_;
	my $fh;
	
	if  ($data ne "" && $data =~ /[^\n\t]/ ) {
#	print "$data\n";
		if ( $curent_element eq "language") {
		   $data =~ s/\s+// ; 
			if ( length ($data) > 1) {
			   push (@list_of_lang_occur , $data ) ;
			}
		}
		elsif ( $curent_element eq "message" && length ($data) > 1) {
			print "$data\n";
			@text_patches = (@text_patches, $data);				
		}
	}
	
}  #End char_hndl

#--------------------------------------------------------------------

sub comment_hndl { # it's just the comment that is automaticaly copied
	my ($p, $data) = @_;
	my $line = $p->current_line;
	
	print  "Comment line $line:\t$data\n";
}	#End comment_hndl

#--------------------------------------------------------------------

sub default_hndl {	#for all the cases of an invalid xml document
	my ( $p, $data ) = @_;
   my $line = $p->current_line;

	if ( $data =~ /^<\?xml/ ) { 	# it' the head
		print "Head-line $line:\t$data\n";
	
	}
	elsif ( $data ne "\n" ) {
	print "voici une irregularite" . $data . "\n";
#	  print " Y'en a marre! :line $line \=> $data\n";
	}
} #End default_hndl
#------------------end of file export_am_msg.pl-------------------------------
