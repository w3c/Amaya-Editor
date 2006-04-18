<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="content-type" content="text/html; charset=windows-1251" />
  <title>Справочное руководство пользователя Amaya</title>
  <meta name="GENERATOR" content="amaya 9.2.2, see http://www.w3.org/Amaya/"
  />
  <link href="style.css" rel="stylesheet" type="text/css" />
  <style type="text/css">
  </style>
</head>

<body xml:lang="ru" lang="ru">
<p style="text-align: center"><img src="../images/w3c_home" alt="W3C" /></p>

<div id="page_body">
<h1 style="text-align : center">Справочное руководство пользователя Amaya</h1>

<h4 style="text-align:center">Версия 8.8 - Февраль 2004<br />
<img src="../images/amaya.gif" alt="Amaya" /></h4>
<hr />
<dl>
  <dt><strong>Авторы:</strong></dt>
    <dd>Ir&#xe8;ne Vatton, Vincent Quint, Jos&#xe9; Kahan, Kim Cramer, Kim
      Nylander, Kathy Rosen, Michael Spinella, и Lori Caldwell LeDoux.
      <p>Обновлением документации занимается <a
      href="http://www.winwriters.com/"
      title="WinWriters web site">WinWriters</a>.</p>
    </dd>
  <dt><strong>Распространители:</strong></dt>
    <dd>Charles McCathieNevile, John Russell, Marja-Riitta Koivunen, Laurent
      Carcone, Bartolom&#xe9; Sintes</dd>
</dl>

<p>Это руководство представляет вашему вниманию пользовательский интерфейс
Amaya для Unix и Windows платформ. Интерфейс вашей программы может иметь
некоторые отличия от описываемого здесь, в зависимости от вашей операционной
системы или ваших настроек конфигурации. Представленное вашему вниманию
руководство не является полным. Оно предназначено для объяснения основных и
самых важных возможностей программы. Этот документ организован как книга, для
её создания и вывода на печать вы можете использовать команду <a
href="MakeBook.html.ru">Создать книгу</a>.</p>

<p>Многие команды в <strong>Amaya</strong> те же самые что и в других
общеизвестных браузерах и редакторах. Они не представлены здесь. Здесь
представлены только специфические аспекты работы с <strong>Amaya</strong>.</p>

<h2><a name="L92011" id="L92011">Запуск Amaya</a></h2>

<p>Для запуска <strong>Amaya</strong> на Unix платформе, введите:</p>
<pre>    amaya [-display host] [document]</pre>

<p>Параметр [-display host] дополнительный. Он определяет главный экран, т.е.
где Amaya должна выводить веб страницы. Если вы пропустите этот параметр,
Amaya будет выводить на экране локального компьютера.</p>

<p>Параметр <code>[document]</code> дополнительный. Это имя файла или URI
первого документа <strong>Amaya</strong> которое будет выводится при запуске
программы. Если вы пропустите этот параметр, Amaya будет выводить документ,
принятый по умолчанию. По умолчанию, этим документом может быть ваша
<em>домашняя страница</em> (смотрите как <a
href="Configure.html.ru">конфигурировать домашнюю страницу</a>) или страница
приветствия Amaya.</p>
<hr />

<h1>Оглавление</h1>
<ul>
  <li><a href="Browsing.html.ru#page_body" rel="subdocument">Просмотр с
    помощью Amaya</a>
    <ul>
      <li><a
        href="browsing/working_with_the_amaya_browser_and_editor.html.ru#page_body"
        rel="subdocument">Работа с браузером и редактором Amaya</a></li>
      <li><a href="browsing/opening_documents.html.ru#page_body"
        rel="subdocument">Открытие документов</a></li>
      <li><a href="browsing/activating_a_link.html.ru#page_body"
        rel="subdocument">Активация ссылки</a></li>
      <li><a href="browsing/about_target_anchors.html.ru#page_body"
        rel="subdocument">Об якорях цели</a></li>
      <li><a
        href="browsing/about_moving_backward_and_forward.html.ru#page_body"
        rel="subdocument">О перемещении Назад и Вперёд</a></li>
      <li><a href="browsing/reloading_a_page.html.ru#page_body"
        rel="subdocument">Обновление страницы</a></li>
      <li><a href="browsing/about_access_keys.html.ru#page_body"
        rel="subdocument">О ключах доступа</a></li>
      <li><a href="browsing/forms.html.ru#page_body"
        rel="subdocument">Просмотр форм</a></li>
    </ul>
  </li>
  <li><a href="Selecting.html.ru#page_body" rel="subdocument">Выбор</a>
    <ul>
      <li><a
        href="selecting/selecting_with_keyboard_and_mouse.html.ru#page_body"
        rel="subdocument">Выбор с помощью клавиатуры и мыши</a></li>
      <li><a href="selecting/selecting_images.html.ru#page_body"
        rel="subdocument">Выбор изображений</a></li>
      <li><a href="selecting/selecting_by_structure.html.ru#page_body"
        rel="subdocument">Выбор по структуре</a></li>
    </ul>
  </li>
  <li><a href="Views.html.ru#page_body" rel="subdocument">Виды документа в
    Amaya</a>
    <ul>
      <li><a href="viewing/working_with_document_views.html.ru#page_body"
        rel="subdocument">Работа с видами документа</a></li>
      <li><a href="viewing/opening_a_view.html.ru#page_body"
        rel="subdocument">Открыть вид</a></li>
      <li><a href="viewing/closing_a_view.html.ru#page_body"
        rel="subdocument">Закрыть вид</a></li>
      <li><a href="viewing/about_synchronized_views.html.ru#page_body"
        rel="subdocument">О синхронизации видов</a></li>
    </ul>
  </li>
  <li><a href="Document.html.ru#page_body" rel="subdocument">Редактирование
    документов</a>
    <ul>
      <li><a
        href="editing_documents/creating_new_documents.html.ru#page_body"
        rel="subdocument">Создание новых документов</a></li>
    </ul>
  </li>
  <li><a href="Creating.html.ru#page_body" rel="subdocument">Создание новых
    элементов</a>
    <ul>
      <li><a href="elements/buttons_and_types_menu.html.ru#page_body"
        rel="subdocument">Кнопки и меню Тип информации</a></li>
      <li><a href="elements/the_structure_menu.html.ru#page_body"
        rel="subdocument">Меню Структура</a></li>
      <li><a href="elements/the_enter_key.html.ru#page_body"
        rel="subdocument">Кнопка Enter</a></li>
      <li><a href="elements/exiting_anchors.html.ru#page_body"
        rel="subdocument">Выход из якорей, стилизованных символьных строк и
        прочее</a></li>
      <li><a href="elements/creating_nested_structures.html.ru#page_body"
        rel="subdocument">Создание вложенных структур</a></li>
      <li><a href="elements/changing_the_document_title.html.ru#page_body"
        rel="subdocument">Изменение заголовка документа</a></li>
    </ul>
  </li>
  <li><a href="Attributes.html.ru#page_body" rel="subdocument">Редактирование
    атрибутов</a>
    <ul>
      <li><a
        href="editing_attributes/working_with_attributes.html.ru#page_body"
        rel="subdocument">Работа с атрибутами</a></li>
      <li><a
        href="editing_attributes/editing_attributes_through_the_attributes_menu.html.ru#page_body"
        rel="subdocument">Редактирование атрибутов через меню
      Атрибуты</a></li>
      <li><a href="editing_attributes/copying_attributes.html.ru#page_body"
        rel="subdocument">Копирование атрибутов</a></li>
      <li><a
        href="editing_attributes/editing_attributes_in_the_structure_view.html.ru#page_body"
        rel="subdocument">Редактирование атрибутов в виде Структура</a></li>
    </ul>
  </li>
  <li><a href="EditChar.html.ru#page_body" rel="subdocument">Редактирование
    ISO-Latin и Unicode символов</a>
    <ul>
      <li><a
        href="editing_iso-latin-1_characters/entering_ISOLatin1_characters_in_amaya.html.ru#page_body"
        rel="subdocument">Ввод символов в Amaya</a></li>
      <li><a
        href="editing_iso-latin-1_characters/about_standard_multikey_support.html.ru#page_body"
        rel="subdocument">О поддержке стандартных комбинаций кнопок</a></li>
      <li><a
        href="editing_iso-latin-1_characters/about_white_space_handling.html.ru#page_body"
        rel="subdocument">Об обработке пробельных символов</a></li>
    </ul>
  </li>
  <li><a href="HTML.html.ru#page_body" rel="subdocument">HTML элементы</a>
    <ul>
      <li><a href="HTML-elements/infoTypes.html.ru#page_body"
        rel="subdocument">Информационные типы в HTML</a></li>
      <li><a href="HTML-elements/structure.html.ru#page_body"
        rel="subdocument">Структура HTML документа</a></li>
      <li><a href="HTML-elements/headings.html.ru#page_body"
        rel="subdocument">Элементы Заголовок</a></li>
      <li><a href="HTML-elements/lists.html.ru#page_body"
        rel="subdocument">Список элементов</a></li>
      <li><a href="HTML-elements/address.html.ru#page_body"
        rel="subdocument">Об авторе</a></li>
      <li><a href="HTML-elements/inline.html.ru#page_body"
        rel="subdocument">Встроенная маркировка</a></li>
      <li><a href="HTML-elements/40styling.html.ru#page_body"
        rel="subdocument">Элементы стиля символов в HTML</a></li>
    </ul>
  </li>
  <li><a href="Changing.html.ru#page_body" rel="subdocument">Реструктуризация
    документов</a>
    <ul>
      <li><a
        href="editing_documents/changing_the_document_structure.html.ru#page_body"
        rel="subdocument">Изменение структуры документа</a></li>
      <li><a href="editing_documents/the_transform_command.html.ru#page_body"
        rel="subdocument">Команда Преобразовать</a></li>
      <li><a
        href="editing_documents/about_inserting_a_division.html.ru#page_body"
        rel="subdocument">О вставке элемента группирования</a></li>
      <li><a
        href="editing_documents/about_merging_elements.html.ru#page_body"
        rel="subdocument">О слиянии элементов</a></li>
      <li><a href="Transform.html.ru#page_body"
        rel="subdocument">Использование файла HTML.trans</a></li>
    </ul>
  </li>
  <li><a href="Links.html.ru#page_body" rel="subdocument">Связи и ссылки</a>
    <ul>
      <li><a href="linking/creating_a_target_anchor.html.ru#page_body"
        rel="subdocument">Создание якоря цели</a></li>
      <li><a href="linking/creating_a_target_element.html.ru#page_body"
        rel="subdocument">Создание элемента Цель</a></li>
      <li><a href="linking/creating_an_external_link.html.ru#page_body"
        rel="subdocument">Создание внешней ссылки</a></li>
      <li><a href="linking/creating_an_internal_link.html.ru#page_body"
        rel="subdocument">Создание внутренней ссылки</a></li>
      <li><a href="linking/changing_a_link.html.ru#page_body"
        rel="subdocument">Изменение ссылки</a></li>
      <li><a
        href="linking/removing_a_link_or_a_target_anchor.html.ru#page_body"
        rel="subdocument">Удаление ссылки или якоря цели</a></li>
    </ul>
  </li>
  <li><a href="Tables.html.ru#page_body" rel="subdocument">Редактирование
    таблиц</a>
    <ul>
      <li><a href="editing_tables/working_with_tables.html.ru#page_body"
        rel="subdocument">Работа с таблицами</a></li>
      <li><a href="editing_tables/create_table.html.ru#page_body"
        rel="subdocument">Создание таблицы</a></li>
      <li><a href="editing_tables/add_column.html.ru#page_body"
        rel="subdocument">Добавление столбцов</a></li>
      <li><a href="editing_tables/add_row.html.ru#page_body"
        rel="subdocument">Добавление строк</a></li>
      <li><a href="editing_tables/add_tbody.html.ru#page_body"
        rel="subdocument">Добавление нового тела таблицы (tbody)</a></li>
      <li><a href="editing_tables/remove_column.html.ru#page_body"
        rel="subdocument">Удаление/Копирование/Вставка столбцов</a></li>
    </ul>
  </li>
  <li><a href="ImageMaps.html.ru#page_body" rel="subdocument">Использование
    изображения и карт</a>
    <ul>
      <li><a
        href="using_image_maps/working_with_image_maps.html.ru#page_body"
        rel="subdocument">Работа с изображением карты</a></li>
      <li><a href="using_image_maps/adding_an_image.html.ru#page_body"
        rel="subdocument">Добавление или замещение изображения</a></li>
      <li><a
        href="using_image_maps/creating_clientside_image_maps.html.ru#page_body"
        rel="subdocument">Создание клиентского изображения карты</a></li>
      <li><a
        href="using_image_maps/using_clientside_image_maps.html.ru#page_body"
        rel="subdocument">Использование клиентского изображения карты</a></li>
      <li><a href="using_image_maps/moving_image_map_areas.html.ru#page_body"
        rel="subdocument">Перемещение областей изображения карты</a></li>
      <li><a
        href="using_image_maps/resizing_image_map_areas.html.ru#page_body"
        rel="subdocument">Изменение размеров областей изображения
      карты</a></li>
      <li><a
        href="using_image_maps/about_alternative_text_and_long_descriptions.html.ru#page_body"
        rel="subdocument">Об альтернативном тексте и длинных
      описаниях</a></li>
    </ul>
  </li>
  <li><a href="Math.html.ru#page_body" rel="subdocument">Редактирование
    математики</a>
    <ul>
      <li><a
        href="editing_mathematics/working_with_math_expressions.html.ru#page_body"
        rel="subdocument">Работа с математическими выражениями</a></li>
      <li><a
        href="editing_mathematics/about_entering_math_constructs_using_the_keyboard.html.ru#page_body"
        rel="subdocument">О вводе математических конструкций с помощью
        клавиатуры</a></li>
      <li><a
        href="editing_mathematics/the_math_palette_and_the_types.html.ru#page_body"
        rel="subdocument">Кнопка Математика и меню Математика</a></li>
      <li><a
        href="editing_mathematics/about_entering_math_characters.html.ru#page_body"
        rel="subdocument">О вводе математических символов</a></li>
      <li><a
        href="editing_mathematics/editing_math_expressions.html.ru#page_body"
        rel="subdocument">Редактирование математических выражений</a></li>
      <li><a
        href="editing_mathematics/viewing_structure_in_mathml.html.ru#page_body"
        rel="subdocument">Просмотр структуры в MathML</a></li>
      <li><a
        href="editing_mathematics/about_linking_in_mathml.html.ru#page_body"
        rel="subdocument">О ссылках в MathML</a></li>
      <li><a href="editing_mathematics/math_issues.html.ru#page_body"
        rel="subdocument">Известные проблемы в MathML</a></li>
    </ul>
  </li>
  <li><a href="SVG.html.ru#page_body" rel="subdocument">Редактирование
    графики</a>
    <ul>
      <li><a href="using_graphics/working_with_graphics.html.ru#page_body"
        rel="subdocument">Работа с графикой</a></li>
      <li><a
        href="using_graphics/creating_graphics_with_the_palette.html.ru#page_body"
        rel="subdocument">Создание графики с помощью таблицы</a></li>
      <li><a href="using_graphics/moving_graphics.html.ru#page_body"
        rel="subdocument">Перемещение графики</a></li>
      <li><a href="using_graphics/painting_graphics.html.ru#page_body"
        rel="subdocument">Раскраска графики</a></li>
      <li><a href="using_graphics/resizing_graphics.html.ru#page_body"
        rel="subdocument">Изменение размеров графики</a></li>
      <li><a href="using_graphics/SVGLibrary.html.ru#page_body"
        rel="subdocument">Управление SVG библиотекой</a>
        <ul>
          <li><a href="using_graphics/UseSVGLib.html.ru#page_body"
            rel="subdocument">Использование SVG библиотеки</a></li>
          <li><a href="using_graphics/AddSVGLib.html.ru#page_body"
            rel="subdocument">Добавление новой модели в библиотеку</a></li>
          <li><a
            href="using_graphics/ChangeSVGLibPresentation.html.ru#page_body"
            rel="subdocument">Как изменить представление SVG
          библиотеки</a></li>
        </ul>
      </li>
    </ul>
  </li>
  <li><a href="Xml.html.ru#page_body" rel="subdocument">Поддержка Xml</a>
    <ul>
      <li><a href="using_xml/loading_xml_document.html.ru#page_body"
        rel="subdocument">Загрузка XML документов</a></li>
      <li><a href="using_xml/xml_using_style.html.ru#page_body"
        rel="subdocument">Использование таблицы стилей в XML</a></li>
      <li><a href="using_xml/editing_xml_document.html.ru#page_body"
        rel="subdocument">Редактирование XML документов</a></li>
    </ul>
  </li>
  <li><a href="StyleSheets.html.ru#page_body" rel="subdocument">Каскадные
    таблицы стилей</a>
    <ul>
      <li><a href="style_sheets/working_with_styles.html.ru#page_body"
        rel="subdocument">Работа со стилями</a></li>
      <li><a
        href="style_sheets/about_applying_style_using_html_elements.html.ru#page_body"
        rel="subdocument">О применении стилей с помощью элементов
      HTML</a></li>
      <li><a
        href="style_sheets/about_linking_external_and_user_style_sheets.html.ru#page_body"
        rel="subdocument">О внешних ссылках и пользовательских стилях</a></li>
      <li><a
        href="style_sheets/creating_and_updating_a_style_attribute.html.ru#page_body"
        rel="subdocument">Создание и обновление атрибута Стиль
      (Style)</a></li>
      <li><a
        href="style_sheets/creating_generic_style_for_an_element_or_a_class.html.ru#page_body"
        rel="subdocument">Создание общего стиля для элемента или
      класса</a></li>
      <li><a
        href="style_sheets/creating_html_style_elements.html.ru#page_body"
        rel="subdocument">Создание элементов стиля HTML</a></li>
      <li><a href="style_sheets/handling_external_css.html.ru#page_body"
        rel="subdocument">Обработка внешних CSS</a></li>
      <li><a href="style_sheets/removing_style.html.ru#page_body"
        rel="subdocument">Удаление стилей</a></li>
    </ul>
  </li>
  <li><a href="Searching.html.ru#page_body" rel="subdocument">Поиск и
    замещение текста</a>
    <ul>
      <li><a
        href="searching_and_replacing_text/searching_and_replacing_text.html.ru#page_body"
        rel="subdocument">Поиск и замещение текста</a></li>
    </ul>
  </li>
  <li><a href="SpellChecking.html.ru#page_body" rel="subdocument">Проверка
    правописания</a>
    <ul>
      <li><a href="spell_checking/spell_checking.html.ru#page_body"
        rel="subdocument">Проверка правописания</a></li>
    </ul>
  </li>
  <li><a href="Publishing.html.ru#page_body" rel="subdocument">Сохранение и
    публикация</a>
    <ul>
      <li><a
        href="saving_and_publishing_documents/saving_and_publishing_documents.html.ru#page_body"
        rel="subdocument">Сохранение и публикация документов</a></li>
      <li><a
        href="saving_and_publishing_documents/about_saving_in_html.html.ru#page_body"
        rel="subdocument">О сохранении в HTML</a></li>
      <li><a
        href="saving_and_publishing_documents/saving_documents_as_text.html.ru#page_body"
        rel="subdocument">Сохранение документов как текст</a></li>
      <li><a
        href="saving_and_publishing_documents/the_save_command.html.ru#page_body"
        rel="subdocument">Команда Сохранить</a></li>
      <li><a
        href="saving_and_publishing_documents/the_saveas_command.html.ru#page_body"
        rel="subdocument">Команда Сохранить как</a></li>
    </ul>
  </li>
  <li><a href="WebDAV.html.ru#page_body" rel="subdocument">Фунции WebDAV в
    Amaya</a></li>
  <li><a href="Printing.html.ru#page_body" rel="subdocument">Печать
    документов</a>
    <ul>
      <li><a href="printing/printing_documents.html.ru#page_body"
        rel="subdocument">Печать документов</a></li>
      <li><a
        href="printing/using_the_setup_and_print_command.html.ru#page_body"
        rel="subdocument">Использование команды Настроить и печатать</a></li>
      <li><a href="printing/print_command.html.ru#page_body"
        rel="subdocument">Команда Печать</a></li>
      <li><a href="printing/windows_platforms.html.ru#page_body"
        rel="subdocument">Печать на платформах Windows</a></li>
      <li><a href="printing/unix_platforms.html.ru#page_body"
        rel="subdocument">Печать на платформах UNIX</a></li>
      <li><a
        href="printing/about_controlling_printing_with_css.html.ru#page_body"
        rel="subdocument">О контроле за печатью с помощью CSS</a></li>
    </ul>
  </li>
  <li><a href="Numbering.html.ru#page_body" rel="subdocument">Нумерация
    разделов</a>
    <ul>
      <li><a href="numbering_sections/numbering_sections.html.ru#page_body"
        rel="subdocument">Нумерация разделов</a></li>
    </ul>
  </li>
  <li><a href="MakeBook.html.ru#page_body" rel="subdocument">Создание
    составных документов</a>
    <ul>
      <li><a
        href="using_the_makebook_function/assembling_large_document_collections.html.ru#page_body"
        rel="subdocument">Объединение коллекции документов</a></li>
    </ul>
  </li>
  <li><a href="Annotations.html.ru#page_body" rel="subdocument">Аннотирование
    документов с помощью Amaya</a>
    <ul>
      <li><a
        href="attaching_annotations/what_is_an_annotation.html.ru#page_body"
        rel="subdocument">Что такое аннотация?</a></li>
      <li><a
        href="attaching_annotations/about_reading_annotations.html.ru#page_body"
        rel="subdocument">О чтении аннотаций</a></li>
      <li><a
        href="attaching_annotations/about_storing_local_annotations.html.ru#page_body"
        rel="subdocument">О локальных аннотациях</a></li>
      <li><a
        href="attaching_annotations/about_storing_remote_annotations.html.ru#page_body"
        rel="subdocument">Об удалённых аннотациях</a></li>
      <li><a href="attaching_annotations/annotation_menu.html.ru#page_body"
        rel="subdocument">Меню Аннотации</a></li>
      <li><a
        href="attaching_annotations/configuring_annotation_settings.html.ru#page_body"
        rel="subdocument">Конфигурирование настроек аннотации</a></li>
      <li><a
        href="attaching_annotations/deleting_an_annotation.html.ru#page_body"
        rel="subdocument">Создание аннотации</a></li>
      <li><a
        href="attaching_annotations/loading_and_presenting_annotations.html.ru#page_body"
        rel="subdocument">Загрузка и представление аннотаций</a></li>
      <li><a
        href="attaching_annotations/moving_annotations.html.ru#page_body"
        rel="subdocument">Перемещение по аннотациям</a></li>
      <li><a
        href="attaching_annotations/replying_to_annotations.html.ru#page_body"
        rel="subdocument">Ответ на аннотации, дискуссионные нити</a></li>
      <li><a href="attaching_annotations/configuring_icons.html.ru#page_body"
        rel="subdocument">Конфигурирование иконок аннотации</a></li>
      <li><a href="attaching_annotations/annotation_issues.html.ru#page_body"
        rel="subdocument">Известные проблемы с аннотациями</a></li>
    </ul>
  </li>
  <li><a href="bookmarks.html.ru#page_body" rel="subdocument">Поддержка
    закладок в Amaya</a></li>
  <li><a href="Configure.html.ru#page_body"
    rel="subdocument">Конфигурирование Amaya</a>
    <ul>
      <li><a href="configuring_amaya/configuring_amaya.html.ru#page_body"
        rel="subdocument">Конфигурирование Amaya</a></li>
      <li><a
        href="configuring_amaya/about_configuration_directory_and_file_conventions.html.ru#page_body"
        rel="subdocument">О директории конфигурации и файле
      соглашений</a></li>
      <li><a href="configuring_amaya/menu_preferences.html.ru#page_body"
        rel="subdocument">Меню Настройки</a></li>
    </ul>
  </li>
  <li><a href="ShortCuts.html.ru#page_body" rel="subdocument">Комбинации
    кнопок</a>
    <ul>
      <li><a
        href="configuring_amaya/about_keyboard_shortcuts.html.ru#page_body"
        rel="subdocument">О комбинациях кнопок</a></li>
    </ul>
  </li>
  <li><a href="Access.html.ru#page_body" rel="subdocument">Доступность в
    Amaya</a>
    <ul>
      <li><a
        href="accessibility_in_amaya/accessibility_in_amaya.html.ru#page_body"
        rel="subdocument">Доступность в Amaya</a></li>
      <li><a
        href="accessibility_in_amaya/about_amayas_accessibility_features.html.ru#page_body"
        rel="subdocument">О возможностях доступности в Amaya</a></li>
      <li><a
        href="accessibility_in_amaya/about_producing_accessible_content.html.ru#page_body"
        rel="subdocument">О создании доступного содержания</a></li>
    </ul>
  </li>
  <li><a href="translate.html.ru#page_body" rel="subdocument">Ошибки
    руссифицированного Amaya и его документация</a></li>
</ul>
<hr />

<p class="policyfooter"><small>Copyright  ©  1994-2005 <a
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
use</a></small> <small>and <a
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
