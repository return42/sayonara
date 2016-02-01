#!/bin/bash

CPP=`find ./src -iname "*.cpp" | xargs grep -L Copyright`
HEADER=`find ./src -iname "*.h" | xargs grep -L Copyright`
for FILE in $CPP ; do 
	BASE=$(basename $FILE)
	echo "/* ${BASE} */" > /tmp/newfile
	cat license.txt >> /tmp/newfile
	cat $FILE >> /tmp/newfile
	cp /tmp/newfile $FILE
	echo "${FILE} written"
	rm /tmp/newfile
done

for FILE in $HEADER ; do 
	BASE=$(basename $FILE)
	echo "/* ${BASE} */" > /tmp/newfile
	cat license.txt >> /tmp/newfile
	cat $FILE >> /tmp/newfile
	cp /tmp/newfile $FILE
	echo "${FILE} written"
	rm /tmp/newfile
done

