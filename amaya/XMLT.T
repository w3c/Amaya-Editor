
TRANSLATION XML;

LINELENGTH 80;

CONST

   SAN = ' ';        { Start of attribute name }
   DoubleQuote = '"';
   CloseTagNL = '>\12';
   CloseEmptyTagNL = '/>\12';

RULES

   XML:
        BEGIN
        Use HTMLTX for HTML;
        Use MathMLT for MathML;
        Use XLinkT for XLink;
        Use SVGT for SVG;
        Use TemplateT for Template;
        Create Attributes;
        Create '>\12';
        indent +2;
        indent -2 after;
        Create '</>\12' After;
        END;

   xmlcomment:
        BEGIN
        NoLineBreak;
        Create '<!--';
        {NoTranslation;}
        Create '-->\12' after;
        END;

   xmlcomment_line:
        if not last create '\12' after;

   xmlpi:
        BEGIN
        NoLineBreak;
        Create '<?';
        {NoTranslation;}
        Create '?>\12' after;
        END;

   xmlpi_line:
        if not last create '\12' after;

   TEXT_UNIT:
        create '\12' after;

   GRAPHICS_UNIT:
        Remove;

   PICTURE_UNIT:
        Remove;

ATTRIBUTES
 
   xml_space = xml_space_default:
        BEGIN
        create SAN;
        create 'xml:space="default"';
        END;
   xml_space = xml_space_preserve:
        BEGIN
        create SAN;
        create 'xml:space="preserve"';
        END;

END
