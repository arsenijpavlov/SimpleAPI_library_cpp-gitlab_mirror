#!/bin/bash

# Описание
# скрипт подготовит внутреннюю структуру статей для SimpleAPI_docs.tree
# - структура будет построена на основе сортировки по алфавиту
# - для создания вложенности нужно создать директорию с тем же именем, что и целевой .md
# Дополнительным аргументом передаётся выбранный язык локализации. Без значения по умолчанию

# для дебага
#cp -f "SimpleAPI_docs.tree" "test.tree"
#out_file="test.tree"

function RenameSpacesToDash {
	local IFS=$'\n'
	local list=$(find . -type d)
	for el in ${list[@]}
	do
		if [[ ${el} == "." || ${el} == ".." ]]; then
			continue
		fi
		
		local new_path=${el// /-}
#		echo "new path name: \"${new_path}\""
		mkdir -p ${new_path}
		
		if [[ ${el} == ${new_path} ]]; then
			continue
		fi
	
		mv ${el}/* ${new_path}
		rm -r ${el}
	done
	
	list=$(find . -name "*.md")
	for el in ${list[@]}
	do
		local new_path=${el// /-}
		
		if [[ ${el} == ${new_path} ]]; then
			continue
		fi
		
#		echo "new name: \"${new_path}\""
		mv ${el} ${new_path}
	done
}

language=${1^^} #два символа (RU/EN and etc.)
if [[ ! ${language} ]]; then
	echo "not found language argument"
	exit
fi

out_file="SimpleAPI_docs.tree"

# удалить предыдущую структуру проекта
START_TEMPLATE="SCRIPT BEGIN"
END_TEMPLATE="SCRIPT END"
sed -i "/${START_TEMPLATE}/,/${END_TEMPLATE}/ {/${START_TEMPLATE}/b; /${END_TEMPLATE}/b; d; }" "${out_file}"

# создать новую структуру проекта
temp_file="NEW_CONTENT.txt"
> "${temp_file}" # обнуляем содержимое временного файла

# переключить настройки Writerside на другую директорию
target_topics_dir="topics_${language}"
sed -i "s/\(^[[:space:]]*<topics dir=\).*\(\/>\)/\1\"${target_topics_dir}\"\2/" writerside.cfg
# собрать список всех вложенностей: все папки и .md документы
main_path=${PWD}
cd ${target_topics_dir}
$(RenameSpacesToDash)

IFS=$'\n'
list=$(find . -name "*.md" | LC_ALL=C sort) #сортировка нужна для корректности конечного вывода
new_list=() #временная переменная для преобразования значений
for el in ${list[@]}
do
	new_list+=("${el#*/}")
done
list=(${new_list[@]})

function debug_print_list {
	echo "-----------"
	echo "debug_print_list:"
	for element in ${@}
	do
		echo -e "\t\"${element}\""
	done
	echo "-----------"
}

function recursive_collection { # $1-level $2-list of elements
	local restored_path=${PWD}
	local current_level=$1
	local spaces_template=""
		for (( i=0; i<${current_level}; i++ ))
		do
			spaces_template+="\t"
		done
	#echo -e "${spaces_template}recursive_collection:"

	local list=($@)
	unset list[0]
	# пройтись по всем вложенностям
	# если в пути есть папка, но на уровне папки нет одноимённого .md документа - пропуск строки
	
	local file_is_found=true # первичное выполение условия
	while [[ ${list[@]} && ${file_is_found} ]] #проходим N раз, пока не обработаем все варианты
	do
		# проходим по всем оставшимся элементам
		file_is_found= # обнуляем условие для выхода из вечного цикла
		for el in ${list[@]}
		do
			#echo -e "${spaces_template}el: \"${el}\""
			# если el является файлом в этой папке
			local current_file="${el##*/}"
			if [[ -f ${current_file} && ${current_file} =~ .*".md" ]]; then
				#echo -e "${spaces_template}is file: \"${current_file}\""
				file_is_found=true # условие опять валидно
	
				local new_list=()
				list=(${list[@]/${current_file}/})
				#echo -e "\t\ttest: \"${current_file%\.md}\""
				# начиная отсюда надо искать то же название папки и обработать древо вложений
				for inner in ${list[@]}
				do
					#echo -e "\t\ttest: \"${inner%%/*}\""
					if [[ ${inner%%/*} == ${current_file%\.md} ]]; then
						#echo -e "${spaces_template}append: \"${inner}\""
						new_list+=(${inner})
						list=(${list[@]/${inner}/})
					fi
				done
				
				# новый список сформирован, продолжаем
				local next_level=$((current_level+1))
				#echo "next_level: ${next_level}"
				# выровнять список, убрать название папки и основной файл .md
				local temp_list=(${new_list[@]})
				new_list=()
				for inner in ${temp_list[@]}
				do
					new_list+=(${inner#*/}) # сохраняем с изменённым значением
				done
				#new_list=$("${new_list[@]}" | sort)
				
				#debug_print_list ${new_list[@]}
				
				if [[ -d ${current_file%\.md} ]]; then
					cd ${current_file%\.md}
					local out_list=$(recursive_collection "${next_level}" "${new_list[@]}")
					cd ${restored_path}
				fi
				
				if [[ ${out_list} ]]; then
					# есть внутренние элементы
					echo -e "${spaces_template}<toc-element topic=\"${current_file}\"/>"
					echo -e "${out_list}"
#					echo -e "${spaces_template}</toc-element> <!-- end of \"${current_file}\" -->"
					out_list= # обнулить, чтобы не влияло на следующую итерацию
				else
					# внутренних элементов нет
					echo -e "${spaces_template}<toc-element topic=\"${current_file}\"/>"
				fi
			fi
		done
	done
	
#	if [[ ! ${file_is_found} ]]; then
#		echo -e "${spaces_template}found stopped"
#	fi
}

recursive_collection "0" "${list[@]}" > "${main_path}/${temp_file}"

cd "${main_path}"
sed -i "s/^/\t/g" "${temp_file}" # добавить табуляцию в начале каждой строки файла
sed -i "/<!-- SCRIPT BEGIN -->/r ${temp_file}" "${out_file}"

#выставить стартовую страницу как первую из списка структуры
read -r start_page < ${temp_file}
# достать значение по шаблону *.md, где * относится к имени файла
start_page=${start_page##*=\"}
start_page=${start_page%%\"*}
sed -i "s/\([[:space:]]*start-page=\"\).*\(\">\)/\1${start_page}\2/" ${out_file}

rm "${temp_file}" # удалить временный файл
