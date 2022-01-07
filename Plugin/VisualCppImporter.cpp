#include "VisualCppImporter.h"

#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

bool VisualCppImporter::OpenWorkspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);

    extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("dsw") || extension == wxT("sln");
    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    if(result) {
        version = 0;
        wxFileInputStream fis(filename);
        wxTextInputStream tis(fis);

        while(!fis.Eof()) {
            wxString line = tis.ReadLine();
            int index = line.Find(wxT("Format Version"));
            if(index != wxNOT_FOUND) {
                wxString value = line.Mid(index + 14).Trim().Trim(false);
                value.ToLong(&version);
                break;
            }
        }

        result = fis.IsOk() && version != 0;
    }

    return result;
}

bool VisualCppImporter::isSupportedWorkspace() { return version >= 5 && version <= 12; }

GenericWorkspacePtr VisualCppImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    switch(version) {
    case 5:
    case 6:
        GenerateFromVC6(genericWorkspace);
        break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        GenerateFromVC7_11(genericWorkspace);
        break;
    }

    return genericWorkspace;
}

void VisualCppImporter::GenerateFromVC6(GenericWorkspacePtr genericWorkspace)
{
    GenericProjectDataListType genericProjectDataList;

    wxFileInputStream fis(wsInfo.GetFullPath());
    wxTextInputStream tis(fis);

    while(!fis.Eof()) {
        wxString line = tis.ReadLine();
        int index = line.Find(wxT("Project:"));
        if(index != wxNOT_FOUND) {
            int end = line.Find(wxT("- Package Owner")) - 1;
            wxString value = line.SubString(index + 8, end).Trim().Trim(false);
            value.Replace(wxT("\""), wxT(""));

            wxStringTokenizer part(value, wxT("="));

            GenericProjectDataType genericProjectData;
            genericProjectData[wxT("projectName")] = part.GetNextToken().Trim().Trim(false);
            genericProjectData[wxT("projectFile")] = part.GetNextToken().Trim().Trim(false);
            genericProjectData[wxT("projectFullPath")] =
                wsInfo.GetPath() + wxFileName::GetPathSeparator() + genericProjectData[wxT("projectFile")];

            // Fix the paths to match linux style
            ConvertToLinuxStyle(genericProjectData["projectFile"]);
            ConvertToLinuxStyle(genericProjectData["projectFullPath"]);

            wxString deps = wxT("");
            while(!fis.Eof()) {
                line = tis.ReadLine();
                index = line.Find(wxT("Project_Dep_Name"));
                if(index != wxNOT_FOUND) {
                    wxString value = line.Mid(index + 16, end).Trim().Trim(false);
                    deps += value + wxT(";");
                }

                index = line.Find(wxT("##########"));
                if(index != wxNOT_FOUND)
                    break;
            }

            genericProjectData[wxT("projectDeps")] = deps;

            genericProjectDataList.push_back(genericProjectData);
        }
    }

    for(GenericProjectDataType& genericProjectData : genericProjectDataList) {
        GenerateFromProjectVC6(genericWorkspace, genericProjectData);
    }
}

void VisualCppImporter::GenerateFromProjectVC6(GenericWorkspacePtr genericWorkspace,
                                               GenericProjectDataType& genericProjectData)
{
    wxFileName projectInfo(genericProjectData[wxT("projectFullPath")]);
    wxFileInputStream projectFIS(projectInfo.GetFullPath());

    if(projectFIS.IsOk()) {
        GenericProjectPtr genericProject = std::make_shared<GenericProject>();
        genericProject->name = genericProjectData[wxT("projectName")];
        genericProject->path = projectInfo.GetPath();

        wxStringTokenizer deps(genericProjectData[wxT("projectDeps")], wxT(";"));

        while(deps.HasMoreTokens()) {
            wxString projectNameDep = deps.GetNextToken().Trim().Trim(false);
            genericProject->deps.Add(projectNameDep);
        }

        std::map<wxString, GenericProjectCfgPtr> genericProjectCfgMap;

        wxTextInputStream projectTIS(projectFIS);

        while(!projectFIS.Eof()) {
            wxString line = projectTIS.ReadLine();

            int index = line.Find(wxT("TARGTYPE"));
            if(index != wxNOT_FOUND) {
                index = line.find_last_of(wxT("\""));
                wxString projectType = line.Mid(index + 1).Trim().Trim(false);

                if(projectType == wxT("0x0102")) {
                    genericProject->cfgType = GenericCfgType::DYNAMIC_LIBRARY;
                } else if(projectType == wxT("0x0104")) {
                    genericProject->cfgType = GenericCfgType::STATIC_LIBRARY;
                } else {
                    genericProject->cfgType = GenericCfgType::EXECUTABLE;
                }
            }

            index = line.Find(wxT("!MESSAGE \""));
            if(index != wxNOT_FOUND) {
                int begin = index + 10;
                int end = line.Find(wxT("\" (based on"));

                wxString projectCfgName = line.SubString(begin, end);
                projectCfgName.Replace(genericProjectData[wxT("projectName")] + wxT(" - "), wxT(""));
                projectCfgName.Replace(wxT("\""), wxT(""));
                projectCfgName.Replace(wxT(" "), wxT("_"));

                GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                genericProjectCfg->name = projectCfgName;
                genericProjectCfg->type = genericProject->cfgType;

                genericProjectCfgMap[projectCfgName] = genericProjectCfg;
            }

            index = line.Find(wxT("Begin Project"));
            if(index != wxNOT_FOUND)
                break;
        }

        wxString virtualPath = wxT("");
        GenericProjectCfgPtr genericProjectCfg;

        wxString line;

        while(!projectFIS.Eof()) {
            line = projectTIS.ReadLine();

            int index = line.Find(wxT("\"$(CFG)\" == "));
            if(index != wxNOT_FOUND) {
                wxString projectCfgName = line.Mid(index + 12).Trim().Trim(false);
                projectCfgName.Replace(genericProjectData[wxT("projectName")] + wxT(" - "), wxT(""));
                projectCfgName.Replace(wxT("\""), wxT(""));
                projectCfgName.Replace(wxT(" "), wxT("_"));

                genericProjectCfg = genericProjectCfgMap[projectCfgName];
            }

            if(genericProjectCfg) {
                index = line.Find(wxT("PROP Intermediate_Dir"));
                if(index != wxNOT_FOUND) {
                    wxString intermediateDirectory = line.Mid(index + 21).Trim().Trim(false);
                    intermediateDirectory.Replace(wxT("\\"), wxT("/"));
                    intermediateDirectory.Replace(wxT("\""), wxT(""));
                    intermediateDirectory.Replace(wxT(" "), wxT("_"));

                    genericProjectCfg->intermediateDirectory = wxT("./") + intermediateDirectory;
                }

                index = line.Find(wxT("ADD CPP"));
                if(index != wxNOT_FOUND) {
                    line = line.Mid(index + 7).Trim().Trim(false);
                    wxStringTokenizer options(line, wxT(" "));

                    while(options.HasMoreTokens()) {
                        wxString value = options.GetNextToken();

                        if(value.Contains(wxT("/I"))) {
                            wxString includePath = options.GetNextToken().Trim().Trim(false);
                            includePath.Replace(wxT("\""), wxT(""));

                            genericProjectCfg->includePath += includePath + wxT(";");
                        } else if(value.Contains(wxT("/D"))) {
                            wxString preprocessor = options.GetNextToken().Trim().Trim(false);
                            preprocessor.Replace(wxT("\""), wxT(""));

                            genericProjectCfg->preprocessor += preprocessor + wxT(";");
                        } else if(value.Contains(wxT("/Yu"))) {
                            wxString preCompiledHeader = value.Trim().Trim(false);
                            preCompiledHeader.Replace(wxT("/Yu"), wxT(""));
                            preCompiledHeader.Replace(wxT("\""), wxT(""));

                            genericProjectCfg->preCompiledHeader += preCompiledHeader + wxT(";");
                        }
                    }
                }

                index = line.Find(wxT("ADD LIB32"));
                if(index != wxNOT_FOUND) {
                    line = line.Mid(index + 9).Trim().Trim(false);
                    wxStringTokenizer options(line, wxT(" "));

                    while(options.HasMoreTokens()) {
                        wxString value = options.GetNextToken();

                        if(value.Contains(wxT("/out:"))) {
                            wxString outputFilename = value.Trim().Trim(false);
                            outputFilename.Replace(wxT("/out:"), wxT(""));
                            outputFilename.Replace(wxT("\""), wxT(""));
                            genericProjectCfg->outputFilename = outputFilename;

                            wxFileName outputFilenameInfo(outputFilename);
                            genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                        }
                    }
                }

                index = line.Find(wxT("ADD LINK32"));
                if(index != wxNOT_FOUND) {
                    line = line.Mid(index + 10).Trim().Trim(false);
                    wxStringTokenizer options(line, wxT(" "));

                    while(options.HasMoreTokens()) {
                        wxString value = options.GetNextToken();

                        if(value.Contains(wxT(".lib"))) {
                            wxString lib = value.Trim().Trim(false);
                            genericProjectCfg->libraries += lib + wxT(";");
                        } else if(value.Contains(wxT("/libpath:"))) {
                            wxString libPath = value.Trim().Trim(false);
                            libPath.Replace(wxT("/libpath:"), wxT(""));
                            libPath.Replace(wxT("\""), wxT(""));
                            genericProjectCfg->libPath += libPath + wxT(";");
                        } else if(value.Contains(wxT("/out:"))) {
                            wxString outputFilename = value.Trim().Trim(false);
                            outputFilename.Replace(wxT("/out:"), wxT(""));
                            outputFilename.Replace(wxT("\""), wxT(""));
                            genericProjectCfg->outputFilename = outputFilename;

                            wxFileName outputFilenameInfo(outputFilename);
                            genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                        }
                    }
                }

                index = line.Find(wxT("PROP Cmd_Line"));
                if(index != wxNOT_FOUND) {
                    wxString cmdLine = line.Mid(index + 13).Trim().Trim(false);
                    cmdLine.Replace(wxT("\""), wxT(""));

                    genericProjectCfg->enableCustomBuild = true;
                    genericProjectCfg->customBuildCmd = cmdLine;
                }

                index = line.Find(wxT("Begin Group"));
                if(index != wxNOT_FOUND) {
                    wxString vpName = line.Mid(index + 11).Trim().Trim(false);
                    vpName.Replace(wxT("\""), wxT(""));

                    if(virtualPath.IsEmpty()) {
                        virtualPath = vpName;
                    } else {
                        virtualPath += wxT("/") + vpName;
                    }
                }

                index = line.Find(wxT("End Group"));
                if(index != wxNOT_FOUND) {
                    int posSeparator = virtualPath.Find(wxT("/"));

                    if(posSeparator == wxNOT_FOUND) {
                        virtualPath = wxT("");
                    } else {
                        virtualPath = virtualPath.Remove(posSeparator);
                    }
                }

                index = line.Find(wxT("Begin Source File"));
                if(index != wxNOT_FOUND) {
                    GenericProjectFilePtr genericProjectFile;

                    while(!projectFIS.Eof()) {
                        line = projectTIS.ReadLine();

                        int index = line.Find(wxT("\"$(CFG)\" == "));
                        if(index != wxNOT_FOUND) {
                            wxString projectCfgName = line.Mid(index + 12).Trim().Trim(false);
                            projectCfgName.Replace(genericProjectData[wxT("projectName")] + wxT(" - "), wxT(""));
                            projectCfgName.Replace(wxT("\""), wxT(""));
                            projectCfgName.Replace(wxT(" "), wxT("_"));

                            genericProjectCfg = genericProjectCfgMap[projectCfgName];
                        }

                        index = line.Find(wxT("SOURCE="));
                        if(index != wxNOT_FOUND) {
                            wxString filename = line.Mid(index + 7).Trim().Trim(false);
                            filename.Replace(wxT("\""), wxT(""));
                            filename.Replace(wxT("\\"), wxT("/"));

                            genericProjectFile = std::make_shared<GenericProjectFile>();
                            genericProjectFile->name = filename;
                            genericProjectFile->vpath = virtualPath;

                            genericProject->files.push_back(genericProjectFile);
                        }

                        index = line.Find(wxT("PROP Exclude_From_Build"));
                        if(index != wxNOT_FOUND) {
                            wxString excludeValue = line.Mid(index + 23).Trim().Trim(false);
                            if(genericProjectFile && excludeValue == wxT("1")) {
                                genericProjectCfg->excludeFiles.push_back(genericProjectFile);
                            }
                        }

                        index = line.Find(wxT("Begin Custom Build"));
                        if(index != wxNOT_FOUND) {
                            while(!projectFIS.Eof()) {
                                line = projectTIS.ReadLine();

                                index = line.Find(wxT("="));
                                if(index != wxNOT_FOUND) {
                                    wxStringTokenizer keyValue(line, wxT("="));
                                    wxString key = keyValue.GetNextToken().Trim().Trim(false);
                                    wxString value = keyValue.GetNextToken().Trim().Trim(false);
                                    genericProjectCfg->envVars[key] = value;
                                }

                                index = line.Find(wxT("\t"));
                                if(index != wxNOT_FOUND) {
                                    wxString command = line;
                                    command.Replace(wxT("\t"), wxT(""));

                                    if(!command.IsEmpty()) {
                                        genericProjectCfg->preBuildCommands.push_back(command);
                                    }
                                }

                                index = line.Find(wxT("End Custom Build"));
                                if(index != wxNOT_FOUND)
                                    break;
                            }
                        }

                        index = line.Find(wxT("End Source File"));
                        if(index != wxNOT_FOUND)
                            break;
                    }
                }
            }
        }

        for(std::pair<wxString, GenericProjectCfgPtr> genericProjectCfg : genericProjectCfgMap) {
            if(genericProjectCfg.second) {
                genericProject->cfgs.push_back(genericProjectCfg.second);
            }
        }

        genericWorkspace->projects.push_back(genericProject);
    }
}

void VisualCppImporter::GenerateFromVC7_11(GenericWorkspacePtr genericWorkspace)
{
    GenericProjectDataListType genericProjectDataList;

    wxFileInputStream fis(wsInfo.GetFullPath());
    wxTextInputStream tis(fis);

    while(!fis.Eof()) {
        wxString line = tis.ReadLine();

        int index = line.Find(wxT("Project("));
        if(index != wxNOT_FOUND) {
            wxStringTokenizer projectToken(line, wxT("="));
            projectToken.GetNextToken();

            projectToken.SetString(projectToken.GetNextToken(), wxT(","));

            wxString projectName = projectToken.GetNextToken().Trim().Trim(false);
            projectName.Replace(wxT("\""), wxT(""));
            wxString projectFile = projectToken.GetNextToken().Trim().Trim(false);
            projectFile.Replace(wxT("\""), wxT(""));
            wxString projectId = projectToken.GetNextToken().Trim().Trim(false);
            projectId.Replace(wxT("\""), wxT(""));

            GenericProjectDataType genericProjectData;
            genericProjectData[wxT("projectId")] = projectId;
            genericProjectData[wxT("projectName")] = projectName;
            genericProjectData[wxT("projectFile")] = projectFile;
            genericProjectData[wxT("projectFullPath")] =
                wsInfo.GetPath() + wxFileName::GetPathSeparator() + projectFile;

            // Fix the paths to match linux style
            ConvertToLinuxStyle(genericProjectData["projectFile"]);
            ConvertToLinuxStyle(genericProjectData["projectFullPath"]);

            wxString deps = wxT("");
            while(!fis.Eof()) {
                line = tis.ReadLine();

                index = line.Find(wxT("ProjectSection(ProjectDependencies)"));
                if(index != wxNOT_FOUND) {
                    while(!fis.Eof()) {
                        line = tis.ReadLine();

                        index = line.Find(wxT("="));
                        if(index != wxNOT_FOUND) {
                            wxStringTokenizer values(line, wxT("="));
                            if(values.HasMoreTokens()) {
                                wxString depId = values.NextToken().Trim().Trim(false);
                                depId.Replace(wxT("\""), wxT(""));
                                deps += depId + wxT(";");
                            }
                        }

                        index = line.Find(wxT("EndProjectSection"));
                        if(index != wxNOT_FOUND)
                            break;
                    }
                }

                index = line.Find(wxT("EndProject"));
                if(index != wxNOT_FOUND)
                    break;
            }

            genericProjectData[wxT("projectDeps")] = deps;

            genericProjectDataList.push_back(genericProjectData);
        }
    }

    for(GenericProjectDataType& genericProjectData : genericProjectDataList) {
        for(GenericProjectDataType& genericProjectDataSub : genericProjectDataList) {
            if(genericProjectData[wxT("projectDeps")].Contains(genericProjectDataSub[wxT("projectId")])) {
                genericProjectData[wxT("projectDeps")].Replace(genericProjectDataSub[wxT("projectId")],
                                                               genericProjectDataSub[wxT("projectName")]);
            }
        }
    }

    for(GenericProjectDataType& genericProjectData : genericProjectDataList) {
        if(version >= 7 && version <= 10)
            GenerateFromProjectVC7(genericWorkspace, genericProjectData);
        else if(version >= 11 && version <= 12)
            GenerateFromProjectVC11(genericWorkspace, genericProjectData);
    }
}

void VisualCppImporter::GenerateFromProjectVC7(GenericWorkspacePtr genericWorkspace,
                                               GenericProjectDataType& genericProjectData)
{
    wxFileName projectInfo(genericProjectData[wxT("projectFullPath")]);

    GenericProjectPtr genericProject = std::make_shared<GenericProject>();
    genericProject->name = genericProjectData[wxT("projectName")];
    genericProject->path = projectInfo.GetPath();

    wxStringTokenizer deps(genericProjectData[wxT("projectDeps")], wxT(";"));

    while(deps.HasMoreTokens()) {
        wxString projectNameDep = deps.GetNextToken().Trim().Trim(false);
        genericProject->deps.Add(projectNameDep);
    }

    wxXmlDocument projectDoc;
    if(projectDoc.Load(projectInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();

        wxXmlNode* vspChild = root->GetChildren();
        std::map<wxString, GenericProjectCfgPtr> genericProjectCfgMap;
        while(vspChild) {
            if(vspChild->GetName() == wxT("Configurations")) {
                wxXmlNode* confChild = vspChild->GetChildren();

                while(confChild) {
                    if(confChild->GetName() == wxT("Configuration")) {
                        wxString name = confChild->GetAttribute(wxT("Name"));
                        wxString projectCfgName = name;
                        projectCfgName.Replace(wxT("|"), wxT("_"));

                        wxStringTokenizer projectCfgNamePart(name, wxT("|"));
                        wxString configurationName = projectCfgNamePart.NextToken();
                        configurationName.Replace(wxT(" "), wxT("_"));
                        wxString platformName = projectCfgNamePart.NextToken();

                        wxString configurationType = confChild->GetAttribute(wxT("ConfigurationType"));
                        wxString outputDirectory = confChild->GetAttribute(wxT("OutputDirectory"));
                        outputDirectory.Replace(wxT("\\"), wxT("/"));
                        outputDirectory = ReplaceDefaultEnvVars(outputDirectory);
                        wxString intermediateDirectory = confChild->GetAttribute(wxT("IntermediateDirectory"));
                        intermediateDirectory.Replace(wxT("\\"), wxT("/"));
                        intermediateDirectory = ReplaceDefaultEnvVars(intermediateDirectory);
                        wxString solutionDir = genericWorkspace->path + wxT("/");
                        solutionDir.Replace(wxT("\\"), wxT("/"));
                        wxString projectDir = genericProject->path + wxT("/");
                        projectDir.Replace(wxT("\\"), wxT("/"));

                        GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                        genericProjectCfg->name = projectCfgName;

                        genericProjectCfg->envVars[wxT("VS_ConfigurationName")] = configurationName;
                        genericProjectCfg->envVars[wxT("VS_PlatformName")] = platformName;
                        genericProjectCfg->envVars[wxT("VS_OutDir")] = outputDirectory + wxT("/");
                        genericProjectCfg->envVars[wxT("VS_IntDir")] = intermediateDirectory + wxT("/");
                        genericProjectCfg->envVars[wxT("VS_SolutionDir")] = solutionDir;
                        genericProjectCfg->envVars[wxT("VS_ProjectName")] = genericProject->name;
                        genericProjectCfg->envVars[wxT("VS_ProjectDir")] = projectDir;

                        if(!intermediateDirectory.IsEmpty()) {
                            genericProjectCfg->intermediateDirectory = intermediateDirectory;
                        } else {
                            genericProjectCfg->intermediateDirectory = wxT("./") + configurationName;
                        }

                        genericProjectCfg->intermediateDirectory = intermediateDirectory;

                        if(configurationType == wxT("4")) {
                            genericProjectCfg->type = GenericCfgType::STATIC_LIBRARY;
                        } else if(configurationType == wxT("2")) {
                            genericProjectCfg->type = GenericCfgType::DYNAMIC_LIBRARY;
                        } else {
                            genericProjectCfg->type = GenericCfgType::EXECUTABLE;
                        }

                        wxXmlNode* toolChild = confChild->GetChildren();

                        while(toolChild) {
                            if(toolChild->GetName() == wxT("Tool")) {
                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCCLCompilerTool")) {
                                    wxString preprocessorDefinitions =
                                        toolChild->GetAttribute(wxT("PreprocessorDefinitions"));
                                    preprocessorDefinitions.Replace(wxT(","), wxT(";"));
                                    preprocessorDefinitions = ReplaceDefaultEnvVars(preprocessorDefinitions);
                                    genericProjectCfg->preprocessor = preprocessorDefinitions;

                                    wxString additionalIncludeDirectories =
                                        toolChild->GetAttribute(wxT("AdditionalIncludeDirectories"));
                                    additionalIncludeDirectories.Replace(wxT(","), wxT(";"));
                                    additionalIncludeDirectories.Replace(wxT("\\"), wxT("/"));
                                    additionalIncludeDirectories = ReplaceDefaultEnvVars(additionalIncludeDirectories);
                                    genericProjectCfg->includePath = additionalIncludeDirectories;

                                    if(toolChild->GetAttribute(wxT("UsePrecompiledHeader")) == wxT("3")) {
                                        wxString precompiledHeaderThrough =
                                            toolChild->GetAttribute(wxT("PrecompiledHeaderThrough"));
                                        genericProjectCfg->preCompiledHeader = precompiledHeaderThrough;
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCLinkerTool")) {
                                    if(toolChild->HasAttribute(wxT("AdditionalDependencies"))) {
                                        wxString additionalDependencies =
                                            toolChild->GetAttribute(wxT("AdditionalDependencies"));
                                        additionalDependencies.Replace(wxT(" "), wxT(";"));
                                        additionalDependencies = ReplaceDefaultEnvVars(additionalDependencies);
                                        genericProjectCfg->libraries = additionalDependencies;
                                    }

                                    if(toolChild->HasAttribute(wxT("AdditionalLibraryDirectories"))) {
                                        wxString additionalLibraryDirectories =
                                            toolChild->GetAttribute(wxT("AdditionalLibraryDirectories"));
                                        additionalLibraryDirectories.Replace(wxT(","), wxT(";"));
                                        additionalLibraryDirectories.Replace(wxT("\\"), wxT("/"));
                                        additionalLibraryDirectories =
                                            ReplaceDefaultEnvVars(additionalLibraryDirectories);
                                        genericProjectCfg->libPath = additionalLibraryDirectories;
                                    }

                                    if(toolChild->HasAttribute(wxT("OutputFile"))) {
                                        wxString outputFile = toolChild->GetAttribute(wxT("OutputFile"));
                                        outputFile.Replace(wxT("\\"), wxT("/"));
                                        outputFile.Replace(wxT(" "), wxT("_"));
                                        outputFile = ReplaceDefaultEnvVars(outputFile);
                                        genericProjectCfg->outputFilename = outputFile;

                                        wxFileName outputFilenameInfo(outputFile);
                                        genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCLibrarianTool")) {
                                    if(toolChild->HasAttribute(wxT("OutputFile"))) {
                                        wxString outputFile = toolChild->GetAttribute(wxT("OutputFile"));
                                        outputFile.Replace(wxT("\\"), wxT("/"));
                                        outputFile.Replace(wxT(" "), wxT("_"));
                                        outputFile = ReplaceDefaultEnvVars(outputFile);
                                        genericProjectCfg->outputFilename = outputFile;

                                        wxFileName outputFilenameInfo(outputFile);
                                        genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCPreBuildEventTool")) {
                                    if(toolChild->HasAttribute(wxT("CommandLine"))) {
                                        wxString commandLine = toolChild->GetAttribute(wxT("CommandLine"));
                                        if(!commandLine.IsEmpty()) {
                                            genericProjectCfg->preBuildCommands.push_back(commandLine);
                                        }
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCPostBuildEventTool")) {
                                    if(toolChild->HasAttribute(wxT("CommandLine"))) {
                                        wxString commandLine = toolChild->GetAttribute(wxT("CommandLine"));
                                        if(!commandLine.IsEmpty()) {
                                            genericProjectCfg->postBuildCommands.push_back(commandLine);
                                        }
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCNMakeTool")) {
                                    if(toolChild->HasAttribute(wxT("BuildCommandLine"))) {
                                        wxString buildCommandLine = toolChild->GetAttribute(wxT("BuildCommandLine"));
                                        if(!buildCommandLine.IsEmpty()) {
                                            if(!genericProjectCfg->enableCustomBuild)
                                                genericProjectCfg->enableCustomBuild = true;

                                            genericProjectCfg->customBuildCmd = buildCommandLine;
                                        }
                                    }

                                    if(toolChild->HasAttribute(wxT("ReBuildCommandLine"))) {
                                        wxString reBuildCommandLine =
                                            toolChild->GetAttribute(wxT("ReBuildCommandLine"));
                                        if(!reBuildCommandLine.IsEmpty()) {
                                            if(!genericProjectCfg->enableCustomBuild)
                                                genericProjectCfg->enableCustomBuild = true;
                                            genericProjectCfg->customRebuildCmd = reBuildCommandLine;
                                        }
                                    }

                                    if(toolChild->HasAttribute(wxT("CleanCommandLine"))) {
                                        wxString cleanCommandLine = toolChild->GetAttribute(wxT("CleanCommandLine"));
                                        if(!cleanCommandLine.IsEmpty()) {
                                            if(!genericProjectCfg->enableCustomBuild)
                                                genericProjectCfg->enableCustomBuild = true;
                                            genericProjectCfg->customCleanCmd = cleanCommandLine;
                                        }
                                    }
                                }
                            }

                            toolChild = toolChild->GetNext();
                        }

                        if(genericProject->cfgType != genericProjectCfg->type)
                            genericProject->cfgType = genericProjectCfg->type;

                        genericProjectCfgMap[projectCfgName] = genericProjectCfg;
                        genericProject->cfgs.push_back(genericProjectCfg);
                    }

                    confChild = confChild->GetNext();
                }
            }

            if(vspChild->GetName() == wxT("Files")) {
                wxXmlNode* filterChild = vspChild->GetChildren();
                AddFilesVC7(filterChild, genericProject, genericProjectCfgMap, wxT(""));
            }

            vspChild = vspChild->GetNext();
        }

        genericWorkspace->projects.push_back(genericProject);
    }
}

void VisualCppImporter::GenerateFromProjectVC11(GenericWorkspacePtr genericWorkspace,
                                                GenericProjectDataType& genericProjectData)
{
    wxFileName projectInfo(genericProjectData[wxT("projectFullPath")]);
    wxFileName filterInfo(projectInfo.GetFullPath() + wxT(".filters"));

    GenericProjectPtr genericProject = std::make_shared<GenericProject>();
    genericProject->name = genericProjectData[wxT("projectName")];
    genericProject->path = projectInfo.GetPath();

    wxStringTokenizer deps(genericProjectData[wxT("projectDeps")], wxT(";"));

    while(deps.HasMoreTokens()) {
        wxString projectNameDep = deps.GetNextToken().Trim().Trim(false);
        genericProject->deps.Add(projectNameDep);
    }

    std::map<wxString, GenericProjectCfgPtr> genericProjectCfgMap;

    wxXmlDocument projectDoc;
    if(projectDoc.Load(projectInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();
        wxXmlNode* projectChild = root->GetChildren();

        while(projectChild) {
            if(projectChild->GetName() == wxT("ItemGroup") && projectChild->HasAttribute(wxT("Label")) &&
               projectChild->GetAttribute(wxT("Label")) == wxT("ProjectConfigurations")) {
                wxXmlNode* projectConfiguration = projectChild->GetChildren();

                while(projectConfiguration && projectConfiguration->HasAttribute(wxT("Include"))) {
                    wxString projectCfgKey = projectConfiguration->GetAttribute(wxT("Include"));
                    wxString projectCfgName = projectCfgKey;
                    projectCfgName.Replace(wxT("|"), wxT("_"));

                    wxStringTokenizer projectCfgNamePart(projectCfgKey, wxT("|"));
                    wxString configurationName = projectCfgNamePart.NextToken();
                    configurationName.Replace(wxT(" "), wxT("_"));
                    wxString platformName = projectCfgNamePart.NextToken();
                    wxString intermediateDirectory = wxT("./") + configurationName;
                    wxString solutionDir = genericWorkspace->path + wxT("/");
                    solutionDir.Replace(wxT("\\"), wxT("/"));
                    wxString projectDir = genericProject->path + wxT("/");
                    projectDir.Replace(wxT("\\"), wxT("/"));

                    GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                    genericProjectCfg->name = projectCfgName;

                    genericProjectCfg->intermediateDirectory = intermediateDirectory;
                    genericProjectCfg->envVars[wxT("VS_Configuration")] = configurationName;
                    genericProjectCfg->envVars[wxT("VS_Platform")] = platformName;
                    genericProjectCfg->envVars[wxT("VS_IntDir")] = intermediateDirectory + wxT("/");
                    genericProjectCfg->envVars[wxT("VS_OutDir")] = intermediateDirectory + wxT("/");
                    genericProjectCfg->envVars[wxT("VS_SolutionDir")] = solutionDir;
                    genericProjectCfg->envVars[wxT("VS_ProjectName")] = genericProject->name;
                    genericProjectCfg->envVars[wxT("VS_ProjectDir")] = projectDir;

                    genericProjectCfgMap[projectCfgKey] = genericProjectCfg;

                    projectConfiguration = projectConfiguration->GetNext();
                }
            }

            if(projectChild->GetName() == wxT("ItemGroup")) {
                wxXmlNode* itemGroupChild = projectChild->GetChildren();

                while(itemGroupChild) {
                    if(itemGroupChild->GetName() == wxT("CustomBuild")) {
                        wxXmlNode* customBuildChild = itemGroupChild->GetChildren();

                        while(customBuildChild) {
                            if(customBuildChild->GetName() == wxT("Command") &&
                               customBuildChild->HasAttribute(wxT("Condition"))) {
                                wxString elemCondition = customBuildChild->GetAttribute(wxT("Condition"));
                                wxString commandLine = customBuildChild->GetNodeContent();
                                wxString projectCfgKey = ExtractProjectCfgName(wxT(""), elemCondition);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];

                                if(genericProjectCfg && !commandLine.IsEmpty()) {
                                    genericProjectCfg->preBuildCommands.push_back(commandLine);
                                }
                            }

                            customBuildChild = customBuildChild->GetNext();
                        }
                    }

                    if(itemGroupChild->GetName() == wxT("ClInclude") || itemGroupChild->GetName() == wxT("ClCompile") ||
                       itemGroupChild->GetName() == wxT("None") || itemGroupChild->GetName() == wxT("Text") ||
                       itemGroupChild->GetName() == wxT("ResourceCompile") ||
                       itemGroupChild->GetName() == wxT("Image") || itemGroupChild->GetName() == wxT("CustomBuild")) {
                        AddFilesVC11(itemGroupChild, genericProject, genericProjectCfgMap);
                    }

                    itemGroupChild = itemGroupChild->GetNext();
                }
            }

            if(projectChild->GetName() == wxT("PropertyGroup")) {
                wxString parentCondition = projectChild->GetAttribute(wxT("Condition"));

                wxXmlNode* propertyGroupChild = projectChild->GetChildren();

                while(propertyGroupChild) {
                    if(propertyGroupChild->GetName() == wxT("ConfigurationType")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString configurationType = propertyGroupChild->GetNodeContent();

                        wxString outputFilename;

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];

                        if(genericProjectCfg) {
                            if(configurationType == wxT("StaticLibrary")) {
                                genericProjectCfg->type = GenericCfgType::STATIC_LIBRARY;
                            } else if(configurationType == wxT("DynamicLibrary")) {
                                genericProjectCfg->type = GenericCfgType::DYNAMIC_LIBRARY;
                            } else {
                                genericProjectCfg->type = GenericCfgType::EXECUTABLE;
                            }

                            if(genericProject->cfgType != genericProjectCfg->type)
                                genericProject->cfgType = genericProjectCfg->type;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("IntDir")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString intermediateDirectory = propertyGroupChild->GetNodeContent();
                        intermediateDirectory.Replace(wxT("\\"), wxT("/"));
                        intermediateDirectory = ReplaceDefaultEnvVars(intermediateDirectory);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg) {
                            genericProjectCfg->intermediateDirectory =
                                intermediateDirectory.SubString(0, intermediateDirectory.Length() - 1);
                            genericProjectCfg->envVars[wxT("VS_IntDir")] = intermediateDirectory;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("OutDir")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString outDir = propertyGroupChild->GetNodeContent();
                        outDir.Replace(wxT("\\"), wxT("/"));
                        outDir = ReplaceDefaultEnvVars(outDir);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg) {
                            genericProjectCfg->envVars[wxT("VS_OutDir")] = outDir;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("LibraryPath")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString libraryPath = propertyGroupChild->GetNodeContent();
                        libraryPath.Replace(wxT("\\"), wxT("/"));
                        libraryPath = ReplaceDefaultEnvVars(libraryPath);

                        // Remove VC++ library path, as it's not relevant for non VC++ environments
                        static const wxRegEx vcLibraryPath(
                            R"(\$\((LibraryPath|VC_LibraryPath_\w+|WindowsSDK_LibraryPath_\w+)\);?)");
                        vcLibraryPath.ReplaceAll(&libraryPath, wxEmptyString);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg) {
                            genericProjectCfg->libPath = libraryPath;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("IncludePath")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString includePath = propertyGroupChild->GetNodeContent();
                        includePath.Replace(wxT("\\"), wxT("/"));
                        includePath = ReplaceDefaultEnvVars(includePath);

                        // Remove VC++ include path, as it's not relevant for non VC++ environments
                        static const wxRegEx vcIncludePath(
                            R"(\$\((IncludePath|VC_IncludePath|WindowsSDK_IncludePath)\);?)");
                        vcIncludePath.ReplaceAll(&includePath, wxEmptyString);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg) {
                            genericProjectCfg->includePath = includePath;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("NMakeBuildCommandLine")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString nMakeBuildCommandLine = propertyGroupChild->GetNodeContent();
                        nMakeBuildCommandLine = ReplaceDefaultEnvVars(nMakeBuildCommandLine);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg && !nMakeBuildCommandLine.IsEmpty()) {
                            if(!genericProjectCfg->enableCustomBuild)
                                genericProjectCfg->enableCustomBuild = true;

                            genericProjectCfg->customBuildCmd = nMakeBuildCommandLine;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("NMakeReBuildCommandLine")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString nMakeReBuildCommandLine = propertyGroupChild->GetNodeContent();
                        nMakeReBuildCommandLine = ReplaceDefaultEnvVars(nMakeReBuildCommandLine);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg && !nMakeReBuildCommandLine.IsEmpty()) {
                            if(!genericProjectCfg->enableCustomBuild)
                                genericProjectCfg->enableCustomBuild = true;

                            genericProjectCfg->customRebuildCmd = nMakeReBuildCommandLine;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("NMakeCleanCommandLine")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString nMakeCleanCommandLine = propertyGroupChild->GetNodeContent();
                        nMakeCleanCommandLine = ReplaceDefaultEnvVars(nMakeCleanCommandLine);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg && !nMakeCleanCommandLine.IsEmpty()) {
                            if(!genericProjectCfg->enableCustomBuild)
                                genericProjectCfg->enableCustomBuild = true;

                            genericProjectCfg->customCleanCmd = nMakeCleanCommandLine;
                        }
                    }

                    propertyGroupChild = propertyGroupChild->GetNext();
                }
            }

            if(projectChild->GetName() == wxT("ItemDefinitionGroup")) {
                wxString parentCondition = projectChild->GetAttribute(wxT("Condition"));

                wxXmlNode* itemDefinitionGroupChild = projectChild->GetChildren();

                while(itemDefinitionGroupChild) {
                    if(itemDefinitionGroupChild->GetName() == wxT("ClCompile")) {
                        wxXmlNode* clcompileChild = itemDefinitionGroupChild->GetChildren();

                        while(clcompileChild) {
                            if(clcompileChild->GetName() == wxT("PreprocessorDefinitions")) {
                                wxString elemCondition = clcompileChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString preprocessorDefinitions = clcompileChild->GetNodeContent();
                                preprocessorDefinitions.Replace(wxT("%(PreprocessorDefinitions)"), wxT(""));
                                preprocessorDefinitions = ReplaceDefaultEnvVars(preprocessorDefinitions);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->preprocessor = preprocessorDefinitions;
                                }
                            }

                            if(clcompileChild->GetName() == wxT("PrecompiledHeaderFile")) {
                                wxString elemCondition = clcompileChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString precompiledHeaderFile = clcompileChild->GetNodeContent();

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->preCompiledHeader = precompiledHeaderFile;
                                }
                            }

                            if(clcompileChild->GetName() == wxT("AdditionalIncludeDirectories")) {
                                wxString elemCondition = clcompileChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalIncludeDirectories = clcompileChild->GetNodeContent();
                                additionalIncludeDirectories.Replace(wxT("%(AdditionalIncludeDirectories)"), wxT(""));
                                additionalIncludeDirectories.Replace(wxT("\\"), wxT("/"));
                                additionalIncludeDirectories = ReplaceDefaultEnvVars(additionalIncludeDirectories);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->includePath = additionalIncludeDirectories;
                                }
                            }

                            clcompileChild = clcompileChild->GetNext();
                        }
                    }

                    if(itemDefinitionGroupChild->GetName() == wxT("Link")) {
                        wxXmlNode* linkChild = itemDefinitionGroupChild->GetChildren();

                        while(linkChild) {
                            if(linkChild->GetName() == wxT("AdditionalDependencies")) {
                                wxString elemCondition = linkChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalDependencies = linkChild->GetNodeContent();
                                additionalDependencies.Replace(wxT("%(AdditionalDependencies)"), wxT(""));
                                additionalDependencies = ReplaceDefaultEnvVars(additionalDependencies);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->libraries = additionalDependencies;
                                }
                            }

                            if(linkChild->GetName() == wxT("AdditionalLibraryDirectories")) {
                                wxString elemCondition = linkChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalLibraryDirectories = linkChild->GetNodeContent();
                                additionalLibraryDirectories.Replace(wxT("%(AdditionalLibraryDirectories)"), wxT(""));
                                additionalLibraryDirectories = ReplaceDefaultEnvVars(additionalLibraryDirectories);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->libPath = additionalLibraryDirectories;
                                }
                            }

                            if(linkChild->GetName() == wxT("OutputFile")) {
                                wxString elemCondition = linkChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString outputFile = linkChild->GetNodeContent();
                                outputFile.Replace(wxT("\\"), wxT("/"));
                                outputFile.Replace(wxT(" "), wxT("_"));
                                outputFile = ReplaceDefaultEnvVars(outputFile);

                                wxFileName outputFilenameInfo(outputFile);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->outputFilename = outputFile;
                                    genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                }
                            }

                            linkChild = linkChild->GetNext();
                        }
                    }

                    if(itemDefinitionGroupChild->GetName() == wxT("Lib")) {
                        wxXmlNode* LibChild = itemDefinitionGroupChild->GetChildren();

                        while(LibChild) {
                            if(LibChild->GetName() == wxT("OutputFile")) {
                                wxString elemCondition = LibChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString outputFile = LibChild->GetNodeContent();
                                outputFile.Replace(wxT("\\"), wxT("/"));
                                outputFile.Replace(wxT(" "), wxT("_"));
                                outputFile = ReplaceDefaultEnvVars(outputFile);

                                wxFileName outputFilenameInfo(outputFile);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->outputFilename = outputFile;
                                    genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                }
                            }

                            LibChild = LibChild->GetNext();
                        }
                    }

                    if(itemDefinitionGroupChild->GetName() == wxT("PreBuildEvent")) {
                        wxXmlNode* preBuildEventChild = itemDefinitionGroupChild->GetChildren();

                        while(preBuildEventChild) {
                            if(preBuildEventChild->GetName() == wxT("Command")) {
                                wxString elemCondition = preBuildEventChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString command = preBuildEventChild->GetNodeContent();
                                if(!command.IsEmpty()) {
                                    GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                    if(genericProjectCfg) {
                                        genericProjectCfg->preBuildCommands.push_back(command);
                                    }
                                }
                            }

                            preBuildEventChild = preBuildEventChild->GetNext();
                        }
                    }

                    if(itemDefinitionGroupChild->GetName() == wxT("PostBuildEvent")) {
                        wxXmlNode* postBuildEventChild = itemDefinitionGroupChild->GetChildren();

                        while(postBuildEventChild) {
                            if(postBuildEventChild->GetName() == wxT("Command")) {
                                wxString elemCondition = postBuildEventChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString command = postBuildEventChild->GetNodeContent();
                                if(!command.IsEmpty()) {
                                    GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                    if(genericProjectCfg) {
                                        genericProjectCfg->postBuildCommands.push_back(command);
                                    }
                                }
                            }

                            postBuildEventChild = postBuildEventChild->GetNext();
                        }
                    }

                    itemDefinitionGroupChild = itemDefinitionGroupChild->GetNext();
                }
            }

            projectChild = projectChild->GetNext();
        }

        for(std::pair<wxString, GenericProjectCfgPtr> genericProjectCfg : genericProjectCfgMap) {
            if(genericProjectCfg.second) {
                genericProject->cfgs.push_back(genericProjectCfg.second);
            }
        }
    }

    wxXmlDocument filterDoc;
    if(filterDoc.Load(filterInfo.GetFullPath())) {
        wxXmlNode* root = filterDoc.GetRoot();
        wxXmlNode* projectChild = root->GetChildren();

        while(projectChild) {
            if(projectChild->GetName() == wxT("ItemGroup")) {
                wxXmlNode* itemGroupChild = projectChild->GetChildren();

                while(itemGroupChild) {
                    if(itemGroupChild->GetName() == wxT("ClInclude") || itemGroupChild->GetName() == wxT("ClCompile") ||
                       itemGroupChild->GetName() == wxT("None") || itemGroupChild->GetName() == wxT("Text") ||
                       itemGroupChild->GetName() == wxT("ResourceCompile") ||
                       itemGroupChild->GetName() == wxT("Image") || itemGroupChild->GetName() == wxT("CustomBuild")) {
                        AddFilesVC11(itemGroupChild, genericProject, genericProjectCfgMap);
                    }

                    itemGroupChild = itemGroupChild->GetNext();
                }
            }

            projectChild = projectChild->GetNext();
        }
    }

    genericWorkspace->projects.push_back(genericProject);
}

wxString VisualCppImporter::ExtractProjectCfgName(const wxString& parentCondition, const wxString& elemCondition)
{
    wxString tmp = !elemCondition.IsEmpty() ? elemCondition : parentCondition;
    tmp.Replace(wxT("'$(Configuration)|$(Platform)'=='"), wxT(""));
    tmp.Replace(wxT("'"), wxT(""));
    return tmp;
}

wxString VisualCppImporter::ReplaceDefaultEnvVars(const wxString& str)
{
    wxString tmp = str;
    tmp.Replace(wxT("$(ConfigurationName)"), wxT("$(VS_ConfigurationName)"));
    tmp.Replace(wxT("$(Configuration)"), wxT("$(VS_Configuration)"));
    tmp.Replace(wxT("$(PlatformName)"), wxT("$(VS_PlatformName)"));
    tmp.Replace(wxT("$(Platform)"), wxT("$(VS_Platform)"));
    tmp.Replace(wxT("$(IntDir)"), wxT("$(VS_IntDir)"));
    tmp.Replace(wxT("$(OutDir)"), wxT("$(VS_OutDir)"));
    tmp.Replace(wxT("$(SolutionDir)"), wxT("$(VS_SolutionDir)"));
    tmp.Replace(wxT("$(ProjectName)"), wxT("$(VS_ProjectName)"));
    tmp.Replace(wxT("$(ProjectDir)"), wxT("$(VS_ProjectDir)"));

    return tmp;
}

void VisualCppImporter::AddFilesVC7(wxXmlNode* filterChild, GenericProjectPtr genericProject,
                                    std::map<wxString, GenericProjectCfgPtr>& genericProjectCfgMap,
                                    wxString preVirtualPath)
{
    while(filterChild) {
        if(filterChild->GetName() == wxT("Filter")) {
            wxString virtualPath = filterChild->GetAttribute(wxT("Name"));
            wxXmlNode* fileChild = filterChild->GetChildren();

            if(!preVirtualPath.IsEmpty()) {
                virtualPath = preVirtualPath + wxT("/") + virtualPath;
            }

            AddFilesVC7(fileChild, genericProject, genericProjectCfgMap, virtualPath);
        } else if(filterChild->GetName() == wxT("File")) {
            wxString filename = filterChild->GetAttribute(wxT("RelativePath"));
            filename.Replace(wxT("\\"), wxT("/"));

            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
            genericProjectFile->name = filename;
            genericProjectFile->vpath = preVirtualPath;

            genericProject->files.push_back(genericProjectFile);

            wxXmlNode* fileConfChild = filterChild->GetChildren();

            while(fileConfChild) {
                if(fileConfChild->GetName() == wxT("FileConfiguration")) {
                    wxString name = fileConfChild->GetAttribute(wxT("Name"));
                    wxString excludedFromBuild = fileConfChild->GetAttribute(wxT("ExcludedFromBuild"));
                    wxString projectCfgName = name;
                    projectCfgName.Replace(wxT("|"), wxT("_"));

                    if(excludedFromBuild == wxT("true")) {
                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgName];
                        if(genericProjectCfg) {
                            genericProjectCfg->excludeFiles.push_back(genericProjectFile);
                        }
                    }

                    wxXmlNode* toolChild = fileConfChild->GetChildren();

                    while(toolChild) {
                        if(toolChild->GetName() == wxT("Tool")) {
                            if(toolChild->GetAttribute(wxT("Name")) == wxT("VCCustomBuildTool")) {
                                wxString commandLine = toolChild->GetAttribute(wxT("CommandLine"));

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgName];
                                if(genericProjectCfg && !commandLine.IsEmpty()) {
                                    genericProjectCfg->preBuildCommands.push_back(commandLine);
                                }
                            }
                        }

                        toolChild = toolChild->GetNext();
                    }
                }

                fileConfChild = fileConfChild->GetNext();
            }
        }

        filterChild = filterChild->GetNext();
    }
}

void VisualCppImporter::AddFilesVC11(wxXmlNode* itemGroupChild, GenericProjectPtr genericProject,
                                     std::map<wxString, GenericProjectCfgPtr>& genericProjectCfgMap)
{
    wxString filename = itemGroupChild->GetAttribute(wxT("Include"));
    filename.Replace(wxT("\\"), wxT("/"));

    GenericProjectFilePtr genericProjectFile = FindProjectFileByName(genericProject, filename);

    if(genericProjectFile) {
        genericProjectFile->name = filename;
    } else {
        genericProjectFile = std::make_shared<GenericProjectFile>();
        genericProjectFile->name = filename;

        genericProject->files.push_back(genericProjectFile);
    }

    wxXmlNode* otherChild = itemGroupChild->GetChildren();
    while(otherChild) {
        if(otherChild->GetName() == wxT("Filter")) {
            wxString content = otherChild->GetNodeContent();
            if(!content.IsEmpty()) {
                genericProjectFile->vpath = content;
            }
        }

        if(otherChild->GetName() == wxT("ExcludedFromBuild")) {
            wxString elemCondition = otherChild->GetAttribute(wxT("Condition"));
            wxString projectCfgKey = ExtractProjectCfgName(wxT(""), elemCondition);
            wxString content = otherChild->GetNodeContent();

            if(content == wxT("true")) {
                if(!projectCfgKey.IsEmpty()) {
                    GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];

                    if(genericProjectCfg) {
                        genericProjectCfg->excludeFiles.push_back(genericProjectFile);
                    }
                } else {
                    for(std::pair<wxString, GenericProjectCfgPtr> genericProjectCfgPair : genericProjectCfgMap) {
                        if(genericProjectCfgPair.second) {
                            genericProjectCfgPair.second->excludeFiles.push_back(genericProjectFile);
                        }
                    }
                }
            }
        }

        otherChild = otherChild->GetNext();
    }
}

GenericProjectFilePtr VisualCppImporter::FindProjectFileByName(GenericProjectPtr genericProject, wxString filename)
{
    for(GenericProjectFilePtr projectFile : genericProject->files) {
        if(projectFile->name == filename)
            return projectFile;
    }

    return nullptr;
}

void VisualCppImporter::ConvertToLinuxStyle(wxString& filepath)
{
    filepath.Replace("\\", "/");
    wxFileName fn(filepath);
    fn.Normalize(wxPATH_NORM_DOTS); // Remove . and ..
    filepath = fn.GetFullPath(wxPATH_UNIX);
}
