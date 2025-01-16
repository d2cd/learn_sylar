#!/bin/bash
cmake --build .
if [ $? -eq 0 ]; then
	echo "build succeeded."
	../bin/test
else
	echo "build failed."
	exit 1
fi
