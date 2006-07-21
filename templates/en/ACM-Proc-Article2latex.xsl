<?xml version='1.0' encoding="iso-8859-1"?>

<!-- This transformation generates the LaTeX code for an article to be
     published in the Proceedings of an ACM conference, following the
     LaTeX style sig-alternate.cls

     The source should be a XHTML document following the XTiger template
     ACM-Proc-Article.xsd
-->
     
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
  version="1.0">

  <xsl:output method="text" encoding="iso-8859-1" />

  <xsl:strip-space elements="xhtml:html" />

  <xsl:preserve-space elements="xhtml:pre" />

  <xsl:template match="xhtml:html">
    <xsl:text>\documentclass{sig-alternate}
\usepackage[latin1]{inputenc}

\begin{document}</xsl:text>
    <xsl:apply-templates select="xhtml:body"/>
    <xsl:text>
\bibliographystyle{abbrv}
\bibliography{DocEng2006} %% prepare a BibTeX file named DocEng2006.bib

\end{document}
    </xsl:text>
  </xsl:template>

  <!-- remove the head element -->
  <xsl:template match="xhtml:head">
  </xsl:template>

  <xsl:template match="xhtml:p[@class='noprint']">
  </xsl:template>

  <xsl:template match="xhtml:div[@class='noprint']">
  </xsl:template>

  <!-- replace character underscore by dash to avoid troubles with LaTeX -->
  <xsl:template match="text()">
    <xsl:value-of select="translate(., '_', '-')"/>
  </xsl:template>

  <!-- remove the bibliography: use BibTeX instead -->
  <xsl:template match="xhtml:div[@class='bibliography']">
  </xsl:template>

  <xsl:template match="xhtml:p[@class='conferenceinfo']/text()">
  </xsl:template>

  <xsl:template match="xhtml:span[@class='conferencename']">
    <xsl:text>\conferenceinfo{</xsl:text>
      <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='conferencedateandloc']">
    <xsl:text> {</xsl:text>
      <xsl:apply-templates/>
    <xsl:text>}
</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='copyrightyear']">
    <xsl:text>\CopyrightYear{</xsl:text>
      <xsl:apply-templates/>
    <xsl:text>}
</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='crdata']">
    <xsl:text>\crdata{</xsl:text>
      <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:h1">
    <xsl:text>\title{</xsl:text>
      <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='authors']">
    <xsl:text>\numberofauthors{</xsl:text><xsl:value-of select="count(xhtml:p[@class='vcard'])"/><xsl:text>}
\author{</xsl:text>
      <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <!-- remove the new lines between paragraphs representing authors -->
  <xsl:template match="xhtml:div[@class='authors']/text()">
  </xsl:template>

  <!-- remove photographs of authors -->
  <xsl:template match="xhtml:p[@class='vcard']/xhtml:img[@class='photo']">
  </xsl:template>

  <xsl:template match="xhtml:span[@class='fn']">
    <xsl:text>\alignauthor </xsl:text>
      <xsl:apply-templates/>
    <xsl:text>\\</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='adr']">
    <xsl:text>   \affaddr{</xsl:text><xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='adr']/xhtml:br">
    <xsl:text>}\\
   \affaddr{</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='email']">
    <xsl:text>}\\
   \email{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='body']">
    <xsl:text>\maketitle</xsl:text>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='abstract']">
    <xsl:text>\begin{abstract}</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>\end{abstract}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='abstract']/xhtml:p[@class='heading']">
  </xsl:template>
 
  <xsl:template match="xhtml:div[@class='descr']/xhtml:p[@class='heading']">
  </xsl:template>

  <xsl:template match="xhtml:div[@class='descr']/text()">
  </xsl:template>

  <xsl:template match="xhtml:p[@class='category']">
    <xsl:text>\category</xsl:text>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xhtml:p[@class='category']/text()">
  </xsl:template>

  <xsl:template match="xhtml:span[@class='catnumlevel1and2']">
    <xsl:text>{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='catnamelevel1and2']">
    <xsl:text>{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='catlevel3']">
    <xsl:text>{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:span[@class='catlevel4']">
    <xsl:text>[</xsl:text><xsl:apply-templates/><xsl:text>]</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:p[@class='terms']">
    <xsl:text>
\terms{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:p[@class='keywords']">
    <xsl:text>
\keywords{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='section']/xhtml:h2">
    <xsl:text>\section{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='section']/xhtml:h3">
    <xsl:text>\subsection{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='section']/xhtml:h4">
    <xsl:text>\subsubsection{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:a[@class='bibref']">
    <xsl:text>\cite{</xsl:text><xsl:value-of select="substring(@href,2)"/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:code">
    <xsl:text>\texttt{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:em">
    <xsl:text>\textit{</xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:ol">
    <xsl:text>\begin{enumerate}</xsl:text>
    <xsl:apply-templates />
    <xsl:text>\end{enumerate}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:ul">
    <xsl:text>\begin{itemize}</xsl:text>
    <xsl:apply-templates />
    <xsl:text>\end{itemize}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:li">
    <xsl:text>\item </xsl:text>
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="xhtml:pre">
    <xsl:text>\begin{verbatim}</xsl:text>
    <xsl:apply-templates />
    <xsl:text>\end{verbatim}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='example']/xhtml:p">
    <xsl:text>\begin{center}{\bf </xsl:text><xsl:apply-templates /><xsl:text>}\end{center}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='figure'] |
                       xhtml:div[@class='topfigure']">
    <xsl:text>\begin{figure}[ht]
\centering</xsl:text>
    <xsl:apply-templates />
    <xsl:text>\end{figure}</xsl:text>
  </xsl:template>

  <xsl:template match="xhtml:div[@class='figure']/xhtml:p |
                       xhtml:div[@class='topfigure']/xhtml:p">
    <xsl:text>\caption{</xsl:text><xsl:apply-templates /><xsl:text>}</xsl:text>
  </xsl:template>

  <!-- all images must be converted to eps format before making the
       transformation -->
  <xsl:template match="xhtml:img">
    <xsl:text>\psfig{file=</xsl:text><xsl:value-of select="substring-before(string(@src),'.')"/><xsl:text>.eps, width=8.45cm,}</xsl:text>
  </xsl:template>

</xsl:stylesheet>
