#!/bin/bash

# ОПИСАНИЕ ###################################################################
# 	Данный скрипт обновит удалённый URL репозитория в локальном хранилище, 
# используя данные с репозитория github.com/arsenijpavlov@gmail.com/rserver
##############################################################################

cd "$(dirname "$(realpath "$0")")"

FILENAME="temp.html"
ADDRESS="https://github.com/arsenijpavlov/rserver/blob/master/README.md"

rm -f ${FILENAME}
wget -O ${FILENAME} ${ADDRESS}

input=$(grep "article class" ${FILENAME})
NEW_IP=$(echo ${input} | grep -oE "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+")
rm -f ${FILENAME}

REMOTE_URL=$(git config --get remote.origin.url)
OLD_IP=$(echo ${REMOTE_URL} | grep -oE "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+")

echo "NEW_IP: ${NEW_IP}"
echo "OLD_IP: ${OLD_IP}"
echo "OLD REMOTE_URL: ${REMOTE_URL}"

if [[ ${OLD_IP} != ${NEW_IP} ]]; then
	REMOTE_URL=$(echo ${REMOTE_URL} | sed "s/${OLD_IP}/${NEW_IP}/")
	git remote set-url origin ${REMOTE_URL}
	echo "NEW REMOTE_URL: ${REMOTE_URL}"
else
	echo "REMOTE_URL did not changed"
fi

