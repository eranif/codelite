#!/bin/sh

## Get list of files to work on
exe_name="CodeLiteUR"
exe_list=$exe_name `ls ./plugins/*.so ./debuggers/*.so`

## Loop over the files, and update the path of the wx library
for file in $exe_list
do
	orig_path=`otool -L $file  | grep libwx | awk '{print $1;}'`
	
	for path in $orig_path
	do
		new_path=`echo $path | xargs basename`
		echo install_name_tool -change $orig_path @executable_path/$new_path $file
		## install_name_tool -change $orig_path @executable_path/$new_path $file
	done
done

