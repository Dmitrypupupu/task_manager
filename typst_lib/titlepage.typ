/* Титульный лист
    [title] - Название работы
    [authors] - Авторы работы
    [teachers] - Преподаватели или другие([position])
    [date] - Дата создания документа
    [education] - Название организации
    [department] - Факультет или институт
    [position] - Должность преподавателя
    [documentName] - Название документа (например "ОТЧЁТ")
    [group] - Название группы
    [city] - Город
    [object] - Предмет/курс
    [hasContentNavigationPage] - Включить оглавление
 */
#let titlepage(
  title: "",
  authors: (),
  teachers: (),
  date: datetime.today(),
  education: "Название организации",
  department: "Факультет",
  position: "преподаватель",
  documentName: "ОТЧЁТ ПО ЛАБОРАТОРНЫМ РАБОТАМ",
  group: "ГРУППА",
  city: "ГОРОД",
  object: "ПРЕДМЕТ",
  hasContentNavigationPage: false,
) = {
  set text(font: "Times New Roman", size: 12pt, lang: "ru", hyphenate: false)
  set page(
    margin: (right: 15mm, left: 15mm, top: 20mm, bottom: 20mm),
    paper: "a4",
  )
  align(center, education)
  v(0.7fr)
  align(center, department)
  v(1fr)
  align(left, stack(dir: ltr, "КУРСОВАЯ РАБОТА (ПРОЕКТ)
ЗАЩИЩЁНА С ОЦЕНКОЙ")) // убрана линия возле оценки
  align(left, "РУКОВОДИТЕЛЬ")
  grid(
    columns: (1.5fr, 1fr, 1.5fr),
    row-gutter: 3pt,
    column-gutter: 10pt,
    align(center, position), "", align(center, teachers.join(", ")),
    line(length: 100%, stroke: 0.5pt), line(length: 100%, stroke: 0.5pt), line(length: 100%, stroke: 0.5pt),
    align(center, text(0.9em, "должность, уч.степень, звание")),
    align(center, text(0.9em, "подпись, дата")),
    align(center, text(0.9em, "инициалы, фамилия")),
  )


  v(1.5fr)
  align(center, text(upper(documentName), size: 1.23em))
  v(0.8fr)
  align(center, stack(dir: ttb, text(title, size: 1.2em), v(5pt), align(
    bottom,
    line(length: 0%, stroke: 0.5pt),
  )))
  v(0.8fr)
  align(center)[по дисциплине: Основы программирования]
  align(center)[#text(object, size: 1.2em)]
  v(2fr)
  grid(
    columns: (1fr, 1fr, 1fr, 1fr),
    column-gutter: 10pt,
    row-gutter: 3pt,
    grid.cell([РАБОТУ ВЫПОЛНИЛ], colspan: 2), text(2.5em, ""), "",

    "СТУДЕНТ гр №", align(center, group), "", align(center, authors.join(", ")),
    line(length: 0%, stroke: 0.5pt),
    line(length: 100%, stroke: 0.5pt),
    line(length: 100%, stroke: 0.5pt),
    line(length: 100%, stroke: 0.5pt),
    grid.cell(text(0.9em, ""), colspan: 2),
    align(center + top, text(0.9em, "подпись, дата")),
    align(center, text(0.9em, "инициалы, фамилия")),
  )
   v(2fr)
   align(center, stack(dir: ltr, city, h(10pt), str(date.year())))
}