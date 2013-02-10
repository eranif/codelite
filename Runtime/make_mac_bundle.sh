#!/bin/sh
enable_debug=$1
ext="u"
if [ "${enable_debug}" = "debug" ]; then
    ext="ud"
fi

fix_shared_object_depends() {
    search_string=$1

    ## Get list of files to work on
    dylibs=`ls ./codelite.app/Contents/MacOS/*.dylib`
    sos=`ls ./codelite.app/Contents/MacOS/*.dylib`
    sos2=`ls ./codelite.app/Contents/SharedSupport/plugins/*.dylib`
    sos3=`ls ./codelite.app/Contents/SharedSupport/debuggers/*.dylib`
    file_list="${dylibs} ${sos} ${sos2} ${sos3} ./codelite.app/Contents/MacOS/codelite "

    ## Since the plugins must use the same wx configuration as the
    ## executable, we can run the following command only once and
    ## use the results to manipulate the plugins as well
    orig_path=`otool -L codelite  | grep ${search_string} | awk '{print $1;}'`

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
    dylibs=`ls ./codelite.app/Contents/MacOS/*.dylib`
    sos=`ls ./codelite.app/Contents/MacOS/*.dylib`
    sos2=`ls ./codelite.app/Contents/SharedSupport/plugins/*.dylib`
    sos3=`ls ./codelite.app/Contents/SharedSupport/debuggers/*.dylib`
    file_list="${dylibs} ${sos} ${sos2} ${sos3} ./codelite.app/Contents/MacOS/codelite "
    
    orig_path=`otool -L codelite  | grep ${search_string} | awk '{print $1;}'`
    
    ## Loop over the files, and update the path of the wx library
    for file in ${file_list}
    do
        new_path=`echo ${orig_path} | xargs basename`
        install_name_tool -change ${orig_path} @executable_path/../MacOS/${new_path} ${file}
        echo install_name_tool -change ${orig_path} @executable_path/../MacOS/${new_path} ${file}
    done
}

fix_codelite_clang_deps() {
    orig_path=`otool -L ./codelite.app/Contents/SharedSupport/codelite-clang  | grep libwx_* | awk '{print $1;}'`

    ## Loop over the files, and update the path of the wx library
    for path in ${orig_path}
    do
        new_path=`echo ${path} | xargs basename`
        install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./codelite.app/Contents/SharedSupport/codelite-clang
    done
    echo install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./codelite.app/Contents/SharedSupport/codelite-clang
    install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./codelite.app/Contents/SharedSupport/codelite-clang
}

fix_codelite_indexer_deps() {

    orig_path=`otool -L ./codelite.app/Contents/SharedSupport/codelite_indexer  | grep libwx_* | awk '{print $1;}'`

    ## Loop over the files, and update the path of the wx library
    for path in ${orig_path}
    do
        new_path=`echo ${path} | xargs basename`
        install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./codelite.app/Contents/SharedSupport/codelite_indexer
    done
}

fix_wxrc_deps() {

    orig_path=`otool -L ./codelite.app/Contents/SharedSupport/wxrc  | grep libwx_* | awk '{print $1;}'`

    ## Loop over the files, and update the path of the wx library
    for path in ${orig_path}
    do
        new_path=`echo ${path} | xargs basename`
        install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./codelite.app/Contents/SharedSupport/wxrc
    done
}

## extract the file name from the Makefile
exe_name=codelite

rm -rf codelite.app
mkdir -p ./codelite.app/Contents/MacOS
mkdir -p ./codelite.app/Contents/Resources
mkdir -p ./codelite.app/Contents/SharedSupport
mkdir -p ./codelite.app/Contents/SharedSupport/plugins
mkdir -p ./codelite.app/Contents/SharedSupport/plugins/resources/
mkdir -p ./codelite.app/Contents/SharedSupport/debuggers
mkdir -p ./codelite.app/Contents/SharedSupport/config

wx_file_list=`otool -L codelite  | grep libwx_* | awk '{print $1;}'`

# fix the script
echo "Running install_name_tool..."

# copy the libs locally, the script will have an easier time finding them this way
mkdir -p lib
cp ../lib/*.dylib ./lib

## copy the wx dlls to the executable path which under Mac is located at ./codelite.app/Contents/MacOS/
for wx_file in ${wx_file_list}
do
    cp ${wx_file} ./codelite.app/Contents/MacOS/
done

## Copy wxrc tool
wx_config=`which wx-config`
wxrc_tool=`dirname ${wx_config}`
wxrc_tool=${wxrc_tool}/utils/wxrc/wxrc

cp ${wxrc_tool} ./codelite.app/Contents/SharedSupport/
cp ../bin/codelite ./codelite.app/Contents/MacOS/codelite

## Fix clang
cp ../sdk/clang/lib/libclang.dylib ./codelite.app/Contents/MacOS/
echo install_name_tool -change @rpath/libclang.dylib @executable_path/libclang.dylib ./codelite.app/Contents/MacOS/codelite
install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./codelite.app/Contents/MacOS/codelite

cp -r rc ./codelite.app/Contents/SharedSupport/
cp -r templates ./codelite.app/Contents/SharedSupport/
cp -r images ./codelite.app/Contents/SharedSupport/
cp -r lexers ./codelite.app/Contents/SharedSupport/

cp astyle.sample ./codelite.app/Contents/SharedSupport/
cp index.html ./codelite.app/Contents/SharedSupport/
cp svnreport.html ./codelite.app/Contents/SharedSupport/
cp *.icns ./codelite.app/Contents/Resources/
cp -pr src/*.gz ./codelite.app/Contents/Resources/
cp -pr codelite-icons.zip ./codelite.app/Contents/SharedSupport/
cp -pr codelite-icons-fresh-farm.zip ./codelite.app/Contents/SharedSupport/

## copy empty layout file
cp config/codelite.layout.default ./codelite.app/Contents/SharedSupport/config/codelite.layout
cp config/accelerators.conf.default ./codelite.app/Contents/SharedSupport/config/
cp config/build_settings.xml.default.mac ./codelite.app/Contents/SharedSupport/config/build_settings.xml.default
cp config/plugins.xml.default ./codelite.app/Contents/SharedSupport/config

## copy default Mac configuration file
cp config/codelite.xml.default.mac ./codelite.app/Contents/SharedSupport/config/codelite.xml.default

## replace the executable name according to the configuration used in the build
cat Info.plist.template | sed s/EXE_NAME/codelite/g >> ./codelite.app/Contents/Info.plist

cp config/debuggers.xml.default ./codelite.app/Contents/SharedSupport/config

## Copy plugins...
cp ../lib/CodeFormatter.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/Debugger.dylib ./codelite.app/Contents/SharedSupport/debuggers/
cp ../lib/Gizmos.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/Subversion2.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/cscope.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/Copyright.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/UnitTestCPP.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/ExternalTools.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/Outline.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/ContinuousBuild.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/SnipWiz.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/wxformbuilder.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/abbreviation.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/QmakePlugin.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/CppCheck.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/MacBundler.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/DatabaseExplorer.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/CallGraph.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/git.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/ZoomNavigator.dylib ./codelite.app/Contents/SharedSupport/plugins/

if [ -f ../lib/wxcrafter.dylib ]; then
    cp ../lib/wxcrafter.dylib ./codelite.app/Contents/SharedSupport/plugins/
    cp -pr ../wxcrafter/wxgui.zip ./codelite.app/Contents/SharedSupport/
fi

cp ../lib/libplugin.dylib ./codelite.app/Contents/MacOS/
cp ../lib/libcodelite.dylib ./codelite.app/Contents/MacOS/
cp ../lib/libwxsqlite3.dylib ./codelite.app/Contents/MacOS/
cp ../lib/libdblayersqlite.dylib ./codelite.app/Contents/MacOS/
cp ../lib/libwxshapeframework.dylib ./codelite.app/Contents/MacOS/

cp ../bin/codelite_indexer  ./codelite.app/Contents/SharedSupport/
cp ../bin/codelite_clang/codelite-clang  ./codelite.app/Contents/SharedSupport/
cp ../bin/codelitegcc/codelitegcc  ./codelite.app/Contents/MacOS/
cp ../bin/codelite_cppcheck/codelite_cppcheck ./codelite.app/Contents/SharedSupport/
cp ./OpenTerm   ./codelite.app/Contents/SharedSupport/
cp plugins/resources/*.*  ./codelite.app/Contents/SharedSupport/plugins/resources/

## Copy the locale files
for lang in locale/* ; do
    lang=`basename $lang`
    if test -f locale/$lang/codelite.mo ; then
        mkdir -p ./codelite.app/Contents/Resources/$lang.lproj/
        cp -f locale/$lang/codelite.mo ./codelite.app/Contents/Resources/$lang.lproj/
    fi
done

fix_codelite_indexer_deps
fix_wxrc_deps
fix_codelite_clang_deps
fix_shared_object_depends libwx_
fix_non_plugins_depends ./lib/libcodelite.dylib
fix_non_plugins_depends ./lib/libplugin.dylib
fix_non_plugins_depends ./lib/libwxsqlite3.dylib
fix_non_plugins_depends ./lib/libwxshapeframework.dylib
fix_non_plugins_depends ./lib/libdblayersqlite.dylib

