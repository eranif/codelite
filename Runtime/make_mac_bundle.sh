#!/bin/sh

## extract the file name from the Makefile
exe_name=`cat ../Makefile | grep ^EXE_NAME_NO_PATH | cut -d= -f2`

rm -rf CodeLite.app
mkdir -p ./CodeLite.app/Contents/MacOS
mkdir -p ./CodeLite.app/Contents/Resources
mkdir -p ./CodeLite.app/Contents/SharedSupport
mkdir -p ./CodeLite.app/Contents/SharedSupport/plugins
mkdir -p ./CodeLite.app/Contents/SharedSupport/debuggers
mkdir -p ./CodeLite.app/Contents/SharedSupport/config

cp ${exe_name} ./CodeLite.app/Contents/MacOS/${exe_name}
cp -r rc ./CodeLite.app/Contents/SharedSupport/
cp -r templates ./CodeLite.app/Contents/SharedSupport/
cp -r images ./CodeLite.app/Contents/SharedSupport/
cp -r lexers ./CodeLite.app/Contents/SharedSupport/

cp astyle.sample ./CodeLite.app/Contents/SharedSupport/
cp index.html ./CodeLite.app/Contents/SharedSupport/
cp svnreport.html ./CodeLite.app/Contents/SharedSupport/
cp icon.icns ./CodeLite.app/Contents/Resources/

## copy empty layout file
cp config/codelite.layout.default ./CodeLite.app/Contents/SharedSupport/config/codelite.layout
cp config/accelerators.conf.default ./CodeLite.app/Contents/SharedSupport/config/
cp config/build_settings.xml ./CodeLite.app/Contents/SharedSupport/config

##
## replace the revision macro
##
cur_rev=`svn info | grep Revision | awk '{print $2;}'`
file_exist=`test -f ./CodeLite.app/Contents/SharedSupport/config/liteeditor.xml`
file_exist=$?
if [ $file_exist -eq 1 ] 
then
	## file does not exist, create an empty file
	touch ./CodeLite.app/Contents/SharedSupport/config/codelite.xml
fi

## empty the file
\cp /dev/null ./CodeLite.app/Contents/SharedSupport/config/codelite.xml

## replace $(Revision) with the actual revision, and create new file
cat config/liteeditor.xml.mac | sed s/\$\(Revision\)/${cur_rev}/g >> ./CodeLite.app/Contents/SharedSupport/config/codelite.xml.default

## replace the executable name according to the configuration used in the build
cat Info.plist.template | sed s/EXE_NAME/${exe_name}/g >> ./CodeLite.app/Contents/Info.plist

cp config/debuggers.xml ./CodeLite.app/Contents/SharedSupport/config

cp ../lib/CodeFormatter.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Debugger.so ./CodeLite.app/Contents/SharedSupport/debuggers/
cp ../lib/Gizmos.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Subversion.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/cscope.so ./CodeLite.app/Contents/SharedSupport/plugins/

cp ../sdk/ctags/ctags-le ./CodeLite.app/Contents/SharedSupport/
