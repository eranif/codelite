#include "vcimporter.h"
#include "wx/filename.h"
#include "macros.h"
#include "wx/tokenzr.h"
#include "workspace.h"
#include "xmlutils.h"

#define NEXT_LINE(line)\
	line.Empty();\
	if(!ReadLine(line)){\
		return false;\
	}

VcImporter::VcImporter(const wxString &fileName)
: m_fileName(fileName)
, m_is(NULL)
, m_tis(NULL)
{
	wxFileName fn(m_fileName);
	m_isOk = fn.FileExists();
	if(m_isOk){
		m_is = new wxFileInputStream(fn.GetFullPath());
		m_tis = new wxTextInputStream(*m_is);
	}
}

VcImporter::~VcImporter()
{
	if(m_is){
		delete m_is;
	}
	if(m_tis){
		delete m_tis;
	}
}

bool VcImporter::ReadLine(wxString &line)
{
	line = wxEmptyString;
	if(m_isOk){
		while(!m_is->Eof()){
			line = m_tis->ReadLine();
			TrimString(line);
			if(line.Length() == 1 || line.Length() == 2 || line.IsEmpty() || line.StartsWith(wxT("#"))){
				//get next line
				continue;
			}else{
				return true;
			}
		}
	}
	return false;
}

bool VcImporter::Import(wxString &errMsg)
{
	wxString line;
	while(true){
		if(!ReadLine(line))
			break;
		if(line.StartsWith(wxT("Project"))){
			if(!OnProject(line, errMsg)){
				return false;
			}
		}
	}
	
	//create LE files
	CreateWorkspace();
	CreateProjects();
	return true;
}

bool VcImporter::OnProject(const wxString &firstLine, wxString &errMsg)
{
	//first line contains the project name, project file path, and project id
	wxStringTokenizer tkz(firstLine, wxT("="));
	if(tkz.CountTokens() != 2){
		errMsg = wxT("Invalid 'Project' section found. expected <expr> = <expr>");
		return false;
	}
	
	tkz.NextToken();
	wxString token = tkz.NextToken();
	TrimString(token);
	wxStringTokenizer tk2(token, wxT(","));
	if(tk2.CountTokens() != 3){
		errMsg = wxT("Invalid 'Project' section found. expected <project name>, <project file path>, <project id>");
		return false;
	}
	
	VcProjectData pd;
	
	pd.name = tk2.NextToken();
	RemoveGershaim(pd.name);

	pd.filepath = tk2.NextToken();
	RemoveGershaim(pd.filepath);
	
	pd.id = tk2.NextToken();
	RemoveGershaim(pd.id);

	m_projects.insert(std::make_pair<wxString, VcProjectData>(pd.id, pd));
	//Skip all lines until EndProject section is found
	wxString line;
	while(true){
		NEXT_LINE(line);
		if(line == wxT("EndProject")){
			return true;
		}
	}
	return false;
}

void VcImporter::RemoveGershaim(wxString &str)
{
	TrimString(str);
	str = str.AfterFirst(wxT('"'));
	str = str.BeforeLast(wxT('"'));
}

void VcImporter::CreateWorkspace()
{
	//create a workspace file from the data we collected
	wxFileName fn(m_fileName);
	wxString errMsg;
	WorkspaceST::Get()->CreateWorkspace(fn.GetName(), fn.GetPath(), errMsg);
}

//
//ConfigurationType = 1 // exe
//ConfigurationType = 2 // dll
//ConfigurationType = 4 // static lib
//
void VcImporter::CreateProjects()
{
	std::map<wxString, VcProjectData>::iterator iter = m_projects.begin();
	for(; iter != m_projects.end(); iter++){
		//load xml file
		VcProjectData pd = iter->second;
		ConvertProject(pd);
	}
}

bool VcImporter::ConvertProject(VcProjectData &data)
{
	wxXmlDocument doc(data.filepath);
	if(!doc.IsOk()){
		return false;
	}

	//to create a project skeleton, we need the project type
	//since VS allows each configuration to be of different
	//type, while LE allows single type for all configurations
	//we use the first configuration type that we find
	wxXmlNode *configs = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("Configurations"));
	if(!configs){
		return false;
	}
	
	//find the first configuration node
	wxXmlNode * config = XmlUtils::FindFirstByTagName(configs, wxT("Configuration"));
	if(!config)	return false;
	//read the configuration type, default is set to Executeable
	long type = XmlUtils::ReadLong(config, wxT("ConfigurationType"), 1);
	wxString projectType;
	wxString errMsg;
	switch(type){
		case 2: //dll
			projectType = Project::DYNAMIC_LIBRARY;
			break;
		case 4:	//static library
			projectType = Project::STATIC_LIBRARY;
			break;
		case 1:	//exe
		default:
			projectType = Project::EXECUTABLE;
			break;
	}
	//now we can create the project
	wxFileName fn(data.filepath);
	fn.MakeAbsolute();
	if(!WorkspaceST::Get()->CreateProject(data.name, fn.GetPath(), projectType, errMsg)){
		return false;
	}
	
	//get the new project instance
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(data.name, errMsg);
	ProjectSettingsPtr le_settings(new ProjectSettings(NULL));
	//remove the default 'Debug' configuration
	le_settings->RemoveConfiguration(wxT("Debug"));
	le_settings->SetProjectType(projectType);

	while(config){
		if(config->GetName() == wxT("Configuration")){
			AddConfiguration(le_settings, config);
		}
		config = config->GetNext();
	}
	proj->SetSettings(le_settings);
	//add all virtual folders
	wxXmlNode *files = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("Files"));
	if(files){
		proj->BeginTranscation();
		CreateFiles(files, wxEmptyString, proj);
		proj->CommitTranscation();
	}
	return true;
}

void VcImporter::AddConfiguration(ProjectSettingsPtr settings, wxXmlNode *config)
{
	//configuration name
	wxString name = XmlUtils::ReadString(config, wxT("Name"));
	name = name.BeforeFirst(wxT('|'));
	name.Replace(wxT(" "), wxT("_"));

	BuildConfigPtr le_conf(new BuildConfig(NULL));
	le_conf->SetName(name);
	le_conf->SetIntermediateDirectory(XmlUtils::ReadString(config, wxT("IntermediateDirectory")));
	//get the compiler settings
	wxXmlNode *cmpNode = XmlUtils::FindNodeByName(config, wxT("Tool"), wxT("VCCLCompilerTool"));
	//get the include directories
	le_conf->SetIncludePath(SplitString(XmlUtils::ReadString(cmpNode, wxT("AdditionalIncludeDirectories"))));
	le_conf->SetPreprocessor(XmlUtils::ReadString(cmpNode, wxT("PreprocessorDefinitions")));

	//if project type is DLL or Executable, copy linker settings as well
	if(	settings->GetProjectType() == Project::EXECUTABLE || settings->GetProjectType() == Project::DYNAMIC_LIBRARY){
		wxXmlNode *linkNode = XmlUtils::FindNodeByName(config, wxT("Tool"), wxT("VCLinkerTool"));
		if(linkNode){
			le_conf->SetOutputFileName(XmlUtils::ReadString(linkNode, wxT("OutputFile")));
			//read in the additional libraries & libpath
			wxString libs = XmlUtils::ReadString(linkNode, wxT("AdditionalDependencies"));
			//libs is a space delimited string
			wxStringTokenizer tk(libs, wxT(" "));
			libs.Empty();
			while(tk.HasMoreTokens()){
				libs << tk.NextToken() << wxT(";");
			}
			le_conf->SetLibraries(libs);
			le_conf->SetLibPath(XmlUtils::ReadString(linkNode, wxT("AdditionalLibraryDirectories")));
		}
	}else{
		// static library
		wxXmlNode *libNode = XmlUtils::FindNodeByName(config, wxT("Tool"), wxT("VCLibrarianTool"));
		if(libNode){
			le_conf->SetOutputFileName(XmlUtils::ReadString(libNode, wxT("OutputFile")));
		}
	}

	//add the configuration
	settings->SetBuildConfiguration(le_conf);
}

void VcImporter::CreateFiles(wxXmlNode *parent, wxString vdPath, ProjectPtr proj)
{
	if( !parent ){
		return;
	}

	wxXmlNode *child = parent->GetChildren();
	while (child) {
		if(child->GetName() == wxT("Filter")){
			// add new virtual directory
			wxString name = XmlUtils::ReadString(child, wxT("Name"));
			wxString tmpPath = vdPath;
			if(tmpPath.IsEmpty() == false){
				tmpPath << wxT(":");
			}
			tmpPath << name;
			proj->CreateVirtualDir(tmpPath);
			CreateFiles(child, tmpPath, proj);

		} else if(child->GetName() == wxT("File")){
			//found a file
			wxString fileName = XmlUtils::ReadString(child, wxT("RelativePath"));
			wxString path = vdPath;
			if(path.IsEmpty()){
				path = wxT("src");
			}
			proj->AddFile(fileName, path);
		}
		child = child->GetNext();
	}
}

wxArrayString VcImporter::SplitString(const wxString &s)
{
	wxArrayString arr;
	wxString s1(s);
	s1.Replace(wxT(","), wxT(";"));
	wxStringTokenizer tk1(s1, wxT(";"));
	while(tk1.HasMoreTokens()){
		arr.Add(tk1.NextToken());
	}
	return arr;
}
