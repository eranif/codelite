#include "CMakeBuilder.h"
#include "globals.h"
#include "workspace.h"

#define CMAKE_BUILD_FOLDER_PREFIX "cmake-build-"
CMakeBuilder::CMakeBuilder()
    : Builder("CMake")
{
}

CMakeBuilder::~CMakeBuilder() {}

bool CMakeBuilder::Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                          bool isProjectOnly, bool force, wxString& errMsg)
{
    wxUnusedVar(project);
    wxUnusedVar(confToBuild);
    wxUnusedVar(arguments);
    wxUnusedVar(isProjectOnly);
    wxUnusedVar(force);
    wxUnusedVar(errMsg);
    return true;
}

wxString CMakeBuilder::GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments)
{
    // The build folder is set to the workspace-path/workspace-config
    wxString command;
    command << "cd " << GetProjectBuildFolder(project, true) << " && " << GetBuildToolCommand(project, confToBuild)
            << " clean";
    return command;
}

wxString CMakeBuilder::GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments)
{
    // The build folder is set to the workspace-path/workspace-config
    wxString command;
    command << "cd " << GetProjectBuildFolder(project, true) << " && " << GetBuildToolCommand(project, confToBuild);
    return command;
}

wxString CMakeBuilder::GetPOCleanCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments)
{
    wxString command;
    command << "cd " << GetProjectBuildFolder(project, true) << " && " << GetBuildToolCommand(project, confToBuild)
            << " clean";
    return command;
}

wxString CMakeBuilder::GetPOBuildCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments)
{
    wxString command;
    command << "cd " << GetProjectBuildFolder(project, true) << " && " << GetBuildToolCommand(project, confToBuild);
    return command;
}

wxString CMakeBuilder::GetPORebuildCommand(const wxString& project, const wxString& confToBuild,
                                           const wxString& arguments)
{
    wxString command;
    command << "cd " << GetProjectBuildFolder(project, true) << " && " << GetBuildToolCommand(project, confToBuild)
            << " clean all";
    return command;
}

wxString CMakeBuilder::GetSingleFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                                        const wxString& fileName)
{
    return wxEmptyString;
}

wxString CMakeBuilder::GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
                                            const wxString& arguments, const wxString& fileName, wxString& errMsg)
{
    return wxEmptyString;
}

wxString CMakeBuilder::GetWorkspaceBuildFolder(bool wrapWithQuotes)
{
    wxFileName fn = clCxxWorkspaceST::Get()->GetFileName();
    wxString workspaceConfig = clCxxWorkspaceST::Get()->GetBuildMatrix()->GetSelectedConfigurationName();

    fn.AppendDir(CMAKE_BUILD_FOLDER_PREFIX + workspaceConfig);
    wxString folder = fn.GetPath();
    if(wrapWithQuotes) { ::WrapWithQuotes(folder); }
    return folder;
}

wxString CMakeBuilder::GetProjectBuildFolder(const wxString& project, bool wrapWithQuotes)
{
    ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(project);
    wxASSERT(p);

    wxString workspaceFolder = GetWorkspaceBuildFolder(wrapWithQuotes);
    wxFileName fn(workspaceFolder, "");
    fn.AppendDir(p->GetName());

    wxString folder;
    folder = fn.GetPath();
    if(wrapWithQuotes) { ::WrapWithQuotes(folder); }
    return folder;
}

wxString CMakeBuilder::GetBuildToolCommand(const wxString& project, const wxString& confToBuild) const
{
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
    if(!bldConf) return wxEmptyString;

    // The 'make' command is part of the compiler settings
    CompilerPtr compiler = bldConf->GetCompiler();
    if(!compiler) return wxEmptyString;

    wxString buildTool = compiler->GetTool("MAKE");
    if(buildTool.Lower().Contains("make")) {
        return buildTool + " -e ";

    } else {
        return buildTool + " ";
    }
}

Builder::OptimalBuildConfig CMakeBuilder::GetOptimalBuildConfig(const wxString& projectType) const
{
    OptimalBuildConfig conf;
    wxChar sep = wxFileName::GetPathSeparator();
    conf.command << "$(WorkspacePath)" << sep << CMAKE_BUILD_FOLDER_PREFIX << "$(WorkspaceConfiguration)" << sep
                 << "output" << sep << "$(ProjectName)";
    conf.workingDirectory << "$(WorkspacePath)" << sep << "cmake-build-$(WorkspaceConfiguration)" << sep << "output";
    return conf;
}
