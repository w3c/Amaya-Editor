#!/usr/bin/perl -w

use strict ;


#############################for parmameters###########################################
# some environement variables for portability  
	if (!defined ($ENV{"HOME"}) ) {
		die "The variable \"home\" cannot be loaded because you certainly not using Linux or
		Unix\n";
	}
my $home = $ENV{"HOME"} . "/";


my $config_file = "$home.amaya/am_dialogues.conf.xml";

my $path_amaya = ""; # way between $home and the Repertory Amaya and libwww
my $dir_obj = ""; #name of the object direcitriy for Amaya
my $dir_perso = ""; #name of the repertory used by users to store new files translated
	( $path_amaya, $dir_obj, $dir_perso) = Configfile::load_parameters($home, $config_file);


#### 	for all the bases
# directory for bases 
my $BASE_directory = "$path_amaya/Amaya/tools/xmldialogues/bases/";
# directory  translated NEW  files
my $OUT_MSG_directory = "$path_amaya/Amaya/config/";

#where redy to translate file are put
my $directory_for_file_to_translate = "$path_amaya/Amaya/tools/xmldialogues/docs/";

#	and the sufix for the generated file created into /docs to help translation
#Warning : no more used
my $specific_sufix = ".amaya.trans"; #used to indicate those specific files

# commentary for begining of the ".h" file
my $comment_for_begining_of_h_file  = "/* Beginning of labels */";


############# TABLES key/value
my %base_name ;	# table for the name of the bases 
	
my %head_dir ; 	# table for the name of the directories where ".h" files are
my %head_name ;	# table for the name of the ".h" files

my %lang_dir ;		# table for the name of the directories where translated texts are
my %lang_sufix ;	# table for the sufix name of the translated texts

# used because some directive of compilations are needed(not used for Amaya dialogue)
my %for_h_file_compilation_begin = ();
my %for_h_file_compilation_end = ();


### for messages of the interface 
	my %types = ();
	$types {1} = "Amaya dialogues";	
	$types {2} = "Amaya general messages";	
	$types {3} = "Thot library dialogues";	
#######and for the dynamic parameters
my %index =qw ( 	1	dia
			2	msg
			3	lib);

# to store the particulary labels that ends the label files used by Amaya
my %ending_label = qw (	dia MAX_EDITOR_LABEL
			msg AMAYA_MSG_MAX
			lib TMSG_LIB_MSG_MAX);

#### 	for Amaya dialogue => dia or $index {1}
  $head_dir{'dia'} = "$path_amaya/Amaya/$dir_obj/amaya/";# idem $head_dir{$index{"1"}} = ...
  $head_name {'dia'}= 'EDITOR.h';
  $lang_dir{'dia'} = $OUT_MSG_directory;
  $lang_sufix {'dia'} = '-amayadialogue';
  $base_name {'dia'} = 'base_am_dia.xml';
  $for_h_file_compilation_begin {'msg'} = "";#never used
  $for_h_file_compilation_end {'msg'} = "";#never used

####	for Amayamsg => msg or $index {2}
 $head_dir {'msg'} = "$path_amaya/Amaya/amaya/";
 $head_name {'msg'} = 'amayamsg.h' ;
 $lang_dir {'msg'} = $OUT_MSG_directory;
 $lang_sufix {'msg'} = '-amayamsg' ;
 $base_name {'msg'} = 'base_am_msg.xml';
 $for_h_file_compilation_begin {'msg'} = "";
 $for_h_file_compilation_end {'msg'} = "";

####	for libdialogue => lib or $index {3}
 $head_dir {'lib'} = "$path_amaya/Amaya/thotlib/include/" ;
 $head_name {'lib'} = 'libmsg.h' ;
 $lang_dir {'lib'} = $OUT_MSG_directory;
 $lang_sufix {'lib'} = '-libdialogue' ;
 $base_name {'lib'} = 'base_am_lib.xml';
 $for_h_file_compilation_begin {'lib'} = "#ifndef LIB_MSG_H\n#define LIB_MSG_H\n";
 $for_h_file_compilation_end {'lib'} = "\n#endif\n";


################################################################################
################################################################################
#################################MAIN###########################################
{
#									load the different modules needed
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
use Iso qw ( &return_code_in_ISO_639);
use Edition qw( &product_translate );

######################launch		

	menu () ;

}
#############################END MAIN###########################################

sub menu {
	my @list = ( 	"Quit",							#0
			"Amaya dialogues (menus with xx-amayadialogue files)",	#1	
			"Amaya general messages ( with xx-amayamsg files)",	#2
			"Thot library dialogues ( with xx-libdialogue files)",	#3
			"Or product the preformated file for translating"	#4
					);
	my $count = 0;
	my $choice = 0;

	#`clear`; # ne veux pas marcher
	print "=======================================================================\n",
			"\t\tAmaya Dialogues Manager\n";
	do {
		do {
			print "\n\tWhat dialogue type would you like to process?\n";
			$count = 0;
			foreach (@list) {
				print "\t  " . $count++ . "=>\t$_\n";
			}
			print "\tOur choice [0]:\t";
			$choice = <STDIN> ;
			chomp ($choice);
			if ($choice eq "") {
				$choice = 0;
			}
		}
		while ( $choice eq "" ||$choice =~ /^\D/ || $choice < 0 || $choice >= $count ) ;
		
		if ( $choice == 1 || $choice == 2 || $choice == 3) {
			menu1 ( $choice );
		}
		elsif ($choice == 4) {
			my $lang = Iso::return_code_in_ISO_639 ();
			foreach (values (%index)) {
				Edition::product_translate (
					$BASE_directory . $base_name {$_},
					$head_name {$_} ,
					$directory_for_file_to_translate,
					$lang_sufix {$_},
					$comment_for_begining_of_h_file,
					$lang
				);
			}
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
		"Go to the previous menu (and Export the current result)",  #0
		"Init the XML base" ,					    #1
		"Add/Update a language", 				    #2
		"Export all dialogues files",				    #3
		);
	my $lang = "";
	my @command = "";

#show the menu
do { # to continue to treat the same type of dialogue 			
	do {
		print "\n\tWhat kind of thing would you proceed on ",$types{$last_choice}, "?\n";
		$count = 0;
		foreach (@list) {
			print "\t  " . $count++ . "=>\t$_\n";
		}
		print "\tOur choice [0]:\t";
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
		$_ = verify ( $BASE_directory . $base_name { $of_what });
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
			Import_am_msg::import_a_language ("en", $ending_label{ $index{ $last_choice}},$comment_for_begining_of_h_file) ;
		}

		$choice = -1; #to avoid problem		
	}
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	elsif ($choice == 2) { #Adding/Updating a language	
		$lang = Iso::return_code_in_ISO_639 () ;
		
		do {
		 	print "\tWhere files of messages are ?:\n"
					. "\t0)In the normal repertory for Amaya (config) or \n"
					. "\t1)In the $path_amaya/Amaya/tools/xmldialogues/in directory\n"
					. "\t2)In your own directory ($dir_perso)\n"
					. "\tOur choice [0] : ";
			$choice = 0;		
			$choice = <STDIN>;
			chomp ($choice ) ;
			if ( $choice eq "") { 
				$choice = 0;
			}
		} while ($choice !~ /^[0-1]/);
		$_ = $choice;
		if (/0/)
			{	
		$Import_am_msg::in_labelfile = $head_dir{ $of_what} . $head_name{ $of_what};
		$Import_am_msg::in_textdirectory = $lang_dir { $of_what};
			}
		elsif (/1/)
			{
		$Import_am_msg::in_labelfile = "$path_amaya/Amaya/tools/xmldialogues/in/" . $head_name{ $of_what};
		$Import_am_msg::in_textdirectory = "$path_amaya/Amaya/tools/xmldialogues/in/";
			}
		else 			
			{
		$Import_am_msg::in_labelfile = "$dir_perso/" . $head_name{ $of_what};
		$Import_am_msg::in_textdirectory = "$dir_perso/";
			}

		#always	
		$Import_am_msg::basefile = $BASE_directory  . $base_name { $of_what};	
		$Import_am_msg::in_textsufix = $lang_sufix { $of_what};	
			
		Import_am_msg::import_a_language ($lang, $ending_label{ $of_what},$comment_for_begining_of_h_file ) ;		
		$choice = -1; #to avoid problem		
	}	
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	elsif ($choice == 3) { # Export all dialogues files
		Export_am_msg::export (	$BASE_directory . $base_name{ $of_what},
					$OUT_MSG_directory,
					$lang_sufix { $of_what},
					$head_dir{ $of_what} . $head_name{ $of_what},
					$ending_label { $of_what},
					$comment_for_begining_of_h_file,
					$for_h_file_compilation_begin {  $of_what},
					$for_h_file_compilation_end { $of_what}
					);
		$choice = -1; #to avoid problem		
	}
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	$_ = $choice;
} while ( $choice != 0 ); 

#to do the update automaticaly
	if ($_ != 3) {
		Export_am_msg::export (	$BASE_directory . $base_name { $of_what},
					$OUT_MSG_directory,
					$lang_sufix {$of_what},
					$head_dir {$of_what} . $head_name { $of_what},
					$ending_label {$of_what},
					$comment_for_begining_of_h_file,
					$for_h_file_compilation_begin { $of_what},
					$for_h_file_compilation_end { $of_what}
					);
	}
}

#-------------------------------------------------------------------------------
#################################################################################
######################### for configuration ####################################
#################################################################################
sub verify {
	do {
	    print "\tAre you certain to want to erase the old base " . $_[0] . " ?\n";
	    print " \tOur choice (Yes ,No )[n]:\t";
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

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------


1;
__END__
this a product by Emmanuel huck done for the W3C during a training period in april-june 2000
