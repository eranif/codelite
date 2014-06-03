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
    sos2=`ls ./codelite.app/Contents/SharedSupport/plugins/*.dylib`
    sos3=`ls ./codelite.app/Contents/SharedSupport/debuggers/*.dylib`
    file_list="${dylibs} ${sos2} ${sos3} ./codelite.app/Contents/MacOS/codelite "

    ## Since the plugins must use the same wx configuration as the
    ## executable, we can run the following command only once and
    ## use the results to manipulate the plugins as well
    orig_path=`otool -L ../bin/codelite  | grep ${search_string} | awk '{print $1;}'`

    ## Loop over the files, and update the path of the wx library
    for file in ${file_list}
    do
        echo "Fixing file $file..."
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
    sos2=`ls ./codelite.app/Contents/SharedSupport/plugins/*.dylib`
    sos3=`ls ./codelite.app/Contents/SharedSupport/debuggers/*.dylib`
    file_list="${dylibs} ${sos2} ${sos3} ./codelite.app/Contents/MacOS/codelite "
    
    ## add codelite-lldb if it exists
    if test -f ./codelite.app/Contents/MacOS/codelite-lldb ; then
        file_list="${file_list} ./codelite.app/Contents/MacOS/codelite-lldb"
    fi

    for SO in ${file_list}
    do   
    orig_path=`otool -L ${SO}  | grep ${search_string} | awk '{print $1;}'`
    if [ ! -z ${orig_path} ]; then
        ## Loop over the files, and update the path of the wx library
        for file in ${file_list}
        do
            new_path=`echo ${orig_path} | xargs basename`
            install_name_tool -change ${orig_path} @executable_path/../MacOS/${new_path} ${file}
        done
    fi
    done
}

fix_codelite_clang_deps() {
    if test -f ./codelite.app/Contents/SharedSupport/codelite-clang ; then
        orig_path=`otool -L ./codelite.app/Contents/SharedSupport/codelite-clang  | grep libwx_* | awk '{print $1;}'`

        ## Loop over the files, and update the path of the wx library
        for path in ${orig_path}
        do
            new_path=`echo ${path} | xargs basename`
            install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./codelite.app/Contents/SharedSupport/codelite-clang
        done
        echo install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./codelite.app/Contents/SharedSupport/codelite-clang
        install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./codelite.app/Contents/SharedSupport/codelite-clang
    fi
}

fix_codelite_lldb_deps() {
    if test -f ./codelite.app/Contents/MacOS/codelite-lldb ; then
        orig_path=`otool -L ./codelite.app/Contents/MacOS/codelite-lldb  | grep libwx_* | awk '{print $1;}'`
        
        ## Fix wxWidgets dependencies
        ## Loop over the files, and update the path of the wx library
        for path in ${orig_path}
        do
            new_path=`echo ${path} | xargs basename`
            install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./codelite.app/Contents/MacOS/codelite-lldb
        done

        echo install_name_tool -change @rpath/liblldb.3.5.0.dylib @executable_path/../SharedSupport/liblldb.3.5.0.dylib ./codelite.app/Contents/MacOS/codelite-lldb
        install_name_tool -change @rpath/liblldb.3.5.0.dylib @executable_path/../SharedSupport/liblldb.3.5.0.dylib ./codelite.app/Contents/MacOS/codelite-lldb
        
        ## Fix libcodelite, libplugin and libwxsqlite
        
    fi
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

fix_codelite_make_deps() {

    orig_path=`otool -L ./codelite.app/Contents/SharedSupport/codelite-make  | grep libwx_* | awk '{print $1;}'`

    ## Loop over the files, and update the path of the wx library
    for path in ${orig_path}
    do
        new_path=`echo ${path} | xargs basename`
        install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./codelite.app/Contents/SharedSupport/codelite-make
    done
}

fix_codelite_terminal_deps() {

    orig_path=`otool -L ./codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/codelite-terminal  | grep libwx_* | awk '{print $1;}'`

    ## Loop over the files, and update the path of the wx library
    for path in ${orig_path}
    do
        new_path=`echo ${path} | xargs basename`
        install_name_tool -change ${path} @executable_path/../../../${new_path} ./codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/codelite-terminal
    done
}

fix_wxrc_deps() {
    if test -f ./codelite.app/Contents/SharedSupport/wxrc ; then
        orig_path=`otool -L ./codelite.app/Contents/SharedSupport/wxrc  | grep libwx_* | awk '{print $1;}'`

        ## Loop over the files, and update the path of the wx library
        for path in ${orig_path}
        do
            new_path=`echo ${path} | xargs basename`
            install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ./codelite.app/Contents/SharedSupport/wxrc
        done
    fi
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
mkdir -p ./codelite.app/Contents/SharedSupport/config/cppcheck

wx_file_list=`otool -L ../bin/codelite  | grep libwx_* | awk '{print $1;}'`

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

if test -f ${wxrc_tool} ; then
    cp ${wxrc_tool} ./codelite.app/Contents/SharedSupport/
fi
cp ../bin/codelite ./codelite.app/Contents/MacOS/codelite

## codelite-terminal bundle
mkdir -p ./codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS
mkdir -p ./codelite.app/Contents/MacOS/codelite-terminal.app/Contents/Resources
cp ../../codelite_terminal/icon.icns ./codelite.app/Contents/MacOS/codelite-terminal.app/Contents/Resources
cp ../bin/codelite-terminal  ./codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/
cp ../../codelite_terminal/Info.plist ./codelite.app/Contents/MacOS/codelite-terminal.app/Contents/

## Fix clang
cp ../../sdk/clang/lib/libclang.dylib ./codelite.app/Contents/MacOS/
echo install_name_tool -change @rpath/libclang.dylib @executable_path/libclang.dylib ./codelite.app/Contents/MacOS/codelite
install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib ./codelite.app/Contents/MacOS/codelite

cp -r ../../Runtime/rc ./codelite.app/Contents/SharedSupport/
cp -r ../../Runtime/templates ./codelite.app/Contents/SharedSupport/
cp -r ../../Runtime/images ./codelite.app/Contents/SharedSupport/
cp -r ../../Runtime/lexers ./codelite.app/Contents/SharedSupport/
cp -r ../../Runtime/gdb_printers ./codelite.app/Contents/SharedSupport/

cp ../../Runtime/astyle.sample ./codelite.app/Contents/SharedSupport/
cp ../../Runtime/index.html ./codelite.app/Contents/SharedSupport/
cp ../../Runtime/svnreport.html ./codelite.app/Contents/SharedSupport/
cp ../../Runtime/*.icns ./codelite.app/Contents/Resources/

cp -pr ../../Runtime/src/*.gz ./codelite.app/Contents/Resources/
cp -pr ../../Runtime/codelite-icons.zip ./codelite.app/Contents/SharedSupport/
cp -pr ../../Runtime/codelite-icons-dark.zip ./codelite.app/Contents/SharedSupport/
cp -pr ../../Runtime/codelite-icons-fresh-farm.zip ./codelite.app/Contents/SharedSupport/

## copy empty layout file
cp ../../Runtime/config/codelite.layout.default ./codelite.app/Contents/SharedSupport/config/codelite.layout
cp ../../sdk/codelite_cppcheck/cfg/*.cfg ./codelite.app/Contents/SharedSupport/config/cppcheck/
cp ../../Runtime/config/accelerators.conf.default ./codelite.app/Contents/SharedSupport/config/
cp ../../Runtime/config/build_settings.xml.default.mac ./codelite.app/Contents/SharedSupport/config/build_settings.xml.default
cp ../../Runtime/config/plugins.xml.default ./codelite.app/Contents/SharedSupport/config

## copy default Mac configuration file
cp ../../Runtime/config/codelite.xml.default.mac ./codelite.app/Contents/SharedSupport/config/codelite.xml.default

## replace the executable name according to the configuration used in the build
cat ../../Runtime/Info.plist.template | sed s/EXE_NAME/codelite/g >> ./codelite.app/Contents/Info.plist

cp ../../Runtime/config/debuggers.xml.mac ./codelite.app/Contents/SharedSupport/config/debuggers.xml.default

## License
cp ../../Runtime/../LICENSE  ./codelite.app/Contents/SharedSupport/

## Copy plugins...
cp ../lib/CodeFormatter.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/DebuggerGDB.dylib ./codelite.app/Contents/SharedSupport/debuggers/
cp ../lib/Wizards.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/Subversion.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/cscope.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/Copyright.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/UnitTestsPP.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/ExternalTools.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/Outline.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/ContinuousBuild.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/SnipWiz.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/wxFormBuilder.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/abbreviation.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/QMakePlugin.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/cppchecker.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/MacBundler.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/DatabaseExplorer.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/Tweaks.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/git.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/ZoomNavigator.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/SFTP.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/CMakePlugin.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/CodeLiteDiff.dylib ./codelite.app/Contents/SharedSupport/plugins/

## Fix LLDB
echo "Installing LLDBDebugger..."
echo cp ../lib/LLDBDebugger.dylib ./codelite.app/Contents/SharedSupport/plugins/
cp ../lib/LLDBDebugger.dylib ./codelite.app/Contents/SharedSupport/plugins/

## copy our debug server
cp ../bin/codelite-lldb  ./codelite.app/Contents/MacOS/
fix_codelite_lldb_deps

## Copy Apple's debugserver (its signed)
echo cp ../../Runtime/debugserver  ./codelite.app/Contents/SharedSupport/
cp ../../Runtime/debugserver  ./codelite.app/Contents/SharedSupport/

cp ../../sdk/lldb/unix/lib/liblldb.3.5.0.dylib ./codelite.app/Contents/SharedSupport/
install_name_tool -change @rpath/liblldb.3.5.0.dylib @executable_path/../SharedSupport/liblldb.3.5.0.dylib ./codelite.app/Contents/SharedSupport/plugins/LLDBDebugger.dylib

if [ -f ../lib/wxcrafter.dylib ]; then
    cp ../lib/wxcrafter.dylib ./codelite.app/Contents/SharedSupport/plugins/
    cp -pr ../../Runtime/../wxcrafter/wxgui.zip ./codelite.app/Contents/SharedSupport/
    cp -pr ../../Runtime/../wxcrafter/wxcrafter.accelerators ./codelite.app/Contents/SharedSupport/plugins/resources
fi

cp ../lib/libplugin.dylib ./codelite.app/Contents/MacOS/
cp ../lib/liblibcodelite.dylib ./codelite.app/Contents/MacOS/
cp ../lib/libwxsqlite3.dylib ./codelite.app/Contents/MacOS/
cp ../lib/libdatabaselayersqlite.dylib ./codelite.app/Contents/MacOS/
cp ../lib/libwxshapeframework.dylib ./codelite.app/Contents/MacOS/

cp ../bin/codelite_indexer  ./codelite.app/Contents/SharedSupport/

if test -f ../bin/codelite-clang ; then
    cp ../bin/codelite-clang  ./codelite.app/Contents/SharedSupport/
fi

cp ../bin/codelite-cc  ./codelite.app/Contents/MacOS/
cp ../bin/codelite-echo  ./codelite.app/Contents/MacOS/
cp ../bin/codelite-make  ./codelite.app/Contents/SharedSupport/
cp ../bin/codelite_cppcheck ./codelite.app/Contents/SharedSupport/
cp ../../Runtime/./OpenTerm   ./codelite.app/Contents/SharedSupport/
cp ../../Runtime/plugins/resources/*.*  ./codelite.app/Contents/SharedSupport/plugins/resources/

## Copy the locale files
for lang in locale/* ; do
    lang=`basename $lang`
    if test -f locale/$lang/codelite.mo ; then
        mkdir -p ./codelite.app/Contents/Resources/$lang.lproj/
        cp -f locale/$lang/codelite.mo ./codelite.app/Contents/Resources/$lang.lproj/
    fi
done

fix_codelite_indexer_deps
fix_codelite_make_deps
fix_codelite_terminal_deps
fix_wxrc_deps
fix_codelite_clang_deps
fix_shared_object_depends libwx_

## the blow fixes the paths embedded in the executable located under codelite.app/Contents/MacOS/
## the function fix_non_plugins_depends accepts search string 
fix_non_plugins_depends lib/liblibcodelite.dylib
fix_non_plugins_depends lib/libplugin.dylib
fix_non_plugins_depends lib/libwxsqlite3.dylib
fix_non_plugins_depends lib/libwxshapeframework.dylib
fix_non_plugins_depends lib/libdatabaselayersqlite.dylib

