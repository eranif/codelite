//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : GenericImporter.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef GENERICIMPORTER_H
#define GENERICIMPORTER_H

#include <map>
#include <memory>
#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>

#ifdef __WXMSW__
#define STATIC_LIBRARY_EXT wxT(".lib")
#define DYNAMIC_LIBRARY_EXT wxT(".dll")
#define EXECUTABLE_EXT wxT(".exe")
#else
#define STATIC_LIBRARY_EXT wxT(".a")
#define DYNAMIC_LIBRARY_EXT wxT(".so")
#define EXECUTABLE_EXT wxT("")
#endif

typedef std::map<wxString, wxString> GenericProjectDataType;
typedef std::vector<GenericProjectDataType> GenericProjectDataListType;
typedef std::map<wxString, wxString> GenericEnvVarsType;
typedef std::pair<wxString, wxString> GenericEnvVarsValueType;

enum class GenericCfgType { DYNAMIC_LIBRARY, STATIC_LIBRARY, EXECUTABLE };

struct GenericProjectFile {
    wxString name;
    wxString vpath;
};

typedef std::shared_ptr<GenericProjectFile> GenericProjectFilePtr;

struct GenericProjectCfg {
    wxString name;
    wxString outputFilename;
    wxString intermediateDirectory;
    wxString includePath;
    wxString preprocessor;
    wxString libraries;
    wxString libPath;
    wxString cCompilerOptions;
    wxString cppCompilerOptions;
    wxString linkerOptions;
    wxString preCompiledHeader;
    wxString command;
    wxString workingDirectory;
    std::vector<wxString> preBuildCommands;
    std::vector<wxString> postBuildCommands;
    std::vector<GenericProjectFilePtr> excludeFiles;
    GenericEnvVarsType envVars;
    GenericCfgType type;
    bool enableCustomBuild;
    wxString customBuildCmd;
    wxString customCleanCmd;
    wxString customRebuildCmd;
};

typedef std::shared_ptr<GenericProjectCfg> GenericProjectCfgPtr;

struct GenericProject {
    wxString name;
    wxString path;
    wxArrayString deps;
    GenericCfgType cfgType;
    std::vector<GenericProjectCfgPtr> cfgs;
    std::vector<GenericProjectFilePtr> files;
};

typedef std::shared_ptr<GenericProject> GenericProjectPtr;

struct GenericWorkspace {
    wxString name;
    wxString path;
    std::vector<GenericProjectPtr> projects;
};

typedef std::shared_ptr<GenericWorkspace> GenericWorkspacePtr;

class GenericImporter
{
public:
    virtual bool OpenWorkspace(const wxString& filename, const wxString& defaultCompiler) = 0;
    virtual bool isSupportedWorkspace() = 0;
    virtual GenericWorkspacePtr PerformImport() = 0;
};

#endif // GENERICIMPORTER_H
