<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Amaya User Manual</title>
  <meta name="GENERATOR" content="amaya 6.4, see http://www.w3.org/Amaya/" />
  <link href="style.css" rel="stylesheet" type="text/css" />
  <style type="text/css">
  </style>
</head>

<body xml:lang="es" lang="es">
<p style="text-align: center"><img src="../images/w3c_home" alt="W3C" /></p>

<div id="page_body">
<h1 style="text-align : center">Manual de Usuario de Amaya</h1>

<h4 style="text-align:center">Versión 6.4 - Agosto 2002<br />
<br />
<img src="../images/amaya.gif" alt="Amaya" />
</h4>
<hr />
<dl>
  <dt><strong>Autores:</strong></dt>
    <dd>Irène Vatton, Vincent Quint, José Kahan, Kim Cramer, Kim Nylander,
      Kathy Rosen, Michael Spinella, and Lori Caldwell LeDoux 
      <p>Actualización de la documentación realizada por <a
      href="http://www.winwriters.com/"
      title="WinWriters web site">WinWriters</a></p>
    </dd>
  <dt><strong>Contributors:</strong></dt>
    <dd>Charles McCathieNevile, John Russell, Marja-Riitta Koivunen, Laurent
      Carcone</dd>
</dl>

<p>Este manual presenta el interface de usuario de Amaya en las plataformas
Unix y Windows. Tu interface de usuario puede ser ligeramente distinto en
función de tu equipo y configuración. Este manual de usuario no está
completo. Su función es ayudar a los usuarios a descubrir las características
pricipales del programa. Este documento está organizado en forma de libro y
puedes utilizar el comando <a href="MakeBook.html.es#L1065">Hacer Libro</a>
para crear e imprimir el manual entero.</p>

<p>Muchos de los comandos de <strong>Amaya</strong> son los mismos de los
editores o procesadores de textos más habituales. Estos comandos no se
comentan aquí. Tan sólo se describen las características particulares de
<strong>Amaya</strong>.</p>

<h2><a name="L92011" id="L92011">Iniciando Amaya</a></h2>

<p>Para poner en marcha <strong>Amaya</strong> en un sistema Unix,
escribe:</p>
<pre>    amaya [-display host] [document]</pre>

<p>El parámetro [-display host] es opcional. Define la pantalla anfitrión
remota en la que Amaya debe mostrar las páginas Web. Si omites este
parámetro, Amaya las mostrará en la pantalla anfitrión local.</p>

<p>El parámetro <code>[document]</code> es opcional. Es el nombre del archivo
o el URI del primer documento que <strong>Amaya</strong> debe mostrar al
empezar. Si omites este parámetro, Amaya muestra un documento por defecto. El
documento por defecto puede ser tu <em>home page</em> (ver cómo <a
href="Configure.html.es#L138">configurar tu Home Page</a>) o la página de
bienvenida de Amaya.</p>
<hr />

<h1>Contenido</h1>
<ul>
  <li><a href="Browsing.html.es#Browsing">Navegando con Amaya</a> 
    <ul>
      <li><a
        href="browsing/working_with_the_amaya_browser_and_editor.html.es#page_body"
        rel="subdocument">Trabajar con el Navegador y Editor Amaya</a></li>
      <li><a href="browsing/about_access_keys.html.es#page_body"
        rel="subdocument">Acerca de las Teclas de Acceso</a></li>
      <li><a
        href="browsing/about_moving_backward_and_forward.html.es#page_body"
        rel="subdocument">Acerca de Ir Adelante o Atrás</a></li>
      <li><a href="browsing/about_target_anchors.html.es#page_body"
        rel="subdocument">Acerca de Destinos</a></li>
      <li><a href="browsing/activating_a_link.html.es#page_body"
        rel="subdocument">Activar un Enlace</a></li>
      <li><a href="browsing/opening_documents.html.es#page_body"
        rel="subdocument">Abrir documentos</a></li>
      <li><a href="browsing/reloading_a_page.html.es#page_body">Recargar una
        página</a></li>
      <li><a href="browsing/forms.html.es"
        rel="subdocument#page_body">Browsing forms</a></li>
    </ul>
  </li>
  <li><a href="Selecting.html.es#Selecting">Selecting</a> 
    <ul>
      <li><a
        href="selecting/selecting_with_keyboard_and_mouse.html.es#page_body"
        rel="subdocument">Selecting with the Keyboard and Mouse</a></li>
      <li><a href="selecting/selecting_by_structure.html.es#page_body"
        rel="subdocument">Selecting by Structure</a></li>
      <li><a href="selecting/selecting_images.html.es#page_body"
        rel="subdocument">Selecting Images</a></li>
    </ul>
  </li>
  <li><a href="Views.html.es#Views">Viendo documentos en Amaya</a> 
    <ul>
      <li><a href="viewing/working_with_document_views.html.es#page_body"
        rel="subdocument">Utilizar las Vistas de Documento</a></li>
      <li><a href="viewing/about_synchronized_views.html.es#page_body"
        rel="subdocument">Acerca de las Vistas sincronizadas</a></li>
      <li><a href="viewing/closing_a_view.html.es#page_body"
        rel="subdocument">Cerrar una Vista</a></li>
      <li><a href="viewing/opening_a_view.html.es#page_body"
        rel="subdocument">Abrir una Vista</a></li>
    </ul>
  </li>
  <li><a href="Document.html.es#Changing">Editing Document</a> 
    <ul>
      <li><a
        href="http://www.w3.org/Amaya/User/editing_documents/creating_new_documents.html.es#page_body"
        rel="subdocument">Creating New document</a></li>
      <li><a href="Creating.html.es#Creating">Creating new elements</a></li>
      <li><a href="elements/buttons_and_types_menu.html.es#page_body"
        rel="subdocument">Botones y Menú Tipos</a></li>
      <li><a href="elements/changing_the_document_title.html.es#page_body"
        rel="subdocument">Changing the Document Title</a></li>
      <li><a href="elements/creating_nested_structures.html.es#page_body"
        rel="subdocument">Creating Nested Structures</a></li>
      <li><a href="elements/exiting_anchors.html.es#page_body"
        rel="subdocument">Exiting Anchors</a></li>
      <li><a href="elements/the_enter_key.html.es#page_body"
        rel="subdocument">The Enter Key</a></li>
      <li><a href="elements/the_structure_menu.html.es#page_body"
        rel="subdocument">The Structure Menu</a></li>
    </ul>
  </li>
  <li><a href="Attributes.html.es#Attributes">Editing attributes</a> 
    <ul>
      <li><a rel="subdocument"
        href="editing_attributes/working_with_attributes.html.es#page_body">Working
        with Attributes</a></li>
      <li><a href="editing_attributes/copying_attributes.html.es#page_body"
        rel="subdocument">Copying Attributes</a></li>
      <li><a
        href="editing_attributes/editing_attributes_in_the_structure_view.html.es#page_body"
        rel="subdocument">Editing Attributes in the Structure View</a></li>
      <li><a
        href="editing_attributes/editing_attributes_through_the_attributes_menu.html.es#page_body"
        rel="subdocument">Editing Attributes Through the Attributes
      Menu</a></li>
    </ul>
  </li>
  <li><a href="EditChar.html.es">Entering ISO-Latin and Unicode
    characters</a> 
    <ul>
      <li><a
        href="editing_iso-latin-1_characters/entering_ISOLatin1_characters_in_amaya.html.es#page_body"
        rel="subdocument">Entering Characters in Amaya</a></li>
      <li><a
        href="editing_iso-latin-1_characters/about_optional_multikey_support.html.es#page_body"
        rel="subdocument">About Optional Multikey Support</a></li>
      <li><a
        href="editing_iso-latin-1_characters/about_standard_multikey_support.html.es#page_body"
        rel="subdocument">About Standard Multikey Support</a></li>
      <li><a
        href="editing_iso-latin-1_characters/about_white_space_handling.html.es#page_body"
        rel="subdocument">About White Space Handling</a></li>
    </ul>
  </li>
  <li><a href="HTML.html.es">HTML elements</a> 
    <ul>
      <li><a href="HTML-elements/infoTypes.html.es">Information types in
        HTML</a></li>
      <li><a href="HTML-elements/structure.html.es">HTML Document
        Structure</a></li>
      <li><a href="HTML-elements/headings.html.es">Heading elements</a></li>
      <li><a href="HTML-elements/lists.html.es">List elements</a></li>
      <li><a href="HTML-elements/address.html.es">About the author</a></li>
      <li><a href="HTML-elements/inline.html.es">Inline markup</a></li>
      <li><a href="HTML-elements/40styling.html.es">Character style elements
        in HTML</a></li>
    </ul>
  </li>
  <li><a href="Changing.html.es#Changing">Restructuring Documents</a> 
    <ul>
      <li><a
        href="editing_documents/about_inserting_a_division.html.es#page_body"
        rel="subdocument">About Inserting a Division</a></li>
      <li><a
        href="editing_documents/about_merging_elements.html.es#page_body"
        rel="subdocument">About Merging Elements</a></li>
      <li><a
        href="editing_documents/changing_the_document_structure.html.es#page_body"
        rel="subdocument">Changing the Document Structure</a></li>
      <li><a href="editing_documents/the_transform_command.html.es#page_body"
        rel="subdocument">The Transform Command</a></li>
      <li><a href="Transform.html.es#page_body" rel="subdocument">Using the
        HTML.trans File</a></li>
    </ul>
  </li>
  <li><a href="Links.html.es#page_body">Linking</a> 
    <ul>
      <li><a href="linking/creating_a_target_anchor.html.es#page_body"
        rel="subdocument">Crear un destino</a></li>
      <li><a href="linking/creating_a_target_element.html.es#page_body"
        rel="subdocument">Crear un Elemento Destino</a></li>
      <li><a href="linking/creating_an_external_link.html.es#page_body"
        rel="subdocument">Creating an External Link</a></li>
      <li><a href="linking/creating_an_internal_link.html.es#page_body"
        rel="subdocument">Crear un Enlace Interno</a></li>
      <li><a href="linking/changing_a_link.html.es#page_body"
        rel="subdocument">Changing a Link</a></li>
      <li><a
        href="linking/removing_a_link_or_a_target_anchor.html.es#page_body"
        rel="subdocument">Removing a Link or a Target Anchor</a></li>
    </ul>
  </li>
  <li><a href="Tables.html.es#page_body">Editing tables</a> 
    <ul>
      <li><a href="editing_tables/working_with_tables.html.es#page_body"
        rel="subdocument">Working with Tables</a></li>
      <li><a href="editing_tables/create_table.html.es#page_body"
        rel="subdocument">Creating a Table</a></li>
      <li><a href="editing_tables/add_column.html.es#page_body"
        rel="subdocument">Adding Columns</a></li>
      <li><a href="editing_tables/add_row.html.es#page_body"
        rel="subdocument">Adding Rows</a></li>
      <li><a href="editing_tables/add_tbody.html.es#page_body"
        rel="subdocument">Adding a New tbody</a></li>
      <li><a href="editing_tables/remove_column.html.es#page_body"
        rel="subdocument">Removing Columns</a></li>
    </ul>
  </li>
  <li><a href="ImageMaps.html.es#page_body">Using Image Maps</a> 
    <ul>
      <li><a
        href="using_image_maps/working_with_image_maps.html.es#page_body"
        rel="subducment">Working with Image Maps</a></li>
      <li><a
        href="using_image_maps/about_alternative_text_and_long_descriptions.html.es#page_body"
        rel="subdocument">About Alternative Text and Long
      Descriptions</a></li>
      <li><a href="using_image_maps/adding_an_image.html.es#page_body"
        rel="subdocument">Adding an Image</a></li>
      <li><a
        href="using_image_maps/creating_clientside_image_maps.html.es#page_body"
        rel="subdocument">Creating Clientside Image Maps</a></li>
      <li><a href="using_image_maps/moving_image_map_areas.html.es#page_body"
        rel="subdocument">Moving Image Map Areas</a></li>
      <li><a
        href="using_image_maps/resizing_image_map_areas.html.es#page_body"
        rel="subdocument">Resizing Image Map Areas</a></li>
      <li><a
        href="using_image_maps/using_clientside_image_maps.html.es#page_body"
        rel="subdocument">Using Clientside Image Maps</a></li>
    </ul>
  </li>
  <li><a href="Math.html.es#page_body">Editing mathematics</a> 
    <ul>
      <li><a
        href="editing_mathematics/working_with_math_expressions.html.es#page_body"
        rel="subdocument">Working with Math Expressions</a></li>
      <li><a
        href="editing_mathematics/about_entering_math_characters.html.es#page_body"
        rel="subdocument">About Entering Math Characters</a></li>
      <li><a
        href="editing_mathematics/about_entering_math_constructs_using_the_keyboard.html.es#page_body"
        rel="subdocument">About Entering Math Constructs Using the
        Keyboard</a></li>
      <li><a
        href="editing_mathematics/about_linking_in_mathml.html.es#page_body"
        rel="subdocument">About Linking in MathML</a></li>
      <li><a
        href="editing_mathematics/editing_math_expressions.html.es#page_body"
        rel="subdocument">Editing Math Expressions</a></li>
      <li><a href="editing_mathematics/math_issues.html.es#page_body"
        rel="subdocument">Math Issues</a></li>
      <li><a
        href="editing_mathematics/the_math_palette_and_the_types.html.es#page_body"
        rel="subdocument">The Math Palette and the Types Menu</a></li>
      <li><a
        href="editing_mathematics/viewing_structure_in_mathml.html.es#page_body"
        rel="subdocument">Viewing Structure in MathML</a></li>
    </ul>
  </li>
  <li><a href="SVG.html.es#page_body">Editing graphics</a> 
    <ul>
      <li><a href="using_graphics/working_with_graphics.html.es#page_body"
        rel="subdocument">Working with Graphics</a></li>
      <li><a
        href="using_graphics/creating_graphics_with_the_palette.html.es#page_body"
        rel="subdocument">Creating Graphics with the Palette</a></li>
      <li><a href="using_graphics/moving_graphics.html.es#page_body"
        rel="subdocument">Moving Graphics</a></li>
      <li><a href="using_graphics/painting_graphics.html.es#page_body"
        rel="subdocument">Painting Graphics</a></li>
      <li><a href="using_graphics/resizing_graphics.html.es#page_body"
        rel="subdocument">Resizing Graphics</a></li>
    </ul>
  </li>
  <li><a href="Xml.html.es#page_body">Xml Support</a></li>
  <li><a href="StyleSheets.html.es#page_body">Style Sheets</a> 
    <ul>
      <li><a href="style_sheets/working_with_styles.html.es#page_body"
        rel="subdocument">Working with Styles</a></li>
      <li><a
        href="style_sheets/about_applying_style_using_html_elements.html.es#page_body"
        rel="subdocument">About Applying Style Using HTML Elements</a></li>
      <li><a
        href="style_sheets/about_linking_external_and_user_style_sheets.html.es#page_body"
        rel="subdocument">About Linking External and User Style
      Sheets</a></li>
      <li><a
        href="style_sheets/creating_and_updating_a_style_attribute.html.es#page_body"
        rel="subdocument">Creating and Updating a Style Attribute</a></li>
      <li><a
        href="style_sheets/creating_generic_style_for_an_element_or_a_class.html.es#page_body"
        rel="subdocument">Creating a Generic Style for an Element or a
        Class</a></li>
      <li><a
        href="style_sheets/creating_html_style_elements.html.es#page_body"
        rel="subdocument">Creating HTML Style Elements</a></li>
      <li><a href="style_sheets/handling_external_css.html.es#page_body"
        rel="subdocument">Handling External CSS</a></li>
      <li><a href="style_sheets/removing_style.html.es#page_body"
        rel="subdocument">Removing Styles</a></li>
    </ul>
  </li>
  <li><a href="Searching.html.es">Searching and replacing text</a></li>
  <li><a href="SpellChecking.html.es#page_body">Spell checking</a></li>
  <li><a href="Publishing.html.es#page_body">Saving and publishing
    documents</a> 
    <ul>
      <li><a
        href="saving_and_publishing_documents/saving_and_publishing_documents.html.es#page_body"
        rel="subdocument">Saving and Publishing Documents</a></li>
      <li><a
        href="saving_and_publishing_documents/about_saving_in_html.html.es#page_body"
        rel="subdocument">About Saving in HTML</a></li>
      <li><a
        href="saving_and_publishing_documents/saving_documents_as_text.html.es#page_body"
        rel="subdocument">Saving Documents as Text</a></li>
      <li><a
        href="saving_and_publishing_documents/the_save_command.html.es#page_body"
        rel="subdocument">The Save Command</a></li>
      <li><a
        href="saving_and_publishing_documents/the_saveas_command.html.es#page_body"
        rel="subdocument">The Save As Command</a></li>
    </ul>
  </li>
  <li><a href="Printing.html.es#page_body">Printing documents</a> 
    <ul>
      <li><a href="printing/printing_documents.html.es#page_body"
        rel="subdocument">Printing Documents</a></li>
      <li><a href="printing/print_command.html.es#page_body"
        rel="subdocument">The Print Command</a></li>
      <li><a
        href="printing/using_the_setup_and_print_command.html.es#page_body"
        rel="subdocument">Using the Setup and Print Command</a></li>
      <li><a
        href="printing/about_controlling_printing_with_css.html.es#page_body"
        rel="subdocument">About Controlling Printing with CSS</a></li>
      <li><a href="printing/windows_platforms.html.es#page_body"
        rel="subdocument">Printing on Windows Platforms</a></li>
      <li><a href="printing/unix_platforms.html.es#page_body"
        rel="subdocument">Printing on UNIX Platforms</a></li>
    </ul>
  </li>
  <li><a href="Numbering.html.es#page_body">Section numbering</a></li>
  <li><a href="MakeBook.html.es#page_body">Assembling large documents</a></li>
  <li><a href="Annotations.html.es#page_body">Annotating documents with
    Amaya</a> 
    <ul>
      <li><a
        href="attaching_annotations/what_is_an_annotation.html.es#page_body"
        rel="subdocument">What is an Annotation?</a></li>
      <li><a
        href="attaching_annotations/about_reading_annotations.html.es#page_body"
        rel="subdocument">About Reading Annotations</a></li>
      <li><a
        href="attaching_annotations/about_storing_local_annotations.html.es#page_body"
        rel="subdocument">About Local Annotations</a></li>
      <li><a
        href="attaching_annotations/about_storing_remote_annotations.html.es#page_body"
        rel="subdocument">About Remote Annotations</a></li>
      <li><a href="attaching_annotations/annotation_menu.html.es#page_body"
        rel="subdocument">The Annotations Menu</a></li>
      <li><a
        href="attaching_annotations/configuring_annotation_settings.html.es#page_body"
        rel="subdocument">Configuring Annotation Settings</a></li>
      <li><a
        href="attaching_annotations/creating_an_annotation.html.es#page_body"
        rel="subdocument">Creating an Annotation</a></li>
      <li><a
        href="attaching_annotations/deleting_an_annotation.html.es#page_body"
        rel="subdocument">Deleting an Annotation</a></li>
      <li><a
        href="attaching_annotations/loading_and_presenting_annotations.html.es#page_body"
        rel="subdocument">Loading and Presenting Annotations</a></li>
      <li><a
        href="attaching_annotations/navigating_annotations.html.es#page_body"
        rel="subdocument">Navigating Annotations</a></li>
      <li><a
        href="attaching_annotations/moving_annotations.html.es#page_body"
        rel="subdocument">Moving Annotations</a></li>
      <li><a
        href="attaching_annotations/replying_to_annotations.html.es#page_body"
        rel="subdocument">Replying to Annotations and Discussion
      Threads</a></li>
      <li><a href="attaching_annotations/configuring_icons.html.es#page_body"
        rel="subdocument">Configuring Annotation Icons</a></li>
      <li><a href="attaching_annotations/annotation_issues.html.es#page_body"
        rel="subdocument">Annotation Issues</a></li>
    </ul>
  </li>
  <li><a href="Configure.html.es#Configuring">Configuring Amaya</a> 
    <ul>
      <li><a href="configuring_amaya/configuring_amaya.html.es#page_body"
        rel="subdocument">Configuring Amaya</a></li>
      <li><a
        href="configuring_amaya/about_configuration_directory_and_file_conventions.html.es#page_body"
        rel="subdocument">About Configuration Directory and File
        Conventions</a></li>
      <li><a
        href="configuring_amaya/about_keyboard_shortcuts.html.es#page_body"
        rel="subdocument">About Keyboard Shortcuts</a></li>
      <li><a href="configuring_amaya/menu_preferences.html.es#page_body"
        rel="subdocument">Menu Preferences</a></li>
    </ul>
  </li>
  <li><a href="ShortCuts.html.es#page_body">Keyboard shortcuts</a></li>
</ul>
<hr />
<address>
  <a href="mailto:vatton@w3.org">I. Vatton</a>
</address>

<p><small><a
href="http://www.w3.org/Consortium/Legal/ipr-notice.html#Copyright">Copyright</a>
&nbsp;©&nbsp;2002 <a href="http://www.w3.org/">W3C</a> (<a
href="http://www.lcs.mit.edu/">MIT</a>, <a
href="http://www.inria.fr/">INRIA</a>, <a
href="http://www.keio.ac.jp/">Keio</a>), All Rights Reserved. W3C <a
href="http://www.w3.org/Consortium/Legal/ipr-notice.html#Legal Disclaimer">liability,</a>
<a
href="http://www.w3.org/Consortium/Legal/ipr-notice.html#W3C Trademarks">trademark</a>,
<a
href="http://www.w3.org/Consortium/Legal/copyright-documents.html">document
use</a> and <a
href="http://www.w3.org/Consortium/Legal/copyright-software.html">software
licensing</a> rules apply. Your interactions with this site are in accordance
with our <a
href="http://www.w3.org/Consortium/Legal/privacy-statement.html#Public">public</a>
and <a
href="http://www.w3.org/Consortium/Legal/privacy-statement.html#Members">Member</a>
privacy statements.</small></p>
</div>

<p></p>
</body>
</html>
