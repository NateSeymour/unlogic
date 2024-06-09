#!/usr/bin/env bash

CONAN_VERSION=$(conan -v)
if [[ -nz $? ]]; then
	echo "Found Conan!"
else
	echo "Could not find Conan!"
	exit 1
fi

if [[ -z "$1" ]]; then
	CONAN_BUILD_TYPE="RelWithDebInfo"
else
	CONAN_BUILD_TYPE="$1"
fi

OUTPUT_FOLDER=$(echo "cmake-build-${CONAN_BUILD_TYPE}" | awk '{print tolower($0)}')
conan install . --output-folder="${OUTPUT_FOLDER}" --build=missing -s build_type="${CONAN_BUILD_TYPE}"
