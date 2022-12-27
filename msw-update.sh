#!/bin/bash -e

# build directory
BUILD_DIR=$(basename $(pwd))
if [ ! -z $1 ]; then
    BUILD_DIR=$1
    shift
fi

# locate the root folder
PWD=$(pwd)
while [ 1 ]; do
    if [ -f ${PWD}/CodeLiteIDE.workspace ]; then
        echo "-- Root directory is: ${PWD}"
        break
    fi
    cd ..
    PWD=$(pwd)
    if [ "${PWD}" = "/" ]; then
        echo -e "ERROR: could not locate root folder"
        exit 1
    fi
done

# source the environment file
. ${PWD}/${BUILD_DIR}/msys2-environment
echo "-- WXIN is set to ${WXWIN}"

INSTALL_DIR="${PWD}/${BUILD_DIR}/install/"
mkdir -p ${INSTALL_DIR}
mkdir -p ${INSTALL_DIR}/plugins
mkdir -p ${INSTALL_DIR}/rc
mkdir -p ${INSTALL_DIR}/config
mkdir -p ${INSTALL_DIR}/lexers

SYSTEM_DLLS="
    libc++.dll
    libcrypto-1_1-x64.dll
    libhunspell-1.7-0.dll
    libsqlite3-0.dll
    libssh.dll
    libunwind.dll
    libwinpthread-1.dll
    zlib1.dll
"

EXECUTABLES="codelite.exe codelite-echo.exe
             ctagsd.exe codelite-make.exe
             codelite-terminal.exe codelite_cppcheck.exe
             makedir.exe
             codelite-cc.exe codelite-exec.exe"

DLLS="
    codelite.dll
    libdapcxx.dll
    libplugin.dll
    libdatabaselayersqlite.dll
    libwxshapeframework.dll
    libwxsqlite3.dll
"

PLUGINS="
    DebuggerGDB.dll
    abbreviation.dll
    CodeFormatter.dll
    ContinuousBuild.dll
    Copyright.dll
    CppChecker.dll
    Cscope.dll
    ExternalTools.dll
    git.dll
    Wizards.dll
    Outline.dll
    snipwiz.dll
    QMakePlugin.dll
    Subversion.dll
    UnitTestsPP.dll
    wxFormBuilder.dll
    ZoomNavigator.dll
    SFTP.dll
    CMakePlugin.dll
    CodeLiteDiff.dll
    DebugAdapterClient.dll
    wxcrafter.dll
    WordCompletion.dll
    SpellCheck.dll
    codelitephp.dll
    WebTools.dll
    HelpPlugin.dll
    AutoSave.dll
    Tail.dll
    EditorConfigPlugin.dll
    codelite_vim.dll
    PHPLint.dll
    PHPRefactoring.dll
    SmartCompletion.dll
    Docker.dll
    LanguageServer.dll
    EOSWiki.dll
    Rust.dll
    Remoty.dll
"

function copy_file() {
    local source=$1
    local target=$2
    echo "-- Installing $source $target"
    cp -f ${source} ${target}
}

# Copy the Executables & DLLs
for exec in ${EXECUTABLES}; do
    copy_file ${PWD}/${BUILD_DIR}/bin/$exec "${INSTALL_DIR}/${exec}"
done

for dll in ${DLLS}; do
    copy_file ${PWD}/${BUILD_DIR}/bin/$dll "${INSTALL_DIR}/${dll}"
done

for plugin in ${PLUGINS}; do
    copy_file ${PWD}/${BUILD_DIR}/bin/$plugin "${INSTALL_DIR}/plugins/${plugin}"
done

for system_dll in ${SYSTEM_DLLS}; do
    copy_file ${MSYS2_HOME}/clang64/bin/${system_dll} "${INSTALL_DIR}/${system_dll}"
done

# Copy configuration files
copy_file ${PWD}/Runtime/config/build_settings.xml.default.win ${INSTALL_DIR}/config/build_settings.xml.default
copy_file ${PWD}/Runtime/config/codelite.xml.default ${INSTALL_DIR}/config/codelite.xml.default
copy_file ${PWD}/Runtime/rc/menu.xrc ${INSTALL_DIR}/rc
copy_file ${PWD}/Runtime/lexers/lexers.json ${INSTALL_DIR}/lexers/lexers.json

# Resource files
copy_file ${PWD}/wxcrafter/wxgui.zip ${INSTALL_DIR}
copy_file ${PWD}/Runtime/PHP.zip ${INSTALL_DIR}
copy_file ${PWD}/Runtime/codelite-bitmaps-dark.zip ${INSTALL_DIR}
copy_file ${PWD}/Runtime/codelite-bitmaps-light.zip ${INSTALL_DIR}

# License file
copy_file ${PWD}/InnoSetup/license.txt ${INSTALL_DIR}/LICENSE

# copy wxWidgets DLLs
copy_file "${WXWIN}/lib/clang_x64_dll/*.dll" "${INSTALL_DIR}/"

# Copy other executables (not created by our build system)
copy_file ${PWD}/ctags/ctags.exe "${INSTALL_DIR}/codelite-ctags.exe"
