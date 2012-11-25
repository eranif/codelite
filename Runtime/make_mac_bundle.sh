#!/bin/sh

fix_shared_object_depends() {
	search_string=$1

	## Get list of files to work on
    dylibs=`ls ./CodeLite.app/Contents/MacOS/*.dylib`
	sos=`ls ./CodeLite.app/Contents/MacOS/*.so`
    sos2=`ls ./CodeLite.app/Contents/SharedSupport/plugins/*.so`
    sos3=`ls ./CodeLite.app/Contents/SharedSupport/debuggers/*.so`
	file_list="${dylibs} ${sos} ${sos2} ${sos3} ./CodeLite.app/Contents/MacOS/${exe_name} "

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
					install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ${file}
			done
	done
}

fix_non_plugins_depends() {
	search_string=$1

	## Get list of files to work on
    dylibs=`ls ./CodeLite.app/Contents/MacOS/*.dylib`
	sos=`ls ./CodeLite.app/Contents/MacOS/*.so`
    sos2=`ls ./CodeLite.app/Contents/SharedSupport/plugins/*.so`
    sos3=`ls ./CodeLite.app/Contents/SharedSupport/debuggers/*.so`
	file_list="${dylibs} ${sos} ${sos2} ${sos3} ./CodeLite.app/Contents/MacOS/${exe_name} "
	
	orig_path=`otool -L ${exe_name}  | grep ${search_string} | awk '{print $1;}'`
	
	## Loop over the files, and update the path of the wx library
	for file in ${file_list}
	do
		new_path=`echo ${orig_path} | xargs basename`
		install_name_tool -change ${orig_path} @executable_path/../MacOS/${new_path} ${file}
		echo install_name_tool -change ${orig_path} @executable_path/../MacOS/${new_path} ${file}
	done
}

fix_codelite_clang_deps() {
	orig_path=`otool -L ./CodeLite.app/Contents/SharedSupport/codelite-clang  | grep libwx_* | awk '{print $1;}'`

	## Loop over the files, and update the path of the wx library
	for path in ${orig_path}
	do
		new_path=`echo ${path} | xargs basename`
		install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./CodeLite.app/Contents/SharedSupport/codelite-clang
	done
	echo install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./CodeLite.app/Contents/SharedSupport/codelite-clang
	install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./CodeLite.app/Contents/SharedSupport/codelite-clang
}

fix_codelite_indexer_deps() {

	orig_path=`otool -L ./CodeLite.app/Contents/SharedSupport/codelite_indexer  | grep libwx_* | awk '{print $1;}'`

	## Loop over the files, and update the path of the wx library
	for path in ${orig_path}
	do
		new_path=`echo ${path} | xargs basename`
		install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./CodeLite.app/Contents/SharedSupport/codelite_indexer
	done
}

## extract the file name from the Makefile
exe_name=codelite

rm -rf CodeLite.app
mkdir -p ./CodeLite.app/Contents/MacOS
mkdir -p ./CodeLite.app/Contents/Resources
mkdir -p ./CodeLite.app/Contents/SharedSupport
mkdir -p ./CodeLite.app/Contents/SharedSupport/plugins
mkdir -p ./CodeLite.app/Contents/SharedSupport/plugins/resources/
mkdir -p ./CodeLite.app/Contents/SharedSupport/debuggers
mkdir -p ./CodeLite.app/Contents/SharedSupport/config

wx_file_list=`otool -L ${exe_name}  | grep libwx_* | awk '{print $1;}'`

# fix the script
echo "Running install_name_tool..."

# copy the libs locally, the script will have an easier time finding them this way
mkdir -p lib
cp ../lib/*.so ./lib

## copy the wx dlls to the executable path which under Mac is located at ./CodeLite.app/Contents/MacOS/
for wx_file in ${wx_file_list}
do
    cp ${wx_file} ./CodeLite.app/Contents/MacOS/
done

cp ${exe_name} ./CodeLite.app/Contents/MacOS/${exe_name}

## Fix clang
cp ../sdk/clang/lib/libclang.dylib ./CodeLite.app/Contents/MacOS/
echo install_name_tool -change @rpath/libclang.dylib @executable_path/libclang.dylib ./CodeLite.app/Contents/MacOS/codelite
install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./CodeLite.app/Contents/MacOS/codelite

cp -r rc ./CodeLite.app/Contents/SharedSupport/
cp -r templates ./CodeLite.app/Contents/SharedSupport/
cp -r images ./CodeLite.app/Contents/SharedSupport/
cp -r lexers ./CodeLite.app/Contents/SharedSupport/

cp astyle.sample ./CodeLite.app/Contents/SharedSupport/
cp index.html ./CodeLite.app/Contents/SharedSupport/
cp svnreport.html ./CodeLite.app/Contents/SharedSupport/
cp *.icns ./CodeLite.app/Contents/Resources/
cp -pr src/*.gz ./CodeLite.app/Contents/Resources/
cp -pr codelite-icons.zip ./CodeLite.app/Contents/SharedSupport/
cp -pr codelite-icons-fresh-farm.zip ./CodeLite.app/Contents/SharedSupport/

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

## Copy plugins...
cp ../lib/CodeFormatter.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Debugger.so ./CodeLite.app/Contents/SharedSupport/debuggers/
cp ../lib/Gizmos.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Subversion2.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/cscope.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Copyright.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/UnitTestCPP.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/ExternalTools.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Outline.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/ContinuousBuild.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/SnipWiz.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/wxformbuilder.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/abbreviation.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/QmakePlugin.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/CppCheck.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/MacBundler.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/DatabaseExplorer.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/CallGraph.so ./CodeLite.app/Contents/SharedSupport/plugins/

if [ -f ../lib/wxcrafter.so ]; then
    cp ../lib/wxcrafter.so ./CodeLite.app/Contents/SharedSupport/plugins/
    cp -pr ../wxcrafter/wxgui.zip ./CodeLite.app/Contents/SharedSupport/
fi

cp ../lib/libpluginu.so ./CodeLite.app/Contents/MacOS/
cp ../lib/libcodeliteu.so ./CodeLite.app/Contents/MacOS/
cp ../lib/libwxsqlite3u.so ./CodeLite.app/Contents/MacOS/
cp ../lib/libdblayersqliteu.so ./CodeLite.app/Contents/MacOS/
cp ../lib/libwxshapeframeworku.so ./CodeLite.app/Contents/MacOS/

cp ./codelite_indexer  ./CodeLite.app/Contents/SharedSupport/
cp ../codelite_clang/codelite-clang  ./CodeLite.app/Contents/SharedSupport/
cp ../codelitegcc/codelitegcc  ./CodeLite.app/Contents/MacOS/
cp ../sdk/codelite_cppcheck/codelite_cppcheck ./CodeLite.app/Contents/SharedSupport/
cp ./OpenTerm   ./CodeLite.app/Contents/SharedSupport/
cp plugins/resources/*.*  ./CodeLite.app/Contents/SharedSupport/plugins/resources/

## Copy the locale files
for lang in locale/* ; do
	lang=`basename $lang`
	if test -f locale/$lang/codelite.mo ; then
		mkdir -p ./CodeLite.app/Contents/Resources/$lang.lproj/
		cp -f locale/$lang/codelite.mo ./CodeLite.app/Contents/Resources/$lang.lproj/
	fi
done

fix_codelite_indexer_deps
fix_codelite_clang_deps
fix_shared_object_depends libwx_
fix_non_plugins_depends ./lib/libcodeliteu.so
fix_non_plugins_depends ./lib/libpluginu.so
fix_non_plugins_depends ./lib/libwxsqlite3u.so
fix_non_plugins_depends ./lib/libwxshapeframeworku.so
fix_non_plugins_depends ./lib/libdblayersqliteu.so

