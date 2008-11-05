#!/bin/sh
## Copy files to the fakeroot directory structure

cur_rev=`svn info | grep Revision | awk '{print $2;}'`
arch=`uname -m`
PKG_NAME=codelite-1.0.${cur_rev}_${arch}.deb

## clear previous packaged files
\rm -fr fakeroot/usr/

EXE_NAME=CodeLiteUR
mkdir -p fakeroot/usr/local/bin/
cp ./Runtime/${EXE_NAME} fakeroot/usr/local/bin/CodeLite
cp Runtime/le_dos2unix.sh fakeroot/usr/local/bin/
cp Runtime/le_killproc.sh fakeroot/usr/local/bin/
cp Runtime/le_exec.sh fakeroot/usr/local/bin/
chmod +x fakeroot/usr/local/bin/le_dos2unix.sh
chmod +x fakeroot/usr/local/bin/le_killproc.sh
chmod +x fakeroot/usr/local/bin/le_exec.sh
cp sdk/ctags/ctags-le fakeroot/usr/local/bin/
mkdir -p fakeroot/usr/local/share/codelite
mkdir -p fakeroot/usr/local/share/codelite/config
mkdir -p fakeroot/usr/local/share/codelite/debuggers
mkdir -p fakeroot/usr/local/share/codelite/templates
mkdir -p fakeroot/usr/local/share/codelite/plugins
mkdir -p fakeroot/usr/local/share/codelite/plugins/resources
mkdir -p fakeroot/usr/local/share/codelite/images
mkdir -p fakeroot/usr/local/share/codelite/rc
mkdir -p fakeroot/usr/local/share/codelite/src
mkdir -p fakeroot/usr/local/share/codelite/lexers
mkdir -p fakeroot/usr/share/applications/
\cp -pr Runtime/config/build_settings.xml.default fakeroot/usr/local/share/codelite/config/build_settings.xml.default
\cp -pr Runtime/config/plugins.xml.default fakeroot/usr/local/share/codelite/config/plugins.xml.default
\cp -pr Runtime/config/debuggers.xml.gtk fakeroot/usr/local/share/codelite/config/debuggers.xml.default
\cp -pr Runtime/config/accelerators.conf.default fakeroot/usr/local/share/codelite/config/accelerators.conf.default
\cp -pr Runtime/config/codelite.xml.default.gtk fakeroot/usr/local/share/codelite/config/codelite.xml.default
\cp -pr Runtime/rc/menu.xrc fakeroot/usr/local/share/codelite/rc/
\cp -pfr Runtime/templates/* fakeroot/usr/local/share/codelite/templates/
\cp -pfr Runtime/lexers/* fakeroot/usr/local/share/codelite/lexers/
\cp -pr Runtime/codelite.desktop fakeroot/usr/share/applications/
\cp -pr static-symbols.tags fakeroot/usr/local/share/codelite/
\cp -pr lib/Debugger.so fakeroot/usr/local/share/codelite/debuggers/
\cp -pr lib/CodeFormatter.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/Copyright.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/Subversion.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/Gizmos.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/cscope.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/UnitTestCPP.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/ExternalTools.so fakeroot/usr/local/share/codelite/plugins/
#\cp -pr lib/SymbolView.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/SnipWiz.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/wxformbuilder.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr lib/abbreviation.so fakeroot/usr/local/share/codelite/plugins/
\cp -pr Runtime/plugins/resources/*.* fakeroot/usr/local/share/codelite/plugins/resources/
\cp -pr Runtime/*.html fakeroot/usr/local/share/codelite/
\cp -pr Runtime/astyle.sample fakeroot/usr/local/share/codelite/
\cp -pr Runtime/src/* fakeroot/usr/local/share/codelite/src/
\cp -pr Runtime/images/*.png fakeroot/usr/local/share/codelite/images/

## Clean all .svn files
find fakeroot/usr/ -name ".svn" | xargs rm -fr

chmod -R 777 fakeroot/usr/local/share/codelite
chmod +x fakeroot/usr/local/bin/CodeLite
echo creating .deb package...
dpkg -b fakeroot ${PKG_NAME}

