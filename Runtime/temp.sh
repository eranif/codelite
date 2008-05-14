#!/bin/sh

## Get list of files to work on
exe_name="./CodeLiteUR"
file_list=`ls ./plugins/*.so ./debuggers/*.so`
file_list="${file_list} ${exe_name} "

## Since the plugins must use the same wx configuration as the 
## executable, we can run the following command only once and
## use the results to manipulate the plugins as well
orig_path=`otool -L ${exe_name}  | grep libwx | awk '{print $1;}'`

## Loop over the files, and update the path of the wx library
for file in ${file_list}
do
	for path in ${orig_path}
	do
		new_path=`echo ${path} | xargs basename`
		echo install_name_tool -change ${orig_path} @executable_path/${new_path} ${file}
		install_name_tool -change ${orig_path} @executable_path/${new_path} ${file}
	done
done

## copy the wx dlls to the exeutable path which under Mac is located at ./CodeLite.app/Contents/MacOS/
for wx_file in ${orig_path}
do
	echo "Copying ${wx_file} to ./CodeLite.app/Contents/MacOS/"
	cp ${wx_file} ./CodeLite.app/Contents/MacOS/
done

