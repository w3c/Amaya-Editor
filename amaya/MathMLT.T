	{ Translation Schema for MathML.

	  I. Vatton			June 1997	}


TRANSLATION MathML;

LineLength 78;

BUFFERS
   ElemPrefixBuffer (variable);
   AttrPrefixBuffer (variable);

CONST
   tagend = '>\12';

VAR
   SOT : '<' ElemPrefixBuffer;        { Start of opening tag }
   SCT : '</' ElemPrefixBuffer;        { Start of closing tag }
   SAN : ' ' AttrPrefixBuffer;  { Start of attribute name }
   SUA : '';                    { Start of unknown attribute }

RULES

   MathML:
        begin
        Use XLinkT for XLink;
        Use SVGT for SVG;
        Use HTMLTX for HTML;
        Use TemplateT for Template;
        create SOT;
        Create 'math';
        Create Attributes;
        Create '>\12';
        indent +2;
        indent -2 after;
        create SCT after;
        Create 'math>' after;
        end;

   XMLcomment:
        begin
        NoLineBreak;
        Create '<!--';
        {NoTranslation;}
        Create '-->\12' after;
        end;

   XMLcomment_line:
        if not last create '\12' after;

   XMLPI:
        begin
        NoLineBreak;
        Create '<?';
        NoTranslation;
        Create '?>\12' after;
        end;

   XMLPI_line:
        if not last create '\12' after;

   Unknown_namespace:
        begin
        NoTranslation;
        Create '\12';
        Create Content;
        Remove;
        Create Attributes;
        Create '>';
        end;

   DOCTYPE:
        begin
        NoLineBreak;
        NoTranslation;
        Create '\12' After;
        end;

   DOCTYPE_line:
        if not last create '\12' after;

   CDATA: BEGIN
        NoLineBreak;
        Create '<![CDATA[';
        NoTranslation;
        Create ']]>\12' After;
        END;

   CDATA_line:
        if not last create '\12' after;

   MROW:
        begin
        create SOT;
        create 'mrow';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mrow>\12' after;
        end;

   MSQRT:
        begin
        create SOT;
        create 'msqrt';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'msqrt>\12' after;
          end;

   SqrtBase:
        begin
        end;

   MENCLOSE:
        begin
        create SOT;
        create 'menclose';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'menclose>\12' after;
          end;

   MROOT:
        begin
        create SOT;
        create 'mroot';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mroot>\12' after;
          end;
   RootBase:
        begin
        end;
   Index:
        begin
        end;

   MFRAC:
        begin
        create SOT;
        create 'mfrac';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mfrac>\12' after;
        end;
   BevelledMFRAC:
        begin
        create SOT;
        create 'mfrac';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mfrac>\12' after;
        end;
   Numerator:
        begin
        end;
   Denominator:
        begin
        end;

   MFENCED:
        begin
        create SOT;
        create 'mfenced';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mfenced>\12' after;
          end;
   FencedExpression:
        begin
        end;
   FencedSeparator:
        Remove;
   OpeningFence:
        Remove;
   ClosingFence:
        Remove;

   MSUBSUP:
        begin
        create SOT;
        create 'msubsup';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'msubsup>\12' after;
        end;
   Base:
        begin
        end;
   Subscript:
        begin
        end;
   Superscript:
        begin
        end;

   MSUB:
        begin
        create SOT;
        create 'msub';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'msub>\12' after;
        end;

   MSUP:
        begin
        create SOT;
        create 'msup';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'msup>\12' after;
        end;

   MUNDER:
        begin
        create SOT;
        create 'munder';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'munder>\12' after;
        end;
   UnderOverBase:
        begin
        end;
   Underscript:
        begin
        end;

   MOVER:
        begin
        create SOT;
        create 'mover';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mover>\12' after;
        end;
   Overscript:
        begin
        end;

   MUNDEROVER:
        begin
        create SOT;
        create 'munderover';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'munderover>\12' after;
        end;

   MMULTISCRIPTS:
        begin
        create SOT;
        create 'mmultiscripts';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mmultiscripts>\12' after;
        end;
   MultiscriptBase:
        begin
        end;
   
   Construct:
        begin
        if not IntPlaceholder and immediately within MSubscript
                begin
                create SOT;
                create 'none/>\12';
                end;
        if not IntPlaceholder and immediately within MSuperscript
                begin
                create SOT;
                create 'none/>\12';
                end;
        if not IntPlaceholder and not immediately within MSubscript
           and not immediately within MSuperscript
                begin
                create SOT;
                create 'mi>';
                create SCT;
                create 'mi>\12';
                end;
        end;
   PostscriptPair:
        if empty Remove;
   PrescriptPair:
        if empty Remove;
   PrescriptPairs:
        if not empty
                begin
                create SOT;
                create 'mprescripts/>\12' before;
                end;

   MTABLE:
        begin
        create SOT;
        create 'mtable';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mtable>\12' after;
        end;

   MTable_head:
        remove;

   MTR:
        begin
        create SOT;
        create 'mtr';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mtr>\12' after;
        end;

   MLABELEDTR:
        begin
        create SOT;
        create 'mlabeledtr';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mlabeledtr>\12' after;
        end;

   LabelCell:
        begin
        create SOT;
        create 'mtd';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mtd>\12' after;
        end;

   MTD:
        begin
        create SOT;
        create 'mtd';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mtd>\12' after;
        end;

   MALIGNGROUP:
        begin
        create SOT;
        create 'maligngroup';
        create Attributes;
        create '/>\12';
        end;

   MALIGNMARK:
        begin
        create SOT;
        create 'malignmark';
        create Attributes;
        create '/>\12';
        end;

   MF:
        begin
        create SOT;
        create 'mo';
        create Attributes;
        create '>';
        create SCT after;
        create 'mo>\12' after;
        end;

   MN:
        begin
        create SOT;
        create 'mn';
        create Attributes;
        create '>';
        create SCT after;
        create 'mn>\12' after;
        end;

   MI:
        begin
        create SOT;
        create 'mi';
        create Attributes;
        create '>';
        create SCT after;
        create 'mi>\12' after;
        end;

   MO:
        begin
        create SOT;
        create 'mo';
        create Attributes;
        create '>';
        create SCT after;
        create 'mo>\12' after;
        end;

   MS:
        begin
        create SOT;
        create 'ms';
        create Attributes;
        create '>';
        create SCT after;
        create 'ms>\12' after;
        end;

   MTEXT:
        begin
        create SOT;
        create 'mtext';
        create Attributes;
        create '>';
        create SCT after;
        create 'mtext>\12' after;
        end;

   C_Empty:
        remove;

   C_Space:
        remove;

   MSPACE:
        begin
        create SOT;
        create 'mspace';
        create Attributes;
        create '/>\12';
        end;

   MSTYLE:
        begin
        create SOT;
        create 'mstyle';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mstyle>\12' after;
        end;

   MERROR:
        begin
        create SOT;
        create 'merror';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'merror>\12' after;
        end;

   MPADDED:
        begin
        create SOT;
        create 'mpadded';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mpadded>\12' after;
        end;

   MPHANTOM:
        begin
        create SOT;
        create 'mphantom';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'mphantom>\12' after;
        end;

   MGLYPH:
        begin
        create SOT;
        create 'mglyph';
        create Attributes;
        create '/>';
        Remove;
        end;

   MACTION:
        begin
        create SOT;
        create 'maction';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'maction>\12' after;
        end;

   SEMANTICS:
        begin
        create SOT;
        create 'semantics';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'semantics>\12' after;
        end;

   ANNOTATION:
        begin
        create SOT;
        create 'annotation';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'annotation>\12' after;
        end;

   ANNOTATION_XML:
        begin
        create SOT;
        create 'annotation-xml';
        create Attributes;
        create tagend;
        indent +2;
        indent -2 after;
        create SCT after;
        create 'annotation-xml>\12' after;
        end;

   TEXT_UNIT:
        begin
        end;

   SYMBOL_UNIT:
        begin
        end;

ATTRIBUTES

 Language:
        begin
        if not TEXT_UNIT and not Root
            begin
            Create ' xml:lang="';
            Create Language;
            Create '"';
            end;
        if Root and RealLang
            begin
            Create ' xml:lang="';
            Create Language;
            Create '"';
            end;
        end;

   class:
        begin
        create SAN;
        create 'class="';
        create class;
        create '"';
        end;
  
   style\240:
        begin
        create SAN;
        create 'style="';
        create style\240;
        create '"';
        end;
  
   id:
        begin
        create SAN;
        create 'id="';
        create id;
        create '"';
        end;
  
   xmlid:
        begin
        create SAN;
        create 'xml:id="';
        create xmlid;
        create '"';
        end;
  
   xref:
        begin
        create SAN;
        create 'xref="';
        create xref;
        create '"';
        end;
  
   other:
        begin
        create SAN;
        create 'other="';
        create other;
        create '"';
        end;
  
   macros:
        begin
        create SAN;
        create 'macros="';
        create macros;
        create '"';
        end;

   display_ = block_:
        begin
        create SAN;
        create 'display="block"';
        end;
   display_ = inline_:
        begin
        create SAN;
        create 'display="inline"';
        end;

   overflow = scroll:
        begin
        create SAN;
        create 'overflow="scroll"';
        end;
   overflow = elide:
        begin
        create SAN;
        create 'overflow="elide"';
        end;
   overflow = truncate:
        begin
        create SAN;
        create 'overflow="truncate"';
        end;
   overflow = scale_:
        begin
        create SAN;
        create 'overflow="scale"';
        end;

   altimg:
        begin
        create SAN;
        create 'altimg="';
        create altimg;
        create '"';
        end;
        
   alttext:
        begin
        create SAN;
        create 'alttext="';
        create alttext;
        create '"';
        end;

   fontsize:
        begin
        create SAN;
        create 'mathsize="';
        create fontsize;
        create '"';        
        end;

   fontweight = normal_:
        begin
        create SAN;
        create 'fontweight="normal"';
        end;
   fontweight = bold_:
        begin
        create SAN;
        create 'fontweight="bold"';
        end;

   fontstyle = normal_:
        begin
        create SAN;
        create 'fontstyle="normal"';
        end;
   fontstyle = italic:
        begin
        create SAN;
        create 'fontstyle="italic"';
        end;

   fontfamily:
        begin
        create SAN;
        create 'fontfamily="';
        create fontfamily;
        create '"';        
        end;

   color:
        begin
        create SAN;
        create 'mathcolor="';
        create color;
        create '"';        
        end;

  mathvariant = normal_:
        begin
        create SAN;
        create 'mathvariant="normal"';
        end;
  mathvariant = bold_:
        begin
        create SAN;
        create 'mathvariant="bold"';
        end;
  mathvariant = italic:
        begin
        create SAN;
        create 'mathvariant="italic"';
        end;
  mathvariant = bold_italic:
        begin
        create SAN;
        create 'mathvariant="bold-italic"';
        end;
  mathvariant = double_struck:
        begin
        create SAN;
        create 'mathvariant="double-struck"';
        end;
  mathvariant = bold_fraktur:
        begin
        create SAN;
        create 'mathvariant="bold-fraktur"';
        end;
  mathvariant = script_:
        begin
        create SAN;
        create 'mathvariant="script"';
        end;
  mathvariant = bold_script:
        begin
        create SAN;
        create 'mathvariant="bold-script"';
        end;
  mathvariant = fraktur_:
        begin
        create SAN;
        create 'mathvariant="fraktur"';
        end;
  mathvariant = sans_serif:
        begin
        create SAN;
        create 'mathvariant="sans-serif"';
        end;
  mathvariant = bold_sans_serif:
        begin
        create SAN;
        create 'mathvariant="bold-sans-serif"';
        end;
  mathvariant = sans_serif_italic:
        begin
        create SAN;
        create 'mathvariant="sans-serif-italic"';
        end;
  mathvariant = sans_serif_bold_italic:
        begin
        create SAN;
        create 'mathvariant="sans-serif-bold-italic"';
        end;
  mathvariant = monospace:
        begin
        create SAN;
        create 'mathvariant="monospace"';
        end;

  mathsize:
        begin
        create SAN;
        create 'mathsize="';
        create mathsize;
        create '"';        
        end;

  mathcolor:
        begin
        create SAN;
        create 'mathcolor="';
        create mathcolor;
        create '"';        
        end;

  mathbackground:
        begin
        create SAN;
        create 'mathbackground="';
        create mathbackground;
        create '"';        
        end;
   
   form = prefix:
        begin
        create SAN;
        create 'form="prefix"';
        end;
   form = infix:
        begin
        create SAN;
        create 'form="infix"';
        end;
   form = postfix:
        begin
        create SAN;
        create 'form="postfix"';
        end;

   fence = true:
        begin
        create SAN;
        create 'fence="true"';
        end;
   fence = false:
        begin
        create SAN;
        create 'fence="false"';
        end;

   separator = true:
        begin
        create SAN;
        create 'separator="true"';
        end;
   separator = false:
        begin
        create SAN;
        create 'separator="false"';
        end;

   lspace:
        begin
        create SAN;
        create 'lspace="';
        create lspace;
        create '"';        
        end;

   rspace:
        begin
        create SAN;
        create 'rspace="';
        create rspace;
        create '"';        
        end;

   stretchy = true:
        begin
        create SAN;
        create 'stretchy="true"';
        end;
   stretchy = false:
        begin
        create SAN;
        create 'stretchy="false"';
        end;

   symmetric = true:
        begin
        create SAN;
        create 'symmetric="true"';
        end;
   symmetric = false:
        begin
        create SAN;
        create 'symmetric="false"';
        end;

   maxsize:
        begin
        create SAN;
        create 'maxsize="';
        create maxsize;
        create '"';        
        end;

   minsize:
        begin
        create SAN;
        create 'minsize="';
        create minsize;
        create '"';        
        end;

   largeop = true:
        begin
        create SAN;
        create 'largeop="true"';
        end;
   largeop = false:
        begin
        create SAN;
        create 'largeop="false"';
        end;

   movablelimits = true:
        begin
        create SAN;
        create 'movablelimits="true"';
        end;
   movablelimits = false:
        begin
        create SAN;
        create 'movablelimits="false"';
        end;

   accent = true:
        begin
        create SAN;
        create 'accent="true"';
        end;
   accent = false:
        begin
        create SAN;
        create 'accent="false"';
        end;

   width\240:
        begin
        create SAN;
        create 'width="';
        create width\240;
        create '"';
        end;

   height\240:
        begin
        create SAN;
        create 'height="';
        create height\240;
        create '"';
        end;

   depth\240:
        begin
        create SAN;
        create 'depth="';
        create depth\240;
        create '"';
        end;

   linebreak_ = auto_:
        begin
        create SAN;
        create 'linebreak="auto"';
        end;
   linebreak_ = newline:
        begin
        create SAN;
        create 'linebreak="newline"';
        end;
   linebreak_ = indentingnewline:
        begin
        create SAN;
        create 'linebreak="indentingnewline"';
        end;
   linebreak_ = nobreak_:
        begin
        create SAN;
        create 'linebreak="nobreak"';
        end;
   linebreak_ = goodbreak:
        begin
        create SAN;
        create 'linebreak="goodbreak"';
        end;
   linebreak_ = badbreak:
        begin
        create SAN;
        create 'linebreak="badbreak"';
        end;

   lquote:
        begin
        create SAN;
        create 'lquote="';
        create lquote;
        create '"';
        end;

   rquote:
        begin
        create SAN;
        create 'rquote="';
        create rquote;
        create '"';
        end;

   linethickness:
        begin
        create SAN;
        create 'linethickness="';
        create linethickness;
        create '"';
        end;

   numalign = left\240:
        begin
        create SAN;
        create 'numalign="left"';
        end;
   numalign = center\240:
        begin
        create SAN;
        create 'numalign="center"';
        end;
   numalign = right\240:
        begin
        create SAN;
        create 'numalign="right"';
        end;

   denomalign = left\240:
        begin
        create SAN;
        create 'denomalign="left"';
        end;
   denomalign = center\240:
        begin
        create SAN;
        create 'denomalign="center"';
        end;
   denomalign = right\240:
        begin
        create SAN;
        create 'denomalign="right"';
        end;

   bevelled = true:
        begin
        create SAN;
        create 'bevelled="true"';
        end;
   bevelled = false:
        begin
        create SAN;
        create 'bevelled="false"';
        end;

   notation = longdiv:
        begin
        create SAN;
        create 'notation="longdiv"';
        end;
   notation = actuarial:
        begin
        create SAN;
        create 'notation="actuarial"';
        end;
   notation = radical:
        begin
        create SAN;
        create 'notation="radical"';
        end;
   notation = box:
        begin
        create SAN;
        create 'notation="box"';
        end;
   notation = roundedbox:
        begin
        create SAN;
        create 'notation="roundedbox"';
        end;
   notation = circle_:
        begin
        create SAN;
        create 'notation="circle"';
        end;

   notation = updiagonalstrike:
        begin
        create SAN;
        create 'notation="updiagonalstrike"';
        end;
   notation = downdiagonalstrike:
        begin
        create SAN;
        create 'notation="downdiagonalstrike"';
        end;
   notation = verticalstrike:
        begin
        create SAN;
        create 'notation="verticalstrike"';
        end;
   notation = horizontalstrike:
        begin
        create SAN;
        create 'notation="horizontalstrike"';
        end;
   notation = madruwb:
        begin
        create SAN;
        create 'notation="madruwb"';
        end;
   notation = left_:
        begin
        create SAN;
        create 'notation="left"';
        end;
   notation = right_:
        begin
        create SAN;
        create 'notation="right"';
        end;
   notation = top_:
        begin
        create SAN;
        create 'notation="top"';
        end;
   notation = bottom_:
        begin
        create SAN;
        create 'notation="bottom"';
        end;


   scriptlevel:
        begin
        create SAN;
        create 'scriptlevel="';
        create scriptlevel;
        create '"';
        end;

   displaystyle = true:
        begin
        create SAN;
        create 'displaystyle="true"';
        end;
   displaystyle = false:
        begin
        create SAN;
        create 'displaystyle="false"';
        end;

   side = left\240:
        begin
        create SAN;
        create 'side="left"';
        end;
   side = right\240:
        begin
        create SAN;
        create 'side="right"';
        end;
   side = leftoverlap:
        begin
        create SAN;
        create 'side="leftoverlap"';
        end;
   side = rightoverlap:
        begin
        create SAN;
        create 'side="rightoverlap"';
        end;

   minlabelspacing:
        begin
        create SAN;
        create 'minlabelspacing="';
        create minlabelspacing;
        create '"';
        end;

   scriptsizemultiplier:
        begin
        create SAN;
        create 'scriptsizemultiplier="';
        create scriptsizemultiplier;
        create '"';
        end;

   scriptminsize:
        begin
        create SAN;
        create 'scriptminsize="';
        create scriptminsize;
        create '"';
        end;

   background\240:
        begin
        create SAN;
        create 'background="';
        create background\240;
        create '"';
        end;

   open:
        begin
        create SAN;
        create 'open="';
        create open;
        create '"';
        end;

   close:
        begin
        create SAN;
        create 'close="';
        create close;
        create '"';
        end;

   separators:
        begin
        create SAN;
        create 'separators="';
        create separators;
        create '"';
        end;

   subscriptshift:
        begin
        create SAN;
        create 'subscriptshift="';
        create subscriptshift;
        create '"';
        end;

   superscriptshift:
        begin
        create SAN;
        create 'superscriptshift="';
        create superscriptshift;
        create '"';
        end;

   accentunder = true:
        begin
        create SAN;
        create 'accentunder="true"';
        end;
   accentunder = false:
        begin
        create SAN;
        create 'accentunder="false"';
        end;

   align = top_:
        begin
        create SAN;
        create 'align="top"';
        end;
   align = bottom_:
        begin
        create SAN;
        create 'align="bottom"';
        end;
   align = center:
        begin
        create SAN;
        create 'align="center"';
        end;
   align = baseline:
        begin
        create SAN;
        create 'align="baseline"';
        end;
   align = axis:
        begin
        create SAN;
        create 'align="axis"';
        end;

   rowalign:
        begin
        create SAN;
        create 'rowalign="';
        create rowalign;
        create '"';
        end;

   rowalign_mtr = top_:
        begin
        create SAN;
        create 'rowalign="top"';
        end;
   rowalign_mtr = bottom_:
        begin
        create SAN;
        create 'rowalign="bottom"';
        end;
   rowalign_mtr = center:
        begin
        create SAN;
        create 'rowalign="center"';
        end;
   rowalign_mtr = baseline:
        begin
        create SAN;
        create 'rowalign="baseline"';
        end;
   rowalign_mtr = axis:
        begin
        create SAN;
        create 'rowalign="axis"';
        end;

   columnalign:
        begin
        create SAN;
        create 'columnalign="';
        create columnalign;
        create '"';
        end;

   columnalign_mtd = left\240:
        begin
        create SAN;
        create 'columnalign="left"';
        end;
   columnalign_mtd = center\240:
        begin
        create SAN;
        create 'columnalign="center"';
        end;
   columnalign_mtd = right\240:
        begin
        create SAN;
        create 'columnalign="right"';
        end;

   groupalign:
        begin
        create SAN;
        create 'groupalign="';
        create groupalign;
        create '"';
        end;

   groupalign_malgr = left\240:
        begin
        create SAN;
        create 'groupalign="left"';
        end;
   groupalign_malgr = center\240:
        begin
        create SAN;
        create 'groupalign="center"';
        end;
   groupalign_malgr = right\240:
        begin
        create SAN;
        create 'groupalign="right"';
        end;
   groupalign_malgr = decimalpoint:
        begin
        create SAN;
        create 'groupalign="decimalpoint"';
        end;

   alignmentscope:
        begin
        create SAN;
        create 'alignmentscope="';
        create alignmentscope;
        create '"';
        end;

   columnwidth:
        begin
        create SAN;
        create 'columnwidth="';
        create columnwidth;
        create '"';
        end;

   rowspacing:
        begin
        create SAN;
        create 'rowspacing="';
        create rowspacing;
        create '"';
        end;

   columnspacing:
        begin
        create SAN;
        create 'columnspacing="';
        create columnspacing;
        create '"';
        end;

   rowlines:
        begin
        create SAN;
        create 'rowlines="';
        create rowlines;
        create '"';
        end;

   columnlines:
        begin
        create SAN;
        create 'columnlines="';
        create columnlines;
        create '"';
        end;

   frame = none\240:
        begin
        create SAN;
        create 'frame="none"';
        end;
   frame = solid\240:
        begin
        create SAN;
        create 'frame="solid"';
        end;
   frame = dashed\240:
        begin
        create SAN;
        create 'frame="dashed"';
        end;

   framespacing:
        begin
        create SAN;
        create 'framespacing="';
        create framespacing;
        create '"';
        end;

   equalrows = true:
        begin
        create SAN;
        create 'equalrows="true"';
        end;
   equalrows = false:
        begin
        create SAN;
        create 'equalrows="false"';
        end;

   equalcolumns = true:
        begin
        create SAN;
        create 'equalcolumns="true"';
        end;
   equalcolumns = false:
        begin
        create SAN;
        create 'equalcolumns="false"';
        end;

   rowspan\240:
        begin
        create SAN;
        create 'rowspan="';
        create rowspan\240;
        create '"';
        end;

   columnspan:
        begin
        create SAN;
        create 'columnspan="';
        create columnspan;
        create '"';
        end;

   edge = left\240:
        begin
        create SAN;
        create 'edge="left"';
        end;
   edge = right\240:
        begin
        create SAN;
        create 'edge="right"';
        end;

   alt:
        begin
        create SAN;
        create 'alt="';
        create alt;
        create '"';
        end;

   index:
        begin
        create SAN;
        create 'index="';
        create index;
        create '"';
        end;

   definitionURL:
        begin
        create SAN;
        create 'definitionURL="';
        create definitionURL;
        create '"';
        end;

   encoding:
        begin
        create SAN;
        create 'encoding="';
        create encoding;
        create '"';
        end;

   actiontype:
        begin
        create SAN;
        create 'actiontype="';
        create actiontype;
        create '"';
        end;

   selection:
        begin
        create SAN;
        create 'selection="';
        create selection;
        create '"';
        end;

   EntityName:
        begin
        create EntityName;
        remove;        { do no output the element content }
        end;

   xml_space = xml_space_default:
        begin
        create SAN;
        create 'xml:space="default"';
        end;
   xml_space = xml_space_preserve:
        BEGIN
        create SAN;
        create 'xml:space="preserve"';
        end;

   Unknown_attribute:
        begin
        create SUA;
        Create Unknown_attribute;
        end;        

TextTRANSLATE
        BEGIN
        '\46' -> '&amp;';        { ampersand, U+0026 ISOnum }
        '\74' -> '&lt;';        { less-than sign, U+003C ISOnum }
        '\76' -> '&gt;';         { greater-than sign, U+003E ISOnum }
        END;

END
