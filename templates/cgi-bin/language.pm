
$language = $ENV{"HTTP_ACCEPT_LANGUAGE"};
unless (defined ($language)) 
  {
    $language = "en";
  }

$fr_pos = index($language,"fr");
$en_pos = index($language,"en");


if ($en_pos == -1)
  {
    $en_pos = 2000;
  }

if ( ($fr_pos >= 0) && ($fr_pos < $en_pos) )
  {
    $lang = "fr";
  }
else
  {
    $lang = "en";
  }


if ($lang eq "fr")
  {
    $T_title = "Nouveau mod&egrave;le de page";
    $T_sub = "S&eacute;lectionnez un mod&egrave;le de page et une feuille de style";
    $T_temp = "Mod&egrave;les de pages";
    $T_css = "Feuilles de style";
    $T_no_css = "Sans feuille de style";
    if ($int_publish)
      {
	$T_url = "Entrez le nom ou l'URL de votre nouveau document";
	$T_howto = "Entrez le nom du document <em><span style=\"color: #0071FF\">nom_doc.html</span></em><span style=\"color:#C1EEFF\"> </span>si vous voulez que l'URL de publication soit calculee a partir de votre repertoire personnel ou entrez une URL complete comme <em><span style=\"color:#0071FF\">http://nom_serveur/.../nom_doc</span></em>";
      }
    else
      {
	$T_url = "Entrez l'URL de votre nouveau document";
	$T_howto = "Entrez une URL complete comme <em><span style=\"color:#0071FF\">http://nom_serveur/.../nom_doc</span></em>";
      } 
  }
else 
  {
    $T_title = "New document from template";
    $T_sub = "Select a template and a stylesheet";
    $T_temp = "Templates";
    $T_css = "Stylesheets";
    $T_no_css = "Whithout stylesheet";
    if ($int_publish)
      {
	$T_url = "Enter here the name or the URL of the new document";
	$T_howto = "Just enter the document\'s name <em><span style=\"color: #0071FF\">doc_name.html</span></em><span style=\"color:#C1EEFF\"> </span>if you want the publication\'s URL to be calculated from your personnal directory or enter a complete URL like <em><span style=\"color:#0071FF\">http://server_name/.../doc_name</span></em>";
      }
    else
      {
	$T_url = "Enter here the URL of the new document";
	$T_howto = "Enter a complete URL like <em><span style=\"color:#0071FF\">http://server_name/.../doc_name</span></em>";
      }
    
  }


