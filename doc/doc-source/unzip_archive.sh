#!/bin/bash

# Данный скрипт распакает архив, который произведётся сборщиком Writerside
# Имя архива изменить во время сборки нельзя, но можно воспользоваться шаблоном "webHelpIN<index>-all.zip"

INSTANCE_NAME="SimpleAPI_docs"
INSTANCE_NAME=${INSTANCE_NAME^^}
template_name="webHelp${INSTANCE_NAME}[0-9]+-all.zip"
found_name=
for file in $(ls *".zip")
do
	if [[ ${file} =~ ${template_name} ]]; then
		found_name=${file}
		break
	fi
done

if [[ ! ${found_name} ]]; then
	echo "not found archive webHelp${INSTANCE_NAME}<index>-all.zip, exit..."
	exit
fi

upper_dir_name=${PWD%/*}
upper_dir_name=${upper_dir_name##*/}
#echo "upper_dir_name: ${upper_dir_name}"
if [[ ${upper_dir_name} != "doc" ]]; then
	echo "upper dir name is not \"doc\", exit..."
	exit
fi

# В папке doc удалить все файлы, кроме директории исходников документации doc-source
whitelist=()
whilelist+="doc-source"
whilelist+="doxygen"
whilelist+="user_manuals"

cd ..
for file in $(ls .)
do
	need_erase=true
	for white in ${whitelist[@]}
	do
		if [[ ${file} == ${white} ]]; then
			need_erase=
		fi
	done
	
	if [[ ${need_erase} ]]; then
		echo "remove \"${file}\""
		rm -rf "${file}"
	fi
done

# Перенести полученный архив в текущую директорию (doc), распаковать и удалить архив
mv "doc-source/${found_name}" "."
unzip ${found_name}
rm ${found_name}

echo "Correctly deployed!"
