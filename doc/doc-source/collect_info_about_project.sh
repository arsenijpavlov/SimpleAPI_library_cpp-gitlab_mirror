#!/bin/bash

SIMPLEAPI_BASE_PATH="$PWD/../../SimpleAPI"

# Описание:
# скрипт должен вывести список публичных методов для указанных классов
# для каждого класса должен быть создан topic в формате *.md
#
#
#

#for file in $(ls "${SIMPLEAPI_BASE_PATH}")
#do
#done

find "${SIMPLEAPI_BASE_PATH}" -name "*.h" -exec grep "class\|enum" {} \;
