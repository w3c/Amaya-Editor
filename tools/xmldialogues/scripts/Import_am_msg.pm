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
	
	my $current_label = "";	#to notice the current label occured ,in which we are
	my $current_tag = ""; #to notice in which tag we are
	my $english_text = "";
	my $found = 0; #boolean used during the parse of a whole label to treat the new adds
	my $modification_necessary = 0; #boolean used during the parse of a whole label to treat the updating
	my @list_of_lang_occur = () ; #as is name
	 	$encodage = ""; #to load the encoding type of the messages
	my $end_label = ""; #to now what the latest label used by Amaya
# packages used

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
	if (shift) {
		$debug = 1;
	}
	$in_textfile = $in_textdirectory . $language_code . $in_textsufix;
	$newbasefile = $basefile . ".new";

#to avoid problems
	@list_of_lang_occur = () ;
	%labels = ();
	%texts = ();


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
################first : read the two files

# to load the label and their references
	my @list = ();
	do {
		$_ = @list = Read_label::init_label ($in_labelfile);
	}while ($_ == 0);	
	{#to fill the hach %labels :
		my $i = $list[0] + 1;
		#print	 "\n", $list[0] , "\n";	
		do {
			$labels{ $list[$i] } = $list[$i + 1];
			$i += 2;
		}while ( $i <= ($list [0] * 3) );
	}
	
#to load the messages and their references
	my $total = 0;		
	if ($encodage) {
		($_, $total, %texts) = Read_text::init_text ("$in_textfile", $encodage);
	}
	else {
		($encodage, $total, %texts) = Read_text::init_text ("$in_textfile");
	}	
							
###############then : parsing now	
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
	
	if ($debug) {
		print "\tThere is ",$_ = @list_of_lang_occur,"langues presentes:\n\t" ;
		foreach ( @list_of_lang_occur ) {
			print $_ . " ";
		}
	}
	
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
	my $expat = shift; 
 	my $element = $current_tag = shift; 	# element is the name of the tag
	my @attributes = ();
	my $numberparam = 0; #double of parametres, because they're going by pair

#	if (!defined ( $element)): for control, unused because function always called
#	when $element is defined ! 
	 
# 	store the attributes and their values	
	@attributes = ( shift);
	while (defined ($attributes[-1]) ) {
		push ( @attributes, shift ); # idem @attributes = (@attributes, shift);
		push ( @attributes, shift );
	}
	$numberparam = @attributes ;

#	use the result 
	if ( $element eq "message" ) {
		if ($attributes[0] eq "xml::lang"  #risque de pb si attributs mal ordonnes 
			&& $attributes[1] eq $language_code
			&& defined ($texts{ $labels { $current_label }}) 	) {
			debug ("message necessitant comparaison");
			$modification_necessary = $found = 1 ;
			my $num = 0;
			while ($attributes[$num] ) {
				if ($attributes[$num] eq "last_update") {
					++$num;
					$date = `date`;
					chomp $date;
					$attributes[$num]= $date; #take the date
					$num++;
				}
				else {
					$num++;
				}
			}

		}
		if ($attributes[0] eq "xml::lang"  # to have the english version when updating 
			&& $attributes[1] eq "en") {
				$english_text = "OK";
		}
		print OUT "\t";	
		addbegintag ( $element,$numberparam, @attributes );					
	}
	elsif ( $element eq "label") {
		$current_label = $attributes[1];#	to remember the last label if there's a text between begin and end tag
		addbegintag ( $element,$numberparam, @attributes );		
		print OUT "\n";#	small things necessary for presentation 
	}
	elsif ( $element eq "base") {
		# to do small modification to indicate new version
		my $num = 0;
		while ($attributes[$num] ) {
			if ($attributes[$num] eq "version") {
				++$attributes[++$num]; #increment the version
				$num++;
			}
			elsif ($attributes[$num] eq "last_update") {
				++$num;
				chomp ($date =`date`);
				$attributes[$num]= $date; #take the date
				$num++;
			}
			else {
				$num++;
			}
		}
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
		print "empty element at line " .  $expat->current_line () . "\n";
	} 
	else {  #treat all the tag mismatched
		print "tag  $element unknown at line " . $expat->current_line () . "\n";
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
				debug ("added");
				chomp ($date = `date`);
				print OUT "\t<message xml::lang=\"$language_code\" last_update=\"$date\" >";
				print OUT $texts{ $labels{$current_label}};
				print OUT "</message>\n";
			}else {
				if ( $current_label  ne $end_label) { # This label is always empty
					print '==> ' . "the label $current_label (ref ";
					print $labels{$current_label} ;
					print ")don't have a translate in the message file\n";
				}
			}	
		}
		$english_text = "";
	}
	elsif ($end_tag eq "control") { 
#	if we add a new language, we have to mention it	
		my $exist = 0;
		foreach $_ (@list_of_lang_occur) {
			if ($_ eq $language_code) {
				$exist = 1;	
				print "\nc'est une mise a jour\n";		
			}	
		}
		if  ($exist == 0) {
			print OUT "\t<language encoding=\"$encodage\">$language_code</language>\n" ;
		}
	}	
	### always
	print OUT "</$end_tag>\n";
}  # End endhndl

#--------------------------------------------------------------------
sub recopy {
my $data = $_[0];	
my $test;

	if ($data ne "\n" && $data ne "\t") {
		print OUT $data;
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
	elsif ( $data !~ /^\n/ && $data !~ /^\t/) {
		if ( $current_tag eq "language") {
			#if ( )
			@list_of_lang_occur = (@list_of_lang_occur, $data) ;
		}
		elsif ($current_tag eq "message") {
			if ( $modification_necessary) {
				if ($data ne $texts{ $labels{$current_label}}) {	
					my $choice = 0 ;
					do {
						print "L'ancienne valeur est :$data\n";
						if ($english_text ne "" && $english_text ne "OK") {
							print "La version anglaise donne :$english_text\n";
						}
						print "La nouvelle serait :" . $texts{ $labels{$current_label}} . "\n";
						print "Si vous voulez effectuer la mise a jour tapez 1,\nsinon 0\n"; 
						$choice = <STDIN>;
					}
					while ( $choice eq "" || $choice =~ /^\D/ || $choice < 0 || $choice >= 2 ) ;
					if ( $choice == 1) {
						$data = $texts{ $labels{$current_label}};
					}
				}	
				$modification_necessary = 0;			
			}
			if ($english_text eq "OK") {					
				$english_text = $data;
			}
			
		}
		recopy ( $data );
	}
	else { # do nothing
	}
} #End default_hndl


1;
__END__
#------------------end of file Import_am_msg.pm-------------------------------


########################no more utility



