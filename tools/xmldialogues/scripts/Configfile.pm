#!/usr/bin/perl -w
package Configfile;

use strict;
use XML::Parser;
BEGIN {
	use vars qw( @ISA @EXPORT @EXPORT_OK );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&load_parameters
						);
}

############################# globals variables 
my $rep_amaya = undef; #Parameter wanted
my $rep_obj_amaya = undef; #Parameter wanted

my $found = 0; #to indicate that all is OK when $found == number of parameters wanted (currently = 2)

my $home = "";
 ################# main  ##################################################
sub load_parameters { 
  	$home = shift;
my $config_file = shift ;

	#to avoid problem
	$rep_amaya = undef;
	$rep_obj_amaya = undef;


# declaration of the parser
	my $parser = new XML::Parser (
				ErrorContext  => 0 ,	#number of lines shown 
											#after a mistake  
				NoExpand	=> 1  #like his name (i.e. don't 
									#translate '&lt'; into '<') 
             );
#	declaration of the subs used when events are noted	
	$parser->setHandlers(
				Start => \&start_,
				End   => \&end_,
			   Default => \&default_
				);


	if (-r "$config_file") {
		open ( IN, "<$config_file") || die "can't read $config_file because: $! \n";	
		$parser->parse (*IN);
		# to control :
		close ( IN ) || warn "can't close $config_file because: $! \n";
		
		$rep_amaya = ask_for_amaya ($rep_amaya);		
		$rep_obj_amaya = ask_for_obj ($rep_obj_amaya);		
		unless ($found == 2) {
			open ( OUT, ">$config_file") || die "can't modify $config_file because: $! \n";	
			printer ();
			close ( OUT ) || warn "can't close $config_file because: $! \n";
		}
	}
	else { ######### first use
		print "\tFile $config_file not found or unexistant\n";
		$rep_amaya = ask_for_amaya ($rep_amaya);		
		$rep_obj_amaya = ask_for_obj ($rep_obj_amaya);
		
		open ( OUT, ">$config_file") || die "can't create $config_file because: $! \n";
		printer ();
		close ( OUT ) || warn "can't close $config_file because: $! \n";
	}


	return ($rep_amaya ,$rep_obj_amaya );
} #end main

#_____________________________________________________.......................
sub printer {

	print OUT 	"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\n"
				. "<configuration>\n"
				. "<rep_amaya name=\"$rep_amaya\"/>\n"
				. "<rep_obj name=\"$rep_obj_amaya\"/>\n"
				. "</configuration>";

}

#_____________________________________________________.......................
sub ask_for_amaya {
	my $text = 	"What the name of the repertory where the source codes for Amaya are\n"
					
					."($home is already known) : ";
	my $repertory = shift;
	
	if (defined ($repertory)) {
		if (-d "$home$repertory/") {
			$found += 1 ;
		}
		else {
			do {
				print $text . "\n";
				print "Answer :\t";
				$repertory = <STDIN>;
				chomp $repertory ;		
			} while ($repertory eq "" || !(-d "$home$repertory/" ));
		}
	}
	else {
		do {
			print $text . "\n";
			print "Answer :\t";
			$repertory = <STDIN>;
			chomp $repertory ;		
		} while ($repertory eq "" || !(-d "$home$repertory/" ));	
	}

return $repertory
} 
######################
sub ask_for_obj {
	my $text = 	"What the name of your object repertory for amaya\n"
					."Ex : LINUX-ELF  "
					."($home$rep_amaya/Amaya is already known) : ";
	my $repertory = shift;
	
	if (defined ($repertory)) {
		if (-d "$home$rep_amaya/Amaya/$repertory") {
			$found += 1 ;
		}
		else {
			do {
				print $text . "\n";
				print "Answer :\t";
				$repertory = <STDIN>;
				chomp $repertory ;		
			} while ($repertory eq "" || !(-d "$home$rep_amaya/Amaya/$repertory" ));
		}
	}
	else {
		do {
			print $text . "\n";
			print "Answer :\t";
			$repertory = <STDIN>;
			chomp $repertory ;		
		} while ($repertory eq "" || !(-d "$home$rep_amaya/Amaya/$repertory" ));	
	}

return $repertory
} 





##############################################################################################################
sub start_ {
	my ($parser, $element, %param ) = @_;
	
	if (defined ($element) ) {
		if ($element eq "configuration"){
			#do nothing
		}
		elsif ($element eq "rep_amaya") {
			if (defined ( $param{"name"}) ) {
				$rep_amaya = $param{"name"};
			}		
		}
		elsif ($element eq "rep_obj") {
			if (defined ( $param{"name"}) ) {
				$rep_obj_amaya = $param{"name"};
			}		
		}
	}

}
######################
##########################################################################################
sub end_ {
	my ($parser, $element, %param) = @_ ;
	 
	if ( defined ($element) ) {
		if ($element eq "configuration") {
		1;}
		else {1;}
	}
}
#######################################################################################
sub default_ {1;} #unused

1;
__END__


