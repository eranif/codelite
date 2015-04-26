#include "VisualCppImporter.h"
#include <wx/tokenzr.h>

#ifdef __WXMSW__
#define STATIC_LIBRARY_EXT wxT(".lib")
#define DYNAMIC_LIBRARY_EXT wxT(".dll")
#define EXECUTABLE_EXT wxT(".exe")
#else
#define STATIC_LIBRARY_EXT wxT(".a")
#define DYNAMIC_LIBRARY_EXT wxT(".so")
#define EXECUTABLE_EXT wxT("")
#endif

VisualCppImporter::VisualCppImporter() {
}

VisualCppImporter::~VisualCppImporter() {
}

bool VisualCppImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler) {
	wsInfo.Assign(filename);
	
	wxString compilerName = defaultCompiler.Lower();
	
	IsGccCompile = compilerName.Contains(wxT("gnu")) || compilerName.Contains(wxT("gcc")) || compilerName.Contains(wxT("g++")) | compilerName.Contains(wxT("mingw"));
	
	wxString extension = wsInfo.GetExt().Lower();
	
	bool isValidExt = extension == wxT("dsw") || extension == wxT("sln");
	bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;
	
	if (result) {
		fis = std::make_shared<wxFileInputStream>(filename);
		tis = std::make_shared<wxTextInputStream>(*fis);
		
		while(!fis->Eof()) {
			wxString line = tis->ReadLine();
			int index = line.Find(wxT("Format Version"));
			if (index != wxNOT_FOUND) {
				wxString value = line.Mid(index + 14).Trim().Trim(false);
				value.ToLong(&version);
				break;
			}
		}
		
		result = fis->IsOk();
	}
	
	return result;
}

bool VisualCppImporter::isSupportedWorkspace() {
	return version >= 6 && version <= 12;
}

GenericWorkspace VisualCppImporter::PerformImport() {
	GenericWorkspace genericWorkspace;
	switch(version) {
		case 6:
		GenerateFromVC6(genericWorkspace);
		break;
	}
	
	return genericWorkspace;
}

void VisualCppImporter::GenerateFromVC6(GenericWorkspace& genericWorkspace) {
	genericWorkspace.name = wsInfo.GetName();
	genericWorkspace.path = wsInfo.GetPath();
	
	while(!fis->Eof()) {
		wxString line = tis->ReadLine();
		int index = line.Find(wxT("Project:"));
		if (index != wxNOT_FOUND) {
			int end = line.Find(wxT("- Package Owner")) - 1;
			wxString value = line.SubString(index + 8, end).Trim().Trim(false);
			value.Replace(wxT("\""), wxT(""));
			value.Replace(wxT(".\\"), wxT(""));
			
			wxStringTokenizer part(value, wxT("="));
			
			wxString projectName = part.GetNextToken().Trim().Trim(false);
			wxString projectFile = part.GetNextToken().Trim().Trim(false);
			
			wxFileName projectInfo(wsInfo.GetPath() + wxFileName::GetPathSeparator() + projectFile);
			wxFileInputStream projectFIS(projectInfo.GetFullPath());
			
			if (projectFIS.IsOk()) {
				GenericProject genericProject;
				genericProject.name = projectName;
				genericProject.path = projectInfo.GetPath();
				
				wxTextInputStream projectTIS(projectFIS);
				
				wxString virtualPath = wxT("");
				wxString outputFilename = wxT("");
				
				while(!projectFIS.Eof()) {
					line = projectTIS.ReadLine();
					
					index = line.Find(wxT("TARGTYPE"));
					if (index != wxNOT_FOUND) {
						index = line.find_last_of(wxT("\""));
						wxString projectType = line.Mid(index + 1).Trim().Trim(false);
						if (projectType == "0x0104") {
							genericProject.cfgType = GenericCfgType::STATIC_LIBRARY;
							outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
							outputFilename += STATIC_LIBRARY_EXT;
							if (IsGccCompile) 
								outputFilename.Replace(wxT("lib"), wxT("a"));
						}
						else if (projectType == "0x0102") {
							genericProject.cfgType = GenericCfgType::DYNAMIC_LIBRARY;
							outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
							outputFilename += DYNAMIC_LIBRARY_EXT;
						}
						else {
							genericProject.cfgType = GenericCfgType::EXECUTABLE;
							outputFilename = wxT("$(IntermediateDirectory)/$(ProjectName)");
							outputFilename += EXECUTABLE_EXT;
						}
					}
					
					index = line.Find(wxT("\"$(CFG)\" == "));
					if (index != wxNOT_FOUND) {
						wxString projectCfgName = line.Mid(index + 12).Trim().Trim(false);
						projectCfgName.Replace(projectName + wxT(" - "), wxT(""));
						projectCfgName.Replace(wxT("\""), wxT(""));
						projectCfgName.Replace(wxT(" "), wxT("_"));
						
						GenericProjectCfg genericProjectCfg;
						genericProjectCfg.name = projectCfgName;
						genericProjectCfg.type = genericProject.cfgType;
						genericProjectCfg.outputFilename = outputFilename;
						
						while(!projectFIS.Eof()) {
							line = projectTIS.ReadLine();
							
							index = line.Find(wxT("PROP Intermediate_Dir"));
							if (index != wxNOT_FOUND) {
								wxString intermediateDirectory = line.Mid(index + 21).Trim().Trim(false);
								intermediateDirectory.Replace(wxT("\""), wxT(""));
								intermediateDirectory.Replace(wxT(" "), wxT("_"));
								intermediateDirectory.Replace(wxT("\\"), wxT("/"));
								genericProjectCfg.intermediateDirectory = wxT("./") + intermediateDirectory;
							}
							
							
							index = line.Find(wxT("ADD CPP"));
							if (index != wxNOT_FOUND) {
								wxString preprocessor = wxT("");
								for (int pos = 0; pos < line.Length(); pos++) {
									if (line.GetChar(pos) == wxT('/') && line.GetChar(pos + 1) == wxT('D')) {
										int count = 0;
										wxString word = wxT("");
										while(count < 2) {
											if (line.GetChar(pos) == wxT('\"')) {
												count++;
											}
											else if (count == 1) {
												word.Append(line.GetChar(pos));
											}
											
											pos++;
										}
										
										preprocessor += word + wxT(";");
									}
								}
								
								genericProjectCfg.preprocessor = preprocessor;
							}
							
							index = line.Find(wxT("ADD LINK32"));
							if (index != wxNOT_FOUND) {
								int begin = index  + 10;
								int end = line.Find(wxT("/")) - 1;
								wxString libraries = line.SubString(begin, end).Trim().Trim(false);
								libraries.Replace(wxT(" "), wxT(";"));
								if (IsGccCompile) 
									libraries.Replace(wxT(".lib"), wxT(""));
								
								genericProjectCfg.libraries = libraries;
								break;
							}
						}
						
						genericProject.cfgs.push_back(genericProjectCfg);
					}
				
					index = line.Find(wxT("Begin Group"));
					if (index != wxNOT_FOUND) {
						virtualPath = line.Mid(index + 11).Trim().Trim(false);
						virtualPath.Replace(wxT("\""), wxT(""));
						virtualPath.Replace(wxT(" "), wxT("_"));
					}
					
					index = line.Find(wxT("SOURCE="));
					if (index != wxNOT_FOUND) {
						wxString filename = line.Mid(index + 7).Trim().Trim(false);
						filename.Replace(wxT("\\"), wxT("/"));
						
						wxFileName fileInfo(projectInfo.GetPath() + wxFileName::GetPathSeparator() + filename);
						
						GenericProjectFile genericProjectFile;
						genericProjectFile.name = filename;
						genericProjectFile.vpath = virtualPath;
						
						wxString extension = fileInfo.GetExt().Lower();
						
						if (extension == wxT("h") || extension == wxT("hpp") || extension == wxT("hxx")) {
							genericProjectFile.type = GenericFileType::HEADER;
						}
						else if (extension == wxT("c") || extension == wxT("cpp") || extension == wxT("cxx")) {
							genericProjectFile.type = GenericFileType::SOURCE;
						}
						else {
							genericProjectFile.type = GenericFileType::OTHER;
						}
						
						genericProject.files.push_back(genericProjectFile);
					}
				}
				
				genericWorkspace.projects.push_back(genericProject);
			}
		}
	}
}
