#!/usr/bin/perl -w
package Read_text; #written by Emmanuel Huck 
use strict;

BEGIN {
	use vars qw( @ISA @EXPORT );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&init_text
						&ask_for_encoding;
						)			
}

# packages used
use Unicode::String qw(utf8 latin1);

my %texts = ();
my $encodage = "";

my @list_of_references_that_dont_have_a_translate = ();

################################################################################
##							sub exported (see how to use it at the end)
################################################################################
sub init_text {
# fill the %texts from $in_textfile that is reading
#return (in one list):
#1)the number of texts recognizerd
#3)the list of the hash variable %text_refs (key = number, vlue = text in utf-8)
my $file = shift;
	$encodage = shift;


my $line_count = 0;
my $line;
my $diese;
my $define;
my @rest = ();

#to avoid problems
%texts = ();
@list_of_references_that_dont_have_a_translate = ();

# open $in_textlfile only if it exists and is readable
	if (!(-r $file)) { print "file $file not found\n"}
   else {
		open (TEXT, "<$file") || die "error of reading $file : $!\n";
		print "\tStart of reading $file\n";
#	reads and adds all the texts
# reads and uses the first line used normaly for the encoding type
		$line = <TEXT> ;
		$line_count++;
		
		chomp ($line); 
		if ($line && $line =~ /^# encoding=/) {
			($diese, $define, $encodage, @rest) = split( /\s/,$line);
			if ( !defined ($encodage) 
				||  ($encodage ne "utf8" && $encodage ne "latin1") ) { #no good encoding
				$encodage = ask_for_encoding ($file);				
			}
			else{ #it's OK
			}
		}
		else { # there isn't a line that mention the encoding
			if ( !defined ($encodage) #because encodage can be force when $encodage is exported
				|| ($encodage ne "utf8" && $encodage ne "latin1") ) { #no good encoding
				$encodage = ask_for_encoding ($file);
			}
			addtext ($line, $line_count, $file);
		}	
	
#	warning, the file must be well-formed without errors and no comments	
		while ( $line = <TEXT> ) {
			chomp ($line);
			$line_count++;
			if ($line ne "") {
				addtext ($line, $line_count, $file);
			}
	  	}
	
		close (TEXT) || warn "problem during $file is closed: $!";
	}
# verification
#	if ($debug){
#		my $i;
#		for ($i = 0 ; $i <= 196 ; $i++) {
#			if (defined ($texts{$i})) { 
#				print "ref $i give the text:\t",$texts{$i},"\n";
#			}	
#		}
#	}
	my $i = 0;
	foreach (keys (%texts)) {
		$i += 1;
	}
	print "\tThere are $i texts recognised\n"	;
	$_ = @list_of_references_that_dont_have_a_translate;
	if ( $_ != 0) {
		print "\tAnd there are $_ references that aren't already translated\n";
	}
	############### if you want the list of those reference read @list_of_references_that_dont_have_a_translate
	print "\tEND of reading $file\n";
	
	return ($encodage, $i, %texts);
	
}#end init_text
#########################################################################
sub ask_for_encoding {
	my $filename = shift;
	my $choice = 0 ;
	
	do {
     	print "\tIn what type of encoding are messages in $filename?\n",
   	  		"\tutf8 (default)=>1 or latin1=>2  [1] : ";
     	$choice = <STDIN> ;
		chomp ($choice);
		if ($choice eq "") {
			$choice = 1;
		}
  	}
  	while ($choice eq "" || $choice =~ /^\D/ || $choice <= 0 || $choice >= 3 );
  	if ($choice == 2) {
			print "\tTo avoid an other question,"
			 		. " please write \"# encoding= latin1\" in the first line\n";
   	  	return "latin1";
  	}
  	else {
			print "\tTo avoid an other question,"
			 		. " please write \"# encoding= utf8\" in the first line\n";
   	  return "utf8";
  	}
}
#########################################################################
sub addtext {
use Unicode::String qw(utf8 latin1);
# extract From a line (!="") given in parameter the elements and adds them in %text 
	my $label_ref;
	my $text = shift;
	my $line = shift;
	my $filename = shift;
	my @else; 
	my $textunicode = Unicode::String->new ();

	if ( $text ne "" ){
		chomp ($text);
		($label_ref,@else) = split (/\s+/, $text); # cut below to spaces		
		#	group @else in $text with ' ' like character space beetween the elements
		$text = join (' ', @else) ;
#	tests if it not a false translate		
		if ( 	!defined ($text) 
				|| ($text =~ /^\*\*/ && $text !~ /^\*\*\*/ )
			 	) {
			push (@list_of_references_that_dont_have_a_translate, $label_ref);
			#print "The reference $label_ref don't have a text at line $line:$text\n";
		}	
		else {
			#	OK but need translate for any html tags and the unicode
			$text =~ s/&/&amp;/g;
			$text	=~ s/</&lt;/g;
			# imports an ISO-latin1 string into UTF-8 if necessary
			if ($encodage eq "latin1") {
				$textunicode = latin1($text);
				$texts{"$label_ref"} = $textunicode;
			}
			else {
				$texts{"$label_ref"} = $text;
			}
		}
	}
	else {
	  print "Message file $filename not well-formed at line $line]\n";
	}
}#end addtext

#########################################################################
1;
__END__


#######that is the good way to use it :


my %b = (); # table keys/values
my @list = Read_text::init_text ("$file", $encoding);
unless ($encoding) {$encoding = $list[0]}
my $total = $list[1];
my $i = 2;
	do {
		$b{$list[$i]} = $list[$i+1];
		$i += 2;
	}while ( $i <= (($total * 2)+1) );
#########or more easy (two cases):
	if ($encoding) {
		($_, $total, %b) = Read_text::init_text ("$file", $encoding);
	}
	else {
		($encoding, $total, %b) = Read_text::init_text ("$file");
	}	
	
	this module was created by Emmanuel HUCK at the W3C in juin 2000
