#!/bin/bash

if [ $# -eq 0 ] ; then
	echo "$0 update | new <iso-639-1>"
	exit
fi



if [ "$1" = "update" ] ; then
	STR=""

	mv src/Languages/sayonara_lang.ts.template src/Languages/sayonara_lang.ts
	for f in ./src/Languages/*.ts ; do

		lupdate -no-obsolete -locations relative . -ts ./$f
		STR="$STR ./$f"
	done
	
	mv src/Languages/sayonara_lang.ts src/Languages/sayonara_lang.ts.template
#	linguist $STR	

elif [ "$1" = "new" ] ; then
	lupdate . -ts ./src/Languages/sayonara_lang_$2.ts

fi
		
