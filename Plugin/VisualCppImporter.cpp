#include "VisualCppImporter.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>

bool VisualCppImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler)
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

bool VisualCppImporter::isSupportedWorkspace()
{
    return version >= 6 && version <= 12;
}

GenericWorkspacePtr VisualCppImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    switch(version) {
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
            value.Replace(wxT(".\\"), wxT(""));

            wxStringTokenizer part(value, wxT("="));

            GenericProjectDataType genericProjectData;
            genericProjectData[wxT("projectName")] = part.GetNextToken().Trim().Trim(false);
            genericProjectData[wxT("projectFile")] = part.GetNextToken().Trim().Trim(false);
            genericProjectData[wxT("projectFullPath")] =
                wsInfo.GetPath() + wxFileName::GetPathSeparator() + genericProjectData[wxT("projectFile")];

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

        wxTextInputStream projectTIS(projectFIS);

        wxString virtualPath = wxT("");

        while(!projectFIS.Eof()) {
            wxString line = projectTIS.ReadLine();

            int index = line.Find(wxT("TARGTYPE"));
            if(index != wxNOT_FOUND) {
                index = line.find_last_of(wxT("\""));
                wxString projectType = line.Mid(index + 1).Trim().Trim(false);
                if(projectType == wxT("0x0104")) {
                    genericProject->cfgType = GenericCfgType::STATIC_LIBRARY;
                } else if(projectType == wxT("0x0102")) {
                    genericProject->cfgType = GenericCfgType::DYNAMIC_LIBRARY;
                } else {
                    genericProject->cfgType = GenericCfgType::EXECUTABLE;
                }
            }

            index = line.Find(wxT("\"$(CFG)\" == "));
            if(index != wxNOT_FOUND) {
                wxString projectCfgName = line.Mid(index + 12).Trim().Trim(false);
                projectCfgName.Replace(genericProjectData[wxT("projectName")] + wxT(" - "), wxT(""));
                projectCfgName.Replace(wxT("\""), wxT(""));
                projectCfgName.Replace(wxT(" "), wxT("_"));

                GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                genericProjectCfg->name = projectCfgName;
                genericProjectCfg->type = genericProject->cfgType;

                while(!projectFIS.Eof()) {
                    line = projectTIS.ReadLine();

                    index = line.Find(wxT("PROP Intermediate_Dir"));
                    if(index != wxNOT_FOUND) {
                        wxString intermediateDirectory = line.Mid(index + 21).Trim().Trim(false);
                        intermediateDirectory.Replace(wxT("\""), wxT(""));
                        intermediateDirectory.Replace(wxT(" "), wxT("_"));
                        intermediateDirectory.Replace(wxT(".\\"), wxT(""));
                        genericProjectCfg->intermediateDirectory = wxT("./") + intermediateDirectory;
                    }

                    index = line.Find(wxT("ADD CPP"));
                    if(index != wxNOT_FOUND) {
                        wxString preprocessor = wxT(""), includePath = wxT(""), preCompiledHeader = wxT("");
                        size_t pos = 0;

                        while(pos < line.Length()) {
                            if(line.GetChar(pos) == wxT('/') && line.GetChar(pos + 1) == wxT('D')) {
                                int count = 0;
                                wxString word = wxT("");
                                while(count < 2) {
                                    if(line.GetChar(pos) == wxT(' ')) {
                                        count++;
                                    } else if(count == 1 && line.GetChar(pos) != wxT('\"')) {
                                        word.Append(line.GetChar(pos));
                                    }

                                    pos++;
                                }

                                preprocessor += word + wxT(";");
                            } else if(line.GetChar(pos) == wxT('/') && line.GetChar(pos + 1) == wxT('I')) {
                                int count = 0;
                                wxString word = wxT("");
                                while(count < 2) {
                                    if(line.GetChar(pos) == wxT(' ')) {
                                        count++;
                                    } else if(count == 1 && line.GetChar(pos) != wxT('\"')) {
                                        word.Append(line.GetChar(pos));
                                    }

                                    pos++;
                                }

                                includePath += word + wxT(";");
                            } else if(line.GetChar(pos) == wxT('/') && line.GetChar(pos + 1) == wxT('Y') &&
                                      line.GetChar(pos + 2) == wxT('u')) {
                                int count = 0;
                                wxString word = wxT("");
                                while(count < 2) {
                                    if(line.GetChar(pos) == wxT('\"')) {
                                        count++;
                                    } else if(count == 1) {
                                        word.Append(line.GetChar(pos));
                                    }

                                    pos++;
                                }

                                preCompiledHeader = word;
                            } else {
                                pos++;
                            }
                        }

                        genericProjectCfg->preprocessor = preprocessor;
                        genericProjectCfg->includePath = includePath;
                        genericProjectCfg->preCompiledHeader = preCompiledHeader;
                    }

                    index = line.Find(wxT("ADD LINK32"));
                    if(index != wxNOT_FOUND) {
                        int begin = index + 10;
                        int end = line.Find(wxT("/")) - 1;
                        line = line.SubString(begin, end).Trim().Trim(false);

                        wxString libraries = wxT("");
                        wxString libPath = wxT("");
                        wxStringTokenizer libs(line, wxT(" "));

                        while(libs.HasMoreTokens()) {
                            wxString lib = libs.GetNextToken();
                            index = static_cast<int>(lib.find_last_of(wxT("\\")));
                            if(index != wxNOT_FOUND) {
                                wxString include = lib.SubString(0, index);
                                lib = lib.Mid(index + 1);
                                libPath += include + wxT(";");
                                libraries += lib + wxT(";");
                            } else {
                                libraries += lib + wxT(";");
                            }
                        }

                        genericProjectCfg->libraries = libraries;
                        genericProjectCfg->libPath = libPath;
                        break;
                    }

                    index = line.Find(wxT("!ENDIF"));
                    if(index != wxNOT_FOUND)
                        break;
                }

                genericProject->cfgs.push_back(genericProjectCfg);
            }

            index = line.Find(wxT("Begin Group"));
            if(index != wxNOT_FOUND) {
                virtualPath = line.Mid(index + 11).Trim().Trim(false);
                virtualPath.Replace(wxT("\""), wxT(""));
            }

            index = line.Find(wxT("SOURCE="));
            if(index != wxNOT_FOUND) {
                wxString filename = line.Mid(index + 7).Trim().Trim(false);
                filename.Replace(wxT("\\"), wxT("/"));

                GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                genericProjectFile->name = filename;
                genericProjectFile->vpath = virtualPath;

                genericProject->files.push_back(genericProjectFile);
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
                            }

                            toolChild = toolChild->GetNext();
                        }

                        if(genericProject->cfgType != genericProjectCfg->type)
                            genericProject->cfgType = genericProjectCfg->type;

                        genericProject->cfgs.push_back(genericProjectCfg);
                    }

                    confChild = confChild->GetNext();
                }
            }

            if(vspChild->GetName() == wxT("Files")) {
                wxXmlNode* filterChild = vspChild->GetChildren();

                while(filterChild) {
                    if(filterChild->GetName() == wxT("Filter")) {
                        wxString virtualPath = filterChild->GetAttribute(wxT("Name"));

                        wxXmlNode* fileChild = filterChild->GetChildren();

                        while(fileChild) {
                            if(fileChild->GetName() == wxT("File")) {
                                wxString filename = fileChild->GetAttribute(wxT("RelativePath"));
                                filename.Replace(wxT("\\"), wxT("/"));

                                GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                                genericProjectFile->name = filename;
                                genericProjectFile->vpath = virtualPath;

                                genericProject->files.push_back(genericProjectFile);
                            }

                            fileChild = fileChild->GetNext();
                        }
                    }

                    filterChild = filterChild->GetNext();
                }
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

    wxXmlDocument projectDoc;
    if(projectDoc.Load(projectInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();
        wxXmlNode* projectChild = root->GetChildren();

        std::map<wxString, GenericProjectCfgPtr> GenericProjectCfgMap;

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

                    GenericProjectCfgMap[projectCfgKey] = genericProjectCfg;

                    projectConfiguration = projectConfiguration->GetNext();
                }
            }

            if(projectChild->GetName() == wxT("PropertyGroup")) {
                wxXmlNode* propertyGroupChild = projectChild->GetChildren();

                while(propertyGroupChild) {
                    wxString parentCondition = projectChild->GetAttribute("Condition");

                    if(propertyGroupChild->GetName() == wxT("ConfigurationType")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute("Condition");
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString configurationType = propertyGroupChild->GetNodeContent();

                        wxString outputFilename;

                        GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];

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
                        wxString elemCondition = propertyGroupChild->GetAttribute("Condition");
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString intermediateDirectory = propertyGroupChild->GetNodeContent();
                        intermediateDirectory.Replace(wxT("\\"), wxT("/"));
                        intermediateDirectory = ReplaceDefaultEnvVars(intermediateDirectory);

                        GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg) {
                            genericProjectCfg->intermediateDirectory =
                                intermediateDirectory.SubString(0, intermediateDirectory.Length() - 1);
                            genericProjectCfg->envVars[wxT("VS_IntDir")] = intermediateDirectory;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("OutDir")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute("Condition");
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString outDir = propertyGroupChild->GetNodeContent();
                        outDir.Replace(wxT("\\"), wxT("/"));
                        outDir = ReplaceDefaultEnvVars(outDir);

                        GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg) {
                            genericProjectCfg->envVars[wxT("VS_OutDir")] = outDir;
                        }
                    }

                    propertyGroupChild = propertyGroupChild->GetNext();
                }
            }

            if(projectChild->GetName() == wxT("ItemDefinitionGroup")) {
                wxString parentCondition = projectChild->GetAttribute("Condition");

                wxXmlNode* itemDefinitionGroupChild = projectChild->GetChildren();

                while(itemDefinitionGroupChild) {
                    if(itemDefinitionGroupChild->GetName() == wxT("ClCompile")) {
                        wxXmlNode* clcompileChild = itemDefinitionGroupChild->GetChildren();

                        while(clcompileChild) {
                            if(clcompileChild->GetName() == wxT("PreprocessorDefinitions")) {
                                wxString elemCondition = clcompileChild->GetAttribute("Condition");
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString preprocessorDefinitions = clcompileChild->GetNodeContent();
                                preprocessorDefinitions.Replace(wxT("%(PreprocessorDefinitions)"), wxT(""));
                                preprocessorDefinitions = ReplaceDefaultEnvVars(preprocessorDefinitions);

                                GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->preprocessor = preprocessorDefinitions;
                                }
                            }

                            if(clcompileChild->GetName() == wxT("PrecompiledHeaderFile")) {
                                wxString elemCondition = clcompileChild->GetAttribute("Condition");
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString precompiledHeaderFile = clcompileChild->GetNodeContent();

                                GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->preCompiledHeader = precompiledHeaderFile;
                                }
                            }

                            if(clcompileChild->GetName() == wxT("AdditionalIncludeDirectories")) {
                                wxString elemCondition = clcompileChild->GetAttribute("Condition");
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalIncludeDirectories = clcompileChild->GetNodeContent();
                                additionalIncludeDirectories.Replace(wxT("%(AdditionalIncludeDirectories)"), wxT(""));
                                additionalIncludeDirectories.Replace(wxT("\\"), wxT("/"));
                                additionalIncludeDirectories = ReplaceDefaultEnvVars(additionalIncludeDirectories);

                                GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
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
                                wxString elemCondition = projectChild->GetAttribute("Condition");
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalDependencies = linkChild->GetNodeContent();
                                additionalDependencies.Replace(wxT("%(AdditionalDependencies)"), wxT(""));
                                additionalDependencies = ReplaceDefaultEnvVars(additionalDependencies);

                                GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->libraries = additionalDependencies;
                                }
                            }

                            if(linkChild->GetName() == wxT("AdditionalLibraryDirectories")) {
                                wxString elemCondition = projectChild->GetAttribute("Condition");
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalLibraryDirectories = linkChild->GetNodeContent();
                                additionalLibraryDirectories.Replace(wxT("%(AdditionalLibraryDirectories)"), wxT(""));
                                additionalLibraryDirectories = ReplaceDefaultEnvVars(additionalLibraryDirectories);

                                GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->libPath = additionalLibraryDirectories;
                                }
                            }

                            if(linkChild->GetName() == wxT("OutputFile")) {
                                wxString elemCondition = projectChild->GetAttribute("Condition");
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString outputFile = linkChild->GetNodeContent();
                                outputFile.Replace(wxT("\\"), wxT("/"));
                                outputFile.Replace(wxT(" "), wxT("_"));
                                outputFile = ReplaceDefaultEnvVars(outputFile);

                                wxFileName outputFilenameInfo(outputFile);

                                GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
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
                                wxString elemCondition = projectChild->GetAttribute("Condition");
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString outputFile = LibChild->GetNodeContent();
                                outputFile.Replace(wxT("\\"), wxT("/"));
                                outputFile.Replace(wxT(" "), wxT("_"));
                                outputFile = ReplaceDefaultEnvVars(outputFile);

                                wxFileName outputFilenameInfo(outputFile);

                                GenericProjectCfgPtr genericProjectCfg = GenericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->outputFilename = outputFile;
                                    genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                }
                            }

                            LibChild = LibChild->GetNext();
                        }
                    }

                    itemDefinitionGroupChild = itemDefinitionGroupChild->GetNext();
                }
            }

            if(!filterInfo.Exists()) {
                if(projectChild->GetName() == wxT("ItemGroup")) {
                    wxXmlNode* itemChild = projectChild->GetChildren();

                    while(itemChild) {
                        if(itemChild->GetName() == wxT("ClInclude") || itemChild->GetName() == wxT("ClCompile") ||
                           itemChild->GetName() == wxT("None") || itemChild->GetName() == wxT("Text") ||
                           itemChild->GetName() == wxT("ResourceCompile")) {

                            wxString virtualPath = "";
                            wxString filename = itemChild->GetAttribute("Include");
                            filename.Replace(wxT("\\"), wxT("/"));

                            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                            genericProjectFile->name = filename;
                            genericProjectFile->vpath = virtualPath;

                            genericProject->files.push_back(genericProjectFile);
                        }

                        itemChild = itemChild->GetNext();
                    }
                }
            }

            projectChild = projectChild->GetNext();
        }

        for(std::pair<wxString, GenericProjectCfgPtr> genericProjectCfg : GenericProjectCfgMap) {
            if(genericProjectCfg.second) {
                genericProject->cfgs.push_back(genericProjectCfg.second);
            }
        }
    }

    wxXmlDocument filterDoc;
    if(filterDoc.Load(filterInfo.GetFullPath())) {
        wxXmlNode* root = filterDoc.GetRoot();
        wxXmlNode* itemGroupChild = root->GetChildren();

        while(itemGroupChild) {
            wxXmlNode* itemChild = itemGroupChild->GetChildren();

            while(itemChild) {
                if(itemChild->GetName() == wxT("ClInclude") || itemChild->GetName() == wxT("ClCompile") ||
                   itemChild->GetName() == wxT("None") || itemChild->GetName() == wxT("Text") ||
                   itemChild->GetName() == wxT("ResourceCompile")) {
                    wxString virtualPath = "";
                    wxString filename = itemChild->GetAttribute("Include");
                    filename.Replace(wxT("\\"), wxT("/"));

                    wxXmlNode* filterChild = itemChild->GetChildren();
                    if(filterChild) {
                        if(filterChild->GetName() == wxT("Filter")) {
                            wxString content = filterChild->GetNodeContent();
                            if(!content.IsEmpty()) {
                                virtualPath = content;
                            }
                        }
                    }

                    GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                    genericProjectFile->name = filename;
                    genericProjectFile->vpath = virtualPath;

                    genericProject->files.push_back(genericProjectFile);
                }

                itemChild = itemChild->GetNext();
            }

            itemGroupChild = itemGroupChild->GetNext();
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