#!/usr/bin/perl -w

#function that read amayamsg.h file and write it on the base_am_msg.xml
# after recognizing the different elements
use strict;

{	
   my $line;
   my $define;
   my $ref_name;
   my $value;
   my @rest;
	my $i;

   my $in_filename = "/home/ehuck/opera/Amaya/amaya/amayamsg.h";
	my $out_basename = "/home/ehuck/opera/Amaya/tools/xmldialogues/docs/base_am_msg.xml" ;
#	an other posybility is to initialize them with parameters like that
#	($filename,$out_basename) = @ARGV;

	
###########################################################################
###                       main
########################################################################
	

#	initialization of the base
	open (OUT, ">$out_basename") || die "can't create $out_basename: $!";
	print OUT "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\n";
	print OUT "<base_message version=\"release num #\" last_update=\"date\" >\n";

# 	indicates the languages that occur into the field <control>, 
#	Engish is mandatory, beause it's the refference 
	print OUT "<control>\n</control>\n";
	
#to indicate the begin of the labels
	print OUT "<messages>\n";
	
	
#	read the file source only if it exists and is readable
	unless (-r $in_filename ) {
		print "fichier $in_filename introuvable";
		}
   else {
		open (IN, "<$in_filename") || die "erreur de lecture de $in_filename: $!";
		}

#	drop the comments at the beginning	
	do {
		$line = <IN>;
		chomp ($line);
		($define,$ref_name,$value,@rest) = split (/ /, $line);
	}	while ($line eq "" || $define ne "#define");
	
#	the first line wich we are interested in is already red	
	$ref_name =~ s/\s//;$ref_name =~ s/\s//; add ($ref_name);
	
#	reads and adds all the labels
#	warning, the rest of the file must be well-formed without errors 	
	while ($line = <IN>) {
		chomp ($line);
		$line =~ s/\s+/ /g;
		($define,$ref_name,$value) = split (/ /, $line);
#		print "$ref_name\n";
		if ($line ne "" && $define eq "#define" ) {
			for ($i = 0,$i <= 5,$i++) {
				$ref_name =~ s/\s//g; # to avoid that $ref_name still had spaces
				}						 # many times because one isn't always enough
			add ($ref_name);
		}
  	}
   
	
	
#	ending 
	print OUT "</messages>\n";
	print OUT "<!--end of base -->\n</base_message>";
   
	close (IN) || die "problem during the IN closed: $!";
	close (OUT) || warn "problem during the OUT closing: $!";
	
	print "\tBASE initialized,\n\t her name is $out_basename \n";

}#########################################################
### 											end main
########################################################

sub add{ #	write the new label given in first parameter in 'OUT'  
 
	print OUT "<label define=\"";
	print OUT "$_[0]";
	if ( $_[0] eq "AMAYA_MSG_MAX") {
		print OUT "\">\n";
		print OUT "<!--this label must still empty end the latest because it";
		print OUT "shows the end of labels that are used for Amaya --></label>\n"
	}
	else {
		print OUT "\"></label>\n";
	}
}
