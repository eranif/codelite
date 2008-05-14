#!/bin/sh

exe_list="CodeLiteUR "
for file in $exe_list
do
	orig_path=`otool -L $file  | grep libwx | awk '{print $1;}'`
	new_path=`otool -L $file  | grep libwx | awk '{print $1;}' | xargs basename`
	echo install_name_tool -change $orig_path $new_path $file
	install_name_tool -change $orig_path @executable_path/$new_path $file
done

