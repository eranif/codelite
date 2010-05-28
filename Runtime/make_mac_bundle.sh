#!/bin/sh

fix_shared_object_depends() {
	search_string=$1

	## Get list of files to work on
	file_list=`ls ../lib/*.so`
	file_list="${file_list} ${exe_name} "

	## Since the plugins must use the same wx configuration as the
	## executable, we can run the following command only once and
	## use the results to manipulate the plugins as well
	orig_path=`otool -L ${exe_name}  | grep ${search_string} | awk '{print $1;}'`

	## Loop over the files, and update the path of the wx library
	for file in ${file_list}
	do
			for path in ${orig_path}
			do
					new_path=`echo ${path} | xargs basename`
					install_name_tool -change ${orig_path} @executable_path/${new_path} ${file}
			done
	done
}

## extract the file name from the Makefile
exe_name=`cat ../Makefile | grep ^EXE_NAME_NO_PATH | cut -d= -f2`

## Run install_name_tool on the executable to bundle
## libwx with the bundle


rm -rf CodeLite.app
mkdir -p ./CodeLite.app/Contents/MacOS
mkdir -p ./CodeLite.app/Contents/Resources
mkdir -p ./CodeLite.app/Contents/SharedSupport
mkdir -p ./CodeLite.app/Contents/SharedSupport/plugins
mkdir -p ./CodeLite.app/Contents/SharedSupport/plugins/resources/
mkdir -p ./CodeLite.app/Contents/SharedSupport/debuggers
mkdir -p ./CodeLite.app/Contents/SharedSupport/config

# fix the script
echo "Running install_name_tool..."
fix_shared_object_depends libwx_
## copy the wx dlls to the exeutable path which under Mac is located at ./CodeLite.app/Contents/MacOS/
for wx_file in ${orig_path}
do
		cp ${wx_file} ./CodeLite.app/Contents/MacOS/
done

fix_shared_object_depends libplugin
fix_shared_object_depends libcodelite
fix_shared_object_depends libwxscintilla
fix_shared_object_depends libwxsqlite

#echo install_name_tool -change /usr/lib/libcurl.4.dylib @executable_path/libcurl.4.dylib ${exe_name}
#install_name_tool -change /usr/lib/libcurl.4.dylib @executable_path/libcurl.4.dylib ${exe_name}
#cp /usr/lib/libcurl.4.dylib ./CodeLite.app/Contents/MacOS/

cp ${exe_name} ./CodeLite.app/Contents/MacOS/${exe_name}

cp -r rc ./CodeLite.app/Contents/SharedSupport/
cp -r templates ./CodeLite.app/Contents/SharedSupport/
cp -r images ./CodeLite.app/Contents/SharedSupport/
cp -r lexers ./CodeLite.app/Contents/SharedSupport/

cp astyle.sample ./CodeLite.app/Contents/SharedSupport/
cp index.html ./CodeLite.app/Contents/SharedSupport/
cp svnreport.html ./CodeLite.app/Contents/SharedSupport/
cp icon.icns ./CodeLite.app/Contents/Resources/
cp -pr src/*.gz ./CodeLite.app/Contents/Resources/

## copy empty layout file
cp config/codelite.layout.default ./CodeLite.app/Contents/SharedSupport/config/codelite.layout
cp config/accelerators.conf.default ./CodeLite.app/Contents/SharedSupport/config/
cp config/build_settings.xml.default.mac ./CodeLite.app/Contents/SharedSupport/config/build_settings.xml.default
cp config/plugins.xml.default ./CodeLite.app/Contents/SharedSupport/config

## copy default Mac configuration file
cp config/codelite.xml.default.mac ./CodeLite.app/Contents/SharedSupport/config/codelite.xml.default

## replace the executable name according to the configuration used in the build
cat Info.plist.template | sed s/EXE_NAME/${exe_name}/g >> ./CodeLite.app/Contents/Info.plist

cp config/debuggers.xml.default ./CodeLite.app/Contents/SharedSupport/config

cp ../lib/CodeFormatter.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Debugger.so ./CodeLite.app/Contents/SharedSupport/debuggers/
cp ../lib/Gizmos.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Subversion2.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/cscope.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Copyright.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/UnitTestCPP.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/ExternalTools.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/SymbolView.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/ContinuousBuild.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/SnipWiz.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/wxformbuilder.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/abbreviation.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/QmakePlugin.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/CppCheck.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/MacBundler.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/libwxscintillau.so ./CodeLite.app/Contents/MacOS/
cp ../lib/libpluginu.so ./CodeLite.app/Contents/MacOS/
cp ../lib/libcodeliteu.so ./CodeLite.app/Contents/MacOS/
cp ../lib/libwxsqlite3u.so ./CodeLite.app/Contents/MacOS/

cp ../sdk/codelite_indexer/codelite_indexer   ./CodeLite.app/Contents/SharedSupport/
cp ../sdk/codelite_cppcheck/codelite_cppcheck ./CodeLite.app/Contents/SharedSupport/
cp ./OpenTerm   ./CodeLite.app/Contents/SharedSupport/
cp plugins/resources/*.*                      ./CodeLite.app/Contents/SharedSupport/plugins/resources/
