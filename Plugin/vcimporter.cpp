//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : vcimporter.cpp
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
#include "vcimporter.h"

#include "macros.h"
#include "workspace.h"
#include "wx/filename.h"
#include "wx/tokenzr.h"
#include "xmlutils.h"

#define NEXT_LINE(line)   \
    line.Empty();         \
    if(!ReadLine(line)) { \
        return false;     \
    }

VcImporter::VcImporter(const wxString& fileName, const wxString& defaultCompiler)
    : m_fileName(fileName)
    , m_is(NULL)
    , m_tis(NULL)
    , m_compiler(defaultCompiler)
    , m_compilerLowercase(defaultCompiler)
{
    m_compilerLowercase.MakeLower();

    wxFileName fn(m_fileName);
    m_isOk = fn.FileExists();
    if(m_isOk) {
        m_is = new wxFileInputStream(fn.GetFullPath());
        m_tis = new wxTextInputStream(*m_is);
    }
}

VcImporter::~VcImporter()
{
    if(m_is) {
        delete m_is;
    }
    if(m_tis) {
        delete m_tis;
    }
}

bool VcImporter::ReadLine(wxString& line)
{
    line = wxEmptyString;
    if(m_isOk) {
        while(!m_is->Eof()) {
            line = m_tis->ReadLine();
            TrimString(line);
            if(line.Length() == 1 || line.Length() == 2 || line.IsEmpty() || line.StartsWith(wxT("#"))) {
                // get next line
                continue;
            } else {
                return true;
            }
        }
    }
    return false;
}

bool VcImporter::Import(wxString& errMsg)
{
    wxString line;
    while(true) {
        if(!ReadLine(line))
            break;
        if(line.StartsWith(wxT("Project"))) {
            if(!OnProject(line, errMsg)) {
                return false;
            }
        }
    }

    // create LE files
    CreateWorkspace();
    CreateProjects();
    return true;
}

bool VcImporter::OnProject(const wxString& firstLine, wxString& errMsg)
{
    // first line contains the project name, project file path, and project id
    wxStringTokenizer tkz(firstLine, wxT("="));
    if(tkz.CountTokens() != 2) {
        errMsg = wxT("Invalid 'Project' section found. expected <expr> = <expr>");
        return false;
    }

    tkz.NextToken();
    wxString token = tkz.NextToken();
    TrimString(token);
    wxStringTokenizer tk2(token, wxT(","));
    if(tk2.CountTokens() != 3) {
        errMsg = wxT("Invalid 'Project' section found. expected <project name>, <project file path>, <project id>");
        return false;
    }

    VcProjectData pd;

    pd.name = tk2.NextToken();
    RemoveGershaim(pd.name);

    pd.filepath = tk2.NextToken();
    RemoveGershaim(pd.filepath);

    // Make sure that the project path has a forward slash style
    pd.filepath.Replace(wxT("\\"), wxT("/"));

    pd.id = tk2.NextToken();
    RemoveGershaim(pd.id);

    std::pair<wxString, VcProjectData> p;
    p.first = pd.id;
    p.second = pd;
    m_projects.insert(p);
    // Skip all lines until EndProject section is found
    wxString line;
    while(true) {
        NEXT_LINE(line);
        if(line == wxT("EndProject")) {
            return true;
        }
    }
    return false;
}

void VcImporter::RemoveGershaim(wxString& str)
{
    TrimString(str);
    str = str.AfterFirst(wxT('"'));
    str = str.BeforeLast(wxT('"'));
}

void VcImporter::CreateWorkspace()
{
    // create a workspace file from the data we collected
    wxFileName fn(m_fileName);
    wxString errMsg;
    clCxxWorkspaceST::Get()->CreateWorkspace(fn.GetName(), fn.GetPath(), errMsg);
}

//
// ConfigurationType = 1 // exe
// ConfigurationType = 2 // dll
// ConfigurationType = 4 // static lib
//
void VcImporter::CreateProjects()
{
    std::map<wxString, VcProjectData>::iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); iter++) {
        // load xml file
        VcProjectData pd = iter->second;
        ConvertProject(pd);
    }
}

bool VcImporter::ConvertProject(VcProjectData& data)
{
    wxXmlDocument doc(data.filepath);
    if(!doc.IsOk()) {
        return false;
    }

    // to create a project skeleton, we need the project type
    // since VS allows each configuration to be of different
    // type, while LE allows single type for all configurations
    // we use the first configuration type that we find
    wxXmlNode* configs = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("Configurations"));
    if(!configs) {
        return false;
    }

    // find the first configuration node
    wxXmlNode* config = XmlUtils::FindFirstByTagName(configs, wxT("Configuration"));
    if(!config)
        return false;
    // read the configuration type, default is set to Executeable
    long type = XmlUtils::ReadLong(config, wxT("ConfigurationType"), 1);
    wxString projectType;
    wxString errMsg;
    switch(type) {
    case 2: // dll
        projectType = PROJECT_TYPE_DYNAMIC_LIBRARY;
        break;
    case 4: // static library
        projectType = PROJECT_TYPE_STATIC_LIBRARY;
        break;
    case 1: // exe
    default:
        projectType = PROJECT_TYPE_EXECUTABLE;
        break;
    }
    // now we can create the project
    wxFileName fn(data.filepath);
    fn.MakeAbsolute();
    if(!clCxxWorkspaceST::Get()->CreateProject(data.name, fn.GetPath(), projectType, "", true, errMsg)) {
        return false;
    }

    // get the new project instance
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(data.name, errMsg);
    ProjectSettingsPtr le_settings(new ProjectSettings(NULL));
    // remove the default 'Debug' configuration
    le_settings->RemoveConfiguration(wxT("Debug"));
    le_settings->SetProjectType(projectType);

    while(config) {
        if(config->GetName() == wxT("Configuration")) {
            AddConfiguration(le_settings, config);
        }
        config = config->GetNext();
    }
    proj->SetSettings(le_settings);
    // add all virtual folders
    wxXmlNode* files = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("Files"));
    if(files) {
        proj->BeginTranscation();
        CreateFiles(files, wxEmptyString, proj);
        proj->CommitTranscation();
    }
    return true;
}

void VcImporter::AddConfiguration(ProjectSettingsPtr settings, wxXmlNode* config)
{
    // configuration name
    wxString name = XmlUtils::ReadString(config, wxT("Name"));
    name = name.BeforeFirst(wxT('|'));
    name.Replace(wxT(" "), wxT("_"));

    BuildConfigPtr le_conf(new BuildConfig(NULL));
    le_conf->SetName(name);
    le_conf->SetIntermediateDirectory(XmlUtils::ReadString(config, wxT("IntermediateDirectory")));
    // get the compiler settings
    wxXmlNode* cmpNode = XmlUtils::FindNodeByName(config, wxT("Tool"), wxT("VCCLCompilerTool"));
    // get the include directories
    le_conf->SetIncludePath(SplitString(XmlUtils::ReadString(cmpNode, wxT("AdditionalIncludeDirectories"))));
    le_conf->SetPreprocessor(XmlUtils::ReadString(cmpNode, wxT("PreprocessorDefinitions")));

    // Select the best compiler for the import process (we select g++ by default)
    le_conf->SetCompilerType(m_compiler);

    // Get the configuration type
    long type = XmlUtils::ReadLong(config, wxT("ConfigurationType"), 1);
    wxString projectType;
    wxString errMsg;
    switch(type) {
    case 2: // dll
        projectType = PROJECT_TYPE_DYNAMIC_LIBRARY;
        break;
    case 4: // static library
        projectType = PROJECT_TYPE_STATIC_LIBRARY;
        break;
    case 1: // exe
    default:
        projectType = PROJECT_TYPE_EXECUTABLE;
        break;
    }

    le_conf->SetProjectType(projectType);

    // if project type is DLL or Executable, copy linker settings as well
    if(settings->GetProjectType(le_conf->GetName()) == PROJECT_TYPE_EXECUTABLE ||
       settings->GetProjectType(le_conf->GetName()) == PROJECT_TYPE_DYNAMIC_LIBRARY) {
        wxXmlNode* linkNode = XmlUtils::FindNodeByName(config, wxT("Tool"), wxT("VCLinkerTool"));
        if(linkNode) {
            wxString outputFileName(XmlUtils::ReadString(linkNode, wxT("OutputFile")));
#ifndef __WXMSW__
            outputFileName.Replace(wxT(".dll"), wxT(".so"));
            outputFileName.Replace(wxT(".exe"), wxT(""));
#endif

            le_conf->SetOutputFileName(outputFileName);

            // read in the additional libraries & libpath
            wxString libs = XmlUtils::ReadString(linkNode, wxT("AdditionalDependencies"));

            // libs is a space delimited string
            wxStringTokenizer tk(libs, wxT(" "));
            libs.Empty();
            while(tk.HasMoreTokens()) {
                libs << tk.NextToken() << wxT(";");
            }
            le_conf->SetLibraries(libs);
            le_conf->SetLibPath(XmlUtils::ReadString(linkNode, wxT("AdditionalLibraryDirectories")));
        }
    } else {
        // static library
        wxXmlNode* libNode = XmlUtils::FindNodeByName(config, wxT("Tool"), wxT("VCLibrarianTool"));
        if(libNode) {

            wxString outputFileName(XmlUtils::ReadString(libNode, wxT("OutputFile")));
            outputFileName.Replace(wxT("\\"), wxT("/"));

            wxString outputFileNameOnly = outputFileName.AfterLast(wxT('/'));
            wxString outputFilePath = outputFileName.BeforeLast(wxT('/'));

            if(m_compilerLowercase.Contains(wxT("gnu"))) {
                if(outputFileNameOnly.StartsWith(wxT("lib")) == false) {
                    outputFileNameOnly.Prepend(wxT("lib"));
                }
                outputFileName.Clear();
                outputFileName << outputFilePath << wxT("/") << outputFileNameOnly;
                outputFileName.Replace(wxT(".lib"), wxT(".a"));
            }
            le_conf->SetOutputFileName(outputFileName);
        }
    }

    // add the configuration
    settings->SetBuildConfiguration(le_conf);
}

void VcImporter::CreateFiles(wxXmlNode* parent, wxString vdPath, ProjectPtr proj)
{
    if(!parent) {
        return;
    }

    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == wxT("Filter")) {
            // add new virtual directory
            wxString name = XmlUtils::ReadString(child, wxT("Name"));
            wxString tmpPath = vdPath;
            if(tmpPath.IsEmpty() == false) {
                tmpPath << wxT(":");
            }
            tmpPath << name;
            proj->CreateVirtualDir(tmpPath);
            CreateFiles(child, tmpPath, proj);

        } else if(child->GetName() == wxT("File")) {
            // found a file
            wxString fileName = XmlUtils::ReadString(child, wxT("RelativePath"));
            wxString path = vdPath;
            if(path.IsEmpty()) {
                path = wxT("src");
            }

            fileName.Replace(wxT("\\"), wxT("/"));
            proj->AddFile(fileName, path);
        }
        child = child->GetNext();
    }
}

wxArrayString VcImporter::SplitString(const wxString& s)
{
    wxArrayString arr;
    wxString s1(s);
    s1.Replace(wxT(","), wxT(";"));
    wxStringTokenizer tk1(s1, wxT(";"));
    while(tk1.HasMoreTokens()) {
        arr.Add(tk1.NextToken());
    }
    return arr;
}
