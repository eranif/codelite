#!/bin/bash -e

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

# build directory
BUILD_DIR=build-release
if [ ! -z $1 ]; then 
    BUILD_DIR=$1
fi

# source the environment file
. ${PWD}/${BUILD_DIR}/msys2-environment
echo "-- WXIN is set to ${WXWIN}"

INSTALL_DIR="/c/Program Files/CodeLite"
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

# Copy the DLLs
for exec in ${EXECUTABLES}; do
    echo "-- cp ${PWD}/${BUILD_DIR}/bin/$exec ${INSTALL_DIR}/${exec}"
    cp ${PWD}/${BUILD_DIR}/bin/$exec "${INSTALL_DIR}/${exec}"
done

for dll in ${DLLS}; do
    echo "-- cp ${PWD}/${BUILD_DIR}/bin/$dll ${INSTALL_DIR}/${dll}"
    cp ${PWD}/${BUILD_DIR}/bin/$dll "${INSTALL_DIR}/${dll}"
done

for plugin in ${PLUGINS}; do
    echo "-- cp ${PWD}/${BUILD_DIR}/bin/$plugin ${INSTALL_DIR}/${plugin}"
    cp ${PWD}/${BUILD_DIR}/bin/$plugin "${INSTALL_DIR}/${plugin}"
done

# copy wxWidgets DLLs
echo "-- Copying wxWidgets DLLs..."
echo cp -f ${WXWIN}/lib/clang_x64_dll/*.dll "${INSTALL_DIR}/"
cp -f ${WXWIN}/lib/clang_x64_dll/*.dll "${INSTALL_DIR}/"
