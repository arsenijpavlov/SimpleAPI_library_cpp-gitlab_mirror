#!/bin/bash

# Описание ===========================================
# Скрипт скопирует файл-шаблон для VersionInfo.h для #
# передачи параметров версии из информации от Git в  #
# проект библиотеки как define                       #
# ====================================================

CURRENT_DIR=$(dirname $(realpath "${0}"))

FILE_NAME="VersionInfo.h"
cp -f "${CURRENT_DIR}/TEMPLATE_${FILE_NAME}" "${CURRENT_DIR}/../SimpleAPI/${FILE_NAME}"

# предварительное заполнение переменных
DEFAULT_VALUE="UNKNOWN(git/.git not found)"
SIMPLE_VERSION="${DEFAULT_VALUE}"
SIMPLE_VERSION_NAME="${DEFAULT_VALUE}"
SIMPLE_VERSION_COMMIT="${DEFAULT_VALUE}"

# проверка наличия Git в системе
result=$(git --version) # прячем консольный вывод
if [[ $? == 0 ]]; then
	# поиск последнего тега по маске и подсчёт количества коммитов от тега до текущего
	GIT_OUTPUT=$(git -C "${CURRENT_DIR}" describe --tags --match="v[0-9a-zA-Z\(\).,-]*" --long)
	
	# разделение прошлого вывода на составляющие
	read -r SIMPLEAPI_VERSION SIMPLEAPI_VERSION_NAME SIMPLEAPI_VERSION_COMMIT < <( echo ${GIT_OUTPUT} \
		| sed -E "s/^(v[0-9.-]*)[_]*([_0-9a-zA-Z.,-]+)-([0-9]+)-g([0-9a-fA-F]+)/\1-b\3 \2 \4/" \
		| sed "s/-b0//" )
	SIMPLEAPI_VERSION_NAME=${SIMPLEAPI_VERSION_NAME//_/ }
	
	# проверка наличия изменений без фиксации
	GIT_DIFF_OUTPUT=$(git -C "${CURRENT_DIR}" diff --name-only)
	if [[ ${GIT_DIFF_OUTPUT} ]]; then
		# есть изменения, надо сделать заполнитель
		GIT_DIFF_OUTPUT=" (with uncommited changes)"
		SIMPLEAPI_VERSION="${SIMPLEAPI_VERSION}${GIT_DIFF_OUTPUT}"
	fi

	echo "SIMPLEAPI_VERSION:        \"${SIMPLEAPI_VERSION}\""
	echo "SIMPLEAPI_VERSION_NAME:   \"${SIMPLEAPI_VERSION_NAME}\""
	echo "SIMPLEAPI_VERSION_COMMIT: \"${SIMPLEAPI_VERSION_COMMIT}\""
fi

# применение как define в коде C/C++
function UpdParameter { # $1 - переменная, $2 - новое значение
	sed -i "s/^\(#define ${1} [[:space:]]*\).*/\1\"${2}\"/" "${CURRENT_DIR}/../SimpleAPI/${FILE_NAME}"
}

UpdParameter "SIMPLEAPI_VERSION"        "${SIMPLEAPI_VERSION}"
UpdParameter "SIMPLEAPI_VERSION_NAME"   "${SIMPLEAPI_VERSION_NAME}"
UpdParameter "SIMPLEAPI_VERSION_COMMIT" "${SIMPLEAPI_VERSION_COMMIT}"

