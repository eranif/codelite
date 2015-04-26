#ifndef GENERICIMPORTER_H
#define GENERICIMPORTER_H

#include <wx/string.h>
#include <vector>

enum class GenericCfgType {
	DYNAMIC_LIBRARY,
	STATIC_LIBRARY,
	EXECUTABLE
};

enum class GenericFileType {
	SOURCE,
	HEADER,
	OTHER
};

struct GenericProjectCfg {
	wxString name;
	wxString outputFilename;
	wxString intermediateDirectory;
	wxString includePath;
	wxString preprocessor;
	wxString libraries;
	wxString libPath;
	GenericCfgType type;
};

struct GenericProjectFile {
	wxString name;
	wxString vpath;
	GenericFileType type;
};

struct GenericProject {
	wxString name;
	wxString path;
	GenericCfgType cfgType;
	std::vector<GenericProjectCfg> cfgs;
	std::vector<GenericProjectFile> files;
};

struct GenericWorkspace {
	wxString name;
	wxString path;
	std::vector<GenericProject> projects;
};


class GenericImporter
{
public:
	virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler) = 0;
	virtual bool isSupportedWorkspace() = 0;
	virtual GenericWorkspace PerformImport() = 0;
};

#endif // GENERICIMPORTER_H
