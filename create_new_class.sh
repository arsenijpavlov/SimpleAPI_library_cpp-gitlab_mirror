#!/bin/bash

if [[ ( -z $1 ) || (-z $(realpath $1) ) ]]; then
	echo "incorrect <path> parameter"
	exit 1
fi
path=$1
path="${path%%\/}"
echo -n "in path [$path] create "

if [[ -z $2 ]]; then
	echo "incorrect <file_name> parameter"
	exit 1
fi
echo "[$2] class file"

echo "#ifndef ${2^^}_H" > "$path/$2.h"
echo "#define ${2^^}_H" >> "$path/$2.h"
echo "" >> "$path/$2.h"
echo "" >> "$path/$2.h"
echo "class $2 {" >> "$path/$2.h"
echo "" >> "$path/$2.h"
echo "}" >> "$path/$2.h"
echo "" >> "$path/$2.h"
echo "#endif // ${2^^}_H" >> "$path/$2.h"

echo "#include \"$2.h\"" > "$path/$2.cpp"
echo "" >> "$path/$2.cpp"
echo "" >> "$path/$2.cpp"
