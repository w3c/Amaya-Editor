#!/usr/bin/perl -w
use templatesconfig;
use CGI qw(param);

# activate for debugging
my $debug = 0;
	       
# SENDTEMP.PL
# This script add a link to the chosen stylesheet and a META field containing 
# the publication's URL of the new file to the chosen template


# Get the form's entries
my $domain_name = $ENV{"SERVER_NAME"};
my $temp_file = param("templ");	
my $css_file = param("css");
my $new_file = param("newfile");

if ($debug) {
  $domain_name = "w3.org";
  $temp_file = "/tmp/titi.html";
  $css_file = "/tmp/titi.css";
  $new_file = "/tmp/newfile";
}

# Construction of templates's and CSS's URLs 
$URL_template="$temp_dir/$temp_file";
$URL_css="http://$domain_name$css_dir/$css_file";

if (!defined $new_file || $new_file eq "")
  {
    $new_file = "http://change.this.url/filename.html";
  }

# Compute the URL of the new file
if ( (!$int_publish) || ( ($int_publish) && (index($new_file,"http://") != -1) ) )
  {
    # the new file's name entry of the form is a complete URL
    $URLnewfile = $new_file;
  }
else
  {
    # the entry is just an HTML filename 
    # Calculation of the user's publication directory
    userdir();
    $URLnewfile ="http://".$domain_name.$user_dir.$new_file;
  }


# Insertion of the META field 
insertmeta();

################
## End of main
################


#################################### SUBROUTINES #########################################

sub userdir
  {
    # Get the user's login
    $user = $ENV{"REMOTE_USER"};
    
    # open the htuserdir file. This file make the correspondence between the a login
    # and a publication directory
    
    open (USERDIR,$userdir_file) || file_htuserdir_not_found();
    
    $user_found = 0;
    while ((!$user_found) && ($ligne = <USERDIR>))
      {
	chomp ($ligne);	   
	if ( index( $ligne , $user ) == -1) {$user_found = 0;}
	else {$user_found = 1;}
	
      }
    
    if (!user_found)
      {
	user_not_found();
      }
    close(USERDIR);
    $user_dir = substr ($ligne,length($user)+1,100);
  }


# Sending the template document with the META field and the CSS's link
sub insertmeta
  {
    
    open (TEMPLATE,$URL_template) || file_template_not_found();  
    $position = -1;
    print("Content-type: text/html\n\n");
    while ($string = <TEMPLATE>) 
      {
	# Seek for </head> 
	$position = index($string,"</head>");       
	if ($position == 0)
	  {						
	    # if <head> found at the beginning of a line -> Insertion du META
	    print("<META name=\"AMAYA_TEMPLATE\" content=$URLnewfile>\n");	
	    if ($css_file ne "none")
	      {
		print("<link href=\"$URL_css\" rel=\"stylesheet\" type=\"text/css\">\n");
	      }
	    print($string);
	  }
	else
	  {	
            # send the line		      	
	    print($string);
	  }	
      }
    close(TEMPLATE);
  }


############################# ERROR MANAGEMENT ###########################


# Failed to open the file htuserdir 
sub file_htuserdir_not_found
  {
    print <<END_of_Multiline_Text;
Content-type: text/html


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
   "http://www.w3.org/TR/REC-html40/loose.dtd">
<html>
<head>
<title>Error</title>
</head>
<body>
<h1 style="background-color: #C8FF95">Error</h1>
<p></p>
<h1>Failed to find the file htuserdir</h1>
<p></p>
<p></p>
</body>
</html>
END_of_Multiline_Text

    die;
  }

# Failed to establish the correspondance between the user and the publication directory
sub user_not_found
  {
    print <<END_of_Multiline_Text;
Content-type: text/html


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
   "http://www.w3.org/TR/REC-html40/loose.dtd">
<html>
<head>
<title>Error</title>
</head>
<body>
<h1 style="background-color: #C8FF95">Error</h1>
<p></p>
<h1>Your login has NOT been assigned a publication directory</h1>
<p></p>
<h2>Please contact your server administrator</h2>
<p></p>
<p></p>
</body>
</html>
END_of_Multiline_Text
die;
  }


# Failed to open the chosen template
sub file_template_not_found
  {
    print <<END_of_Multiline_Text;
Content-type: text/html

<html>
<head>
<title>Error</title>
</head>
<body>
<h1 style="background-color: #C8FF95">Error</h1>
<p></p>
<h1>The chosen template $URL_template wasn\'t found</h1>

END_of_Multiline_Text

    print("</body>");
    print("</html>");
    die;
  }
  

