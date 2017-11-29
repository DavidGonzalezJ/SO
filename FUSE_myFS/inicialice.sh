chmod +x my-fsck-static-64 
if [ -d "mount-point" ]; then
	rm -R -f "mount-point"
	echo "mount-point directory deleted... [OK]"
	rm "virtual-disk"
fi

mkdir "mount-point"
./fs-fuse -t 2097152 -a virtual-disk -f '-d -s mount-point'

