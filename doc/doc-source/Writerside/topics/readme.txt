Логика поиска путей:
структура становится вложенной для FILE.md если существует одноимённая директория FILE, внутри которой есть другие документы .md

FILE.md
FILE
|-- INNER_FILE.md

даст итоговую структуру документов:
<toc-element topic="FILE.md">
	<toc-element topic="INNER_FILE.md"/>
</toc-element>

Writeside считает за ошибку дубликаты .md по внутреннему заголовку H1

