#!/bin/bash

shopt -s nocasematch

SOURCEDIR=${0%%${0##*/}}
SOURCEDIR=${SOURCEDIR%%/}
if [[ "$SOURCEDIR" == "" ]]; then
	 SOURCEDIR="."
fi

if [ ! -d $SOURCEDIR/build ]; then
	mkdir $SOURCEDIR/build
fi


if [[ "$1" == "CLEAN" ]]; then
	echo "Cleaning build dir..."
	rm -Rf $SOURCEDIR/build/*
fi

cd $SOURCEDIR/build

cmake ..

if (( $? )); then
	echo "--- cmake failed"
	exit 1
fi

make

if (( $? )); then
	echo "--- compile failed"
	exit 2
fi

cd $OLDPWD
cd $SOURCEDIR/docs
pandoc -s -w man gntp-notify.md -o ../man/gntp-notify.1
pandoc -s -w man gntp-register.md -o ../man/gntp-register.1

cd $OLDPWD

echo "Success!  Binary file(s) should be in $SOURCEDIR/build"

