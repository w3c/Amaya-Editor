#!/usr/bin/perl -w

use strict ;


#############################for parmameters###########################################
# some environement variables for portability  
my $home = $ENV{"HOME"} ;
my $config_file = "$home/.amaya/am_dialogues.conf";
my $rep_amaya = rep_amaya (); # way between $home and the Repertory Amaya and libwww
my $rep_obj = rep_obj (); #name of the object direcitriy for Amaya


#### 	for all the bases
# directory for bases , label and translated NEW  files
my $OUT_PUT_directory = "$home/$rep_amaya/Amaya/tools/xmldialogues/docs";


my %base_name ;	# table for the name of the bases 
	
my %head_dir ; 	# table for the name of the directories where ".h" files are
my %head_name ;	# table for the name of the ".h" files

my %lang_dir ;		# table for the name of the directories where translated texts are
my %lang_sufix ;	# table for the sufix name of the translated texts

my %index =qw ( 	1	dia
						2	msg
						3	lib
						4	corrd);

#### 	for Amaya dialogue => dia or $index {1}
  $head_dir{'dia'} = "$home/$rep_amaya/Amaya/$rep_obj/amaya/";# idem $head_dir{$index{"1"}} = ...
  $head_name {'dia'}= 'EDITOR.h';
  $lang_dir{'dia'} = "$home/$rep_amaya/Amaya/config/";
  $lang_sufix {'dia'} = '-amayadialogue';
  $base_name {'dia'} = 'base_am_dia.xml';

####	for Amayamsg => msg or $index {2}
 $head_dir {'msg'} = "$home/$rep_amaya/Amaya/amaya/";
 $head_name {'msg'} = 'amayamsg.h' ;
 $lang_dir {'msg'} = "$home/$rep_amaya/Amaya/config/";
 $lang_sufix {'msg'} = '-amayamsg' ;
 $base_name {'msg'} = 'base_am_msg.xml';

####	for libdialogue => lib or $index {3}
 $head_dir {'lib'} = "$home/$rep_amaya/Amaya/thotlib/include/" ;
 $head_name {'lib'} = 'libmsg.h' ;
 $lang_dir {'lib'} = "$home/$rep_amaya/Amaya/config/";
 $lang_sufix {'lib'} = '-libdialogue' ;
 $base_name {'lib'} = 'base_am_lib.xml';


####	for corrdialogue => corrd or $index {4}
 $head_dir {'corrd'} = "$home/$rep_amaya/Amaya/thotlib/internals/h/" ;
 $head_name {'corrd'} = 'corrmsg.h' ;
 $lang_dir {'corrd'} = "$home/$rep_amaya/Amaya/config/";
 $lang_sufix {'corrd'} = '-corrdialogue' ;
 $base_name {'corrd'} = 'base_am_corrd.xml';



################################################################################

#load the different modules needed
use Initialisation qw(	&create_base );

use Import_am_msg qw( &import_a_language
								$in_labelfile
								$basefile
								$in_textdirectory
								$in_textsufix
							);
use Export_am_msg qw ( &export );
use Forcer qw ( &forcer);


#################################MAIN###########################################
{

menu () ;

}#############################END MAIN###########################################
sub menu {
	my @list = ( 	"Rien",
						"Amaya dialogue",
						"Amayamsg",
						"libdialogue",
						"corrdialogue") ;
	my $count = 0;
	my $choice = 0;

	`clear`; # ne veux pas marcher
	print "=======================================================================",
			"\t\tGestionnaire des dialogues d'Amaya\n";
	do {
		do {
			print "\nQue voulez-vous traiter?\n";
			$count = 0;
			foreach (@list) {
				print "\t" . $count++ . "=>\t$_\n";
			}
			print " Votre choix:\t";
			$choice = <STDIN> ;
			chomp ($choice);
		}
		while ( $choice =~ /^\D/ || $choice < 0 || $choice >= $count ) ;
		
		if ( $choice == 1 || $choice == 2 || $choice == 3 || $choice == 4) {
			menu1 ( $choice );
		}
		
	}
	while ( $choice != 0 );
	
}
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
sub menu1 {
	my $last_choice = shift ;
	my $count;
	my $choice;
	my @list = ( 
		"Rien", 
		"Initialiser la base" ,
		"Ajouter/ Mettre a jour une langue", 
		"Editer les fichiers de dialogue") ;		
	if ($last_choice == 1 )	{
		push (@list,"Forcer la base a se mettre en conformite avec \"EDITOR.h\"" ); 
	}	
	my $lang = "";
	my @command = "";
			
	do {
		print "\nQue voulez-vous lui appliquer?\n";
		$count = 0;
		foreach (@list) {
			print "\t" . $count++ . "=>\t$_\n";
		}
		print " Votre choix:\t";
		$choice = <STDIN> ;
		chomp ($choice); 
	}
	while ( $choice =~ /^\D/ || $choice < 0 || $choice >= $count ) ;
	print "\n";
	
	if ($choice == 1) {
		Initialisation::create_base ( $head_dir{ $index{ $last_choice} }, 
												$head_name{ $index{ $last_choice} },
												$OUT_PUT_directory, 
												$base_name { $index{ $last_choice} });
		# to initialise with english
		print "Remplissage de la base avec l'anglais par default\n";
		$Import_am_msg::in_labelfile = $head_dir{ $index{ $last_choice}} . $head_name{ $index{ $last_choice}};
		$Import_am_msg::basefile = $OUT_PUT_directory . "/" . $base_name { $index{ $last_choice}};
		$Import_am_msg::in_textdirectory = $lang_dir { $index{ $last_choice}};
		$Import_am_msg::in_textsufix = $lang_sufix { $index{ $last_choice}};
		Import_am_msg::import_a_language ("en") ;

	}
	elsif ($choice == 2) {
	
		print "Quelle langue voulez vous traiter?\n";
		$lang = <STDIN>;
		chomp $lang;
		{	
		$Import_am_msg::in_labelfile = $head_dir{ $index{ $last_choice}} . $head_name{ $index{ $last_choice}};
		$Import_am_msg::basefile = $OUT_PUT_directory . "/" . $base_name { $index{ $last_choice}};
		$Import_am_msg::in_textdirectory = $lang_dir { $index{ $last_choice}};
		$Import_am_msg::in_textsufix = $lang_sufix { $index{ $last_choice}};
		Import_am_msg::import_a_language ($lang) ;
		}
	}	
	elsif ($choice == 3) {
		Export_am_msg::export (	$OUT_PUT_directory . $base_name{ $index{ $last_choice}},
										$OUT_PUT_directory,
										$lang_sufix { $index{ $last_choice}},
										$head_name{ $index{ $last_choice}}
										);
	}
	elsif ($choice == 4) { # n'arrive que si last_choice = 1
		Forcer::forcer ( 
							);
	
	}	
		
}

#-------------------------------------------------------------------------------
######################### for configuration ####################################
#-------------------------------------------------------------------------------
 sub rep_amaya {# to load configuration parameter "amaya_rep"
	my $name = "notOK";
	my $line;	
	my $found  = 0;
	
 	if (-r "$config_file") {
		open ( CONFIG, "<$config_file")|| die "erreur de lecture de $config_file: $!";
		do {
			$line = <CONFIG>;
			if (defined ($line) && $line =~ /^amaya_rep=/ ) {
				chomp ($line);
				($line,$name ) = split ( /=/, $line );
				if (-d "$home/$name"){
					if ( $found == -1) { $found = 2;}
					else {$found = 1}
				}
				else{
					$found = -1;
				}
			}			
		}
		while ( $line && $found <= 0);
		close (CONFIG);
		
		if ( $found == 0 ) {
			do {
				print "1Donnez le nom du repertoire ou vous avez place vos repertoire Amaya et libwww. Exemple : opera . $home/ est deja pris en compte\n" ;
				$name = <STDIN>;
				chomp ($name) ;
			}
			while ($name ne "" && !(-d "$home/$name" ) ) ;
			open ( CONFIG, ">>$config_file")|| die "erreur de lecture de $config_file: $!";
			print CONFIG "amaya_rep=$name\n";
			close (CONFIG);
		}
		elsif ($found == 1){ # no pb
		}
		else {# a modification is necessary	
			if (! (-d "$home/$name")) { 	
				do {
					print "Donnez le nom du repertoire ou vous avez place vos repertoire Amaya et libwww. Exemple : opera . $home/ est deja pris en compte\n" ;
					$name = <STDIN>;
					chomp $name ;
				}
				while ($name ne "" && !(-d "$home/$name" ) ) ;
			}
				open ( CONFIG, "<$config_file")|| die "erreur de lecture de $config_file: $!";			
				open ( NEW, ">$config_file.new") || die "erreur de modification de $config_file: $!"; 		
				while ($line = <CONFIG> ) {
					if ($line !~ /^amaya_rep=/ ) {
						print NEW $line;
					}
					else { #recopy only one time 
						if ($found <= 0 || $found == 2) {
							$found = 1;
							print NEW  "amaya_rep=$name\n";
						}	
					}
				}
				if ($found != 1) { #si pb
					print NEW  "amaya_rep=$name\n";
				}
				close (CONFIG);
				close (NEW);
				rename ( "$config_file.new", $config_file )  || 	
			 	die "can't rename $config_file.new to $config_file during modification of
				the configuration file because of: $! \nthe old file still exist, the
				new file name is $config_file\n";
				print "OK\n";
		}
	}
	else  { 
		print "fichier $config_file introuvable ou inexistant\n";
		do {
			print "3Donnez le nom du repertoire ou vous avez place vos repertoire Amaya et libwww. Exemple : opera . $home/ est deja pris en compte\n" ;
			$name = <STDIN>;
			chomp $name ;
		}
		while ($name ne "" && !(-d "$home/$name" ) ) ;
		open ( CONFIG, ">$config_file")|| die "erreur de lecture de $config_file: $!";
		print CONFIG "amaya_rep=$name\n";
		close (CONFIG);
	}
	return $name;
} 
#----------------------------------------------------------------------------
sub rep_obj { # to load configuration parameter "obj_rep"
	my $name = "notOK";
	my $line;	
	my $found  = 0;
	
 	if (-r "$config_file") {
		open ( CONFIG, "<$config_file")|| die "erreur de lecture de $config_file: $!";
		do {
			$line = <CONFIG>;
			if (defined ($line) && $line =~ /^obj_rep=/ ) {
				chomp ($line);
				($line,$name ) = split ( /=/, $line );
				if (-d "$home/$rep_amaya/Amaya/$name"){
					if ( $found == -1) { $found = 2;}
					else {$found = 1}
				}
				else{
					$found = -1;
				}
			}			
		}
		while ( $line && $found <= 0);
		close (CONFIG);
		
		if ( $found == 0 ) {
			do {
				print "Donnez le nom du repertoire objet d'Amaya . Exemple : LINUX_ELF . $home/ est deja pris en compte\n";
				$name = <STDIN>;
				chomp ($name) ;
			}
			while ($name ne "" && !(-d "$home/$rep_amaya/Amaya/$name" ) ) ;
			open ( CONFIG, ">>$config_file")|| die "erreur de lecture de $config_file: $!";
			print CONFIG "obj_rep=$name\n";
			close (CONFIG);
		}
		elsif ($found == 1){ # no pb
		}
		else {# a modification is necessary	
			if (! (-d "$home/$rep_amaya/Amaya/$name")) { 	
				do {
					print "Donnez le nom du repertoire objet d'Amaya . Exemple : LINUX_ELF . $home/ est deja pris en compte\n";
					$name = <STDIN>;
					chomp $name ;
				}
				while ($name ne "" && !(-d "$home/$rep_amaya/Amaya/$name" ) ) ;
			}
				open ( CONFIG, "<$config_file")|| die "erreur de lecture de $config_file: $!";			
				open ( NEW, ">$config_file.new") || die "erreur de modification de $config_file: $!"; 		
				while ($line = <CONFIG> ) {
					if ($line !~ /^obj_rep=/ ) {
						print NEW $line;
					}
					else { #recopy only one time 
						if ($found <= 0 || $found == 2) {
							$found = 1;
							print NEW  "obj_rep=$name\n";
						}	
					}
				}
				if ($found != 1) { #si pb
					print NEW  "obj_rep=$name\n";
				}
				close (CONFIG);
				close (NEW);
				rename ( "$config_file.new", $config_file )  || 	
			 	die "can't rename $config_file.new to $config_file during modification of
				the configuration file because of: $! \nthe old file still exist, the
				new file name is $config_file\n";
				print "OK\n";
		}
	}
	else  { 
		print "fichier $config_file introuvable ou inexistant\n";
		do {
			print "Donnez le nom du repertoire objet d'Amaya . Exemple : LINUX_ELF . $home/ est deja pris en compte\n" ;
			$name = <STDIN>;
			chomp $name ;
		}
		while ($name ne "" && !(-d "$home/$rep_amaya/Amaya/$name" ) ) ;
		open ( CONFIG, ">$config_file")|| die "erreur de lecture de $config_file: $!";
		print CONFIG "obj_rep=$name\n";
		close (CONFIG);
	}
	return $name;
} 
#----------------------------------------------------------------------------
