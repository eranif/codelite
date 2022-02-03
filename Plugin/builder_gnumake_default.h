//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : builder_gnumake.h
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
#ifndef BUILDER_GNUMAKE_DEFAULT_H
#define BUILDER_GNUMAKE_DEFAULT_H

#include "builder.h"
#include "codelite_exports.h"
#include "project.h"
#include "workspace.h"

#include <wx/txtstrm.h>
#include <wx/wfstream.h>
/*
 * Build using a generated (Gnu) Makefile - this is made as a traditional multistep build :
 *  sources -> (preprocess) -> compile -> link -> exec/lib.
 */
class WXDLLIMPEXP_SDK BuilderGnuMake : public Builder
{
protected:
    size_t m_objectChunks;
    Project::FilesMap_t* m_projectFilesMetadata;
    bool m_isWindows = false;
    bool m_isMSYSEnv = false;

protected:
    enum eBuildFlags {
        kCleanOnly = (1 << 0),
        kAddCleanTarget = (1 << 1),
        kIncludePreBuild = (1 << 2),
        kIncludePostBuild = (1 << 3),
    };

public:
    BuilderGnuMake();
    BuilderGnuMake(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions);
    virtual ~BuilderGnuMake();

    // Implement the Builder Interface
    virtual bool Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                        bool isProjectOnly, bool force, wxString& errMsg);
    virtual wxString GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
    virtual wxString GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
    virtual wxString GetPOBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
    virtual wxString GetPOCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
    virtual wxString GetSingleFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                                      const wxString& fileName);
    virtual wxString GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
                                          const wxString& arguments, const wxString& fileName, wxString& errMsg);
    virtual wxString GetPORebuildCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments);
    virtual OptimalBuildConfig GetOptimalBuildConfig(const wxString& projectType) const;

protected:
    virtual wxString MakeDir(const wxString& path);
    virtual wxString GetIntermediateDirectory(ProjectPtr proj, BuildConfigPtr bldConf) const;

protected:
    virtual void CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text);
    void CreateSrcList(ProjectPtr proj, const wxString& confToBuild, wxString& text);
    void CreateObjectList(ProjectPtr proj, const wxString& confToBuild, wxString& text);
    virtual void CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName,
                                   const wxString& projName, const wxArrayString& depsProj);
    virtual void CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text);
    void CreateCleanTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text);
    // Override default methods defined in the builder interface
    virtual wxString GetBuildToolCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments, bool isCommandlineCommand) const;

    bool SendBuildEvent(int eventId, const wxString& projectName, const wxString& configurationName);
    bool HandleResourceFiles() const;
    bool IsResourceFile(const Compiler::CmpFileTypeInfo& file_type) const;

private:
    void GenerateMakefile(ProjectPtr proj, const wxString& confToBuild, bool force, const wxArrayString& depsProj);
    void CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
    void CreateMakeDirsTarget(const wxString& targetName, wxString& text);
    void CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, const wxString& projName);
    void CreatePreBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
    void CreatePostBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
    void CreatePreCompiledHeaderTarget(BuildConfigPtr bldConf, wxString& text);
    void CreateCustomPreBuildEvents(BuildConfigPtr bldConf, wxString& text);
    void CreateCustomPostBuildEvents(BuildConfigPtr bldConf, wxString& text);

    wxString GetCdCmd(const wxFileName& path1, const wxFileName& path2);

    wxString ParseIncludePath(const wxString& paths, const wxString& projectName, const wxString& selConf);
    wxString ParseLibPath(const wxString& paths, const wxString& projectName, const wxString& selConf);
    wxString ParseLibs(const wxString& libs);
    wxString ParsePreprocessor(const wxString& prep);
    bool HasPrebuildCommands(BuildConfigPtr bldConf) const;
    bool HasPostbuildCommands(BuildConfigPtr bldConf) const;

    wxString GetProjectMakeCommand(const wxFileName& wspfile, const wxFileName& projectPath, ProjectPtr proj,
                                   const wxString& confToBuild);
    wxString GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString& target, size_t flags);
    wxString DoGetCompilerMacro(const wxString& filename);
    wxString DoGetTargetPrefix(const wxFileName& filename, const wxString& cwd, CompilerPtr cmp);
    wxString GetRelinkMarkerForProject(const wxString& projectName) const;
};
#endif // BUILDER_GNUMAKE_DEFAULT_H
