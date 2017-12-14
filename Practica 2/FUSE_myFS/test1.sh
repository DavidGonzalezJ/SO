#!/bin/bash

TMP_DIRECTORY=tmp
MPOINT="./mount-point"
FILE1="fuseLib.c"
FILE2="myFS.h"
FILE3="file3.txt"
SRC="./src"

if [ -d "$TMP_DIRECTORY" ]; then
	rmdir -p $TMP_DIRECTORY
	echo "$TMP_DIRECTORY directory deleted... [OK]"
fi

mkdir $TMP_DIRECTORY

echo "Copying file 1"
cp $SRC/$FILE1 $TMP_DIRECTORY
cp $SRC/$FILE1 $MPOINT
read -p "Press enter..."

echo "Copying file 2"
cp $SRC/$FILE2 $TMP_DIRECTORY	
cp $SRC/$FILE2 $MPOINT
read -p "Press enter..."

./my-fsck-static-64 virtual-disk

DIFF_FILE1=$(diff "$MPOINT/$FILE1" "./$TMP_DIRECTORY/$FILE1")
if [ "$DIFF_FILE1" == "" ];
then
        echo "CORRECT  files are the same"
else
        echo "DIFFERENT files are different"
     #   echo "$DIFF_FILE1"
fi
read -p "Press enter..."
DIFF_FILE2=$(diff "$MPOINT/$FILE2" "./$TMP_DIRECTORY/$FILE2")
if [ "$DIFF_FILE2" == "" ];
then
        echo "CORRECT  files are the same"
else
        echo "DIFFERENT files are different"
       # echo "$DIFF_FILE2"
fi
read -p "Press enter..."

truncate --size=-4096 "./$TMP_DIRECTORY/$FILE1"
truncate --size=-4096 "$MPOINT/$FILE1"

./my-fsck-static-64 virtual-disk

DIFF_FILE1=$(diff $MPOINT/$FILE1 $SRC/$FILE1)
if [ "$DIFF_FILE1" == "" ];
then
        echo "EQUAL: files are the same"
else
        echo "CORRECT files are different"
       # echo "$DIFF_FILE1"
fi
read -p "Press enter..."

echo "Copying file 3"
echo "Archivo 3" > "$SRC/$FILE3"
cp "$SRC/$FILE3" "./$TMP_DIRECTORY"
cp "$SRC/$FILE3" "$MPOINT"
read -p "Press enter..."

./my-fsck-static-64 virtual-disk

DIFF_FILE1=$(diff $MPOINT/$FILE3 $SRC/$FILE3)
if [ "$DIFF_FILE1" == "" ]
then
        echo "CORRECT: files are the same"
else
        echo "files are different"
      #  echo "$DIFF_FILE1"
fi

read -p "Press enter..."
truncate --size=+4096 "./$TMP_DIRECTORY/$FILE1"
truncate --size=+4096 "$MPOINT/$FILE2"

./my-fsck-static-64 virtual-disk

DIFF_FILE1=$(diff "$MPOINT/$FILE2" "$SRC/$FILE2")
if [ "$DIFF_FILE1" == "" ];
then
        echo "files are the same"
else
        echo "CORRECT: files are different"
      #  echo "$DIFF_FILE1"
fi


