#!/usr/bin/perl -w

use strict ;
 

#############################for parmameters###########################################
# some environement variables for portability  
my $home = $ENV{"HOME"} . "/";
my $config_file = "$home.amaya/am_dialogues.conf.xml";
my $rep_amaya = ""; # way between $home and the Repertory Amaya and libwww
my $rep_obj = ""; #name of the object direcitriy for Amaya
	( $rep_amaya,$rep_obj) = load_parameters($home, $config_file);
	

#### 	for all the bases
# directory for bases 
my $BASE_directory = "$home$rep_amaya/Amaya/tools/xmldialogues/bases/";
# directory  translated NEW  files
my $OUT_MSG_directory = "$home$rep_amaya/Amaya/config/";

#	sufix for the generated file created into /docs to help translation
my $specific_sufix = ".amaya.trans"; #used to indicate those specific files
# commentary for begining of the ".h" file
my $comment_for_begining_of_h_file  = "/*that is the real begin of labels used*/";


############# TABLES key/value
my %base_name ;	# table for the name of the bases 
	
my %head_dir ; 	# table for the name of the directories where ".h" files are
my %head_name ;	# table for the name of the ".h" files

my %lang_dir ;		# table for the name of the directories where translated texts are
my %lang_sufix ;	# table for the sufix name of the translated texts



### for messages of the interface 
	my %types = ();
	$types {1} = "Amaya dialogues";	
	$types {2} = "Amaya general messages";	
	$types {3} = "Thot library dialogues";	
	$types {4} = "Spell checker dialogues";
#######and for the dynamic parameters
my %index =qw ( 	1	dia
						2	msg
						3	lib
						4	corrd);

# to store the particulary labels that ends the label files used by Amaya
my %ending_label = qw (	dia MAX_EDITOR_LABEL
								msg AMAYA_MSG_MAX
								lib TMSG_LIB_MSG_MAX
								corrd MSG_MAX_CHECK);

#### 	for Amaya dialogue => dia or $index {1}
  $head_dir{'dia'} = "$home$rep_amaya/Amaya/$rep_obj/amaya/";# idem $head_dir{$index{"1"}} = ...
  $head_name {'dia'}= 'EDITOR.h';
  $lang_dir{'dia'} = $OUT_MSG_directory;
  $lang_sufix {'dia'} = '-amayadialogue';
  $base_name {'dia'} = 'base_am_dia.xml';

####	for Amayamsg => msg or $index {2}
 $head_dir {'msg'} = "$home$rep_amaya/Amaya/amaya/";
 $head_name {'msg'} = 'amayamsg.h' ;
 $lang_dir {'msg'} = $OUT_MSG_directory;
 $lang_sufix {'msg'} = '-amayamsg' ;
 $base_name {'msg'} = 'base_am_msg.xml';

####	for libdialogue => lib or $index {3}
 $head_dir {'lib'} = "$home$rep_amaya/Amaya/thotlib/include/" ;
 $head_name {'lib'} = 'libmsg.h' ;
 $lang_dir {'lib'} = $OUT_MSG_directory;
 $lang_sufix {'lib'} = '-libdialogue' ;
 $base_name {'lib'} = 'base_am_lib.xml';


####	for corrdialogue => corrd or $index {4}
 $head_dir {'corrd'} = "$home$rep_amaya/Amaya/thotlib/internals/h/" ;
 $head_name {'corrd'} = 'corrmsg.h' ;
 $lang_dir {'corrd'} = $OUT_MSG_directory;
 $lang_sufix {'corrd'} = '-corrdialogue' ;
 $base_name {'corrd'} = 'base_am_corrd.xml';
 


################################################################################
################################################################################
#################################MAIN###########################################
{
#load the different modules needed
use Initialisation qw(	&create_base );
use Import_am_msg qw( 	&import_a_language
								$in_labelfile
								$basefile
								$in_textdirectory
								$in_textsufix
								$encodage );
use Export_am_msg qw ( 	&export );
use Dial_tool_box qw ( 	&add_label
								&delete_label );
use Forcer 			qw ( 	&forcer );

use Configfile qw ( &load_parameters );

#launch		

	menu () ;

}
#############################END MAIN###########################################
sub menu {
	my @list = ( 	"Quit",																	#0
						"Amaya dialogues (menus with xx-amayadialogue files)",	#1	
						"Amaya general messages ( with xx-amayamsg files)",		#2
						"Thot library dialogues ( with xx-libdialogue files)",	#3
						"Spell checker dialogues ( with xx-corrdialogue files)");#4
	my $count = 0;
	my $choice = 0;

	`clear`; # ne veux pas marcher
	print "=======================================================================\n",
			"\t\tAmaya Dialogues Manager\n";
	do {
		do {
			print "\nWhat dialogue type would you like to process?\n";
			$count = 0;
			foreach (@list) {
				print "\t" . $count++ . "=>\t$_\n";
			}
			print "Our choice [0]:\t";
			$choice = <STDIN> ;
			chomp ($choice);
			if ($choice eq "") {
				$choice = 0;
			}
		}
		while ( $choice eq "" ||$choice =~ /^\D/ || $choice < 0 || $choice >= $count ) ;
		
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
	my $of_what = $index{ $last_choice}; 
	my @list = ( 
		"Go to the previous menu", 					#0
		"Init the XML base" ,							#1
		"Add/Update a language", 						#2
		"Export all dialogues files",					#3
		"Add a label",										#4
		"Delete a label",									#5						
		"Force the base into conformity with a label file(\".h\")"#6 
		);
	my $lang = "";
	my @command = "";

#show the menu
do { # to continue to treat the same type of dialogue 			
	do {
		print "\nWhat kind of thing would you proceed on ",$types{$last_choice}, "?\n";
		$count = 0;
		foreach (@list) {
			print "\t" . $count++ . "=>\t$_\n";
		}
		print "Our choice [0]:\t";
		$choice = <STDIN> ;
		chomp ($choice);
		if ($choice eq "") {
			$choice = 0;
		} 
	}
	while ( $choice eq "" || $choice =~ /^\D/ || $choice < 0 || $choice >= $count ) ;
	print "\n";

##|||||||||||||||||||||||||||||||| treats the answer\\\\\\\\\\\\\\\\\\\\\\\\\\\
	if ($choice == 1) { #Init the XML base
		$_ = verify ();
		if ( /^y/i || /^yes/i ) {
			Initialisation::create_base ( $head_dir{ $of_what }, 
													$head_name{ $of_what },
													$BASE_directory, 
													$base_name { $of_what },
													$comment_for_begining_of_h_file);
			# to initialise with english
			print "\n\tNow,fill the base with english by default\n\n";
			$Import_am_msg::in_labelfile = $head_dir{ $index{ $last_choice}} . $head_name{ $index{ $last_choice}};
			$Import_am_msg::basefile = $BASE_directory . $base_name { $index{ $last_choice}};
			$Import_am_msg::in_textdirectory = $lang_dir { $index{ $last_choice}};
			$Import_am_msg::in_textsufix = $lang_sufix { $index{ $last_choice}};
			$Import_am_msg::encodage = "latin1";
			Import_am_msg::import_a_language ("en", $ending_label{ $index{ $last_choice}}) ;
		}

		$choice = -1; #to avoid problem		
	}
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	elsif ($choice == 2) { #Adding/Updating a language	
		print "\tWhat language do you want to treat? (in two letters like the standard ISO-638-1988 i.e.: en or fr)\n";
		$lang = <STDIN>;
		chomp $lang;
		do {
		 	print "\tAre files in the normal repertory for Amaya (0)or (for \"new\")the \"IN\" repertory (1)?\n",
					"\tOur choice [0] : ";
			$choice = <STDIN>;
			chomp ($choice ) ;
			if ( $choice eq "") { 
				$choice = 0;
			}
		} while ($choice != /^[0-1]/);
		$_ = $choice;
		if (/0/)
			{	
		$Import_am_msg::in_labelfile = $head_dir{ $of_what} . $head_name{ $of_what};
		$Import_am_msg::basefile = $BASE_directory  . $base_name { $of_what};
		$Import_am_msg::in_textdirectory = $lang_dir { $of_what};
		$Import_am_msg::in_textsufix = $lang_sufix { $of_what};
			}
		else #if (/1/)
			{
		$Import_am_msg::in_labelfile = "$home$rep_amaya/Amaya/tools/xmldialogues/in/" . $head_name{ $of_what};
		$Import_am_msg::basefile = $BASE_directory  . $base_name { $of_what};
		$Import_am_msg::in_textdirectory = "$home$rep_amaya/Amaya/tools/xmldialogues/in/";
		$Import_am_msg::in_textsufix = $lang_sufix { $of_what};				
			}
		Import_am_msg::import_a_language ($lang, $ending_label{ $of_what} ) ;		
		$choice = -1; #to avoid problem		
	}	
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	elsif ($choice == 3) { # Export all dialogues files
		Export_am_msg::export (	$BASE_directory . $base_name{ $of_what},
										$OUT_MSG_directory,
										$lang_sufix { $of_what},
										$head_dir{ $of_what} . $head_name{ $of_what},
										$ending_label { $of_what}
										);
		$choice = -1; #to avoid problem		
	}
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	elsif ($choice == 4) { # Add a label
		Dial_tool_box::add_label ( $BASE_directory,
											$base_name{ $of_what} 
											);						
		$choice = -1; #to avoid problem		
	}
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	elsif ($choice == 5) { # Delete a label
		Dial_tool_box::delete_label ( $BASE_directory,
												$base_name{ $of_what} 
												);
		$choice = -1; #to avoid problem		
	}
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	elsif ($choice == 6) { # n'arrive que si last_choice = 1
		#en principe ne reprend que EDITOR.h		
		$_ = verify ();
		if ( /^y/i || /^yes/i ) {
			Forcer::forcer ( 	$BASE_directory,
									$base_name{ $of_what},
									$head_dir{ $of_what},
									$head_name{ $of_what},
									$comment_for_begining_of_h_file
								);	
		}
		$choice = -1; #to avoid problem		
	}	
} while ( $choice != 0 ); 

#to do the update automaticaly
		Export_am_msg::export (	$BASE_directory . $base_name{ $of_what},
										$OUT_MSG_directory,
										$lang_sufix { $of_what},
										$head_dir{ $of_what} . $head_name{ $of_what},
										$ending_label { $of_what}
										);

}

#-------------------------------------------------------------------------------
#################################################################################
######################### for configuration ####################################
#################################################################################
sub verify {
	do {
			print "\tAre you certain to want to erase the old base (Yes ,No )?\n";
			print " \tOur choice [n]:\t";
			$_ = <STDIN>;
			chomp;
			if ($_ eq "") {
			$_ = "n";
		} 
		}
		while ($_ !~ /^y/i
				&& $_ !~ /^n/i					
				&& $_ !~ /^yes/i					
				&& $_ !~ /^no/i					
		);
		return $_ ;
}

#-------------------------------------------------------------------------------
 
#----------------------------------------------------------------------------
