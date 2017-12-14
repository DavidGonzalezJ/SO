#!/bin/bash
TMP_DIR=tmp
OPUT_DIR=out
MYTAR=mytar.mtar
FILE1=file1.txt
FILE2=file2.txt
FILE3=file3.dat

echo "Patricia Cabrero y David González MytarTest:"

# Check that the mytar exist 
if [ ! -e ./mytar ]; then
	echo "No existe Mytar ❌"
	exit -1 # fail

# Check that mytar is an executable file
elif [ ! -x ./mytar ]; then
	echo "Mytar no es ejecutable ❌"
	exit -1 # fail
fi

echo "Mytar encontrada y ejecutable! ✓"
echo ""


if [ -d "$TMP_DIR" ]; then
rm -rf -- $TMP_DIR
echo "Existía un directorio $TMP_DIR y se ha borrado correctamente ✓"
fi

mkdir $TMP_DIR
cd $TMP_DIR

if [ ! -e $FILE1 ]; then
	touch $FILE1 # Create a hello World file
	echo "Hello World!" > $FILE1
	echo "$FILE1 se ha creado correctamente ✓"
fi

if [ ! -e $FILE2 ]; then
	touch $FILE2
	head -10 /etc/passwd > $FILE2
	echo "$FILE2 se ha creado correctamente ✓"
fi

if [ ! -e $FILE3 ]; then
	touch $FILE3 
	head -c 1024 /dev/urandom > $FILE3
	echo "$FILE3 se ha creado correctamente ✓"
fi

./../mytar -cf $MYTAR $FILE1 $FILE2 $FILE3


if [ ! -d $OPUT_DIR ]; then
	mkdir $OPUT_DIR 
	echo "Directorio out creado correctamente ✓"
fi

cp ./$MYTAR ./$OPUT_DIR/$MYTAR


cd $OPUT_DIR
./../../mytar -xf $MYTAR

if diff ../$FILE1 $FILE1 >/dev/null ; then
	echo "Los archivos $FILE1 son iguales ✓"
else
	echo "Los archivos $FILE1 no son iguales ❌"
	exit -1
fi
if diff ../$FILE2 $FILE2 >/dev/null ; then
	echo "Los archivos $FILE2 son iguales ✓"
else
	echo "Los archivos $FILE2 no son iguales ❌"
	exit -1
fi
if diff ../$FILE3 $FILE3 >/dev/null ; then
	echo "Los archivos $FILE3 son iguales ✓"
else
	echo "Los archivos $FILE3 no son iguales ❌"
	exit -1
fi
echo ""
echo "Completado con exito!"
exit 0