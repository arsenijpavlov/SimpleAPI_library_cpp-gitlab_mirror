#!/bin/bash

if [[ ${1,,} == "help" ]]; then
	echo "./create_new_class <path> <filename>"
	exit 1
fi

if [[ ( -z $1 ) || (-z $(realpath $1) ) ]]; then
	echo "incorrect <path> parameter"
	exit 1
fi
path=$1
path="${path%%\/}"
echo -n "in path [$path] create "

if [[ -z $2 ]]; then
	echo "incorrect <filename> parameter"
	exit 1
fi
echo "[$2] class file"

# схема с #ifndef HEADER_H создаёт много проблем, когда имена файлов пересекаются с существующими в другом проекте, заменено на #pragma once
echo "#pragma once" > "$path/$2.h"
echo "" >> "$path/$2.h"
echo "" >> "$path/$2.h"
echo "class $2 {" >> "$path/$2.h"
echo "" >> "$path/$2.h"
echo "};" >> "$path/$2.h"
echo "" >> "$path/$2.h"

echo "#include \"$2.h\"" > "$path/$2.cpp"
echo "" >> "$path/$2.cpp"
echo "" >> "$path/$2.cpp"
