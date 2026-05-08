#!/bin/bash

cp -f "SimpleAPI_docs.tree" "test.tree"
out_file="test.tree"
del_template="toc-element"

# удалить предыдущую структуру проекта
sed -i "/${del_template}/d" "${out_file}"

# создать новую структуру проекта
temp_file="NEW_CONTENT.txt"
> "${temp_file}" # обнуляем содержимое временного файла

# собрать список всех вложенностей: все папки и .md документы
main_path=${PWD}
cd topics
IFS=$'\n'
list=$(find . -name "*.md")
new_list=() #временная переменная для преобразования значений
for el in ${list[@]}
do
	new_list+=("${el#*/}")
done
list=(${new_list[@]})

while [[ ${list[@]} ]] #проходим N раз, пока не обработаем все варианты
do
	# проходим по всем оставшимся элементам
	for el in ${list[@]}
	do
		echo "el: \"${el}\""
		
		# если el является файлом в этой папке
		if [[ ${el##/*} == "" ]]; then
			echo -e "\tis file"
		fi
		
		list=(${list[@]/${el}/})
	done
done

exit
cat "${temp_file}"
sed -i "/<!-- SCRIPT BEGIN -->/r ${temp_file}" "${out_file}"
rm "${temp_file}" # удалить временный файл
