#!/bin/bash

######################################################
# Описание: скрипт делает ровно то же самое, что и 
# Writerside, но при этом сохраняет формат .md
######################################################

MAIN_DIR="$(dirname "$(realpath "$0")")"
cd "${MAIN_DIR}"

SUFFIX="ru"
RESULT_FILE="SimpleAPI_docs_${SUFFIX^^}.md"
SOURCE_DIR="Writerside/topics_${SUFFIX^^}"

# создаём файл с нуля
echo "# SimpleAPI" > "${RESULT_FILE}"
echo "_________" >> "${RESULT_FILE}"
echo "" >> "${RESULT_FILE}"

# и начинаем его заполнять всем содержимым документов-исходников в порядке очереди
for file in $(ls "${SOURCE_DIR}")
do
	path_to_file="${SOURCE_DIR}/${file}"
	# директории оставляем на поиск второго порядка
	if [[ -f "${path_to_file}" && "${file}" =~ .*".md" ]]; then
		echo "file: \"${file}\""
		# прописываем содержимое в общий файл
		cat "${path_to_file}" >> "${RESULT_FILE}"
		echo "" >> "${RESULT_FILE}" # пустая строка между вставками
		
		# если есть папка с тем же именем, проходим по её содержимому и добавляем к общему файлу
		path_to_dir="${path_to_file%.md}"
		if [[ -d "${path_to_dir}" ]]; then
			echo -e "\tdir: \"${file%.md}\""
			for file2 in $(ls "${path_to_dir}")
			do
				echo -e "\tfile: \"${file2}\""
				
				path_to_file="${path_to_dir}/${file2}"
				# прописываем содержимое в общий файл
				cat "${path_to_file}" >> "${RESULT_FILE}"
				echo "" >> "${RESULT_FILE}" # пустая строка между вставками 
			done
		fi
		
		# разграничиваем разделы
		echo "---------------------------------" >> "${RESULT_FILE}"
	fi
done

# перемещаем итоговый файл на уровень выше
mv "${RESULT_FILE}" ../
