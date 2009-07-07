#!/usr/bin/perl -w
package Import_am_msg;

use strict;
use XML::Parser;
use Unicode::String qw(utf8 latin1);

BEGIN {
	use vars qw( @ISA @EXPORT @EXPORT_OK );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&import_a_language
						);
	@EXPORT_OK = 	qw(
						$in_labelfile
						$basefile
						
						$in_textdirectory
						$in_textsufix
						$encodage
						);
	
}	
use vars  qw(		$in_labelfile
						$basefile
						$in_textdirectory
						$in_textsufix
						$encodage
						);


# Global variables 
	
# declaration static of the variables used 
	my $date = `date`;
	chomp ($date);

# $debug is a boolean used during the typing and used for tests
	my $debug = 0;
	
	
# declaration static of the files used 
	my $language_code = "";
	my $in_textfile = "";
	my $newbasefile = "";

# declaration of the global variables used in all the process	
	my %labels = ();	#values of label (=key)and their reference 
	my %texts = ();	#references (=key) and their text
	
	my $current_tag = ""; #to notice in which tag we are
	my $current_label = "";	#to notice the current label occured ,in which we are
	my $current_language = "";
	
	my $text = ""; # to store the message text
	my $english_text = "";# and the english version
	
	my $found = 0; #boolean used during the parse of a whole label to treat the new adds
	my @list_of_lang_occur = () ; #as is name
	 	$encodage = ""; #to load the encoding type of the messages
	my $end_label = ""; #to now what the latest label used by Amaya
# packages used
	my $warning = 0;

use Read_label qw ( &init_label );
use Read_text qw ( &init_text );

################################################################################
##							sub exported
################################################################################
#	WARNING: the four parameter in @EXPORT_OK must always be felt (rempli)

#	WARNING: nead a paramater at call to indicate the language treat (i.e "en")
#	This is a small parser that makes the import
# 	that means have the header file (".h"), new_translated_dialogue and base files
#	for all labels in the base, the process search for the same label name in
#	to the ".h" and is corresponding text and after verification add it in
#	the updated base  
sub import_a_language {
	$language_code = shift;
	$end_label = shift;
	 my $comment_at_the_begining = shift; 
	 
	if (shift) {
		$debug = 1;
	}
	$in_textfile = $in_textdirectory . $language_code . $in_textsufix;
	$newbasefile = $basefile . ".new";

#to avoid problems
	@list_of_lang_occur = () ;
	%labels = ();
	%texts = ();
	$warning = 0;

# declaration of the parser
	my $parser = new XML::Parser (
				ErrorContext  => 0, #number of lines shown after a mistake  
				NoExpand	=> 1  #like his name (i.e. don't translate '&lt;' into '<') 
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
################first: read the two files

# to load the label and their references
	my @list = ();
	do {
	    $_ = @list = Read_label::init_label ($in_labelfile, $comment_at_the_begining);
	}while ($_ == 0);	
	#to fill the hach %labels:
	my $i = $list[0] + 1;
	do {
			$labels{ $list[$i] } = $list[$i + 1];
			$i += 2;
	} while ( $i <= ($list [0] * 3) );
	
	
#to load the messages and their references into %texts
	my $total = 0;
print	  'encoding ==> '. "$encodage". "\n";
	if ($encodage) {
		($_, $total, %texts) = Read_text::init_text ("$in_textfile", $encodage);
	}
	else {
		($encodage, $total, %texts) = Read_text::init_text ("$in_textfile");
	}	
							
############### then: parsing now	
	open (IN,"<$basefile") || die "can't read $basefile because: $!\n";
	open (OUT,">$newbasefile") || die "can't create $newbasefile because: $!\n";
	debug ("\n\tBegin of the parse");

	$parser->parse (*IN); 
	
	close (IN) || die "problem during $basefile.old is closed: $!";
	close (OUT) || die "problem during $newbasefile is closed: $!";
			
	debug ( "the encodage is $encodage ");
	if ($warning == 1) {
		print "WARNING!!\nIt is the english version that couldn't be empty\n";
	}
	print "\tEnd Add/Update a language\n";
	
	if ($debug) {
		print "\tThere is ",$_ = @list_of_lang_occur,"languages:\n\t" ;
		foreach ( @list_of_lang_occur ) {
			print $_ . " ";
		}
	}
	rename ( $newbasefile, $basefile )  || 	
		 	die "can't rename $newbasefile to $basefile because of: $! \nthe old base still exist, the new base name is $newbasefile\n";							
	
} #end sub import_a_language

################################################################################
## 								End  sub main
################################################################################

sub debug { #wrote messages for debuging when necessary ($debug = 1)
	if ($debug) { 
		print $_[0];
		print "\n" ;
	}
}#end debug

#--------------------------------------------------------------------
#--------------------------------------------------------------------

#------------------THE HANDLERS--------------------------------------

sub start_hndl {
	my $expat; 
 	my $element = ""; 	# element is the name of the tag
	my %attributes = ();
	
	($expat,$element,%attributes) = @_ ; #to store the parameters: element and attributes and their values	
	 $current_tag = $element ;
	 
#unused
	my $numberparam = 0; 
	foreach (keys (%attributes)) {
		$numberparam++;
	} 
	

#	use the result 
	if ( $element eq "message" ) {
		if (	defined ($attributes{"xml:lang"}) ) {
			$current_language = $attributes{"xml:lang"};
			print OUT "\t";	
			addbegintag ( $element, %attributes );
			$text = ""; ############to reboot it
			if ($current_language eq $language_code) {
				$found = 1;
			}
		}
		else {
			print "==> The <$element> at line " . $expat->current_line () . 
					"don't have the attribute xml:lang\n";
		}
	}
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	
	elsif ( $element eq "label") {
		if (defined ($attributes {"define"}) ) {
			$current_label = $attributes{"define"};#	to remember the last label if there's a text between begin and end tag
			addbegintag ( $element, %attributes );		
			print OUT "\n";#	small things necessary for presentation 
		} else {
			print "==> The <$element> at line " . $expat->current_line () . 
					"don'thave the attribute define\n";
		}
	}
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	
	elsif ( $element eq "base") {
		# to do small modification to indicate new version
		if ( defined ( $attributes{"version"}) ) {
			$attributes{"version"} += 1; #increment the version
		} else {
			print "==> The <$element> at line " . $expat->current_line () . 
					"don'thave a version attribute\n";
		}

		addbegintag ( $element, %attributes );		
		print OUT "\n";#	small things necessary for presentation 
	}
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	
	elsif ( $element eq "language" ) {
		#print OUT "\t";	
		addbegintag ( $element, %attributes );					
	}	
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	
	elsif ($element eq "control") {
		addbegintag ( $element, %attributes );		
		print OUT "\n";#	small things necessary for presentation 
	}
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\		
	elsif ($element eq "messages") {
		addbegintag ( $element);		
		print OUT "\n";#	small things necessary for presentation 
	}
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\		
	elsif ( $element eq "") { #I don't now while it's possible
		#print "empty element at line " .  $expat->current_line () . " of the base\n";
	} 
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	
	else {  #treat all the tag mismatched
		print "tag  $element unknown at line " . $expat->current_line () . " of the base\n";
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
	my ($tag , %att) = @_;
	  
	print OUT "<" . $tag;
	foreach (keys (%att)) {
		print OUT " " . $_ . "=\"" . $att {$_}.  "\"";
	}	
	print OUT ">";
}

#--------------------------------------------------------------------

sub end_hndl { #	do the modification if necessary
	my ($p, $end_tag) = @_ ;  
	
		
	if ($end_tag eq "label") {	
		if ($found == 1) {
			$found = 0;
		}
        else { # there is no translation for this label within the base
#print '==> ' . "NOTRASLATE-- $current_label". "\n";
			if ($texts{ $labels{$current_label}}) {
				print OUT "\t<message xml:lang=\"$language_code\">";
				print OUT $texts{ $labels{$current_label}};
				print OUT "</message>\n";
			}
			else {
			    if ( $current_label ne $end_label) {# This label is always empty
				if ( $language_code eq "en") {
				    $warning = 1;
				}
				print '==> ' . "No translation for the $current_label";
				print " in the message file\n";
			    }
			}
		}
		$text = "";
		$english_text = "";
		$current_label = "";
	}
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	elsif ($end_tag eq "message") {
		if ($current_language eq "en" ) {
			$english_text = $text;
#print '==> ' . "TRACE-- $current_label". "\n";
		}
		elsif ($current_language eq $language_code) {			
		    if (defined ($labels{$current_label} )
			&& defined ($texts{$labels{$current_label}} )
		        && ("$text" ne "$texts{$labels{$current_label}}")) {
			my $choice = 0;
			do {
			    print "Replace: \"$text\"\n";
#\\\\\				print "By:" . $texts{$labels{$current_label}} . "\n";
			    print "By:      \"$texts{$labels{$current_label}}\"\n";
			    if ($english_text ne "" && $english_text ne "OK") {
				print "English version is: $english_text\n";
			    }
			    print "If you agree with this update, type 1, else 0\nOur choice [1]:\n"; 
			    $choice = <STDIN>; chomp $choice;
			    if ($choice eq "") {
				$choice = 1;
			    }
			} while ( $choice eq "" || $choice =~ /^\D/ || $choice < 0 || $choice >= 2 ) ;
			if ( $choice == 1) {
			    $text = $texts{$labels{$current_label}};
			}
		    }
		    else {
			#no problem they are the same or it's a new translation
			#print "label->$current_label val->$labels{$current_label} . "\n";
      print "Text:    \"$text\"\n";
      print "Correct: \"$texts{$labels{$current_label}}\"\n";
		    }
		}
		else {
#print '==> ' . "SKIP-- $current_language". "\n";
    }
		#always
		recopy ($text);
		#$current_language = $current_tag = undef;
    $current_tag = "";
	}	
#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
		elsif ($end_tag eq "control") { 
#	if we add a new language, we have to mention it	
		my $exist = 0;
		foreach $_ (@list_of_lang_occur) {
			if ($_ eq $language_code) {
				$exist = 1;	
				print "\n\tIt is an update\n";		
			}	
		}
		if  ($exist == 0) {
			print OUT "<language encoding=\"$encodage\">$language_code</language>\n" ;
		}
	}	
####################### always do:
	print OUT "</$end_tag>\n";

}  # End endhndl

#--------------------------------------------------------------------
sub recopy {
my $data = Unicode::String->new( shift ) ;
#my $data = $_[0];	
my $test;

	if ($data->utf8 ne "\n" && $data->utf8 ne "\t") {
		print OUT $data->utf8;
	}
}	#End recopy
#--------------------------------------------------------------------

#sub char_hndl { # never used because of NoExpand
#	use Unicode::String qw(utf8 latin1);
#	my ($p, $data) = @_;
#	recopy ( $data ) ;
#	if ( $current_tag eq "language") {
#		@list_of_lang_occur = (@list_of_lang_occur, $data) ;		
#	}
#	else {
#		debug ("c.est normal");
#	}
#	print $data,"===";
#}  #End char_hndl

#--------------------------------------------------------------------

sub comment_hndl { # it's just the comment that is automaticaly copied
	my ($p, $data) = @_;
	my $line = $p->current_line ();
	
#	debug ("Comment line $line:\t$data");
	print OUT "<!--$data-->";
}	#End comment_hndl

#--------------------------------------------------------------------

sub default_hndl {	#for all the cases of an invalid xml document
	my ( $p, $data ) = @_;
   my $line = $p->current_line ();

	if ( $data =~ /^<\?xml/ ) { 	# it' the head
		debug ("Head-line $line:\t$data");
		print OUT "$data\n";	
	}
	
	#to used all the text recognized
	elsif ($data !~ /^\t/) {
		if ( $current_tag eq "language") {
			@list_of_lang_occur = (@list_of_lang_occur, $data) ;
			recopy ($data);
		}
		elsif ($current_tag eq "message") {
			$text .= $data;	
		}
	}
	else { # do nothing
      #print "=>\"$text\" Data:\"$data->utf8\"\n";
	}
} #End default_hndl


1;
__END__
#------------------end of file Import_am_msg.pm-------------------------------


Package write by Emmanuel Huck
