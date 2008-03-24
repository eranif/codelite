#!/bin/sh

rm -rf CodeLite.app
mkdir -p ./CodeLite.app/Contents/MacOS
mkdir -p ./CodeLite.app/Contents/Resources
mkdir -p ./CodeLite.app/Contents/SharedSupport
mkdir -p ./CodeLite.app/Contents/SharedSupport/plugins
mkdir -p ./CodeLite.app/Contents/SharedSupport/debuggers
mkdir -p ./CodeLite.app/Contents/SharedSupport/config

cp CodeLiteUR ./CodeLite.app/Contents/MacOS/CodeLiteUR
cp -r rc ./CodeLite.app/Contents/SharedSupport/
cp -r templates ./CodeLite.app/Contents/SharedSupport/
cp -r images ./CodeLite.app/Contents/SharedSupport/
cp -r lexers ./CodeLite.app/Contents/SharedSupport/

cp astyle.sample ./CodeLite.app/Contents/SharedSupport/
cp index.html ./CodeLite.app/Contents/SharedSupport/
cp svnreport.html ./CodeLite.app/Contents/SharedSupport/
cp Info.plist ./CodeLite.app/Contents/
cp icon.icns ./CodeLite.app/Contents/Resources/

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
	touch ./CodeLite.app/Contents/SharedSupport/config/liteeditor.xml
fi

## empty the file
\cp /dev/null ./CodeLite.app/Contents/SharedSupport/config/liteeditor.xml

## replace $(Revision) with the actual revision, and create new file
cat config/liteeditor.xml.mac | sed s/\$\(Revision\)/${cur_rev}/g >> ./CodeLite.app/Contents/SharedSupport/config/liteeditor.xml

cp config/debuggers.xml ./CodeLite.app/Contents/SharedSupport/config

cp ../lib/CodeFormatter.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Debugger.so ./CodeLite.app/Contents/SharedSupport/debuggers/
cp ../lib/Gizmos.so ./CodeLite.app/Contents/SharedSupport/plugins/
cp ../lib/Subversion.so ./CodeLite.app/Contents/SharedSupport/plugins/

cp ../sdk/ctags/ctags-le ./CodeLite.app/Contents/SharedSupport/
