#!/usr/bin/perl -w


#	WARNING: nead a paramater at call to indicate the language treat (i.e "en")
#	This is a small parser that makes the import
# 	that means have the header file (".h"), new_translated_dialogue and base files
#	for all labels in the base, the process search for the same label name in
#	to the ".h" and is corresponding text and after verification add it in
#	the updated base  

package main;

use XML::Parser;
use Unicode::String qw(utf8 latin1);
# read User configuration file
#use Am_var_global;

use strict;
# Global variables and default values
my $script_dir = "/home/ehuck/manuperl";
my $in_labeldirectory;
my $in_labelfile_name;
my $in_textdirectory;
my $in_textsufix;
	
my $base_directory = "/home/ehuck/xmldoc/";
my $basename = "base_am_msg.xml"; 
	
# declaration static of the variables used 
my $date = "date";

eval `cat $script_dir/Am_var_global.dat`;


# inutile pour l'instant:
#Use XML_SetCharacterDataHandler.
#    $us = Unicode::String->new( [$initial_value] )
#    $us->latin1("something");
#     equivalent a
#    $us = latin1("something");


# $debug is a boolean used during the typing and used for tests
	my $debug = 1;
# declaration static of the files used 
	my $language_code = $ARGV[0];
   my $in_labelfile = $in_labeldirectory . $in_labelfile_name;
	my $in_textfile = $in_textdirectory . $language_code . $in_textsufix;
	my $basefile = $base_directory . $basename; 
	my $newbasefile = $basefile . ".new";

# declaration of the global variables used in all the process
	my %labels;	#values of label (=key)and their reference 
	my %texts;	#references (=key) and their text
	my $current_label;	#to notice the current label occured ,in which we are
	my $current_tag; #to nitice in which tag we are
	my $old_text;
	my $found = 0; #boolean used during the parse of a whole label 
	my $lang_already_exist = 0 ;
	my $encodage = ""; #to load the encoding type of the messages
################
## main main
################
{

# declaration of the parser
	my $parser = new XML::Parser (
				ErrorContext  => 0, #number of lines shown 
															  #after a mistake  
				NoExpand	=> 1  #like his name (i.e. don't
																#translate '&lt'; into '<') 
            # Namespaces    => 0, ?????
            # ParseParamEnt => 1,  ?????
            );
#	declaration of the subs used when events are noted	
	$parser->setHandlers(
				Start => \&start_hndl,
			   End   => \&end_hndl,
			   #Char => \&char_hndl,#unused because NoExpand isn't 'pris en
				  							#compte' for the 'Char' event 
			   Comment => \&comment_hndl,
			   Default => \&default_hndl
				);

	initlabel ();
	inittext ();						
	
	open (IN,"<$basefile") || die "can't read $basefile because: $!\n";
	open (OUT,">$newbasefile") || die "can't create $newbasefile because: $!\n";
	debug ("\n\tBegin of the parse");

	$parser->parse (*IN); 
	
	close (IN) || die "problem during $basefile.old is closed: $!";
	close (OUT) || die "problem during $newbasefile is closed: $!";
	rename ( $newbasefile, $basefile )  || 	
		 	die "can't rename $newbasefile to $basefile because of: $! \nthe old base still exist, the new base name is $newbasefile\n";							
			
debug ( "the encodage is $encodage ");
	print "\tEnd IMPORT\n";
}################
## End main
################

sub debug { #wrote messages when necessary ($debug = 1)
	if ($debug) { 
		print $_[0];
		print "\n" ;
	}
}#end debug

#--------------------------------------------------------------------

sub addlabel {
# extract From a line (!="") given in parameter the elements and adds them in %labels 
	my $label;
	my $label_ref;
	my @else; 

	if ( $_[0] ne "" && $_[0] =~ /^#define/i ) {
		chomp ($_[0]);
#		$_[0] =~ s/\s{2,}/ /; #because there is often a lot of spaces 
		($_,$label,$label_ref,@else) = split (/\s+/, $_[0]);
#		$label =~ s/\s+//; # to avoid that $label still had spaces						 
#		debug ("$label $label_ref\n");
		$labels{$label} = $label_ref;
	}
	else {
	   print "label file $in_labelfile not well-formed at line $_[1]\n";
   }
} #end addlabel

#--------------------------------------------------------------------

sub initlabel {# fill the %labels from $in_labelfile that is reading
	my $line;
	my $line_count = 0; # used to indicate the line of an error
	
# open $in_labelfile only if it exists and is readable
	unless (-r $in_labelfile){ print "fichier $in_labelfile introuvable"}
   else {open (LABEL, "<$in_labelfile") || die "erreur de lecture de $in_labelfile: $!";}

#	drop the comments at the beginning	
#	comments have to be either empty lines either don't begin with "#define" 
	do{
		$line = <LABEL>;
		$line_count++;
	} while ($line eq "" || !($line =~ /^#define/i));		
#	the first line witch we are interested in have been already read	
	 if ($line ne "") {
	    addlabel ($line,$line_count);
	 }
	
#	reads and adds all the labels
#	warning, the rest of the file must be well-formed without errors 	
	while ($line = <LABEL>) {
		$line_count++;	
		if ($line ne "") {
			addlabel ($line, $line_count);
		}
  	}
	close (LABEL) || die "problem during LABEL'file is closed: $!";

# verification
#	if ($debug){
#		my $i;my %otherside;
#		%otherside = reverse %labels ;
#		for ($i = 0 ; $i <= 198 ; $i++) {
#			print "the label is ",$otherside{"$i"},"\t\tand the reference is $i\n";
#		}
#	}
	
	print " % labels initialized ! \n";
} #end initlabel

#--------------------------------------------------------------------
sub addtext {
# extract From a line (!="") given in parameter the elements and adds them in %text 
	my $label_ref;
	my $text;
	my @else; 

	if ( $_[0] ne "" ){
		chomp ($_[0]);
		($label_ref,@else) = split (/\s+/, $_[0]); # cut the spaces
		
#	group @else in $text with ' ' like character space beetween the elements
		$text = join (' ', @else);
#	tests if it not a false translate		
		if ( $text =~ /^\*\*/ ) {
			print "the reference don't have a text at line $_[1]\n";
		}	
		else {
#	OK but need translate for any html tags and the unicode
			$text =~ s/&/&amp;/g;
			$text	=~ s/</&lt;/g;
# import an ISO-latin1 string into UTF-8 if necessary
			$text	= latin1($text);
		#debug ("$label_ref  pour $text");
			$texts{"$label_ref"} = $text->utf8;
		}
	}
	else {
	  print "text file not well-formed at line $_[1]\n";
	}
}#end addtext

#--------------------------------------------------------------------

sub inittext { 
# fill the %text from $in_textfile that is the file source
# and search into the (first)line (# encoding= ...) the encoding 
	my $line;
	my $line_count = 0;
	my $diese;
	my $define;
	my @rest;
	
# open $in_textlfile only if it exists and is readable
	unless (-r $in_textfile){ print "fichier $in_textfile introuvable"}
   else {open (TEXT, "<$in_textfile") || die "erreur de lecture de $in_textfile: $!";}

	
#	reads and adds all the texts

#	warning, the file must be well-formed without errors and no comments	
	while ( $line = <TEXT> ) {
		$line_count++;
		if ($line =~ /^# encoding=/) {
			($diese, $define, $encodage, @rest) = split( /\s/,$line);
			debug("the encodage is$encodage voila");
		}
		elsif ($line ne "") {
			addtext ($line, $line_count);
		}
  	}
	unless ( $encodage eq "utf8"|| $encodage eq "latin1" ) {
		do {
			print "\tin what type of encoding are messages ?\n\tutf8 (default) or latin1\n";
			chomp ($encodage = <STDIN> );
		}while ($encodage ne "utf8" && $encodage ne "latin1");
	}
	
	close (TEXT) || die "problem during text'file is closed: $!";

# verification
#	if ($debug){
#		my $i;
#		for ($i = 0 ; $i <= 196 ; $i++) {
#			print "ref $i give the text:\t",$texts{"$i"},"\n";
#		}
#	}
	print " % texts initialized ! \n";

} #end inittext

#--------------------------------------------------------------------

#------------------THE HANDLERS--------------------------------------

sub start_hndl {
	my $expat = shift; 
 	my $element = $current_tag = shift; 	# element is the name of the tag
	my @attributes = ();
	my $numberparam = 0; #double of parametres, because they're going by pair

#	if (!defined ( $element)): for control, unused because function always called
#	when $element is defined ! 
	 
# 	store the attributes and their value	
	@attributes = ( shift);
	while (defined ($attributes[-1]) ) {
		push ( @attributes, shift ); # idem @attributes = (@attributes, shift);
		push ( @attributes, shift );
	}
	$numberparam = @attributes ;

#	use the result 
	if ( $element eq "message" ) {
		if ($attributes[0] eq "xml::lang"  #risque de pb si attributs mal ordonnes 
			&& $attributes[1] eq $language_code) {
			debug ("message necessitant comparaison");
			$found = 1 ;
			# A TRAITER			
#			print "this translate already exist for $current_label at ";
#			print $expat->current_line . "\n" ;
		}
		print OUT "\t";	
		addbegintag ( $element,$numberparam, @attributes );					
	}
	elsif ( $element eq "label") {
		$current_label = $attributes[1];#	to remember the last label if there's a text between begin and end tag
		addbegintag ( $element,$numberparam, @attributes );		
		print OUT "\n";#	small things necessary for presentation 
	}
	elsif ( $element eq "base_message") {
		addbegintag ( $element,$numberparam, @attributes );		
		print OUT "\n";#	small things necessary for presentation 
	}
	elsif ( $element eq "language" ) {
		print OUT "\t";	
		addbegintag ( $element,$numberparam, @attributes );					
	}	
	elsif ($element eq "control") {
		addbegintag ( $element,$numberparam, @attributes );		
		print OUT "\n";#	small things necessary for presentation 
	}
	elsif ($element eq "messages") {
		addbegintag ( $element,$numberparam, @attributes );		
		print OUT "\n";#	small things necessary for presentation 
	}
	elsif ( $element eq "") {
		print "empty element at line " .  $expat->current_line . "\n";
	} 
	else {  #treat all the tag mismatched
		print "tag  $element unknown at line " . $expat->current_line . "\n";
	}	


#	control 		
#	if ($debug && defined ($element)){
#   	print "tag=$element and his attributes:\t";
#		$i = 0;
#		while ( ($i < $numberparam) && defined ($attributes[$i]) ){		
#			print $attributes[$i++] . "\t" . $attributes[$i++];
#		}
#		print "\n";	
#	}
}  # End start_hndl

#--------------------------------------------------------------------

sub addbegintag { #	automatical copy WITHOUT modification to the attributes	
	my $i = 2 ;
	 
	print OUT "<" . $_[0];
	while ( ($i < $_[1]) && defined ($_[$i]) ) {
		print OUT ' ' . $_[$i++] . '="' . $_[$i++] . '"';	
	}
	print OUT ">";
}

#--------------------------------------------------------------------

sub end_hndl { #	do the modification if necessary
	my ($p, $end_tag) = @_;
		
	if ($end_tag eq "label") {	
		if ($found == 1) {
			$found = 0;
		}else { # there isn't yet a translate for this label into the base
			if ( $texts{ $labels{$current_label}} ) {
				print OUT "\t<message xml::lang=\"$language_code\" last_update=\"$date\" >";
				print OUT $texts{ $labels{$current_label}};
				print OUT "</message>\n";
				debug ("added");
			}else {
				if ( $current_label ne "AMAYA_MSG_MAX") { # This label is always empty
					print '==> ' . "the label $current_label (ref ";
					print "$labels{$current_label}" ;
					print ")don't have a translate in the message file\n";
				}
			}	
		}
	}
	elsif ($end_tag eq "control") { 
#	if we add a new language, we have to mention it	
		unless ($lang_already_exist) {
			print OUT "\t<language encoding=\"$encodage\">$language_code</language>\n" ;
		}
	}	
	print OUT "</$end_tag>\n";
}  # End endhndl

#--------------------------------------------------------------------
sub recopy {
my $data = $_[0];	
my $test;

	if ($data ne "\n" && $data ne "\t") {
#		print $data ."\n";
	
$test = $data;	
		if ($test ne $data) {
		    debug ("$test was converted into $data\n");
		}
		print OUT $data;
	}
}	#End recopy

sub char_hndl {
#	use Unicode::String qw(utf8 latin1);
	my ($p, $data) = @_;
	recopy ( $data );
	if ($current_tag eq "language" && $data eq $language_code ) {
		$lang_already_exist = 1 ;		
	}
}  #End char_hndl

#--------------------------------------------------------------------

sub comment_hndl { # it's just the comment that is automaticaly copied
	my ($p, $data) = @_;
	my $line = $p->current_line;
	
#	debug ("Comment line $line:\t$data");
	print OUT "<!--$data-->";
}	#End comment_hndl

#--------------------------------------------------------------------

sub default_hndl {	#for all the cases of an invalid xml document
	my ( $p, $data ) = @_;
   my $line = $p->current_line;

	if ( $data =~ /^<\?xml/ ) { 	# it' the head
		debug ("Head-line $line:\t$data");
		print OUT "$data\n";
	
	}
	elsif ( $data ne "\n" || $data ne "\t") {
	  	recopy ( $data );
	}
	else {
		print " Y'en a marre! :line $line \=> $data\n";
	}
} #End default_hndl
#------------------end of file Import_am_msg.pl-------------------------------



