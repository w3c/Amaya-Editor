#!/usr/bin/perl -w
package Iso;

use strict;


BEGIN {
	use vars qw( @ISA @EXPORT );
	use Exporter;

	@ISA = ('Exporter');# AutoLoader;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
	@EXPORT = 	qw(
						&return_code_in_ISO_639
						&belongs_to_list
						);
}



###########################################################################
#-----------THe  ISO-639 table (up to date, see the http://www.iana.org)--------------

my %iso_639 = qw (
aa Afar
ab Abkhazian
af Afrikaans
am Amharic
ar Arabic
as Assamese
ay Aymara
az Azerbaijani
ba Bashkir
be Byelorussian
bg Bulgarian
bh Bihari
bi Bislama
bn Bengali/Bangla
bo Tibetan
br Breton
ca Catalan
cn Chinese_simplified
co Corsican
cs Czech
cy Welsh
da Danish
de German
dz Bhutani
el Greek
en English
eo Esperanto
es Spanish
et Estonian
eu Basque
fa Persian
fi Finnish
fj Fiji
fo Faeroese
fr French
fy Frisian
ga Irish
gd Scots/Gaelic
gl Galician
gn Guarani
gu Gujarati
ha Hausa
hi Hindi
hr Croatian
hu Hungarian
hy Armenian
ia Interlingua
ie Interlingue
ik Inupiak
in Indonesian
is Icelandic
it Italian
iw Hebrew
ja Japanese
ji Yiddish
jw Javanese
ka Georgian
kk Kazakh
kl Greenlandic
km Cambodian
kn Kannada
ko Korean
ks Kashmiri
ku Kurdish
ky Kirghiz
la Latin
ln Lingala
lo Laothian
lt Lithuanian
lv Latvian/Lettish
mg Malagasy
mi Maori
mk Macedonian
ml Malayalam
mn Mongolian
mo Moldavian
mr Marathi
ms Malay
mt Maltese
my Burmese
na Nauru
ne Nepali
nl Dutch
no Norwegian
oc Occitan
om (Afan)/Oromo
or Oriya
pa Punjabi
pl Polish
ps Pashto/Pushto
pt Portuguese
qu Quechua
rm Rhaeto-Romance
rn Kirundi
ro Romanian
ru Russian
rw Kinyarwanda
sa Sanskrit
sd Sindhi
sg Sangro
sh Serbo-Croatian
si Singhalese
sk Slovak
sl Slovenian
sm Samoan
sn Shona
so Somali
sq Albanian
sr Serbian
ss Siswati
st Sesotho
su Sundanese
sv Swedish
sw Swahili
ta Tamil
te Tegulu
tg Tajik
th Thai
ti Tigrinya
tk Turkmen
tl Tagalog
tn Setswana
to Tonga
tr Turkish
ts Tsonga
tt Tatar
tw Chinese_Taiwan
uk Ukrainian
ur Urdu
uz Uzbek
vi Vietnamese
vo Volapuk
wo Wolof
xh Xhosa
yo Yoruba
zh Chinese
zu Zulu
);

##############################################################################
#									MAIN
#
#
############################################################################
sub return_code_in_ISO_639 {
	my $lang;
	my $redo = 0;
	do {	
		print "\tWhat language do you want to treat?\n"
				."\t(answer in two letters like the standard ISO-639 i.e.: en or fr)\n"
				."\tYour answer? (if you want a listing type 0 ):\t";
			$lang = <STDIN>;
			chomp $lang;
			if ($lang eq "") {
				$redo = 1;
			}
			else {
				if ($lang eq "0" ) {
					print "This is the semantic of the standard ISO-639 \n";
					foreach (keys (%iso_639)) {
						print "\tFor  " . $iso_639 {$_} ."\ttype\t$_\n";
					}
					print "\n";
					$redo = 1;
				}
				else {
					if ( belongs_to_list ($lang) == 1) {
						$redo = 0;
					}
					else {
						$redo = 1; 
					}
				}
			}			
	} while ($redo == 1);
	
	return $lang;
}
#----------------------------------------------------------------------------
sub  belongs_to_list { ## return true if its a true iso639 code in two letters
	my $lang = shift;
	
	if (defined ( $iso_639 {$lang})) {
		return 1;
	}
	else {
		return 0;
	}
}
#_----------------------------------------------------------------------------



1;
__END__

this to treat the ISO-639 for the names of the dialogue files for Amaya
