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
my $dir_amaya = undef; #Parameter wanted
my $dir_obj_amaya = undef; #Parameter wanted
my $dir_perso_amaya = undef; #Parameter wanted

my $number_of_parameters = 3;

my $found = 0; #to indicate that all is OK when $found == number of parameters wanted (currently = 2)

my $home = "";
 ################# main  ##################################################
sub load_parameters { 
  	$home = shift;
my $config_file = shift ;

	#to avoid problem
	$dir_amaya = undef;
	$dir_obj_amaya = undef;
	$dir_perso_amaya = undef;

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
		
		$dir_amaya = ask_for_amaya ($dir_amaya);		
		$dir_obj_amaya = ask_for_obj ($dir_obj_amaya);
		$dir_perso_amaya = ask_for_perso ($dir_perso_amaya);	
		
			
		unless ($found == $number_of_parameters) {
			open ( OUT, ">$config_file") || die "can't modify $config_file because: $! \n";	
			printer ();
			close ( OUT ) || warn "can't close $config_file because: $! \n";
		}
	}
	else { ######### first use
		print "\tFile $config_file not found or unexistant\n";
		$dir_amaya = ask_for_amaya ($dir_amaya);		
		$dir_obj_amaya = ask_for_obj ($dir_obj_amaya);
		$dir_perso_amaya = ask_for_perso ($dir_perso_amaya);
		
		open ( OUT, ">$config_file") || die "can't create $config_file because: $! \n";
		printer ();
		close ( OUT ) || warn "can't close $config_file because: $! \n";
	}


	return ($dir_amaya ,$dir_obj_amaya, $dir_perso_amaya );
} #end main

#_____________________________________________________.......................
sub printer {

	print OUT 	"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\n"
				. "<configuration>\n"
				. "\t<dir_amaya name=\"$dir_amaya\"/>\n"
				. "\t<dir_obj name=\"$dir_obj_amaya\"/>\n"
				. "\t<dir_perso name=\"$dir_perso_amaya\"/>\n"
				. "</configuration>";

}

#_____________________________________________________.......................
sub ask_for_amaya {
	my $text = 	"Give the path of the parent directory of the directory Amaya. \n"
					
					. "[by default $home ]: ";
	my $directory = shift;
	
	if (defined ($directory)) {
		if (-d "$directory") {
			$found += 1 ;
		}
		else {
			do {
				print $text . "\n";
				print "Answer :\t";
				$directory = <STDIN>;
				chomp $directory ;	
				if ($directory eq "") {
					$directory = $home;
				}
			} while ($directory eq "" || !(-d "$directory" ));
		}
	}
	else {
		do {
			print $text . "\n";
			print "Answer :\t";
			$directory = <STDIN>;
			chomp $directory ;
			if ($directory eq "") {
					$directory = $home;
			}		
		} while ($directory eq "" || !(-d "$directory" ));	
	}
	
	
	
return $directory;
} 
######################
sub ask_for_obj {
	my $text = 	"Give the name of your object dirertory:\n"
					."Ex : LINUX-ELF  "
					."($dir_amaya/Amaya is already known) : ";
	my $directory = shift;
	
	if (defined ($directory)) {
		if (-d "$dir_amaya/Amaya/$directory") {
			$found += 1 ;
		}
		else {
			do {
				print $text . "\n";
				print "Answer :\t";
				$directory = <STDIN>;
				chomp $directory ;		
			} while ($directory eq "" || !(-d "$dir_amaya/Amaya/$directory" ));
		}
	}
	else {
		do {
			print $text . "\n";
			print "Answer :\t";
			$directory = <STDIN>;
			chomp $directory ;		
		} while ($directory eq "" || !(-d "$dir_amaya/Amaya/$directory" ));	
	}

return $directory;
} 
######################
sub ask_for_perso {
	my $text = 	"\tGive the path of the dirertory where input files are stored.\n"
					. "\t(don't end with \"/\" ) :";
	my $directory = shift;
	
	if (defined ($directory)) {
		if (-d "$directory") {
			$found += 1 ;
		}
		else {
			do {
				print $text . "\n";
				print "Answer :\t";
				$directory = <STDIN>;
				chomp $directory ;		
			} while ($directory eq "" || !(-d "$directory" ));
		}
	}
	else {
		do {
			print $text . "\n";
			print "Answer :\t";
			$directory = <STDIN>;
			chomp $directory ;		
		} while ($directory eq "" || !(-d "$directory" ));	
	}

return $directory;
} 



##############################################################################################################
sub start_ {
	my ($parser, $element, %param ) = @_;
	
	if (defined ($element) ) {
		if ($element eq "configuration"){
			#do nothing
		}
		elsif ($element eq "dir_amaya") {
			if (defined ( $param{"name"}) ) {
				$dir_amaya = $param{"name"};
			}		
		}
		elsif ($element eq "dir_obj") {
			if (defined ( $param{"name"}) ) {
				$dir_obj_amaya = $param{"name"};
			}		
		}
		elsif ($element eq "dir_perso") {
			if (defined ( $param{"name"}) ) {
				$dir_perso_amaya = $param{"name"};
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


