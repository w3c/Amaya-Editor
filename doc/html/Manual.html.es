<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <title>Manual de Usuario de Amaya</title>
  <meta name="GENERATOR" content="amaya 9.1, see http://www.w3.org/Amaya/" />
  <link href="style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">
<p style="text-align: center"><img src="../images/w3c_home" alt="W3C" /></p>

<div id="page_body">
<h1 style="text-align : center">Manual de Usuario de Amaya</h1>

<h4 style="text-align:center">Versión 9.0 - Diciembre de 2004<br />
<br />
<img src="../images/amaya.gif" alt="Amaya" /></h4>
<hr />
<dl>
  <dt><strong>Autores:</strong></dt>
    <dd>Irène Vatton, Vincent Quint, José Kahan, Kim Cramer, Kim Nylander,
      Kathy Rosen, Michael Spinella y Lori Caldwell LeDoux
      <p>Actualización de la documentación realizada por <a
      href="http://www.winwriters.com/"
      title="Sitio web de WinWriters">WinWriters</a></p>
    </dd>
  <dt><strong>Colaboradores:</strong></dt>
    <dd>Charles McCathieNevile, John Russell, Marja-Riitta Koivunen, Laurent
      Carcone y Bartolomé Sintes</dd>
</dl>

<p>Este manual presenta el interface de usuario de Amaya en los sistemas
operativos Unix y Windows. Tu interface de usuario puede ser ligeramente
distinto en función de tu equipo y configuración. Este manual de usuario no
está completo. Su función es ayudar a los usuarios a descubrir las
características pricipales del programa. Este documento está organizado en
forma de libro y puedes utilizar el comando <a href="MakeBook.html.es">Hacer
Libro</a> para crear e imprimir el manual entero.</p>

<p>Muchos de los comandos de <strong>Amaya</strong> son los mismos que se
emplean en los editores o procesadores de textos más habituales. Estos
comandos no se comentan aquí. Tan sólo se describen aquí las características
particulares de <strong>Amaya</strong>.</p>

<h2><a name="L92011" id="L92011">Iniciar Amaya</a></h2>

<p>Para poner en marcha <strong>Amaya</strong> en Unix, escribe:</p>
<pre>    amaya [-display host] [-profile browser|display] [document]</pre>

<p>El parámetro <code>-display host</code> es opcional. Define la pantalla
anfitrión remota en la que Amaya debe mostrar las páginas Web. Si omites este
parámetro, Amaya las mostrará en la pantalla anfitrión local.</p>

<p>El parámetro <code>-profile browser|display</code> es opcional. Permite
iniciar la aplicación en modo navegador (<code>-profile browser</code>) o en
modo display (<code>-profile display</code>).</p>

<p>El parámetro <code>[document]</code> es opcional. Es el nombre del archivo
o el URI del primer documento que <strong>Amaya</strong> debe mostrar al
empezar. Si omites este parámetro, Amaya muestra un documento por omisión. El
documento por omisión puede ser tu <em>página de inicio</em> (ver cómo <a
href="Configure.html.es">configurar tu Página de inicio</a>) o la página de
bienvenida de Amaya.</p>
<hr />

<h1>Contenido</h1>
<ul>
  <li><a href="Browsing.html.es#page_body" rel="subdocument">Navegar con
    Amaya</a>
    <ul>
      <li><a
        href="browsing/working_with_the_amaya_browser_and_editor.html.es#page_body"
        rel="subdocument">Trabajar con el Navegador y Editor Amaya</a></li>
      <li><a href="browsing/opening_documents.html.es#page_body"
        rel="subdocument">Abrir documentos</a></li>
      <li><a href="browsing/activating_a_link.html.es#page_body"
        rel="subdocument">Activar un Enlace</a></li>
      <li><a href="browsing/about_target_anchors.html.es#page_body"
        rel="subdocument">Acerca de Destinos</a></li>
      <li><a
        href="browsing/about_moving_backward_and_forward.html.es#page_body"
        rel="subdocument">Acerca de Ir Adelante o Atrás</a></li>
      <li><a href="browsing/reloading_a_page.html.es#page_body"
        rel="subdocument">Recargar una página</a></li>
      <li><a href="browsing/about_access_keys.html.es#page_body"
        rel="subdocument">Acerca de las Teclas de Acceso</a></li>
      <li><a href="browsing/forms.html.es#page_body"
        rel="subdocument">Navegar formularios</a></li>
    </ul>
  </li>
  <li><a href="Selecting.html.es#page_body" rel="subdocument">Seleccionar</a>
    <ul>
      <li><a
        href="selecting/selecting_with_keyboard_and_mouse.html.es#page_body"
        rel="subdocument">Seleccionar con el Teclado y el Ratón</a></li>
      <li><a href="selecting/selecting_images.html.es#page_body"
        rel="subdocument">Seleccionar imágenes</a></li>
      <li><a href="selecting/selecting_by_structure.html.es#page_body"
        rel="subdocument">Seleccionar por Estructura</a></li>
    </ul>
  </li>
  <li><a href="Views.html.es#page_body" rel="subdocument">Ver documentos en
    Amaya</a>
    <ul>
      <li><a href="viewing/working_with_document_views.html.es#page_body"
        rel="subdocument">Utilizar las Vistas de Documento</a></li>
      <li><a href="viewing/opening_a_view.html.es#page_body"
        rel="subdocument">Abrir una Vista</a></li>
      <li><a href="viewing/closing_a_view.html.es#page_body"
        rel="subdocument">Cerrar una Vista</a></li>
      <li><a href="viewing/about_synchronized_views.html.es#page_body"
        rel="subdocument">Acerca de las Vistas sincronizadas</a></li>
    </ul>
  </li>
  <li><a href="Document.html.es#page_body" rel="subdocument">Editar
    Documentos</a>
    <ul>
      <li><a
        href="editing_documents/creating_new_documents.html.es#page_body"
        rel="subdocument">Crear nuevos documentos</a></li>
    </ul>
  </li>
  <li><a href="Creating.html.es#page_body" rel="subdocument">Crear nuevos
    elementos</a>
    <ul>
      <li><a href="elements/buttons_and_types_menu.html.es#page_body"
        rel="subdocument">Botones y Menús XHTML y XML</a></li>
      <li><a href="elements/the_structure_menu.html.es#page_body"
        rel="subdocument">Menú Estructura</a></li>
      <li><a href="elements/the_enter_key.html.es#page_body"
        rel="subdocument">La tecla Intro</a></li>
      <li><a href="elements/exiting_anchors.html.es#page_body"
        rel="subdocument">Salir de anclas, cadenas con estilo, etc</a></li>
      <li><a href="elements/creating_nested_structures.html.es#page_body"
        rel="subdocument">Crear estructuras anidadas</a></li>
      <li><a href="elements/changing_the_document_title.html.es#page_body"
        rel="subdocument">Cambiar el título del documento</a></li>
    </ul>
  </li>
  <li><a href="Attributes.html.es#page_body" rel="subdocument">Editar
    atributos</a>
    <ul>
      <li><a rel="subdocument"
        href="editing_attributes/working_with_attributes.html.es#page_body">Trabajar
        con atributos</a></li>
      <li><a
        href="editing_attributes/editing_attributes_through_the_attributes_menu.html.es#page_body"
        rel="subdocument">Editar atributos con el menú Atributos</a></li>
      <li><a href="editing_attributes/copying_attributes.html.es#page_body"
        rel="subdocument">Copiar atributos</a></li>
      <li><a
        href="editing_attributes/editing_attributes_in_the_structure_view.html.es#page_body"
        rel="subdocument">Editar atributos en la vista Estructura</a></li>
    </ul>
  </li>
  <li><a href="EditChar.html.es#page_body" rel="subdocument">Escribir
    caracteres Unicode e ISO-Latin</a>
    <ul>
      <li><a
        href="editing_iso-latin-1_characters/entering_ISOLatin1_characters_in_amaya.html.es#page_body"
        rel="subdocument">Escribir caracteres</a></li>
      <li><a
        href="editing_iso-latin-1_characters/about_standard_multikey_support.html.es#page_body"
        rel="subdocument">Combinaciones de teclas en las versiones Motif y
        GTK</a></li>
      <li><a
        href="editing_iso-latin-1_characters/about_white_space_handling.html.es#page_body"
        rel="subdocument">Acerca de los espacios en blanco</a></li>
    </ul>
  </li>
  <li><a href="HTML.html.es#page_body" rel="subdocument">Elementos HTML</a>
    <ul>
      <li><a href="HTML-elements/infoTypes.html.es#page_body"
        rel="subdocument">Tipos de información en HTML</a></li>
      <li><a href="HTML-elements/structure.html.es#page_body"
        rel="subdocument">Estructura de un documento HTML</a></li>
      <li><a href="HTML-elements/headings.html.es#page_body"
        rel="subdocument">Elementos de encabezado</a></li>
      <li><a href="HTML-elements/lists.html.es#page_body"
        rel="subdocument">Elementos de lista</a></li>
      <li><a href="HTML-elements/address.html.es#page_body"
        rel="subdocument">Acerca del autor</a></li>
      <li><a href="HTML-elements/inline.html.es#page_body"
        rel="subdocument">Marcado en línea</a></li>
      <li><a href="HTML-elements/40styling.html.es#page_body"
        rel="subdocument">Elementos de estilo de carácter en HTML</a></li>
    </ul>
  </li>
  <li><a href="Changing.html.es#page_body" rel="subdocument">Reestructurar
    documentos</a>
    <ul>
      <li><a
        href="editing_documents/changing_the_document_structure.html.es#page_body"
        rel="subdocument">Cambiar la estructura del documento</a></li>
      <li><a href="editing_documents/the_transform_command.html.es#page_body"
        rel="subdocument">El comando Transformar</a></li>
      <li><a
        href="editing_documents/about_inserting_a_division.html.es#page_body"
        rel="subdocument">Insertar una división</a></li>
      <li><a
        href="editing_documents/about_merging_elements.html.es#page_body"
        rel="subdocument">Unir elementos</a></li>
      <li><a href="Transform.html.es#page_body" rel="subdocument">Utilizar el
        archivo HTML.trans</a></li>
    </ul>
  </li>
  <li><a href="Links.html.es#page_body" rel="subdocument">Enlazar</a>
    <ul>
      <li><a href="linking/creating_a_target_anchor.html.es#page_body"
        rel="subdocument">Crear un destino</a></li>
      <li><a href="linking/creating_a_target_element.html.es#page_body"
        rel="subdocument">Crear un Elemento Destino</a></li>
      <li><a href="linking/creating_an_external_link.html.es#page_body"
        rel="subdocument">Crear un Enlace Externo</a></li>
      <li><a href="linking/creating_an_internal_link.html.es#page_body"
        rel="subdocument">Crear un Enlace Interno</a></li>
      <li><a href="linking/changing_a_link.html.es#page_body"
        rel="subdocument">Cambiar un enlace</a></li>
      <li><a
        href="linking/removing_a_link_or_a_target_anchor.html.es#page_body"
        rel="subdocument">Eliminar un Enlace o Destino</a></li>
    </ul>
  </li>
  <li><a href="Tables.html.es#page_body" rel="subdocument">Editar tablas</a>
    <ul>
      <li><a href="editing_tables/working_with_tables.html.es#page_body"
        rel="subdocument">Trabajar con tablas</a></li>
      <li><a href="editing_tables/create_table.html.es#page_body"
        rel="subdocument">Crear una tabla</a></li>
      <li><a href="editing_tables/add_column.html.es#page_body"
        rel="subdocument">Añadir columnas</a></li>
      <li><a href="editing_tables/add_row.html.es#page_body"
        rel="subdocument">Añadir filas</a></li>
      <li><a href="editing_tables/add_tbody.html.es#page_body"
        rel="subdocument">Añadir cuerpos de tabla</a></li>
      <li><a href="editing_tables/remove_column.html.es#page_body"
        rel="subdocument">Eliminar/Copiar/Pegar columnas</a></li>
    </ul>
  </li>
  <li><a href="ImageMaps.html.es#page_body" rel="subdocument">Utilizar
    imágenes y mapas de imagen</a>
    <ul>
      <li><a
        href="using_image_maps/working_with_image_maps.html.es#page_body"
        rel="subdocument">Trabajar con mapas de imagen</a></li>
      <li><a href="using_image_maps/adding_an_image.html.es#page_body"
        rel="subdocument">Insertar o sustituir imágenes</a></li>
      <li><a
        href="using_image_maps/creating_clientside_image_maps.html.es#page_body"
        rel="subdocument">Crear mapas de imagen cliente</a></li>
      <li><a
        href="using_image_maps/using_clientside_image_maps.html.es#page_body"
        rel="subdocument">Usar mapas de imagen cliente</a></li>
      <li><a href="using_image_maps/moving_image_map_areas.html.es#page_body"
        rel="subdocument">Mover áreas de mapas de imagen</a></li>
      <li><a
        href="using_image_maps/resizing_image_map_areas.html.es#page_body"
        rel="subdocument">Cambiar el tamaño de las áreas de mapa de
      imagen</a></li>
      <li><a
        href="using_image_maps/about_alternative_text_and_long_descriptions.html.es#page_body"
        rel="subdocument">Texto alternativo y descripciones largas</a></li>
    </ul>
  </li>
  <li><a href="Math.html.es#page_body" rel="subdocument">Editar MathML</a>
    <ul>
      <li><a
        href="editing_mathematics/working_with_math_expressions.html.es#page_body"
        rel="subdocument">Trabajar con expresiones matemáticas</a></li>
      <li><a
        href="editing_mathematics/about_entering_math_constructs_using_the_keyboard.html.es#page_body"
        rel="subdocument">Crear expresiones matemáticas con el
      teclado</a></li>
      <li><a
        href="editing_mathematics/the_math_palette_and_the_types.html.es#page_body"
        rel="subdocument">La paleta MathML y el menú XML &gt; MathML</a></li>
      <li><a
        href="editing_mathematics/about_entering_math_characters.html.es#page_body"
        rel="subdocument">Escribir caracteres matemáticos</a></li>
      <li><a
        href="editing_mathematics/editing_math_expressions.html.es#page_body"
        rel="subdocument">Editar expresiones matemáticas</a></li>
      <li><a
        href="editing_mathematics/viewing_structure_in_mathml.html.es#page_body"
        rel="subdocument">Ver la estructura en MathML</a></li>
      <li><a
        href="editing_mathematics/about_linking_in_mathml.html.es#page_body"
        rel="subdocument">Enlazar en MathML</a></li>
      <li><a href="editing_mathematics/math_issues.html.es#page_body"
        rel="subdocument">Problemas conocidos en relación con MathML</a></li>
    </ul>
  </li>
  <li><a href="SVG.html.es#page_body" rel="subdocument">Editar gráficos</a>
    <ul>
      <li><a href="using_graphics/working_with_graphics.html.es#page_body"
        rel="subdocument">Trabajar con gráficos</a></li>
      <li><a
        href="using_graphics/creating_graphics_with_the_palette.html.es#page_body"
        rel="subdocument">Crear gráficos con la paleta</a></li>
      <li><a href="using_graphics/moving_graphics.html.es#page_body"
        rel="subdocument">Mover gráficos</a></li>
      <li><a href="using_graphics/painting_graphics.html.es#page_body"
        rel="subdocument">Colorear gráficos</a></li>
      <li><a href="using_graphics/resizing_graphics.html.es#page_body"
        rel="subdocument">Cambiar el tamaño de los gráficos</a></li>
      <li><a href="using_graphics/SVGLibrary.html.es#page_body"
        rel="subdocument">Gestionar la biblioteca SVG</a>
        <ul>
          <li><a href="using_graphics/UseSVGLib.html.es#page_body"
            rel="subdocument">Usar la biblioteca SVG</a></li>
          <li><a href="using_graphics/AddSVGLib.html.es#page_body"
            rel="subdocument">Añadir un nuevo modelo en una
          biblioteca</a></li>
          <li><a
            href="using_graphics/ChangeSVGLibPresentation.html.es#page_body"
            rel="subdocument">Cómo cambiar la presentación de la biblioteca
            SVG</a></li>
        </ul>
      </li>
    </ul>
  </li>
  <li><a href="Xml.html.es#page_body" rel="subdocument">Soporte de XML</a>
    <ul>
      <li><a href="using_xml/loading_xml_document.html.es#page_body"
        rel="subdocument">Cargar documentos XML</a></li>
      <li><a href="using_xml/xml_using_style.html.es#page_body"
        rel="subdocument">Utilizar estilos en XML</a></li>
      <li><a href="using_xml/editing_xml_document.html.es#page_body"
        rel="subdocument">Editar documentos XML</a></li>
    </ul>
  </li>
  <li><a href="StyleSheets.html.es#page_body" rel="subdocument">Hojas de
    estilo</a>
    <ul>
      <li><a href="style_sheets/working_with_styles.html.es#page_body"
        rel="subdocument">Trabajar con estilos</a></li>
      <li><a
        href="style_sheets/about_applying_style_using_html_elements.html.es#page_body"
        rel="subdocument">Aplicar estilos utilizando elementos HTML</a></li>
      <li><a
        href="style_sheets/about_linking_external_and_user_style_sheets.html.es#page_body"
        rel="subdocument">Enlazar hojas de estilo externas y de
      usuario</a></li>
      <li><a
        href="style_sheets/creating_and_updating_a_style_attribute.html.es#page_body"
        rel="subdocument">Crear y actualizar un atributo de estilo</a></li>
      <li><a
        href="style_sheets/creating_generic_style_for_an_element_or_a_class.html.es#page_body"
        rel="subdocument">Crear un estilo genérico para un elemento o
        clase</a></li>
      <li><a
        href="style_sheets/creating_html_style_elements.html.es#page_body"
        rel="subdocument">Crear elementos de estilo HTML</a></li>
      <li><a href="style_sheets/handling_external_css.html.es#page_body"
        rel="subdocument">Gestionar hojas de estilo externas</a></li>
      <li><a href="style_sheets/removing_style.html.es#page_body"
        rel="subdocument">Eliminar estilos</a></li>
    </ul>
  </li>
  <li><a href="Searching.html.es#page_body" rel="subdocument">Buscar y
    reemplazar texto</a>
    <ul>
      <li><a
        href="searching_and_replacing_text/searching_and_replacing_text.html.es#page_body"
        rel="subdocument">Buscar y reemplazar texto</a></li>
    </ul>
  </li>
  <li><a href="SpellChecking.html.es#page_body" rel="subdocument">Corrector
    ortográfico</a>
    <ul>
      <li><a href="spell_checking/spell_checking.html.es#page_body"
        rel="subdocument">Corrector ortográfico</a></li>
    </ul>
  </li>
  <li><a href="Publishing.html.es#page_body" rel="subdocument">Guardar y
    publicar</a>
    <ul>
      <li><a
        href="saving_and_publishing_documents/saving_and_publishing_documents.html.es#page_body"
        rel="subdocument">Guardar y publicar documentos</a></li>
      <li><a
        href="saving_and_publishing_documents/about_saving_in_html.html.es#page_body"
        rel="subdocument">Guardar como HTML</a></li>
      <li><a
        href="saving_and_publishing_documents/saving_documents_as_text.html.es#page_body"
        rel="subdocument">Guardar documentos como texto</a></li>
      <li><a
        href="saving_and_publishing_documents/the_save_command.html.es#page_body"
        rel="subdocument">El comando Guardar</a></li>
      <li><a
        href="saving_and_publishing_documents/the_saveas_command.html.es#page_body"
        rel="subdocument">El comando Guardar como</a></li>
    </ul>
  </li>
  <li><a href="WebDAV.html.es#page_body" rel="subdocument">Funciones
    WebDAV</a></li>
  <li><a href="Printing.html.es#page_body" rel="subdocument">Imprimir
    documentos</a>
    <ul>
      <li><a href="printing/printing_documents.html.es#page_body"
        rel="subdocument">Imprimir documentos</a></li>
      <li><a
        href="printing/using_the_setup_and_print_command.html.es#page_body"
        rel="subdocument">El comando Configurar e imprimir</a></li>
      <li><a href="printing/print_command.html.es#page_body"
        rel="subdocument">El comando Imprimir</a></li>
      <li><a href="printing/windows_platforms.html.es#page_body"
        rel="subdocument">Imprimir en Windows</a></li>
      <li><a href="printing/unix_platforms.html.es#page_body"
        rel="subdocument">Imprimir en Unix</a></li>
      <li><a
        href="printing/about_controlling_printing_with_css.html.es#page_body"
        rel="subdocument">Controlar la impresión con CSS</a></li>
    </ul>
  </li>
  <li><a href="Numbering.html.es#page_body" rel="subdocument">Numerar
    encabezados y generar Índice de materias</a>
    <ul>
      <li><a href="Numbering.html.es#page_body" rel="subdocument">Numerar
        encabezados y generar Índice de materias</a></li>
    </ul>
  </li>
  <li><a href="MakeBook.html.es#page_body" rel="subdocument">Unir varios
    documentos</a>
    <ul>
      <li><a
        href="using_the_makebook_function/assembling_large_document_collections.html.es#page_body"
        rel="subdocument">Unir varios documentos</a></li>
    </ul>
  </li>
  <li><a href="Annotations.html.es#page_body"
    rel="subdocument">Anotaciones</a>
    <ul>
      <li><a
        href="attaching_annotations/what_is_an_annotation.html.es#page_body"
        rel="subdocument">¿Qué es una anotación?</a></li>
      <li><a
        href="attaching_annotations/about_reading_annotations.html.es#page_body"
        rel="subdocument">Leer anotaciones</a></li>
      <li><a
        href="attaching_annotations/about_storing_local_annotations.html.es#page_body"
        rel="subdocument">Anotaciones locales</a></li>
      <li><a
        href="attaching_annotations/about_storing_remote_annotations.html.es#page_body"
        rel="subdocument">Anotaciones remotas</a></li>
      <li><a href="attaching_annotations/annotation_menu.html.es#page_body"
        rel="subdocument">Menú Anotaciones</a></li>
      <li><a
        href="attaching_annotations/configuring_annotation_settings.html.es#page_body"
        rel="subdocument">Configuración de anotaciones</a></li>
      <li><a
        href="attaching_annotations/creating_an_annotation.html.es#page_body"
        rel="subdocument">Crear una anotación</a></li>
      <li><a
        href="attaching_annotations/deleting_an_annotation.html.es#page_body"
        rel="subdocument">Borrar una anotación</a></li>
      <li><a
        href="attaching_annotations/loading_and_presenting_annotations.html.es#page_body"
        rel="subdocument">Cargar y mostrar anotaciones</a></li>
      <li><a
        href="attaching_annotations/navigating_annotations.html.es#page_body"
        rel="subdocument">Navegar por las anotaciones</a></li>
      <li><a
        href="attaching_annotations/moving_annotations.html.es#page_body"
        rel="subdocument">Mover anotaciones</a></li>
      <li><a
        href="attaching_annotations/replying_to_annotations.html.es#page_body"
        rel="subdocument">Contestar anotaciones e hilos de discusión</a></li>
      <li><a href="attaching_annotations/configuring_icons.html.es#page_body"
        rel="subdocument">Configurar los iconos de anotación</a></li>
      <li><a href="attaching_annotations/annotation_issues.html.es#page_body"
        rel="subdocument">Problemas conocidos con las anotaciones</a></li>
    </ul>
  </li>
  <li><a href="bookmarks.html.es#page_body"
  rel="subdocument">Marcadores</a></li>
  <li><a href="Configure.html.es#page_body" rel="subdocument">Configurar
    Amaya</a>
    <ul>
      <li><a href="configuring_amaya/configuring_amaya.html.es#page_body"
        rel="subdocument">Configurar Amaya</a></li>
      <li><a
        href="configuring_amaya/about_configuration_directory_and_file_conventions.html.es#page_body"
        rel="subdocument">Directorio de configuración y convenciones de
        archivo</a></li>
      <li><a href="configuring_amaya/menu_preferences.html.es#page_body"
        rel="subdocument">Menú Preferencias</a></li>
    </ul>
  </li>
  <li><a href="ShortCuts.html.es#page_body" rel="subdocument">Atajos de
    teclado</a>
    <ul>
      <li><a
        href="configuring_amaya/about_keyboard_shortcuts.html.es#page_body"
        rel="subdocument">Atajos de teclado</a></li>
    </ul>
  </li>
  <li><a href="Access.html.es#page_body" rel="subdocument">Accesibilidad en
    Amaya</a>
    <ul>
      <li><a
        href="accessibility_in_amaya/accessibility_in_amaya.html.es#page_body"
        rel="subdocument">Accesibilidad en Amaya</a></li>
      <li><a
        href="accessibility_in_amaya/about_amayas_accessibility_features.html.es#page_body"
        rel="subdocument">Acerca de las características de accesibilidad de
        Amaya</a></li>
      <li><a
        href="accessibility_in_amaya/about_producing_accessible_content.html.es#page_body"
        rel="subdocument">Crear contenido accesible</a></li>
    </ul>
  </li>
</ul>
<hr />

<p class="policyfooter"><small>Copyright  © 1994-2005 <a
href="http://www.inria.fr/">INRIA</a> and <a
href="http://www.w3.org/">W3C</a><sup>®</sup>(<a
href="http://www.lcs.mit.edu/">MIT</a>, <a
href="http://www.ercim.org/">ERCIM</a>, <a
href="http://www.keio.ac.jp/">Keio</a>), All Rights Reserved. W3C <a
href="http://www.w3.org/Consortium/Legal/ipr-notice.html#Legal_Disclaimer">liability,</a>
<a
href="http://www.w3.org/Consortium/Legal/ipr-notice.html#W3C_Trademarks">trademark</a>,
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
</body>
</html>
