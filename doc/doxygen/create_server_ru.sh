#!/bin/bash

MAIN_DIR=$(dirname $(realpath ${0}))
cd "${MAIN_DIR}/ru/html"
python3 -m http.server 8000


