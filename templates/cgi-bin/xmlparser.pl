#!/usr/bin/perl
use XML::Parser;


my $p = new XML::Parser(ErrorContext => 2,
			Handlers     => {Start => \&start_handler,
					 End   => \&end_handler,
					 Char  => \&char_handler
					}
		       );

if ($lang eq "fr")
  {
    $xml_file = $xml_fr;
  }



$p->parsefile($xml_file);


################
## End of main
################


sub start_handler
  {
    
    my $xp = shift;
    my $el = shift;
    
    if (defined($el))
      {
	if ($el eq "template")
	  {
	    my $attrname = shift;
	    my $attrval  = shift;
	    if (defined ($attrname) && $attrname eq "file")
	      {
		push(@filetempl,$attrval);
	      }
	  }
	elsif ($el eq "stylesheet")
	  { 
	    my $attrname = shift;
	    my $attrval  = shift;
	    if (defined ($attrname) && $attrname eq "file")
	      {
		push(@filestyle,$attrval);
	      }
	  }
        elsif ($el eq "templates")
	  {
	    my $attrname = shift;
	    my $attrval  = shift;
	    while (defined ($attrname))
	      {
		$attrname = shift;
		$attrval  = shift;
	      }
	  }
	
      }
  }  # End start_handler

#######################

sub end_handler
{
 	
}

########################


sub char_handler
  {
    my ($xp, $text) = @_;
    
    if (length($text)) 
      {
	
	$text = $xp->xml_escape($text);
	if ($xp->in_element("name"))
	  {           
	    if  ($xp->within_element("template"))
	      {
		push(@nametempl,$text);
	      }
	    elsif($xp->within_element("stylesheet"))
	      {		    
		push(@namestyle,$text);
	      }
	    
	  }
	elsif ($xp->in_element("description"))
	  {     
	    if  ($xp->within_element("template"))
	      {
		if (!($text =~ /^\s*$/))
		  {
		    chomp ($text);
		    push(@desctempl,$text);
		  }
	      }
	    elsif($xp->within_element("stylesheet"))
	      {		     
		if (!($text =~ /^\s*$/))
		  {
		    chomp ($text);
		    push(@descstyle,$text);
		  }
	      } 
	  }
      }
  } 


# This test function displays the values of the 6 arrays
sub affiche_tabs                        
  {
    
    print ("\n*********CONTENU DES TABLEAUX\n"); 
    print ("****noms des templates :\n");
    foreach $b (@nametempl){
      print($b."\n");
    }
    print ("****fichiers templates :\n");   
    foreach $b (@filetempl){
      print($b."\n");
    }
    print ("****description des templates :\n"); 
    foreach $b (@desctempl){
      print($b."--\n");
    }
    print ("****noms des style :\n");             
    foreach $b (@namestyle){
      print($b."\n");
    }
    print ("****fichiers style :\n"); 
    foreach $b (@filestyle){
      print($b."\n");
    }
    print ("****description des style :\n");
    foreach $b (@descstyle){
      print($b."\n");
    }
}

