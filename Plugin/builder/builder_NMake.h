//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : builder_NMake.h
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
#ifndef BUILDER_NMAKE_H
#define BUILDER_NMAKE_H

#include "builder.h"
#include "codelite_exports.h"
#include "project.h"
#include "workspace.h"

#include <wx/txtstrm.h>
#include <wx/wfstream.h>
/*
 * Build using a generated CMakefile - this is made as a traditional multistep build :
 *  sources -> (preprocess) -> compile -> link -> exec/lib.
 */
class WXDLLIMPEXP_SDK BuilderNMake : public Builder
{
    size_t m_objectChunks = 1;
    bool m_hasObjectPCH = false;
    clProjectFile::Vec_t m_allFiles;

public:
    BuilderNMake();
    BuilderNMake(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions);
    virtual ~BuilderNMake();

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
    virtual wxString GetStaticLibSuffix() const { return ".lib"; }

protected:
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

private:
    void GenerateMakefile(ProjectPtr proj, const wxString& confToBuild, bool force, const wxArrayString& depsProj);
    void CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
    void CreateMakeDirsTarget(ProjectPtr proj, BuildConfigPtr bldConf, const wxString& targetName, wxString& text);
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
    wxString GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString& target,
                                   bool addCleanTarget, bool cleanOnly);
    wxString DoGetCompilerMacro(const wxString& filename);
    wxString DoGetTargetPrefix(const wxFileName& filename, const wxString& cwd, CompilerPtr cmp);
    wxString DoGetMarkerFileDir(const wxString& projname, const wxString& projectPath = "");
};
#endif // BUILDER_NMAKE_H
