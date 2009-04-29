
      { Thot structure schema for Scalable Vector Graphics }

STRUCTURE SVG;

DEFPRES SVGP;

ATTR

   { global attributes for all SVG elements }
   {stdAttrs}
   id = text;
   xml_base = text;
   xmlid = Text;                { xml:id }
   xml_space = xml_space_default, xml_space_preserve;

   { attributes for internal processing }
   Unknown_attribute = text;
   Ghost_restruct = text;
   PseudoClass = text;
   Highlight = Yes_;        { to show the SVG element corresponding to the
                              current selection in the source view }
   Namespace = text;        { for children of element foreignObject }
   IsCopy = Yes_;

CONST

   C_Empty = ' ';

STRUCT

{ Document Structure }

   SVG
        (ATTR requiredFeatures = text;      {testAttrs}
              requiredExtensions = text;    {testAttrs}
              systemLanguage = text;        {testAttrs}
              baseProfile = text;
              externalResourcesRequired = false, true;
              class = text;
              style_ = text;
              { PresentationAttributes-All }
              color = text;                      {PresentationAttributes-Color}
              color_interpolation = auto, sRGB, linearRGB, inherit;
                                                 {PresentationAttributes-Color}
              color_rendering = auto, optimizeSpeed, optimizeQuality, inherit;
                                                 {PresentationAttributes-Color}
              enable_background = text;     {PresentationAttributes-Containers}
              flood_color = text;              {PresentationAttributes-feFlood}
              flood_opacity = text;            {PresentationAttributes-feFlood}
              fill = text;                  {PresentationAttributes-FillStroke}
              fill_opacity =  text;         {PresentationAttributes-FillStroke}
              fill_rule = nonzero, evenodd, inherit;
                                            {PresentationAttributes-FillStroke}
              stroke = text;                {PresentationAttributes-FillStroke}
              stroke_dasharray = text;      {PresentationAttributes-FillStroke}
              stroke_dashoffset = text;     {PresentationAttributes-FillStroke}
              stroke_linecap = butt, round, square, inherit;
                                            {PresentationAttributes-FillStroke}
              stroke_linejoin = miter, round, bevel, inherit;
                                            {PresentationAttributes-FillStroke}
              stroke_miterlimit = text;     {PresentationAttributes-FillStroke}
              stroke_opacity = text;        {PresentationAttributes-FillStroke}
              stroke_width = text;          {PresentationAttributes-FillStroke}
              color_interpolation_filters = auto, sRGB, linearRGB, inherit;
                                      {PresentationAttributes-FilterPrimitives}
              font_family = text;    {PresentationAttributes-FontSpecification}
              font_size = text;      {PresentationAttributes-FontSpecification}
              font_size_adjust =text;{PresentationAttributes-FontSpecification}
              font_stretch = normal_, wider, narrower, ultra_condensed,
                   extra_condensed, condensed, semi_condensed, semi_expanded,
                   expanded, extra_expanded, ultra_expanded, inherit;
                                     {PresentationAttributes-FontSpecification}
              font_style = normal_, italic, oblique_, inherit;
                                     {PresentationAttributes-FontSpecification}
              font_variant = normal_, small_caps, inherit;
                                     {PresentationAttributes-FontSpecification}
              font_weight = normal_, bold_, bolder, lighter, w100, w200, w300,
                            w400, w500, w600, w700, w800, w900, inherit;
                                     {PresentationAttributes-FontSpecification}
              stop_color = text;             {PresentationAttributes-Gradients}
              stop_opacity = text;           {PresentationAttributes-Gradients}
              clip_path = text;               {PresentationAttributes-Graphics}
              clip_rule = nonzero, evenodd, inherit;
                                              {PresentationAttributes-Graphics}
              cursor_ = text;                 {PresentationAttributes-Graphics}
              display_ = inline, block, list_item, run_in, compact, marker_,
                         table, inline_table, table_row_group,
                         table_header_group, table_footer_group, table_row,
                         table_column_group, table_column, table_cell,
                         table_caption, none, inherit; 
                                              {PresentationAttributes-Graphics}
              filter_ = text;                 {PresentationAttributes-Graphics}
              image_rendering = auto, optimizeSpeed, optimizeQuality, inherit;
                                              {PresentationAttributes-Graphics}
              mask_ = text;                   {PresentationAttributes-Graphics}
              opacity_ = text;                {PresentationAttributes-Graphics}
              pointer_events = visiblePainted, visibleFill, visibleStroke,
                         visible, painted, fill__, stroke_, all, none, inherit;
                                              {PresentationAttributes-Graphics}
              shape_rendering = auto, optimizeSpeed, crispEdges,
                                geometricPrecision, inherit;
                                              {PresentationAttributes-Graphics}
              text_rendering = auto, optimizeSpeed, optimizeLegibility,
                               geometricPrecision, inherit;
                                              {PresentationAttributes-Graphics}
              visibility_ = visible, hidden_, inherit;
                                              {PresentationAttributes-Graphics}
              color_profile_ = text;            {PresentationAttributes-Images}
              lighting_color = text;   {PresentationAttributes-LightingEffects}
              marker_start = text;             {PresentationAttributes-Markers}
              marker_mid = text;               {PresentationAttributes-Markers}
              marker_end = text;               {PresentationAttributes-Markers}
              alignment_baseline = baseline, top, before_edge, text_top,
                        text_before_edge, middle, bottom, after_edge,
                        text_bottom, text_after_edge, ideographic_, lower,
                        hanging_, mathematical_, inherit;
                                   {PresentationAttributes-TextContentElements}
              baseline_shift = text;
                                   {PresentationAttributes-TextContentElements}
              direction_ = ltr_, rtl_, inherit;
                                   {PresentationAttributes-TextContentElements}
              dominant_baseline = auto, autosense_script, no_change, reset,
                      ideographic_, lower, hanging_, mathematical_, inherit;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal = text;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical = text;
                                   {PresentationAttributes-TextContentElements}
              kerning = text;      {PresentationAttributes-TextContentElements}
              letter_spacing = text;
                                   {PresentationAttributes-TextContentElements}
              text_anchor = start, middle, end__, inherit;
                                   {PresentationAttributes-TextContentElements}
              text_decoration = text;
                                   {PresentationAttributes-TextContentElements}
              unicode_bidi = normal_, embed_, bidi_override, inherit;
                                   {PresentationAttributes-TextContentElements}
              word_spacing = text; {PresentationAttributes-TextContentElements}
              writing_mode = lr_tb, rl_tb, tb_rl, lr, rl, tb, inherit;
                                          {PresentationAttributes-TextElements}
              clip = text;                     {PresentationAttributes-Viewports}
              overflow = visible, hidden_, scroll, auto, inherit;
                                             {PresentationAttributes-Viewports}
              viewBox = text;
              preserveAspectRatio = text;
              zoomAndPan = disable, magnify; 
              onfocusin = text;              {graphicsElementEvents}
              onfocusout = text;             {graphicsElementEvents}
              onactivate = text;             {graphicsElementEvents}
              onclick = text;                {graphicsElementEvents}
              onmousedown = text;            {graphicsElementEvents}
              onmouseup = text;              {graphicsElementEvents}
              onmouseover = text;            {graphicsElementEvents}
              onmousemove = text;            {graphicsElementEvents}
              onmouseout = text;             {graphicsElementEvents}
              onload = text;                 {graphicsElementEvents}
              onunload = text;               {documentEvents}
              onabort = text;                {documentEvents}
              onerror = text;                {documentEvents}
              onresize = text;               {documentEvents}
              onscroll = text;               {documentEvents}
              onzoom = text;                 {documentEvents}
              version = text;
              x = text;
              y = text; 
              width_ = text;
              height_ = text;
              contentScriptType = text;
              contentStyleType = text;
              RealLang = Yes_;
              Charset = text;
              UnresolvedRef = Yes_)
        = LIST OF (GraphicsElement) + (XMLcomment, XMLPI, CDATA,
                                       Unknown_namespace);

   DOCTYPE = LIST OF (DOCTYPE_line = TEXT);        
        
   Unknown_namespace = TEXT;
        
   XMLcomment = LIST OF (XMLcomment_line = TEXT) - (XMLcomment);

   XMLPI = LIST OF (XMLPI_line = TEXT);

   CDATA = LIST OF (CDATA_line = TEXT);

   g
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              transform = text;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload)                               {graphicsElementEvents}
        = LIST OF (GraphicsElement);

   defs
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload)                               {graphicsElementEvents}
        = LIST OF (GraphicsElement);

   desc
        (ATTR  class; style_;
               content_ = text)                      {StructuredText}
        = TEXT;
        
   title
        (ATTR  class; style_;
               content_)                             {StructuredText}
        = TEXT;
        
   symbol_  { not in SVG Tiny }
        (ATTR externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              viewBox;
              preserveAspectRatio; 
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload)                               {graphicsElementEvents}
        = LIST OF (GraphicsElement);

   use_
        (ATTR xlink_type = simple;       {xlinkRefAttrs}
              xlink_role = text;         {xlinkRefAttrs}
              xlink_arcrole = text;      {xlinkRefAttrs}
              xlink_title = text;        {xlinkRefAttrs}
              xlink_show = other, embed; {xlinkRefAttrs}
              xlink_actuate = onLoad;    {xlinkRefAttrs}
              xlink_href = text;
              requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x; 
              y;
              width_;
              height_)
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              END);
        
{ Images }

   image
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              clip; overflow;                  {PresentationAttributes-Viewports}
              transform;
              preserveAspectRatio;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x; 
              y;
              width_;
              height_)                
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              PICTURE;
              SVG_Image = SVG;
              END);

{ Conditional Processing }

   switch
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;)                              {graphicsElementEvents}
        = LIST OF (CASE OF
              desc; title; metadata;
              path; text_; rect; circle_; ellipse; line_; polyline; polygon;
              use_; image; Timeline_cross; SVG; g; switch; a; foreignObject;
              script_; style__; symbol_; clipPath;
              animate; set_; animateMotion; animateColor; animateTransform;
              END);

{ Styling }

   style__  { not in SVG Tiny }
        (ATTR type = text;
              media = text;
              title_ = text)
        = TEXT;

{ Paths }

   path
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              d = text;
              pathLength = text)
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              GRAPHICS;
              END);
        
{ Basic Shapes }

   rect
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x;
              y;
              width_;
              height_;
              rx = text;
              ry = text)
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              GRAPHICS;
              END);
        
   circle_
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              cx = text;
              cy = text;
              r = text)
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              GRAPHICS;
              END);
        
   ellipse
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              cx;
              cy;
              rx;
              ry)
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              GRAPHICS;
              END);
        
   line_
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x1 = text;
              y1 = text;
              x2 = text;
              y2 = text)
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              GRAPHICS;
              END);
        
   polyline
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              points = text)
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              GRAPHICS;
              END);

   polygon
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              points)
        = LIST OF (CASE OF
              desc; title; metadata;
              animate; set_; animateMotion; animateColor; animateTransform;
              GRAPHICS;
              END);

{ Text }
        
   text_
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x;
              y;
              dx = text;
              dy = text;
              rotate = text;
              textLength = text;
              lengthAdjust = spacing_, spacingAndGlyphs;)
        = LIST OF (CASE OF
                   tspan;        { must be the first option, to allow the Return
                                  key to create tspan elements }
                   TEXT; desc; title; metadata;
                   tref; textPath; altGlyph; a;
                   animate; set_; animateMotion; animateColor; animateTransform;
                   END);

   tspan  { not in SVG Tiny }
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x;
              y;
              dx;
              dy;
              rotate;
              textLength;
              lengthAdjust;)
        = LIST OF (SpanElement = CASE OF
                      TEXT;
                      desc; title; metadata;
                      tspan; tref; altGlyph;
                      a;
                      animate; set_; animateColor;
                      END);

   tref  { not in SVG Tiny }
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x;
              y;
              dx;
              dy;
              rotate;
              textLength;
              lengthAdjust;)
        = LIST OF (CASE OF
                   desc; title; metadata;
                   animate; set_; animateColor;
                   END);

   textPath  { not in SVG Tiny }
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              startOffset = text;
              textLength;
              lengthAdjust; 
              method = align, stretch; 
              spacing = auto, exact)
        = LIST OF (CASE OF
                   TEXT;
                   desc; title; metadata;
                   tspan; tref; altGlyph; a;
                   animate; set_; animateColor;
                   END);

   altGlyph  { not in SVG Tiny }
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              glyphRef_ = text; 
              format = text;
              requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired; 
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x;
              y;
              dx;
              dy;
              rotate)
        = TEXT;

   altGlyphDef  { not in SVG Tiny }
        = BEGIN
          CASE OF
              LIST OF (glyphRef);
              LIST OF (altGlyphItem);
              END;
          END;

   altGlyphItem  { not in SVG Tiny }
        = LIST OF (glyphRef);
        
   glyphRef { not in SVG Tiny }
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              class; style_;
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              glyphRef_;
              format;
              x;
              y;
              dx;
              dy)
        = CONSTANT C_Empty;

{ Marker Symbols }

   marker  { not in SVG Tiny, not in SVG Basic }
        (ATTR externalResourcesRequired; 
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              viewBox;
              preserveAspectRatio;
              refX = text; 
              refY = text; 
              markerUnits = strokeWidth, userSpaceOnUse;
              markerWidth = text; 
              markerHeight = text; 
              orient = text)
        = LIST OF (GraphicsElement);
        
{ Color }

   color_profile  { not in SVG Tiny } 
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              local = text;
              name = text;
              rendering_intent = auto, perceptual, relative_colorimetric,
                                 saturation, absolute_colorimetric)
        = LIST OF
            (CASE OF
              desc; title; metadata;
             END);

{ Gradients and Patterns }

   linearGradient  { not in SVG Tiny } 
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              externalResourcesRequired;
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              gradientUnits = userSpaceOnUse, objectBoundingBox;
              gradientTransform = text;
              x1; y1; x2; y2;      
              spreadMethod = pad, reflect, repeat)
        = LIST OF
            (CASE OF
              desc; title; metadata;
              stop; animate; set_; animateTransform;
             END);

   radialGradient  { not in SVG Tiny } 
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              externalResourcesRequired;
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              gradientUnits;
              gradientTransform;
              cx; cy; r;
              fx = text; 
              fy = text; 
              spreadMethod)
        = LIST OF
            (CASE OF
              desc; title; metadata;
              stop; animate; set_; animateTransform;
             END);        

   stop  { not in SVG Tiny } 
        (ATTR 
              class; style_;              
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              offset = text)
        = LIST OF
             (CASE OF
                animate; set_; animateColor;
              END);
        
   pattern  { not in SVG Tiny } 
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              viewBox;
              preserveAspectRatio;
              patternUnits = userSpaceOnUse, objectBoundingBox;
              patternContentUnits = userSpaceOnUse, objectBoundingBox;
              patternTransform = text;
              x;
              y;
              width_;
              height_)
        = LIST OF (GraphicsElement);

{ Clipping, Masking and Compositing }

   clipPath  { not in SVG Tiny }
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              transform;
              clipPathUnits = userSpaceOnUse, objectBoundingBox;)
        = AGGREGATE
            desc; title; metadata;
            LIST OF
              (CASE OF
                 path; text_; rect; circle_; ellipse; line_; polyline; polygon;
                 use_; animate; set_; animateMotion; animateColor;
                 animateTransform;
               END);
          END;

   mask  { not in SVG Tiny } 
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              maskUnits = userSpaceOnUse, objectBoundingBox;
              maskContentUnits = userSpaceOnUse, objectBoundingBox;
              x;
              y;
              width_;
              height_)              
        = LIST OF (GraphicsElement);

{ Filter Effects }

   filter
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              externalResourcesRequired;
              class; style_;        
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              filterUnits = userSpaceOnUse, objectBoundingBox;
              primitiveUnits = userSpaceOnUse, objectBoundingBox;
              x;
              y;
              width_;
              height_;
              filterRes = text)
        = LIST OF
            (CASE OF
              desc; title; metadata;
              feBlend; feFlood; 
              feColorMatrix; feComponentTransfer; 
              feComposite; feConvolveMatrix; feDiffuseLighting;
              feDisplacementMap; 
              feGaussianBlur; feImage; feMerge; 
              feMorphology; feOffset; feSpecularLighting; 
              feTile; feTurbulence;
              animate; set_;
             END);        

   feDistantLight
        (ATTR azimuth = text;
              elevation = text)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   fePointLight
        (ATTR x;
              y;
              z        = text)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feSpotLight
        (ATTR x;
              y;
              z;
              pointsAtX = text;
              pointsAtY = text;
              pointsAtZ = text;
              specularExponent = text;
              limitingConeAngle = text)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feBlend
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result = text;              {filter_primitive_attributes_with_in}
              in_ = text;                 {filter_primitive_attributes_with_in}
              in2 = text;
              mode = normal_, multiply, screen, darken, lighten;)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feColorMatrix
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
               type__ = matrix, saturate, hueRotate, luminanceToAlpha;
              values = text)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feComponentTransfer
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_)                        {filter_primitive_attributes_with_in}
        = BEGIN
            ? feFuncR;
            ? feFuncG;
            ? feFuncB;
            ? feFuncA;
          END;

   feFuncR
        (ATTR type___ = identity, table, discrete, linear, gamma; {component_transfer_function_attributes}
              tableValues = text;      {component_transfer_function_attributes}
              slope_ = text;           {component_transfer_function_attributes}
              intercept = text;        {component_transfer_function_attributes}
              amplitude = text;        {component_transfer_function_attributes}
              exponent = text;         {component_transfer_function_attributes}
              offset_ = text;)         {component_transfer_function_attributes}
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feFuncG
        (ATTR type___;                 {component_transfer_function_attributes}
              tableValues;             {component_transfer_function_attributes}
              slope_;                  {component_transfer_function_attributes}
              intercept;               {component_transfer_function_attributes}
              amplitude;               {component_transfer_function_attributes}
              exponent;                {component_transfer_function_attributes}
              offset_)                 {component_transfer_function_attributes}
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feFuncB
        (ATTR type___;                 {component_transfer_function_attributes}
              tableValues;             {component_transfer_function_attributes}
              slope_;                  {component_transfer_function_attributes}
              intercept;               {component_transfer_function_attributes}
              amplitude;               {component_transfer_function_attributes}
              exponent;                {component_transfer_function_attributes}
              offset_)                 {component_transfer_function_attributes}
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feFuncA
        (ATTR type___;                 {component_transfer_function_attributes}
              tableValues;             {component_transfer_function_attributes}
              slope_;                  {component_transfer_function_attributes}
              intercept;               {component_transfer_function_attributes}
              amplitude;               {component_transfer_function_attributes}
              exponent;                {component_transfer_function_attributes}
              offset_)                 {component_transfer_function_attributes}
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feComposite
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
              in2;
              operator = over, in__, out, atop, xor, arithmetic;
              k1 = text;
              k2 = text;
              k3 = text;
              k4 = text)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feConvolveMatrix
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
              order = text;
              kernelMatrix = text;
              divisor = text;
              bias = text;
              targetX = integer;
              targetY = integer;
              edgeMode = duplicate, wrap, none;
              kernelUnitLength = text;
              preserveAlpha = text)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feDiffuseLighting
        (ATTR class; style_;        
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              color_interpolation_filters;{PresentationAttributes-FilterPrimitives}
              lighting_color;          {PresentationAttributes-LightingEffects}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
              surfaceScale = text;
              diffuseConstant = text;
              kernelUnitLength)
        = BEGIN
          CASE OF feDistantLight; fePointLight; feSpotLight; END;
          LIST OF
            (CASE OF
              animate; set_; animateColor;
             END);
          END;
        
   feDisplacementMap
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
              in2;
              scale_ = text;
              xChannelSelector = R, G, B, A;
              yChannelSelector = R, G, B, A)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feFlood
        (ATTR class; style_;
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_)                        {filter_primitive_attributes_with_in}
        = LIST OF
            (CASE OF
              animate; set_; animateColor;
             END);

   feGaussianBlur
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
              stdDeviation = text)
        = LIST OF
            (CASE OF
              animate; set_;
             END);
        
   feImage
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              externalResourcesRequired; 
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow)                 {PresentationAttributes-Viewports}
        = LIST OF
            (CASE OF
              animate; set_; animateTransform;
             END);

   feMerge
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes}
              y;                          {filter_primitive_attributes}
              width_;                     {filter_primitive_attributes}
              height_;                    {filter_primitive_attributes}
              result)                     {filter_primitive_attributes}
        = LIST OF (feMergeNode);

   feMergeNode
        (ATTR in_)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feMorphology
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
              operator_ = erode, dilate;
              radius = text)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feOffset
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
              dx;
              dy)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

   feSpecularLighting
        (ATTR class; style_;        
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              color_interpolation_filters;{PresentationAttributes-FilterPrimitives}
              lighting_color;          {PresentationAttributes-LightingEffects}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_;                        {filter_primitive_attributes_with_in}
              surfaceScale;
              specularConstant = text;
              specularExponent;
              kernelUnitLength)
        = BEGIN
          CASE OF feDistantLight; fePointLight; feSpotLight; END;
          LIST OF
            (CASE OF
              animate; set_; animateColor;
             END);
          END;
        
   feTile
        (ATTR color_interpolation_filters;{PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes_with_in}
              y;                          {filter_primitive_attributes_with_in}
              width_;                     {filter_primitive_attributes_with_in}
              height_;                    {filter_primitive_attributes_with_in}
              result;                     {filter_primitive_attributes_with_in}
              in_)                        {filter_primitive_attributes_with_in}
        = LIST OF
            (CASE OF
              animate; set_;
             END);

  feTurbulence
        (ATTR color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              x;                          {filter_primitive_attributes}
              y;                          {filter_primitive_attributes}
              width_;                     {filter_primitive_attributes}
              height_;                    {filter_primitive_attributes}
              result;                     {filter_primitive_attributes}
              baseFrequency = text;
              numOctaves = integer;
              seed = text;
              stitchTiles = stitch, noStitch;
              type____ = fractalNoise, turbulence)
        = LIST OF
            (CASE OF
              animate; set_;
             END);

{ Interactivity }

   cursor { not in SVG Tiny, not in SVG Basic}
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              x; y)
        = AGGREGATE
            desc; title; metadata;
          END;

{ Linking }

   a
        (ATTR xlink_href;
              requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              target_ = text)
        = LIST OF (CASE OF
                   TEXT; 
                   GraphicsElement;
                   END);

   view { not in SVG Tiny }
        (ATTR externalResourcesRequired;
              viewBox;
              preserveAspectRatio;
              zoomAndPan;
              viewTarget = text)
        = AGGREGATE
            desc; title; metadata;
          END;

{ Scripting }

   script_  { not in SVG Tiny }
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;
              externalResourcesRequired;
              type)
        = TEXT;

{ Animation }

   animate
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              onbegin = text;   {animationEvents}
              onend = text;     {animationEvents}
              onrepeat = text;  {animationEvents}
              xlink_type; xlink_role; xlink_arcrole;  {animElementAttrs}
              xlink_title; xlink_show; xlink_actuate; {animElementAttrs}
              xlink_href;                             {animElementAttrs}
              attributeName_ = text;  {animAttributeAttrs}
              attributeType = text;
              begin_ = text;  {animTimingAttrs}
              dur = text;
              end_ = text;
              min_ = text;
              max_ = text;
              restart = always, never, whenNotActive;
              repeatCount = text;
              repeatDur = text;
              fill_ = remove_, freeze;
              calcMode = discrete, linear, paced, spline;  {animValueAttrs}
              values;
              keyTimes = text;
              keySplines = text;
              from = text;
              to_ = text;
              by = text;
              additive = replace, sum;  {animAdditionAttrs}
              accumulate = none_, sum;)
        = AGGREGATE
              desc; title; metadata;
          END;

   set_
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              onbegin; onend; onrepeat; {animationEvents}
              xlink_type; xlink_role; xlink_arcrole;  {animElementAttrs}
              xlink_title; xlink_show; xlink_actuate; {animElementAttrs}
              xlink_href;                             {animElementAttrs}
              attributeName_; attributeType; {animAttributeAttrs}
              begin_; dur; end_; min_; max_; restart; repeatCount; repeatDur; fill_;
                                                              {animTimingAttrs}
              to_;)
        = AGGREGATE
              desc; title; metadata;
          END;

   animateMotion
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              onbegin; onend; onrepeat; {animationEvents}
              xlink_type; xlink_role; xlink_arcrole;  {animElementAttrs}
              xlink_title; xlink_show; xlink_actuate; {animElementAttrs}
              xlink_href;                             {animElementAttrs}
              attributeName_; attributeType; {animAttributeAttrs}
              begin_; dur; end_; min_; max_; restart; repeatCount; repeatDur; fill_;
                                                              {animTimingAttrs}
              calcMode; values; keyTimes; keySplines; from; to_; by;
              additive; accumulate; {animAdditionAttrs}
              path_ = text;
              keyPoints = text;
              rotate;
              origin = text;)
        = AGGREGATE
              desc; title; metadata; ? mpath;
          END;

   mpath
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href;  {see XLink.S}
              externalResourcesRequired;)
        = AGGREGATE
              desc; title; metadata;
          END;

   animateColor
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              onbegin; onend; onrepeat; {animationEvents}
              xlink_type; xlink_role; xlink_arcrole;  {animElementAttrs}
              xlink_title; xlink_show; xlink_actuate; {animElementAttrs}
              xlink_href;                             {animElementAttrs}
              attributeName_; attributeType; {animAttributeAttrs}
              begin_; dur; end_; min_; max_; restart; repeatCount; repeatDur; fill_;
                                                              {animTimingAttrs}
              calcMode; values; keyTimes; keySplines; from; to_; by;
                                                               {animValueAttrs}
              additive; accumulate;) {animAdditionAttrs}
        = AGGREGATE
              desc; title; metadata;
          END;
        
   animateTransform
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
              externalResourcesRequired;
              onbegin; onend; onrepeat; {animationEvents}
              xlink_type; xlink_role; xlink_arcrole;  {animElementAttrs}
              xlink_title; xlink_show; xlink_actuate; {animElementAttrs}
              xlink_href;                             {animElementAttrs}
              attributeName_; attributeType; {animAttributeAttrs}
              begin_; dur; end_; min_; max_; restart; repeatCount; repeatDur; fill_;
                                                              {animTimingAttrs}
              calcMode; values; keyTimes; keySplines; from; to_; by;
                                                               {animValueAttrs}
              additive; accumulate;  {animAdditionAttrs}
              type_ = translate, scale, rotate_, skewX, skewY;)
        = AGGREGATE
              desc; title; metadata;
          END;

{ Fonts }

   font_
        (ATTR externalResourcesRequired;
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              horiz_origin_x = text;
              horiz_origin_y = text;
              horiz_adv_x = text;
              vert_origin_x = text;
              vert_origin_y = text;
              vert_adv_y = text)
        = AGGREGATE
            desc; title; metadata;
            font_face; missing_glyph;
            LIST OF
              (CASE OF
                 glyph; hkern; vkern;
               END);
          END;

   glyph
        (ATTR class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              unicode = text;
              glyph_name = text;
              d;
              orientation = text;
              arabic_form = text;
              lang = text;
              horiz_adv_x; vert_origin_x; vert_origin_y; vert_adv_y)
        = LIST OF (GraphicsElement);

   missing_glyph
        (ATTR class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              d;
              horiz_adv_x; vert_origin_x; vert_origin_y; vert_adv_y)              
        = LIST OF (GraphicsElement);

   hkern
        (ATTR u1 = text;
              g1 = text;
              u2 = text;
              g2 = text;
              k = text)
        = CONSTANT C_Empty;

   vkern { not in SVG Tiny }
        (ATTR u1; g1; u2; g2; k)
        = CONSTANT C_Empty;

   font_face
        (ATTR font_family; font_style; font_variant; font_weight; font_size;
              font_stretch_ = text;
              unicode_range = text;
              units_per_em = text;
              panose_1 = text;
              stemv = text;
              stemh = text;
              slope = text;
              cap_height = text;
              x_height = text;
              accent_height = text;
              ascent = text;
              descent = text;
              widths = text;
              bbox = text;
              ideographic = text;
              alphabetic = text;
              mathematical = text;
              hanging = text;
              v_ideographic = text;
              v_alphabetic = text;
              v_mathematical = text;
              v_hanging = text;
              underline_position = text;
              underline_thickness = text;
              strikethrough_position = text;
              strikethrough_thickness = text;
              overline_position = text;
              overline_thickness = text)
        = AGGREGATE
             desc; title; metadata; ? font_face_src; ? definition_src;
          END;

   font_face_src
        = LIST OF (CASE OF font_face_uri; font_face_name; END);

   font_face_uri { not in SVG Tiny }
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href)
        = LIST OF (font_face_format);

   font_face_format { not in SVG Tiny }
        (ATTR string = text)
        = CONSTANT C_Empty;

   font_face_name
        (ATTR name)
        = CONSTANT C_Empty;

   definition_src
        (ATTR xlink_type; xlink_role; xlink_arcrole;   {xlinkRefAttrs}
              xlink_title; xlink_show; xlink_actuate;  {xlinkRefAttrs}
              xlink_href)
        = CONSTANT C_Empty;

{ Metadata }
        
   metadata = TEXT;

{ Extensibility }

   foreignObject  { not in SVG Tiny }
        (ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}

              externalResourcesRequired; 
              class; style_;
              { PresentationAttributes-All }
              color; color_interpolation; {PresentationAttributes-Color}
              color_rendering;            {PresentationAttributes-Color}
              enable_background;          {PresentationAttributes-Containers}
              flood_color; flood_opacity; {PresentationAttributes-feFlood}
              fill; fill_opacity;           {PresentationAttributes-FillStroke}
              fill_rule; stroke;            {PresentationAttributes-FillStroke}
              stroke_dasharray;             {PresentationAttributes-FillStroke}
              stroke_dashoffset;            {PresentationAttributes-FillStroke}
              stroke_linecap;               {PresentationAttributes-FillStroke}
              stroke_linejoin;              {PresentationAttributes-FillStroke}
              stroke_miterlimit;            {PresentationAttributes-FillStroke}
              stroke_opacity; stroke_width; {PresentationAttributes-FillStroke}
              color_interpolation_filters; {PresentationAttributes-FilterPrimitives}
              font_family; font_size;{PresentationAttributes-FontSpecification}
              font_size_adjust;      {PresentationAttributes-FontSpecification}
              font_stretch;          {PresentationAttributes-FontSpecification}
              font_style;            {PresentationAttributes-FontSpecification}
              font_variant;          {PresentationAttributes-FontSpecification}
              font_weight;           {PresentationAttributes-FontSpecification}
              stop_color; stop_opacity;   {PresentationAttributes-Gradients}
              clip_path; clip_rule; cursor_; {PresentationAttributes-Graphics}
              display_; filter_;             {PresentationAttributes-Graphics}
              image_rendering; mask_;        {PresentationAttributes-Graphics}
              opacity_; pointer_events;      {PresentationAttributes-Graphics}
              shape_rendering;               {PresentationAttributes-Graphics}
              text_rendering; visibility_;   {PresentationAttributes-Graphics}
              color_profile_;             {PresentationAttributes-Images}
              lighting_color;         {PresentationAttributes-LightingEffects}
              marker_start; marker_mid;       {PresentationAttributes-Markers}
              marker_end;                     {PresentationAttributes-Markers}
              alignment_baseline;  {PresentationAttributes-TextContentElements}
              baseline_shift;      {PresentationAttributes-TextContentElements}
              direction_;           {PresentationAttributes-TextContentElements}
              dominant_baseline;   {PresentationAttributes-TextContentElements}
              glyph_orientation_horizontal;
                                   {PresentationAttributes-TextContentElements}
              glyph_orientation_vertical;
                                   {PresentationAttributes-TextContentElements}
              kerning;                   {PresentationAttributes-TextContentElements}
              letter_spacing;           {PresentationAttributes-TextContentElements}
              text_anchor;           {PresentationAttributes-TextContentElements}
              text_decoration;           {PresentationAttributes-TextContentElements}
              unicode_bidi;           {PresentationAttributes-TextContentElements}
              word_spacing;           {PresentationAttributes-TextContentElements}
              writing_mode;                 {PresentationAttributes-TextElements}
              clip; overflow;                 {PresentationAttributes-Viewports}
              transform;
              onfocusin; onfocusout; onactivate;    {graphicsElementEvents}
              onclick; onmousedown; onmouseup;      {graphicsElementEvents}
              onmouseover; onmousemove; onmouseout; {graphicsElementEvents}
              onload;                               {graphicsElementEvents}
              x; 
              y;
              width_;
              height_;
              content_)                              {StructuredText}
        = BEGIN CASE OF
              HTML; MathML; TEXT;
              END;
          END;

{ The following elements are not defined in the SVG DTD }


  Timeline_cross
        (ATTR x; 
              y;
        opacity_; 
          width_;
          height_)
        = PICTURE;

  GraphicsElement
        = CASE OF
              desc; title; metadata; defs;
              path; text_; rect; circle_; ellipse; line_; polyline; polygon;
              use_; image; SVG; g; view; switch; a; altGlyphDef;
              script_; style__; symbol_; marker; clipPath; mask;
              linearGradient; radialGradient; pattern; filter; cursor; font_;
              animate; set_; animateMotion; animateColor; animateTransform;
              color_profile; font_face;
              Timeline_cross; 
              END;

EXCEPT

   { exceptions for elements }
   SVG:           IsDraw, MoveResize, NoMove, ClickableSurface;
   g:             NoMove, NoResize, NoCreate, IsGroup, NoShowBox;
   marker:        IsMarker;
   defs:          NoMove, NoResize, NoShowBox, NoCreate;
   linearGradient:NoMove, NoResize, NoShowBox, NoCreate;
   stop:                NoMove, NoResize, NoShowBox, NoCreate;
   rect:          MoveResize, HighlightChildren, NoShowBox, NoCreate, EmptyGraphic, UsePaintServer;
   circle_:       MoveResize, HighlightChildren, NoShowBox, NoCreate, EmptyGraphic, UsePaintServer;
   ellipse:       MoveResize, HighlightChildren, NoShowBox, NoCreate, EmptyGraphic, UsePaintServer;
   line_:         MoveResize, HighlightChildren, NoShowBox, NoCreate, EmptyGraphic, UseMarkers;
   polyline:      MoveResize, HighlightChildren, NoShowBox, NoCreate, EmptyGraphic, UsePaintServer, UseMarkers;
   polygon:       MoveResize, HighlightChildren, NoShowBox, NoCreate, EmptyGraphic, UsePaintServer, UseMarkers;
   path:                MoveResize, HighlightChildren, NoShowBox, NoCreate, EmptyGraphic, UsePaintServer, UseMarkers;
   text_:         MoveResize, NoResize, NoShowBox, ReturnCreateWithin,
                        NoCreate, UsePaintServer;
   tspan:                NoShowBox, UsePaintServer; 
   use_:                NoMove, NoResize, HighlightChildren, NoShowBox,
                        NoCreate, IsGroup, EmptyGraphic;
   tref:                EmptyGraphic;
   image:                MoveResize, HighlightChildren, NoShowBox, EmptyGraphic, IsImg;
   symbol_:              NoMove, NoResize,  HighlightChildren,NoShowBox,
                  NoCreate;
   a:                    NoMove, NoResize, HighlightChildren, NoShowBox,
                  NoCreate, ClickableSurface;
   script_:              NoMove, NoResize, NoShowBox, NoCreate;
   style__:       NoMove, NoResize, NoShowBox, NoCreate;
   switch:        NoMove, NoResize, NoShowBox, NoCreate, HighlightChildren;
   foreignObject: MoveResize, HighlightChildren, NoCreate, NewRoot;
   SVG_Image:     Hidden, SelectParent;
   GRAPHICS:            SelectParent;
   PICTURE:       MoveResize, SelectParent{NoMove, NoResize};
   TEXT:          NoMove, NoResize;
   Unknown_namespace:   NoCreate;
   DOCTYPE:             NoCut, NoSpellCheck, NotAnElementNode;
   DOCTYPE_line:        Hidden, NoCut;
   XMLcomment_line:     Hidden, MarkupPreserve;
   XMLPI_line:          Hidden, MarkupPreserve;
   CDATA_line:          Hidden, MarkupPreserve;
   XMLPI:                ReturnCreateNL, NoSpellCheck, NoReplicate, NotAnElementNode;
   XMLcomment:          ReturnCreateNL, NoReplicate, NotAnElementNode;

   { exceptions for attributes }
   id:                CssId; {Default id attribute}
   class:             CssClass, DuplicateAttr;
   PseudoClass:              Invisible, CssPseudoClass;
   Unknown_attribute: Invisible;
   Highlight:                Invisible; 
   Ghost_restruct:    Invisible;
   Namespace:                Invisible;
   RealLang:                Invisible;
   Charset:                  Invisible;
   UnresolvedRef:     Invisible;
   IsCopy:	       Invisible;
   Timeline_cross:    MoveResize, NoResize, HighlightChildren, NoShowBox, NoCut;
   { the following attributes are made invisible to prevent the Attributes
     menu from becoming too long }
   xml_base:                      Invisible;
   requiredFeatures:        Invisible;
   requiredExtensions:Invisible;
   baseProfile:                    Invisible;
   externalResourcesRequired:        Invisible;
   color_interpolation:        Invisible;
   color_rendering:          Invisible;
   enable_background:        Invisible;
   flood_color:                    Invisible;
   flood_opacity:            Invisible;
   fill_rule:                      Invisible;
   stroke_dasharray:        Invisible;
   stroke_dashoffset:        Invisible;
   stroke_linecap:          Invisible;
   stroke_linejoin:          Invisible;
   stroke_miterlimit:        Invisible;
   color_interpolation_filters:        Invisible;
   font_size_adjust:        Invisible;
   font_stretch:            Invisible;
   font_variant:            Invisible;
   stop_color:                    Invisible;
   stop_opacity:            Invisible;
   clip_path:                      Invisible;
   clip_rule:                      Invisible;
   cursor_:                        Invisible;
   display_:                      Invisible;
   filter_:                        Invisible;
   image_rendering:          Invisible;
   mask_:                        Invisible;
   pointer_events:        Invisible;
   shape_rendering:        Invisible;
   text_rendering:        Invisible;
   color_profile_:        Invisible;
   lighting_color:        Invisible;
   marker_start:        Invisible;
   marker_mid:                Invisible;
   marker_end:                Invisible;
   alignment_baseline:        Invisible;
   baseline_shift:        Invisible;
   dominant_baseline:        Invisible;
   glyph_orientation_horizontal: Invisible;
   glyph_orientation_vertical:        Invisible;
   kerning:                Invisible;
   letter_spacing:        Invisible;
   word_spacing:        Invisible;
   writing_mode:        Invisible;
   clip:                Invisible;
   overflow:                Invisible;
   preserveAspectRatio:        Invisible;
   zoomAndPan:                Invisible;
   onfocusin:                Invisible;
   onfocusout:                Invisible;
   onactivate:                Invisible;
   onclick:                Invisible;
   onmousedown:                Invisible;
   onmouseup:                Invisible;
   onmouseover:                Invisible;
   onmousemove:                Invisible;
   onmouseout:                Invisible;
   onload:                Invisible;
   onabort:                Invisible;
   onerror:                Invisible;
   onresize:                Invisible;
   onscroll:                Invisible;
   onzoom:                Invisible;
   contentScriptType:        Invisible;
   contentStyleType:        Invisible;
   xlink_role:                Invisible;
   xlink_arcrole:        Invisible;
   xlink_title:                Invisible;
   xlink_show:                Invisible;
   xlink_actuate:        Invisible;
   rotate:                Invisible;
   textLength:                Invisible;
   lengthAdjust:        Invisible;
   startOffset:                Invisible;
   method:                Invisible;
   spacing:                Invisible;
   format:                Invisible;
   refX:                Invisible;
   refY:                Invisible;
   markerUnits:                Invisible;
   markerWidth:                Invisible;
   markerHeight:        Invisible;
   orient:                Invisible;
   local:                Invisible;
   name:                Invisible;
   rendering_intent:        Invisible;
   gradientUnits:        Invisible;
   gradientTransform:        Invisible;
   spreadMethod:        Invisible;
   fx:                        Invisible;
   fy:                        Invisible;
   offset:                Invisible;
   patternUnits:        Invisible;
   patternContentUnits:        Invisible;
   patternTransform:        Invisible;
   clipPathUnits:        Invisible;
   maskUnits:                Invisible;
   maskContentUnits:        Invisible;
   target_:                Invisible;
   viewTarget:                Invisible;
   horiz_origin_x:        Invisible;
   horiz_origin_y:        Invisible;
   horiz_adv_x:                Invisible;
   vert_origin_x:        Invisible;
   vert_origin_y:        Invisible;
   vert_adv_y:                Invisible;
   orientation:                Invisible;
   arabic_form:                Invisible;
   lang:                Invisible;
   font_stretch_:        Invisible;
   unicode_range:        Invisible;
   units_per_em:        Invisible;
   panose_1:                Invisible;
   stemv:                Invisible;
   stemh:                Invisible;
   slope:                Invisible;
   cap_height:                Invisible;
   x_height:                Invisible;
   accent_height:        Invisible;
   ascent:                Invisible;
   descent:                Invisible;
   widths:                Invisible;
   bbox:                Invisible;
   ideographic:                Invisible;
   alphabetic:                Invisible;
   mathematical:        Invisible;
   hanging:                Invisible;
   v_ideographic:        Invisible;
   v_alphabetic:        Invisible;
   underline_position:        Invisible;
   underline_thickness:        Invisible;
   strikethrough_position:        Invisible;
   strikethrough_thickness:        Invisible;
   overline_position:        Invisible;
   overline_thickness:        Invisible;
   xml_space:                SpacePreserve;
END
