#!/usr/bin/perl -w
package Dial_tool_box;

use strict;
use XML::Parser;
use Unicode::String qw(utf8 latin1);
	#to indicate that all string will be in utf8 (as they are read) by default
	#	Unicode::String->stringify_as( utf8 );
BEGIN {
	use vars qw( @ISA @EXPORT );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&delete_label
						&add_label
						);

}
##############################################################################
									#global variables
my $base_repertory = "";  #"/$home/opera/Amaya/tools/xmldialogues/xmldoc/";
my $name_of_base = "";#"base_am_msg.xml";

my $head_line = ""; #to store the head line
my $base_message = ""; #to store the tag named like
my $control = ""; #to store the contro block									
my %labels = (); #to store the all the label whith teir text
my @labels = (); #to keep the label sort (en francais ordre) 

# variables used during parsing
my $current_block = ""; #values = (control, messages)
my $current_label = "";

my $THE_label = "";
									
##############################################################################
sub add_label {
	$base_repertory = shift; 
 	$name_of_base = shift;
my $base = $base_repertory . $name_of_base ;

	read_the_base ();
	if ( ask1() ) {
		modify($base,"OK");
	}
}
#------------------------------------------------------------------------
sub delete_label {
	$base_repertory = shift; 
 	$name_of_base = shift;
my $base = $base_repertory . $name_of_base ;

	read_the_base ();
	if (ask2()) {
		modify($base,"remove");
	}
}						

#######################END SUB EXPORTED###########################################

sub ask1 {
my $textunicode = Unicode::String->new ();
my $text;
	print "\tWhat label would you like to add?\n";
	$THE_label = <STDIN>;
	chomp $THE_label;
	if ($labels{$THE_label}) {
		print "\tThe label you want to add already exists !\n";
		return 0;
	}
	else {
		$labels{$THE_label} = "<label define=\"$THE_label\">";
		#for english
		do {
			print "\tWhat the english text for  this label ($THE_label)?\n";
			$text = <STDIN>;chomp $text;
			print "Are you OK for:\n\t $text \n\t(Yes or No)? :\n";
			$_ = <STDIN>;
		}
		while ( $_ eq "" || $_ !~ /^y/ );
		$text =~ s/&/&amp;/g; # to avoid problem with message in HTML
		$text	=~ s/</&lt;/g;
		$textunicode = latin1($text); #because english is typed from the terminal in latin1
		$labels{$THE_label} .= "\n\t<message xml:lang=\"en\">";
		$labels{$THE_label} .= $text ;
		$labels{$THE_label} .= "</message>\n";
		#for french
		do {
			print "\tWhat the french text for  this label ($THE_label)?\n";
			$text = <STDIN>;chomp $text;
			print "\tAre you OK for:\n\t $text \n\t(Yes or No)? :\n";
			$_ = <STDIN>;
		}
		while ( $_ eq "" || $_ !~ /^y/ );
		$text =~ s/&/&amp;/g; # to avoid problem with message in HTML
		$text	=~ s/</&lt;/g;
		$textunicode = latin1($text); #because english is typed from the terminal in latin1
		$labels{$THE_label} .= "\n\t<message xml:lang=\"fr\">";
		$labels{$THE_label} .= $text ;
		$labels{$THE_label} .= "</message>\n";
		
		#the end of label added
		$labels{$THE_label} .= "</label>\n";
		# no more need to : push (@labels , $THE_label);
		return 1;
	}
}
#------------------------------------------------------------------------------
sub ask2 {
	print "\tWhat label would you like to delete?\n";
	$THE_label = <STDIN>;
	chomp $THE_label;
	if (defined ($labels{$THE_label}) ){
		#erase this value ?
		return 1;
	}
	else {
		print "\tThe label you want to delete never exists !\n";
		return 0;
	}

}
#------------------------------------------------------------------------------
sub modify {
	my $base = shift ;
	my $erase = shift;
	rename ( $base, "$base.old" )  || 	
		 	die "can't rename $base to $base.old because of: $! \n",
				"the old base still exist, you can redo the action";							
	open (OUT, ">$base");
	
	chomp ($head_line);
	print OUT $head_line;
	print OUT $base_message; 
	print OUT $control;
	print OUT "\n<messages>";
	if ($erase && $erase =~ /remove/) {
		foreach (@labels) {
			if ( $_ ne $THE_label) {
				print OUT $labels{$_},"\n";
			}
		}
	}
	else {
		foreach (@labels) {
			if ($_ eq "MAX_EDITOR_LABEL" || $_ =~ /MSG_MAX/) { #to add the new
																#label before the last one 
				print OUT $labels{$THE_label},"\n";
			}
			print OUT $labels{$_},"\n";
		}
	}
	
	print OUT "\n</messages>\n</base>";							

	close (OUT);
}
##############################################################################
##############################################################################
sub read_the_base {
	my $base_name = $base_repertory . $name_of_base ;
# declaration of the parser
	my $parser = new XML::Parser (
				ErrorContext  => 0, #number of lines shown after a mistake  
				NoExpand	=> 1  #like his name (i.e. don't translate '&lt'; into '<') 
             );
#	declaration of the subs used when events are noted	
	$parser->setHandlers(
				Start => \&start_,
			   End   => \&end_,
			   Comment => \&comment_,
			   Default => \&default_
				);
# to avoid problems
	%labels = (); 
 	@labels = (); 

	
	unless ( -r $base_name ) {
		print "fichier $base_name introuvable";
	} else {
		open ( IN, "<$base_name" ) || die "can't read $base_name because: $! \n";
	}
	
	$parser->parse (*IN);
	
	
	close (IN);


}
#-----------------------------------------------------------------------------
sub start_  {
#	my %argument = ();
	my ($p, $tag,%argument) = @_ ;
	my $num = 0;
	
	#foreach (keys (%argument)) { #used for control and debugging
	#print $_,$argument{$_}, "\n";}
	
	if ($tag eq "message") {	
#		print "message line " .$p->current_line() ." not into the block <messages>\n" unless ($current_block eq "messages" );
		if (defined ($current_label)) {
			$labels {$current_label} .= "\n\t<$tag";
			foreach ( keys (%argument)) {
				$labels {$current_label} .= " $_=\"" . $argument{ $_ } . "\"";
			}
			$labels {$current_label} .= ">" ;
		}
		else {
			print "error, tag $tag not well placed at line ",$p->current_line(),"\n";
		}	
	}
	elsif ($tag eq "label") {
	   unless (defined ($argument{"define"}) ) {
			print "label at line ",$p->current_line ()," don't have name\n";
		}
		$current_label = $argument{"define"};
		push (@labels, $current_label);
		$labels {$current_label} = "\n<$tag";
		foreach ( keys (%argument)) {
			$labels {$current_label} .= " $_=\"" . $argument{ $_ } . "\"";
		}
		$labels {$current_label} .= ">" ; 			
	}	
	elsif ($tag eq "control") { #into $control
		$control = "\n<$tag>";
		$current_block= $tag;
	}
	elsif ($tag eq "language") { #into $control
		$control .= "\n\t<$tag";
		foreach ( keys (%argument)) {
			$control .= " $_=\"" . $argument{ $_ } . "\"";
		}
		$control .= ">" ; 
	}
	elsif ($tag eq "base") { #into $head_line
		$base_message = "\n<$tag";
		$argument{ "version" } += 1;
		foreach ( keys (%argument)) {
			$base_message .= " $_=\"" . $argument{ $_ } . "\"";
		}
		$base_message .= ">" ;
	}
	elsif ($tag eq "messages") {
		$current_block = "messages";		
	}	

}
#-----------------------------------------------------------------------------
sub end_ {
	my ($p, $tag) = @_ ;
	
	if ($tag eq "message") {
		$labels{$current_label} .= "</$tag>";
	}
	elsif ($tag eq "label") {
		$labels{$current_label} .= "\n</$tag>";
		$current_label = undef; 		
	}
	elsif ($tag eq "control") {
		$control .= "\n</$tag>";
		$current_block =undef;
	}
	elsif ($tag eq "language") {
		$control .= "</$tag>";	
	}
	elsif ($tag eq "messages") {
		$current_block = undef;
	}
	
}
#-----------------------------------------------------------------------------
sub comment_ {
	1 ; #dont store them, because there is a lose of place
}
#-----------------------------------------------------------------------------
sub default_ {
	my ($p, $data) = @_ ;
   my $line = $p->current_line();

	if ( $data =~ /^<\?xml/ ) { 	# it' the head
		$head_line = "$data\n";	
	}
	elsif ($current_block) {
		if ($current_block eq "messages") {
			if ($current_label && $data !~ /\n/ && $data !~ /\t/) {
				$labels{$current_label} .= $data;
			}
		}
		else { # eq "control"
			if ( $data !~ /\n/ && $data !~ /\t/) {
				$control .= $data;
			}
		}
	}	
} #End default_hndl
#-----------------------------------------------------------------------------

##############################################################################

1;
__END__
warning : this treatment make lose of XML commentaries
this is a small package used for small manipulations of the bases like 
-add and remove label
-edit file for help translation by a contributor
