#!/usr/bin/perl
use templatesconfig;
use language;

#  CREATEFORM.PL
#  This script uses the xmlparser module
#  It creates a form displayed by Amaya


# Call of the XMLPARSER module
# This module creates 6 arrays containing data for templates and stylesheets
#-----------------------------------------------------------------------------
require("xmlparser.pl");

$domain_name = $ENV{"SERVER_NAME"}; 

# Display of the first part of HTML code
print_debut_html();	       
            
# Creation of middle part of HTML code (Radio boxes, fields) 
print_radio_boxes();	    

# Display of the last part of HTML code
print_fin_html();      	



################
## End of main
################


#################################### SUBROUTINES ############################

sub print_debut_html
  {    
$URL_css_form = "http://".$domain_name.$css_form;
print <<END_of_Multiline_Text;
Content-type: text/html

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
   "http://www.w3.org/TR/REC-html40/loose.dtd">
<html>
 <head>
  <title>$T_title</title>
  <link href=$URL_css_form rel=\"STYLESHEET\" type=\"text/css\">
 </head>
<body>
 <h1>
  <img src=$amaya_logo align="middle" alt="amaya">
  &nbsp;$T_title&nbsp;
  <img src=$amaya_logo align="middle" alt="amaya">
 </h1>
 <hr>
 <h3>$T_sub</h3>
 <P></P>
END_of_Multiline_Text

print(" <form method=\"get\" action=\"http://".$domain_name."/cgi-bin/sendtemp.pl\">");

print <<END_of_Multiline_Text;
<table width="100%">
<tbody>
 <tr>
  <th>
   $T_temp
  </th>
  <th>
   $T_css
  </th>
 </tr>
 <tr>
<td>

END_of_Multiline_Text
}


sub print_radio_boxes
  {
    
    print("<div style=\"margin-left: 20pt\">");
    print("<dl>");
    
    # loop for templates
    for ($i = 0; $i < @desctempl; $i++)
      {

	# Addition of a radio box for each template
        print("<dt><input type=\"radio\" name=\"templ\" value=\"$filetempl[$i]\"");
	# Display of the templates's name and description
        print (">&nbsp;$nametempl[$i]</dt>\n");
        print ("<dd>$desctempl[$i]<p></p></dd>\n");
      }
    print("</dl>");
    print("</div>");
    print("</td>");

    print("<td>");
    print("<div style=\"margin-left: 20pt\">");
    print("<dl>");    
    # loop for stylesheets    
    for ($i = 0; $i < @descstyle; $i++)
      {
	# Addition of a radio box for each CSS
        print("<dt><input type=\"radio\" name=\"css\" value=\"$filestyle[$i]\"");
	# Display of the CSS's name and description
	print(">&nbsp;$namestyle[$i]</dt>\n");
        print("<dd>$descstyle[$i]<p></p></dd>\n");
      }
   
    # Addition of the radio box "Whithout stylesheet notched by default    
    print("<dt><input type=\"radio\" name=\"css\" value=\"none\" checked><pre> $T_no_css</pre></dt>\n"); 

    print("</dl>");
    print("</div>");
    print("</td>");
    print("</tr>");
}


# ################################fin html#############################

sub print_fin_html
  {  
    print <<END_of_Multiline_Text;
    </tbody>
   </table>
   <h3>$T_url</h3>
   <p style="font-size: 10pt">$T_howto</p>


   <p>
    <input type="text" size="80" name="newfile">
   </p>
   <p style="text-align: right">
    <input type="reset" value="Reset" name="Reset">
    <input type="submit" value="OK">
   </p>
  </form>
 </body>
</html>

END_of_Multiline_Text
  }
