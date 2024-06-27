#include "CodeBlocksImporter.h"

#include "macros.h"

#include <wx/filefn.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>

bool CodeBlocksImporter::OpenWorkspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);

    extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("cbp") || extension == wxT("workspace");

    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    return result;
}

bool CodeBlocksImporter::isSupportedWorkspace()
{
    wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
        CHECK_PTR_RET_FALSE(root);
        wxString nodeName = root->GetName();
        if((nodeName == wxT("CodeBlocks_workspace_file") || nodeName == wxT("CodeBlocks_project_file")))
            return true;
    }
    return false;
}

GenericWorkspacePtr CodeBlocksImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    if(extension == wxT("cbp")) {
        GenericProjectDataType genericProjectData;
        genericProjectData[wxT("projectFullPath")] = wsInfo.GetFullPath();
        GenerateFromProject(genericWorkspace, genericProjectData);
    } else if(extension == wxT("workspace")) {
        GenerateFromWorkspace(genericWorkspace);
    }

    return genericWorkspace;
}

void CodeBlocksImporter::GenerateFromWorkspace(GenericWorkspacePtr genericWorkspace)
{
    GenericProjectDataListType genericProjectDataList;
    wxCopyFile(wsInfo.GetFullPath(), wsInfo.GetFullPath() + wxT(".backup"));
    wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
        if(root) {
            wxXmlNode* rootChild = root->GetChildren();

            while(rootChild) {
                if(rootChild->GetName() == wxT("Workspace")) {
                    wxXmlNode* workspaceChild = rootChild->GetChildren();

                    while(workspaceChild) {
                        if(workspaceChild->GetName() == wxT("Project") &&
                           workspaceChild->HasAttribute(wxT("filename"))) {
                            wxString filename = workspaceChild->GetAttribute(wxT("filename"));
                            filename.Replace(wxT("\\"), wxT("/"));
                            wxFileName filenameInfo(filename);

                            GenericProjectDataType genericProjectData;
                            genericProjectData[wxT("projectFullPath")] =
                                wsInfo.GetPath() + wxFileName::GetPathSeparator() + filename;

                            wxString deps = wxT("");
                            wxXmlNode* projectChild = workspaceChild->GetChildren();
                            while(projectChild) {
                                if(projectChild->GetName() == wxT("Depends") &&
                                   projectChild->HasAttribute(wxT("filename"))) {
                                    wxString filename = projectChild->GetAttribute(wxT("filename"));
                                    wxString projectFullPath =
                                        wsInfo.GetPath() + wxFileName::GetPathSeparator() + filename;

                                    wxXmlDocument depsProject;
                                    if(depsProject.Load(projectFullPath)) {
                                        wxXmlNode* rootDeps = depsProject.GetRoot();
                                        if(rootDeps) {
                                            wxXmlNode* rootDepsChild = rootDeps->GetChildren();

                                            while(rootDepsChild) {
                                                if(rootDepsChild->GetName() == wxT("Project")) {
                                                    wxXmlNode* projectDepsChild = rootDepsChild->GetChildren();

                                                    while(projectDepsChild) {
                                                        if(projectDepsChild->GetName() == wxT("Option") &&
                                                           projectDepsChild->HasAttribute(wxT("title"))) {
                                                            wxString title =
                                                                projectDepsChild->GetAttribute(wxT("title"));
                                                            deps += title + wxT(";");
                                                        }

                                                        projectDepsChild = projectDepsChild->GetNext();
                                                    }
                                                }

                                                rootDepsChild = rootDepsChild->GetNext();
                                            }
                                        }
                                    }
                                }

                                projectChild = projectChild->GetNext();
                            }

                            genericProjectData[wxT("projectDeps")] = deps;

                            genericProjectDataList.push_back(genericProjectData);
                        }

                        workspaceChild = workspaceChild->GetNext();
                    }
                }

                rootChild = rootChild->GetNext();
            }
        }
    }

    for(GenericProjectDataType& genericProjectData : genericProjectDataList) {
        GenerateFromProject(genericWorkspace, genericProjectData);
    }
}

void CodeBlocksImporter::GenerateFromProject(GenericWorkspacePtr genericWorkspace,
                                             GenericProjectDataType& genericProjectData)
{
    wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(genericProjectData[wxT("projectFullPath")])) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
        if(root) {
            wxXmlNode* rootChild = root->GetChildren();

            while(rootChild) {
                if(rootChild->GetName() == wxT("Project")) {
                    wxString globalCompilerOptions = wxT(""), globalIncludePath = wxT(""),
                             globalLinkerOptions = wxT(""), globalLibPath = wxT(""), globalLibraries = wxT("");

                    wxFileName projectInfo(genericProjectData[wxT("projectFullPath")]);
                    GenericProjectPtr genericProject = std::make_shared<GenericProject>();
                    genericProject->path = projectInfo.GetPath();

                    wxStringTokenizer deps(genericProjectData[wxT("projectDeps")], wxT(";"));

                    while(deps.HasMoreTokens()) {
                        wxString projectNameDep = deps.GetNextToken().Trim().Trim(false);
                        genericProject->deps.Add(projectNameDep);
                    }

                    genericWorkspace->projects.push_back(genericProject);

                    wxXmlNode* projectChild = rootChild->GetChildren();

                    while(projectChild) {
                        if(projectChild->GetName() == wxT("Option") && projectChild->HasAttribute(wxT("title"))) {
                            genericProject->name = projectChild->GetAttribute(wxT("title"));
                        }

                        if(projectChild->GetName() == wxT("Build")) {
                            wxXmlNode* buildChild = projectChild->GetChildren();

                            while(buildChild) {
                                if(buildChild->GetName() == wxT("Target") && buildChild->HasAttribute(wxT("title"))) {
                                    GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                                    genericProjectCfg->name = buildChild->GetAttribute(wxT("title"));
                                    genericProject->cfgs.push_back(genericProjectCfg);

                                    wxXmlNode* targetChild = buildChild->GetChildren();
                                    while(targetChild) {
                                        if(targetChild->GetName() == wxT("Option") &&
                                           targetChild->HasAttribute(wxT("output"))) {
                                            wxString output = targetChild->GetAttribute(wxT("output"));
                                            if(output.Contains(wxT("#")))
                                                output.Replace(wxT("#"), wxT(""));
                                            genericProjectCfg->outputFilename = output;
                                        }

                                        if(targetChild->GetName() == wxT("Option") &&
                                           targetChild->HasAttribute(wxT("working_dir"))) {
                                            wxString working_dir = targetChild->GetAttribute(wxT("working_dir"));
                                            if(working_dir.Contains(wxT("#")))
                                                working_dir.Replace(wxT("#"), wxT(""));
                                            genericProjectCfg->workingDirectory = working_dir;
                                        }

                                        if(targetChild->GetName() == wxT("Option") &&
                                           targetChild->HasAttribute(wxT("type"))) {
                                            wxString projectType = targetChild->GetAttribute(wxT("type"));

                                            if(projectType == wxT("2")) {
                                                genericProject->cfgType = GenericCfgType::STATIC_LIBRARY;
                                            } else if(projectType == wxT("3")) {
                                                genericProject->cfgType = GenericCfgType::DYNAMIC_LIBRARY;
                                            } else {
                                                genericProject->cfgType = GenericCfgType::EXECUTABLE;
                                            }

                                            genericProjectCfg->type = genericProject->cfgType;
                                        }

                                        if(targetChild->GetName() == wxT("Compiler")) {
                                            wxString compilerOptions = wxT(""), includePath = wxT("");

                                            wxXmlNode* compilerChild = targetChild->GetChildren();
                                            while(compilerChild) {
                                                if(compilerChild->GetName() == wxT("Add") &&
                                                   compilerChild->HasAttribute(wxT("option"))) {
                                                    compilerOptions +=
                                                        compilerChild->GetAttribute(wxT("option")) + wxT(" ");
                                                }

                                                if(compilerChild->GetName() == wxT("Add") &&
                                                   compilerChild->HasAttribute(wxT("directory"))) {
                                                    includePath +=
                                                        compilerChild->GetAttribute(wxT("directory")) + wxT(";");
                                                }

                                                compilerChild = compilerChild->GetNext();
                                            }

                                            if(includePath.Contains(wxT("#")))
                                                includePath.Replace(wxT("#"), wxT(""));

                                            genericProjectCfg->cCompilerOptions = compilerOptions;
                                            genericProjectCfg->cppCompilerOptions = compilerOptions;
                                            genericProjectCfg->includePath = includePath;
                                        }

                                        if(targetChild->GetName() == wxT("Linker")) {
                                            wxString linkerOptions = wxT(""), libPath = wxT(""), libraries = wxT("");

                                            wxXmlNode* linkerChild = targetChild->GetChildren();
                                            while(linkerChild) {
                                                if(linkerChild->GetName() == wxT("Add") &&
                                                   linkerChild->HasAttribute(wxT("option"))) {
                                                    linkerOptions +=
                                                        linkerChild->GetAttribute(wxT("option")) + wxT(" ");
                                                }

                                                if(linkerChild->GetName() == wxT("Add") &&
                                                   linkerChild->HasAttribute(wxT("directory"))) {
                                                    libPath += linkerChild->GetAttribute(wxT("directory")) + wxT(";");
                                                }

                                                if(linkerChild->GetName() == wxT("Add") &&
                                                   linkerChild->HasAttribute(wxT("library"))) {
                                                    libraries += linkerChild->GetAttribute(wxT("library")) + wxT(";");
                                                }

                                                linkerChild = linkerChild->GetNext();
                                            }

                                            if(libPath.Contains(wxT("#")))
                                                libPath.Replace(wxT("#"), wxT(""));

                                            if(libraries.Contains(wxT("#")))
                                                libraries.Replace(wxT("#"), wxT(""));

                                            genericProjectCfg->linkerOptions = linkerOptions;
                                            genericProjectCfg->libPath = libPath;
                                            genericProjectCfg->libraries = libraries;
                                        }

                                        if(targetChild->GetName() == wxT("ExtraCommands")) {
                                            wxXmlNode* extraCommandsChild = targetChild->GetChildren();
                                            while(extraCommandsChild) {
                                                if(extraCommandsChild->GetName() == wxT("Add") &&
                                                   extraCommandsChild->HasAttribute(wxT("before"))) {
                                                    wxString command = extraCommandsChild->GetAttribute(wxT("before"));
                                                    if(command.Contains(wxT("#")))
                                                        command.Replace(wxT("#"), wxT(""));
                                                    genericProjectCfg->preBuildCommands.push_back(command);
                                                }

                                                if(extraCommandsChild->GetName() == wxT("Add") &&
                                                   extraCommandsChild->HasAttribute(wxT("after"))) {
                                                    wxString command = extraCommandsChild->GetAttribute(wxT("after"));
                                                    if(command.Contains(wxT("#")))
                                                        command.Replace(wxT("#"), wxT(""));
                                                    genericProjectCfg->postBuildCommands.push_back(command);
                                                }

                                                extraCommandsChild = extraCommandsChild->GetNext();
                                            }
                                        }

                                        if(targetChild->GetName() == wxT("MakeCommands")) {
                                            wxXmlNode* makeCommandsChild = targetChild->GetChildren();

                                            while(makeCommandsChild) {
                                                if(makeCommandsChild->GetName() == wxT("Build") &&
                                                   makeCommandsChild->HasAttribute(wxT("command"))) {
                                                    wxString buildCommand =
                                                        makeCommandsChild->GetAttribute(wxT("command"));

                                                    if(!genericProjectCfg->enableCustomBuild)
                                                        genericProjectCfg->enableCustomBuild = true;

                                                    genericProjectCfg->customBuildCmd = buildCommand;
                                                } else if(makeCommandsChild->GetName() == wxT("Clean") &&
                                                          makeCommandsChild->HasAttribute(wxT("command"))) {
                                                    wxString cleanCommand =
                                                        makeCommandsChild->GetAttribute(wxT("command"));

                                                    if(!genericProjectCfg->enableCustomBuild)
                                                        genericProjectCfg->enableCustomBuild = true;

                                                    genericProjectCfg->customCleanCmd = cleanCommand;
                                                }

                                                makeCommandsChild = makeCommandsChild->GetNext();
                                            }
                                        }

                                        targetChild = targetChild->GetNext();
                                    }
                                }

                                buildChild = buildChild->GetNext();
                            }
                        }

                        if(projectChild->GetName() == wxT("Compiler")) {
                            wxXmlNode* compilerChild = projectChild->GetChildren();
                            while(compilerChild) {
                                if(compilerChild->GetName() == wxT("Add") &&
                                   compilerChild->HasAttribute(wxT("option"))) {
                                    globalCompilerOptions += compilerChild->GetAttribute(wxT("option")) + wxT(" ");
                                }

                                if(compilerChild->GetName() == wxT("Add") &&
                                   compilerChild->HasAttribute(wxT("directory"))) {
                                    globalIncludePath += compilerChild->GetAttribute(wxT("directory")) + wxT(";");
                                }

                                compilerChild = compilerChild->GetNext();
                            }

                            if(globalIncludePath.Contains(wxT("#")))
                                globalIncludePath.Replace(wxT("#"), wxT(""));
                        }

                        if(projectChild->GetName() == wxT("Linker")) {
                            wxXmlNode* linkerChild = projectChild->GetChildren();
                            while(linkerChild) {
                                if(linkerChild->GetName() == wxT("Add") && linkerChild->HasAttribute(wxT("option"))) {
                                    globalLinkerOptions += linkerChild->GetAttribute(wxT("option")) + wxT(" ");
                                }

                                if(linkerChild->GetName() == wxT("Add") &&
                                   linkerChild->HasAttribute(wxT("directory"))) {
                                    globalLibPath += linkerChild->GetAttribute(wxT("directory")) + wxT(";");
                                }

                                if(linkerChild->GetName() == wxT("Add") && linkerChild->HasAttribute(wxT("library"))) {
                                    globalLibraries += linkerChild->GetAttribute(wxT("library")) + wxT(";");
                                }

                                linkerChild = linkerChild->GetNext();
                            }

                            if(globalLibPath.Contains(wxT("#")))
                                globalLibPath.Replace(wxT("#"), wxT(""));
                        }

                        if(projectChild->GetName() == wxT("Unit") && projectChild->HasAttribute(wxT("filename"))) {
                            wxString vpath = wxT("");
                            wxXmlNode* unitChild = projectChild->GetChildren();

                            while(unitChild) {
                                if(unitChild->GetName() == wxT("Option") &&
                                   unitChild->HasAttribute(wxT("virtualFolder"))) {
                                    vpath = unitChild->GetAttribute(wxT("virtualFolder"));
                                }
                                unitChild = unitChild->GetNext();
                            }

                            wxString projectFilename = projectChild->GetAttribute(wxT("filename"));
                            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                            genericProjectFile->name = projectFilename;
                            genericProjectFile->vpath = vpath;
                            genericProject->files.push_back(genericProjectFile);
                        }

                        projectChild = projectChild->GetNext();
                    }

                    for(GenericProjectCfgPtr genericProjectCfg : genericProject->cfgs) {
                        genericProjectCfg->cCompilerOptions += globalCompilerOptions;
                        genericProjectCfg->cppCompilerOptions += globalCompilerOptions;
                        genericProjectCfg->includePath += globalIncludePath;
                        genericProjectCfg->linkerOptions += globalLinkerOptions;
                        genericProjectCfg->libPath += globalLibPath;
                        genericProjectCfg->libraries += globalLibraries;
                    }
                }

                rootChild = rootChild->GetNext();
            }
        }
    }
}
