#!/bin/sh
################################################################################
## This file is part of CodeLite IDE and is released
## under the terms of the GNU General Public License as published by
##    the Free Software Foundation; either version 2 of the License, or
##    (at your option) any later version.
################################################################################


## Copy files to the fakeroot directory structure
cur_rev=`LC_ALL=C svn info | grep Revision | awk '{print $2;}'`
arch=`uname -m`

if [ "${arch}" = "i686" ]; then
    arch="i386"
elif [ "${arch}" = "x86_64" ]; then
    arch="amd64"
fi

## Parse command line argument for getting install prefix
parseCommandLine()
{
    case $1 in
        "-d")   PREFIX="/usr";;

        "-u")   PREFIX="/usr"
                cur_rev="${cur_rev}-ubuntu0";;

        "--help" | "-h")    usage
                            exit 0;;
        *   )   usage
                exit 1;;
    esac
}

## Show help
usage()
{
    cat <<EOF
Usage:
  `basename $0` [options]

Options:
  -h | --help   this help screen
  -d            make deb-package for Debian
  -u            make deb-package for Ubuntu

EOF
}

## Making control and postinst files
generateDebianFiles()
{
    cat > fakeroot/DEBIAN/control <<EOF
Package: codelite
Version: 2.9.0.${cur_rev}
Section: devel
Priority: optional
Architecture: ${arch}
Essential: no
Depends: libwxgtk2.8-0, libgtk2.0-0
Recommends: gdb
Conflicts: codelite-plugins
Installed-Size: ${inst_size}
Maintainer: Eran Ifrah [eran.ifrah@gmail.com]
Description: A powerful open-source, cross platform code editor
     Codelite is designed for the C/C++ programming languages, and is regularly tested
     on Ubuntu lucid and Debian lenny/squeeze.
		 CodeLite is distributed under the terms of the GPL license.
		 CodeLite uses a sophisticated yet intuitive interface, which allows user easily to
		 create and build complex projects.

EOF
    cat > fakeroot/DEBIAN/postinst <<EOF
#!/bin/sh
echo "Running postinst step..."

## Menu updating
if [ "\$1" = "configure" ] && [ -x "\`which update-menus 2>/dev/null\`" ]; then
    update-menus
fi
echo "Done"
EOF
    chmod 0755 fakeroot/DEBIAN/postinst
}

## Making destop file
generateDesktopFile()
{
    rm -fr fakeroot/usr/share/applications/codelite.desktop
    cp -fr Runtime/codelite.desktop fakeroot/usr/share/applications/codelite.desktop
    chmod +x fakeroot/usr/share/applications/codelite.desktop
}

## Making menu entry for WMs
generateMenuFile()
{
    rm -fr fakeroot/usr/share/menu/codelite
    echo "?package(codelite):needs=\"x11\" section=\"Apps/Programming\" \ " >> fakeroot/usr/share/menu/codelite
    echo "        title=\"codelite\" command=\"${PREFIX}/bin/codelite\" \ " >> fakeroot/usr/share/menu/codelite
    echo "        icon=\"${PREFIX}/share/codelite/images/cubes.png\" " >> fakeroot/usr/share/menu/codelite
    echo "" >> fakeroot/usr/share/menu/codelite
}

## Parse command line options
parseCommandLine $1

EXE_NAME=codelite

## clear previous packaged files
rm -fr fakeroot
mkdir -p fakeroot/DEBIAN/
mkdir -p fakeroot/${PREFIX}/bin/

cp ./Runtime/${EXE_NAME} fakeroot/${PREFIX}/bin/codelite
cp sdk/codelite_indexer/codelite_indexer   fakeroot/${PREFIX}/bin/
cp sdk/codelite_cppcheck/codelite_cppcheck fakeroot/${PREFIX}/bin/
cp Runtime/codelite_exec                   fakeroot/${PREFIX}/bin/
cp Runtime/codelite_kill_children          fakeroot/${PREFIX}/bin/
cp Runtime/codelite_xterm                  fakeroot/${PREFIX}/bin/
cp Runtime/codelite_fix_files              fakeroot/${PREFIX}/bin/

chmod +x fakeroot/${PREFIX}/bin/codelite_fix_files
chmod +x fakeroot/${PREFIX}/bin/codelite_kill_children
chmod +x fakeroot/${PREFIX}/bin/codelite_xterm
chmod +x fakeroot/${PREFIX}/bin/codelite_exec
mkdir -p fakeroot/${PREFIX}/share/codelite
mkdir -p fakeroot/${PREFIX}/share/codelite/config
mkdir -p fakeroot/${PREFIX}/share/codelite/templates
mkdir -p fakeroot/${PREFIX}/share/codelite/images
mkdir -p fakeroot/${PREFIX}/share/codelite/rc
mkdir -p fakeroot/${PREFIX}/share/codelite/src
mkdir -p fakeroot/${PREFIX}/share/codelite/lexers
mkdir -p fakeroot/${PREFIX}/lib/codelite/
mkdir -p fakeroot/${PREFIX}/lib/codelite/debuggers
mkdir -p fakeroot/${PREFIX}/lib/codelite/resources

mkdir -p fakeroot/usr/share/applications/
mkdir -p fakeroot/usr/share/menu/
mkdir -p fakeroot/usr/share/locale/

## Copy the locale files
for lang in Runtime/locale/* ; do
	lang=`basename $lang`
	if test -f Runtime/locale/$lang/codelite.mo ; then
		mkdir -p fakeroot/usr/share/locale/$lang/LC_MESSAGES
		cp -f Runtime/locale/$lang/codelite.mo fakeroot/usr/share/locale/$lang/LC_MESSAGES/
	fi
done

cp -pr Runtime/config/build_settings.xml.default fakeroot/${PREFIX}/share/codelite/config/build_settings.xml.default
cp -pr Runtime/config/plugins.xml.default fakeroot/${PREFIX}/share/codelite/config/plugins.xml.default
cp -pr Runtime/config/debuggers.xml.gtk fakeroot/${PREFIX}/share/codelite/config/debuggers.xml.default
cp -pr Runtime/config/accelerators.conf.default fakeroot/${PREFIX}/share/codelite/config/accelerators.conf.default
cp -pr Runtime/config/codelite.xml.default.gtk fakeroot/${PREFIX}/share/codelite/config/codelite.xml.default
cp -pr Runtime/rc/menu.xrc fakeroot/${PREFIX}/share/codelite/rc/
cp -pfr Runtime/templates/* fakeroot/${PREFIX}/share/codelite/templates/
cp -pfr Runtime/lexers/* fakeroot/${PREFIX}/share/codelite/lexers/
cp -pr lib/Debugger.so fakeroot/${PREFIX}/lib/codelite/debuggers/
cp -pr lib/CodeFormatter.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/Copyright.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/Subversion2.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/Gizmos.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/cscope.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/UnitTestCPP.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/ExternalTools.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/ContinuousBuild.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/SymbolView.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/SnipWiz.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/wxformbuilder.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/abbreviation.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/QmakePlugin.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/CppCheck.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/libwxscintillau.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/libpluginu.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/libcodeliteu.so fakeroot/${PREFIX}/lib/codelite/
cp -pr lib/libwxsqlite3u.so fakeroot/${PREFIX}/lib/codelite/
cp -pr Runtime/plugins/resources/*.* fakeroot/${PREFIX}/lib/codelite/resources/
cp -pr Runtime/*.html fakeroot/${PREFIX}/share/codelite/
cp -pr Runtime/astyle.sample fakeroot/${PREFIX}/share/codelite/
cp -pr Runtime/src/* fakeroot/${PREFIX}/share/codelite/src/
cp -pr Runtime/images/*.png fakeroot/${PREFIX}/share/codelite/images/
cp -pr Runtime/codelite-icons.zip fakeroot/${PREFIX}/share/codelite/

cp -pr AUTHORS fakeroot/${PREFIX}/share/codelite/
cp -pr COPYING fakeroot/${PREFIX}/share/codelite/

chmod 0644 fakeroot/${PREFIX}/lib/codelite/debuggers/*.so
chmod 0644 fakeroot/${PREFIX}/lib/codelite/*.so
chmod 0666 fakeroot/${PREFIX}/share/codelite/codelite-icons.zip

## Clean all .svn files
find fakeroot -name ".svn" | xargs rm -fr

## Get installed size
inst_size=`du -s fakeroot/usr/ | awk -F" " '{ print $1 }'`

## Make debain files
generateDebianFiles

## Make desktop file
generateDesktopFile

## Make menu entry
generateMenuFile

chmod +x fakeroot/${PREFIX}/bin/codelite
chmod +x fakeroot/${PREFIX}/bin/codelite_indexer
chmod +x fakeroot/${PREFIX}/bin/codelite_cppcheck

fakeroot dpkg -b fakeroot/ ./
